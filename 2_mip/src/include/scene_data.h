#ifndef _FACES_
#define _FACES_

#include "ift.h"

typedef struct face {
    // Vector and Point
    iftMatrix* face_center;
    iftMatrix* normal_vector;
    int edge_p1_idx[4];
    int edge_pn_idx[4];
} Face;

typedef struct vertex {
    iftMatrix* vertex;
} Vertex;

void InitEdges(
    float x_p1, float y_p1, float z_p1, float x_pn, float y_pn, float z_pn
);

void InitFaceArray(Face *face, float x, float y, float z, float h);
void InitNormalVectorMatrix(Face *face, float x, float y, float z, float h);
void InitSceneVertex(Vertex *vertex, float x, float y, float z, float h);
void SetFaceEdges(
    Face *face, int vertex_1, int vertex_2, int vertex_3, int vertex_4
);

Face *GetSceneFaces(iftImage *img);
Vertex* GetSceneVertexes(iftImage *img);

void DestroyFaces(Face* scene_faces);
void DestroySceneVertex(Vertex* scene_vertexes);

#endif