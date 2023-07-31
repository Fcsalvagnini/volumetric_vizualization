#include "graphical_context.h"
#include "scene_data.h"
#include "normal.h"
#include "global.h"
#include "ray_casting.h"
#include "dda3d.h"

GraphicalContext *GetGraphicalContext(
    iftImage *img, iftImage *mask
)
{
    printf("[INFO] Generating Graphical Context!\n");
    GraphicalContext *graphicalContext = malloc(sizeof(GraphicalContext));
    graphicalContext->normalIndexes = NULL;
    graphicalContext->distances = NULL;
    graphicalContext->sceneFaces = GetSceneFaces(img);
    graphicalContext->sceneVertexes = GetSceneVertexes(img);
    SetPhongModel(graphicalContext);
    SetObjects(graphicalContext);

    return graphicalContext;
}

void SetObjects(GraphicalContext *graphicalContext) {
    // Sets object's visibility and random colors reflectance
    // Comment about code limited to 3 objects
    graphicalContext->nObjects = 3;
    graphicalContext->sceneObjects = malloc(
        graphicalContext->nObjects * sizeof(Object)
    );

    for (size_t idx = 0; idx < graphicalContext->nObjects; idx++) {
        graphicalContext->sceneObjects[idx].viewed = false;
        SetRandomRGB(graphicalContext, idx);
        printf("[INFO] Object %ld has color: %fR %fG %fB\n",
                idx,
                graphicalContext->sceneObjects[idx].colorReflectance[0],
                graphicalContext->sceneObjects[idx].colorReflectance[1],
                graphicalContext->sceneObjects[idx].colorReflectance[2]
        );
    }
}

void SetRandomRGB(GraphicalContext *graphicalContext, int objectIdx) {
    graphicalContext->sceneObjects[objectIdx].colorReflectance[0] =
        ((float) rand()) / ((float) RAND_MAX);
    graphicalContext->sceneObjects[objectIdx].colorReflectance[1] =
        ((float) rand()) / ((float) RAND_MAX);
    graphicalContext->sceneObjects[objectIdx].colorReflectance[2] =
        ((float) rand()) / ((float) RAND_MAX);
    // Generates YCbCr for Slices Coloring
    iftColor RGB;
    RGB.val[0] = (int)(MAX_INT
            * graphicalContext->sceneObjects[objectIdx].colorReflectance[0]);
    RGB.val[1] = (int)(MAX_INT
            * graphicalContext->sceneObjects[objectIdx].colorReflectance[1]);
    RGB.val[2] = (int)(MAX_INT
            * graphicalContext->sceneObjects[objectIdx].colorReflectance[2]);
    graphicalContext->sceneObjects[objectIdx].colorReflectanceYCbCr = iftRGBtoYCbCr(
        RGB, MAX_INT
    );
}

void SetPhongModel(GraphicalContext *graphicalContext) {
    graphicalContext->phongModel = malloc(sizeof(PhongModel));
    graphicalContext->phongModel->normal = malloc((181 * 360 + 1) * sizeof(iftVector));
    graphicalContext->phongModel->ka = 0.1;
    graphicalContext->phongModel->ks = 0.2;
    graphicalContext->phongModel->kd = 0.7;
    graphicalContext->phongModel->ns = 5;
    graphicalContext->phongModel->ra = MAX_INT;
    graphicalContext->phongModel->dMin = IFT_INFINITY_FLT;
    graphicalContext->phongModel->dMax = IFT_INFINITY_FLT_NEG;
}

void DestroyGraphicalContext(GraphicalContext *graphicalContext) {
    DestroyFaces(graphicalContext->sceneFaces);
    DestroySceneVertex(graphicalContext->sceneVertexes);
    free(graphicalContext->phongModel->normal);
    free(graphicalContext->phongModel);

    if (graphicalContext->normalIndexes != NULL) {
        iftDestroyImage(&graphicalContext->normalIndexes);
    }
    if (graphicalContext->distances != NULL) {
        iftDestroyImage(&graphicalContext->distances);
    }

    free(graphicalContext->sceneObjects);
    free(graphicalContext);
}

