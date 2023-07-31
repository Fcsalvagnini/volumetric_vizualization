#include "ift.h"
#include "rendering.h"
#include "graphical_context.h"
#include "dda3d.h"
#include "global.h"
#include "normal.h"
#include "ray_casting.h"
#include "phong.h"

void RenderOpaqueScene(
    iftImage* img, iftImage* maskImg, iftImage* renderedImg,
    GraphicalContext* graphicalContext, iftMatrix* pointwisePlaneOps,
    iftMatrix* planeNormalTransformed, iftMatrix* pMatrix, iftMatrix* p0Matrix
) {
    // Iterates over the plane casting rays
    iftVector planeNormalTransformedVector = {
        .x = -planeNormalTransformed->val[0],
        .y = -planeNormalTransformed->val[1],
        .z = -planeNormalTransformed->val[2]
    };
    iftVoxel u = (iftVoxel){.z = 0};
    iftPoint intersectionPoints[2];
    iftPoint *dda3DPoints = NULL;
    int nPoints;
    iftVoxel *surfaceVoxel = NULL;
    int label, voxelIdx, uIdx;
    iftColor YCbCr;
    for (u.x = 0; u.x < renderedImg->xsize; u.x++) {
        for (u.y = 0; u.y < renderedImg->ysize; u.y++) {
            // Apply Transformations on visualization plane
            pMatrix->val[0] = u.x;
            pMatrix->val[1] = u.y;
            p0Matrix = iftMultMatrices(pointwisePlaneOps, pMatrix);

            // Verify Intersection (If it happens, runs DDA3D)
            if (VerifyIntersection(
                img, graphicalContext->sceneFaces, intersectionPoints,
                planeNormalTransformed, p0Matrix
            )) {
                dda3DPoints = PerformDDA3D(img, intersectionPoints, &nPoints);
                // Finds ISO Surface
                surfaceVoxel = FindSurfaceVoxel(
                    img, maskImg, dda3DPoints, nPoints,
                    planeNormalTransformedVector, graphicalContext
                );

                if (surfaceVoxel != NULL) {
                    uIdx = iftGetVoxelIndex(renderedImg, u);
                    voxelIdx = iftGetVoxelIndex(maskImg, surfaceVoxel[0]);
                    label = maskImg->val[voxelIdx];
                    YCbCr = GetYcbCrFromPhong(
                        voxelIdx, label - 1, graphicalContext,
                        planeNormalTransformedVector
                    );
                    renderedImg->val[uIdx] = YCbCr.val[0];
                    renderedImg->Cb[uIdx] = YCbCr.val[1];
                    renderedImg->Cr[uIdx] = YCbCr.val[2];
                    free(surfaceVoxel);
                }

                free(dda3DPoints);
            }

            iftDestroyMatrix(&p0Matrix);
        }
    }
}

iftImage *Rendering(
        iftImage* img, iftImage* maskImg, GraphicalContext* graphicalContext,
        int tiltAlpha, int spinBeta
) {
    // Initializes necessary variables
    float volumeDiagonal = sqrt(
        pow(img->xsize, 2) + pow(img->ysize, 2) + pow(img->zsize, 2)
    );
    float xCenter, yCenter, zCenter;
    xCenter = (float)img->xsize / 2.0;
    yCenter = (float)img->ysize / 2.0;
    zCenter = (float)img->zsize / 2.0;

    // Initializes image to hold MIP
    iftImage* renderedImg = iftCreateImage(
        (int)ceil(volumeDiagonal),
        (int)ceil(volumeDiagonal),
        1
    );
    printf(
        "[INFO] RENDERED IMG: x_size: %d | y_size: %d | z_size: %d\n",
        renderedImg->xsize, renderedImg->ysize, renderedImg->zsize
    );

    // Sets color Image as black
    iftSetImage(renderedImg, (int)(pow(2, 16)/16));
    iftSetCbCr(renderedImg, (int)(pow(2, 16)/2));

    // Creates Matrixes
    iftMatrix* pMatrix = iftCreateMatrix(1, 4);
    pMatrix->val[2] = -(volumeDiagonal/2.0);
    pMatrix->val[3] = 1.0;

    iftMatrix* p0Matrix = NULL;
    iftMatrix* xTiltMatrix = iftRotationMatrix(0, -tiltAlpha);
    iftMatrix* ySpinMatrix = iftRotationMatrix(1, -spinBeta);

    // Translation Matrixes
    iftVector translationVector;
    translationVector = (iftVector) {
        .x = -(volumeDiagonal/2.0), .y = -(volumeDiagonal/2.0),
        .z = -(volumeDiagonal/2.0)
    };
    iftMatrix* jTranslationMatrix = iftTranslationMatrix(translationVector);
    translationVector = (iftVector) {
        .x = xCenter, .y = yCenter, .z = zCenter
    };
    iftMatrix* jcTranslationMatrix = iftTranslationMatrix(translationVector);

    /* First, let us define the matrix to project the visualization plane
    (for each voxel)
    */
    iftMatrix* pointwisePlaneOps = iftMultMatricesChain(
        4, jcTranslationMatrix, xTiltMatrix, ySpinMatrix, jTranslationMatrix
    );
    // Then, transform the normal vector of our visualization plane.
    iftMatrix* planeNormal = iftCreateMatrix(1, 4);
    planeNormal->val[2] = 1.0;
    iftMatrix* planeNormalTransformed = iftMultMatricesChain(
        3, xTiltMatrix, ySpinMatrix, planeNormal
    );

    // Calculates Normal for each visible object surface and DMin and DMax
    UpdatesGraphicalContext(
        img, maskImg, renderedImg, graphicalContext, pointwisePlaneOps,
        planeNormalTransformed, pMatrix, p0Matrix
    );

    printf("[INFO] Dmin %f\n", graphicalContext->phongModel->dMin);
    printf("[INFO] Dmax %f\n", graphicalContext->phongModel->dMax);

    // Computes Phong's for each visible object surface
    RenderOpaqueScene(
        img, maskImg, renderedImg, graphicalContext, pointwisePlaneOps,
        planeNormalTransformed, pMatrix, p0Matrix
    );

    // Destroy Matrixes
    iftDestroyMatrix(&pMatrix);
    iftDestroyMatrix(&xTiltMatrix);
    iftDestroyMatrix(&ySpinMatrix);
    iftDestroyMatrix(&jTranslationMatrix);
    iftDestroyMatrix(&jcTranslationMatrix);
    iftDestroyMatrix(&pointwisePlaneOps);
    iftDestroyMatrix(&planeNormal);

    return renderedImg;
}