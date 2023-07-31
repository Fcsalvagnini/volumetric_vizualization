#ifndef _RAY_CASTING_
#define _RAY_CASTING_

#include "ift.h"
#include "scene_data.h"
#include "graphical_context.h"

float GetTheta(iftVector planeNormal, iftVector surfaceNormal);

iftPoint GetPoint(
    iftMatrix* p0Matrix, float lambda, iftMatrix* planeNormalTransformed
);

float CalculateLambda(
    Face face, iftMatrix* planeNormalTransformed, iftMatrix* p0Matrix
);

bool VerifyIntersection(
    iftImage* img, Face* faces, iftPoint* intersectionPoints,
    iftMatrix* planeNormalTransformed, iftMatrix* p0Matrix
);

iftVoxel *FindSurfaceVoxel(
    iftImage* img, iftImage* maskImg, iftPoint* dda3DPoints, int nPoints,
    iftVector planeNormal,  GraphicalContext* graphicalContext
);

#endif