iftSet *GetMaskObjectBorder(iftImage *mask, int label) {
    iftAdjRel *A = iftSpheric(1);
    iftSet *S = NULL;
    iftVoxel u, v;
    int q;

    // Iterates over all image pixels to get object border
    for (size_t p = 0; p < mask->n; p++) {
        // Only border values
        if (mask->val[p] == label) {
            u = iftGetVoxelCoord(mask, p);
            // Iterates of the adjacent voxels
            for (size_t adj_idx = 1; adj_idx < A->n; adj_idx++) {
                v = iftGetAdjacentVoxel(A, u, adj_idx);
                // Verifies if it is a valid voxel
                if (iftValidVoxel(mask, v)) {
                    q = iftGetVoxelIndex(mask, v);
                    if (mask->val[q] != label) {
                        iftInsertSet(&S, p);
                        break;
                    }
                }
            }
        }
    }

    // Release resources
    iftDestroyAdjRel(&A);

    return S;
}

void UpdatesGraphicalContext(
    iftImage* img, iftImage* maskImg,  iftImage* renderedImg,
    GraphicalContext* graphicalContext, iftMatrix* pointwisePlaneOps,
    iftMatrix* planeNormalTransformed, iftMatrix* pMatrix, iftMatrix* p0Matrix
) {
    // Computes normal for each object
    graphicalContext->normalIndexes = iftCreateImageFromImage(maskImg);
    iftSetImage(
        graphicalContext->normalIndexes, -1
    );
    GenerateNormalTable(graphicalContext);
    iftSet **S = malloc(3 * sizeof(iftSet));
    for (size_t label = 0; label < 3; label++) {
        printf("[INFO] Computing normal for Label %ld\n", label + 1);
        S[label] = GetMaskObjectBorder(maskImg, label + 1);
        // Computes normal for each object and store in an image
        if (graphicalContext->normalMethod == 0) {
            ComputesNormalScene(img, maskImg, graphicalContext, S[label]);
        }
        else if (graphicalContext->normalMethod == 1) {
            ComputesNormalObject(
                img, maskImg, graphicalContext, S[label],
                label + 1
            );
        }
    }

    // Computes distances maximum and minimum
    // Iterates over the plane casting rays
    iftVector planeNormalTransformedVector = {
        .x = -planeNormalTransformed->val[0],
        .y = -planeNormalTransformed->val[1],
        .z = -planeNormalTransformed->val[2]
    };
    graphicalContext->distances = iftCreateImageFromImage(maskImg);
    iftVoxel u = (iftVoxel){.z = 0};
    iftPoint intersectionPoints[2];
    iftPoint *dda3DPoints = NULL;
    int nPoints;
    iftVoxel *surfaceVoxel = NULL;
    int voxelIdx, nSurfaces;
    iftPoint surfacePoint, planePoint;
    float distance;
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
                surfaceVoxel = FindSurfaceVoxels(
                    img, maskImg, dda3DPoints, nPoints,
                    planeNormalTransformedVector,
                    graphicalContext, &nSurfaces
                );

                if (surfaceVoxel != NULL) {
                    for (size_t sIdx = 0; sIdx < nSurfaces; sIdx++) {
                        voxelIdx = iftGetVoxelIndex(maskImg, surfaceVoxel[sIdx]);

                        planePoint = (iftPoint) {
                            .x = p0Matrix->val[0],
                            .y = p0Matrix->val[1],
                            .z = p0Matrix->val[2],
                        };
                        surfacePoint = (iftPoint) {
                            .x = surfaceVoxel[sIdx].x,
                            .y = surfaceVoxel[sIdx].y,
                            .z = surfaceVoxel[sIdx].z,
                        };
                        // Only consider first surface voxel for dMax and dMin
                        if (sIdx == 0) {
                            distance = iftPointDistance(planePoint, surfacePoint);
                            if (distance > graphicalContext->phongModel->dMax) {
                                graphicalContext->phongModel->dMax = distance;
                            }
                            if (distance < graphicalContext->phongModel->dMin) {
                                graphicalContext->phongModel->dMin = distance;
                        }
                        }
                        graphicalContext->distances->val[voxelIdx] = distance;
                    }
                    free(surfaceVoxel);
                }

                free(dda3DPoints);
            }

            iftDestroyMatrix(&p0Matrix);
        }
    }

    // Releases resources
    free(S);
}