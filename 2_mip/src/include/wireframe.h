#ifndef _WIREFRAME_
#define _WIREFRAME_

#include "ift.h"
#include "scene_data.h"

bool VerifyFaceVisibility(iftMatrix* face_normal_vector_transformed);
bool VerifyEdge(
    Face face, Vertex* vertexes, iftPoint* visible_vertexes, int idx_edge,
    iftMatrix* pointwise_ops_point
);

void WireFrame(
    iftImage* img, iftImage* mip_img, Face* faces, Vertex* vertexes,
    int tilt_alpha, int spin_beta
);
void DrawWireframe(
    iftImage* mip_img, iftPoint* dda2d_points, int n_points, float radius
);

#endif