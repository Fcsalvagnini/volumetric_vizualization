#ifndef _CURVILINEAR_CUT_
#define _CURVILINEAR_CUT_

#include "ift.h"
#include "scene_data.h"

iftPoint *GetCutPoint(
    iftImage* img, iftImage* envelop_img, iftPoint* dda3d_points, int n_points,
    int depth_cut
);

iftPoint GetPoint(
    iftMatrix* p0_matrix, float lambda, iftMatrix* plane_normal_vector
);

float CalculateLambda(
    Face face, iftMatrix* plane_normal_vector, iftMatrix* p0_matrix
);

bool VerifyIntersection(
    iftImage* mip_img, Face* faces, iftPoint* intersection_points,
    iftMatrix* plane_normal_vector, iftMatrix* p0_matrix
);

iftImage *GetCurvilinearCut(
    iftImage* img, iftImage* envelop_img, Face* faces, int tilt_alpha,
    int spin_beta, int depth_cut
);

#endif