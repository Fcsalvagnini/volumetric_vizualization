#include "normal.h"

void ComputesNormal(
    iftImage* img, iftImage* maskImg, GraphicalContext* graphicalContext,
    iftSet* S
) {
    int p = 0, q = 0, nIdx;
    iftVoxel u, v;
    iftAdjRel *A = iftSpheric(3.0);
    iftVector grad, tmpGrad;
    float intensityGrad, vectorMagnitude, multiplier, alpha = 2.0;
    while (S != NULL) {
        p = iftRemoveSet(&S);
        u = iftGetVoxelCoord(img, p);
        grad = (iftVector){.x = 0.0, .y = 0.0, .z = 0.0};
        // Iterates over adjacent voxels
        for (size_t adjIdx = 1; adjIdx < A->n; adjIdx++) {
            v = iftGetAdjacentVoxel(A, u, adjIdx);
            if (iftValidVoxel(img, v)) {
                q = iftGetVoxelIndex(img, v);
                if (maskImg->val[q] == maskImg->val[p]) {
                    intensityGrad = img->val[q] - img->val[p];
                }
                else {
                    intensityGrad = 0 - img->val[p];
                }
                tmpGrad = (iftVector) {
                    .x = v.x - u.x,
                    .y = v.y - u.y,
                    .z = v.z - u.z
                };
                vectorMagnitude = iftVectorMagnitude(tmpGrad);
                tmpGrad = (iftVector) {
                    .x = intensityGrad * (tmpGrad.x / vectorMagnitude),
                    .y = intensityGrad * (tmpGrad.y / vectorMagnitude),
                    .z = intensityGrad * (tmpGrad.z / vectorMagnitude)
                };
                grad = (iftVector) {
                    .x = grad.x + tmpGrad.x,
                    .y = grad.y + tmpGrad.y,
                    .z = grad.z + tmpGrad.z
                };
            }
        }
        // Corrects vector orientation
        vectorMagnitude = iftVectorMagnitude(grad);
        grad = (iftVector) {
            .x = grad.x / vectorMagnitude,
            .y = grad.y / vectorMagnitude,
            .z = grad.z / vectorMagnitude
        };
        u = (iftVoxel) {
            .x = (int)((float)(u.x) + alpha * grad.x + 0.5),
            .y = (int)((float)(u.y) + alpha * grad.y + 0.5),
            .z = (int)((float)(u.z) + alpha * grad.z + 0.5)
        };
        if (
            maskImg->val[p] == maskImg->val[iftGetVoxelIndex(maskImg, u)]
        )
        {
            multiplier = -1;
        }
        else {
            multiplier = 1;
        }
        grad = (iftVector) {
            .x = multiplier * grad.x,
            .y = multiplier * grad.y,
            .z = multiplier * grad.z
        };

        nIdx = GetClosestNormal(grad);

        // Stores index to Closest Normal
        graphicalContext->normalIndexes->val[p] = nIdx;
    }
    iftDestroyAdjRel(&A);
}

int GetClosestNormal(iftVector grad) {
    int idx, a, b;
    float gamma, alpha;
    if (grad.x == 0 && grad.y == 0 && grad.z == 0) {
        idx = 0;
        return idx;
    }

    gamma = asin(grad.z);
    b = (int)(gamma * (180.0 / IFT_PI));
    /*
    cos(alpha) = x / cos(gamma)
    sin(alpha) = y / cos(gamma)
    tg(alpha) = sin(alpha) / cos(alpha)
    tg(alpha) = (y / cos(gamma)) / (x / cos(gamma))
    tg(alpha) = y / x
    alpha = atg(y/x)
    */
    alpha = atan2(grad.y, grad.x);
    a = (int)(alpha * (180.0 / IFT_PI));
    if (a < 0) {
        alpha += 180;
    }

    // Shifts b from -90 to 90
    // Multiply by 360, each b step means 360 index
    // Add a to index and 1 of grad(0, 0, 0)
    idx = (b + 90) * 360 + a + 1;

    return idx;
}

void GenerateNormalTable(GraphicalContext* graphicalContext) {
    double gamma, alpha;

    graphicalContext->phongModel->normal[0].x = 0.0;
    graphicalContext->phongModel->normal[0].y = 0.0;
    graphicalContext->phongModel->normal[0].z = 0.0;
    int normalIndex = 1;

    for (int b = -90; b <= 90; b++) {
        gamma = b * (IFT_PI / 180.0);
        for (int a = 0; a <= 359; a++) {

            alpha = a * (IFT_PI / 180.0);
            graphicalContext->phongModel->normal[normalIndex] = (iftVector){
                .x = cos(gamma) * cos(alpha),
                .y = cos(gamma) * sin(alpha),
                .z = sin(gamma)
            };
            normalIndex++;
        }
    }
}