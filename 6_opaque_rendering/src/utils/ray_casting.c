#include "ray_casting.h"

iftPoint GetPoint(
    iftMatrix* p0Matrix, float lambda, iftMatrix* planeNormalTransformed
)
{
    iftPoint point;
    point.x = p0Matrix->val[0] + lambda * planeNormalTransformed->val[0];
    point.y = p0Matrix->val[1] + lambda * planeNormalTransformed->val[1];
    point.z = p0Matrix->val[2] + lambda * planeNormalTransformed->val[2];

    return point;
}

float CalculateLambda(
    Face face, iftMatrix* planeNormalTransformed, iftMatrix* p0Matrix
)
{
    float lambda;
    lambda =
        (
            (
                face.face_center->val[0] * face.normal_vector->val[0]
                + face.face_center->val[1] * face.normal_vector->val[1]
                + face.face_center->val[2] * face.normal_vector->val[2]
            )
            - (
                p0Matrix->val[0] * face.normal_vector->val[0]
                + p0Matrix->val[1] * face.normal_vector->val[1]
                + p0Matrix->val[2] * face.normal_vector->val[2]
            )
        )
        /
        (
            planeNormalTransformed->val[0] * face.normal_vector->val[0]
            + planeNormalTransformed->val[1] * face.normal_vector->val[1]
            + planeNormalTransformed->val[2] * face.normal_vector->val[2]
        );

    return lambda;
}

bool VerifyIntersection(
    iftImage* img, Face* faces, iftPoint* intersectionPoints,
    iftMatrix* planeNormalTransformed, iftMatrix* p0Matrix
)
{
    float lambda = 0;
    float lambdaMin = IFT_INFINITY_FLT, lambdaMax = IFT_INFINITY_FLT_NEG;
    iftPoint tmpPoint;
    iftVoxel u;
    bool intersection = false;
    for(size_t idx=0; idx < 6; idx++) {
        lambda = CalculateLambda(faces[idx], planeNormalTransformed, p0Matrix);

        /*If lambda greater than 0 computes image point.
        If valid point (Inside image domain), updates lambda maximum and minimum.
        */
        if (lambda > 0) {
            tmpPoint = GetPoint(p0Matrix, lambda, planeNormalTransformed);
            u.x = (int)(tmpPoint.x + 0.5);
            u.y = (int)(tmpPoint.y + 0.5);
            u.z = (int)(tmpPoint.z + 0.5);
            if(iftValidVoxel(img, u))
            {
                if(!intersection) {
                    intersection = true;
                }
                if(lambda < lambdaMin) {
                    lambdaMin = lambda;
                }
                if(lambda > lambdaMax) {
                    lambdaMax = lambda;
                }
            }
        }
    }

    if(intersection) {
        intersectionPoints[0] = GetPoint(
            p0Matrix, lambdaMin, planeNormalTransformed
        );
        intersectionPoints[1] = GetPoint(
            p0Matrix, lambdaMax, planeNormalTransformed
        );
        return intersection;
    }
    else {
        return intersection;
    }
}

float GetTheta(iftVector planeNormal, iftVector surfaceNormal) {
    float theta;

    theta = acos(
        iftVectorInnerProduct(surfaceNormal, planeNormal)
        / (iftVectorMagnitude(surfaceNormal) * iftVectorMagnitude(planeNormal))
    );

    return theta;
}

iftVoxel* FindSurfaceVoxel(
    iftImage* img, iftImage* maskImg, iftPoint* dda3DPoints, int nPoints,
    iftVector planeNormal,  GraphicalContext* graphicalContext
) {
    int voxelIndex, label;
    iftAdjRel *A = iftSpheric(1.0);
    iftVoxel u, v;
    iftVoxel *surfaceVoxel = NULL;
    float theta;
    for (size_t idx = 0; idx < nPoints; idx++) {
        u = (iftVoxel) {
            .x = ceil(dda3DPoints[idx].x),
            .y = ceil(dda3DPoints[idx].y),
            .z = ceil(dda3DPoints[idx].z),
        };
        for (size_t adjIdx = 0; adjIdx < A->n; adjIdx++) {
            v = iftGetAdjacentVoxel(A, u, adjIdx);
            if (iftValidVoxel(img, v)) {
                voxelIndex = iftGetVoxelIndex(img, v);
                label = maskImg->val[voxelIndex];
                if (
                    label != 0
                    && graphicalContext->normalIndexes->val[voxelIndex] != -1
                ) {
                    iftVector surfaceNormal =
                        graphicalContext->phongModel->normal
                        [
                            graphicalContext->normalIndexes->val[voxelIndex]
                        ];
                    theta = GetTheta(planeNormal, surfaceNormal);
                    if (theta >= 0 && theta < (IFT_PI / 2)) {
                        iftDestroyAdjRel(&A);

                        surfaceVoxel = malloc(sizeof(iftVoxel));
                        surfaceVoxel->x = v.x;
                        surfaceVoxel->y = v.y;
                        surfaceVoxel->z = v.z;
                        return surfaceVoxel;
                    }
                }
            }
        }
    }

    iftDestroyAdjRel(&A);

    return surfaceVoxel;
}