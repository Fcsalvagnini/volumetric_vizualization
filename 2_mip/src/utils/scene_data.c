#include "ift.h"
#include "scene_data.h"

void InitFaceArray(Face *face, float x, float y, float z, float h) {
    face->face_center = iftCreateMatrix(1, 4);
    face->face_center->val[0] = x;
    face->face_center->val[1] = y;
    face->face_center->val[2] = z;
    face->face_center->val[3] = h;
}

void InitNormalVectorArray(Face *face, float x, float y, float z, float h) {
    face->normal_vector = iftCreateMatrix(1, 4);
    face->normal_vector->val[0] = x;
    face->normal_vector->val[1] = y;
    face->normal_vector->val[2] = z;
    face->normal_vector->val[3] = h;
}

void InitSceneVertex(Vertex *vertex, float x, float y, float z, float h)
{
    vertex->vertex = iftCreateMatrix(1, 4);
    vertex->vertex->val[0] = x;
    vertex->vertex->val[1] = y;
    vertex->vertex->val[2] = z;
    vertex->vertex->val[3] = h;
}

Vertex* GetSceneVertexes(iftImage *img) {
    int nx, ny, nz;
    nx = img->xsize;
    ny = img->ysize;
    nz = img->zsize;

    Vertex* scene_vertexes = malloc(8 * sizeof(Vertex));

    InitSceneVertex(
        &scene_vertexes[0],
        0, 0, 0, 1
    );
    InitSceneVertex(
        &scene_vertexes[1],
        nx - 1, 0, 0, 1
    );
    InitSceneVertex(
        &scene_vertexes[2],
        0, ny -1, 0, 1
    );
    InitSceneVertex(
        &scene_vertexes[3],
        nx - 1, ny - 1, 0, 1
    );
    InitSceneVertex(
        &scene_vertexes[4],
        0, 0, nz - 1, 1
    );
    InitSceneVertex(
        &scene_vertexes[5],
        nx - 1, 0, nz - 1, 1
    );
    InitSceneVertex(
        &scene_vertexes[6],
        0, ny - 1, nz - 1, 1
    );
    InitSceneVertex(
        &scene_vertexes[7],
        nx - 1, ny - 1, nz - 1, 1
    );

    return scene_vertexes;
}

void SetFaceEdges(
    Face *face, int vertex_1, int vertex_2, int vertex_3, int vertex_4
)
{
    face->edge_p1_idx[0] = vertex_1;
    face->edge_pn_idx[0] = vertex_2;
    face->edge_p1_idx[1] = vertex_2;
    face->edge_pn_idx[1] = vertex_3;
    face->edge_p1_idx[2] = vertex_3;
    face->edge_pn_idx[2] = vertex_4;
    face->edge_p1_idx[3] = vertex_4;
    face->edge_pn_idx[3] = vertex_1;
}

Face* GetSceneFaces(iftImage *img) {
    int nx, ny, nz;
    nx = img->xsize;
    ny = img->ysize;
    nz = img->zsize;

    Face* scene_faces = malloc(6 * sizeof(Face));
    // Face 0
    InitFaceArray(
        &scene_faces[0],
        nx - 1, ny/2.0, nz/2.0, 1
    );
    InitNormalVectorArray(
        &scene_faces[0],
        1, 0, 0, 0
    );
    SetFaceEdges(
        &scene_faces[0],
        1, 3, 7, 5
    );
    // Face 1
    InitFaceArray(
        &scene_faces[1],
        0, ny/2.0, nz/2.0, 1
    );
    InitNormalVectorArray(
        &scene_faces[1],
        -1, 0, 0, 0
    );
    SetFaceEdges(
        &scene_faces[1],
        0, 2, 6, 4
    );
    // Face 2
    InitFaceArray(
        &scene_faces[2],
        nx/2.0, ny - 1, nz/2.0, 1
    );
    InitNormalVectorArray(
        &scene_faces[2],
        0, 1, 0, 0
    );
    SetFaceEdges(
        &scene_faces[2],
        2, 3, 7, 6
    );
    // Face 3
    InitFaceArray(
        &scene_faces[3],
        nx/2.0, 0, nz/2.0, 1
    );
    InitNormalVectorArray(
        &scene_faces[3],
        0, -1, 0, 0
    );
    SetFaceEdges(
        &scene_faces[3],
        0, 1, 5, 4
    );
    // Face 4
    InitFaceArray(
        &scene_faces[4],
        nx/2.0, ny/2.0, nz - 1, 1
    );
    InitNormalVectorArray(
        &scene_faces[4],
        0, 0, 1, 0
    );
    SetFaceEdges(
        &scene_faces[4],
        4, 5, 7, 6
    );
    // Face 5
    InitFaceArray(
        &scene_faces[5],
        nx/2.0, ny/2.0, 0, 1
    );
    InitNormalVectorArray(
        &scene_faces[5],
        0, 0, -1, 0
    );
    SetFaceEdges(
        &scene_faces[5],
        0, 1, 3, 2
    );

    return scene_faces;
}

void DestroyFaces(Face* scene_faces) {
    for (size_t idx = 0; idx < 6; idx++) {
        iftDestroyMatrix(&scene_faces[idx].face_center);
        iftDestroyMatrix(&scene_faces[idx].normal_vector);
    }
    free(scene_faces);
}

void DestroySceneVertex(Vertex* scene_vertexes) {
    for (size_t idx = 0; idx < 8; idx++) {
        iftDestroyMatrix(&scene_vertexes[idx].vertex);
    }
    free(scene_vertexes);
}