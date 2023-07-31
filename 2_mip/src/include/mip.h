#ifndef _MIP_
#define _MIP_

#include "ift.h"

int GetMaximumIntensity(iftImage* img, iftPoint* dda3d_points, int n_points);

iftPoint GetPoint(
    iftMatrix* p0_matrix, float lambda, iftMatrix* plane_normal_vector
);

float CalculateLambda(
    Face face, iftMatrix* plane_normal_vector, iftMatrix* p0_matrix
);

iftImage *GetMaximumIntensityProjection(
    iftImage* img, Face* faces, int tilt_alpha, int spin_beta
);

bool VerifyIntersection(
    iftImage* mip_img, Face* faces, iftPoint* intersection_points,
    iftMatrix* plane_normal_vector, iftMatrix* p0_matrix
);

#endif