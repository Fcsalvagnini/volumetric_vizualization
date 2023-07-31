#include "ift.h"
#include "scene_data.h"
#include "dda2d.h"

bool VerifyFaceVisibility(iftMatrix* face_normal_vector_transformed) {
    if (-face_normal_vector_transformed->val[2] > 0) {
        return true;
    }
    else {
        return false;
    }
}

void DrawWireframe(
        iftImage* mip_img, iftPoint* dda2d_points, int n_points, float radius
) {
    int voxel_index;
    iftAdjRel *A = iftCircular(radius);
    iftVoxel u;
    iftVoxel v;
    u.z = 0;
    for (size_t idx = 0; idx < n_points; idx++) {
        // Adds 0.5 to round
        u.x = (int)(dda2d_points[idx].x + 0.5);
        u.y = (int)(dda2d_points[idx].y + 0.5);
        voxel_index = iftGetVoxelIndex(mip_img, u);
        mip_img->val[voxel_index] = pow(2, 16) - 1;
        for (size_t i=1; i < A->n; i++) {
            v = iftGetAdjacentVoxel(A, u, i);
            if (iftValidVoxel(mip_img, v)) {
                voxel_index = iftGetVoxelIndex(mip_img, v);
                mip_img->val[voxel_index] = pow(2, 16) - 1;
            }
        }
    }
    iftDestroyAdjRel(&A);
}

bool VerifyEdge(
    Face face, Vertex* vertexes, iftPoint* visible_vertexes, int idx_edge,
    iftMatrix* pointwise_ops_point
)
{
    int index_vertex_p1 = face.edge_p1_idx[idx_edge];
    int index_vertex_pn = face.edge_pn_idx[idx_edge];
    iftPoint vertex_p1 = {
        .x = vertexes[index_vertex_p1].vertex->val[0],
        .y = vertexes[index_vertex_p1].vertex->val[1],
        .z = vertexes[index_vertex_p1].vertex->val[2]
    };
    iftPoint vertex_pn = {
        .x = vertexes[index_vertex_pn].vertex->val[0],
        .y = vertexes[index_vertex_pn].vertex->val[1],
        .z = vertexes[index_vertex_pn].vertex->val[2]
    };
    float p1_inner_product =
        (vertex_p1.x - face.face_center->val[0]) * face.normal_vector->val[0]
        + (vertex_p1.y - face.face_center->val[1]) * face.normal_vector->val[1]
        + (vertex_p1.z - face.face_center->val[2]) * face.normal_vector->val[2];
    float pn_inner_product =
        (vertex_pn.x - face.face_center->val[0]) * face.normal_vector->val[0]
        + (vertex_pn.y - face.face_center->val[1]) * face.normal_vector->val[1]
        + (vertex_pn.z - face.face_center->val[2]) * face.normal_vector->val[2];

    if (p1_inner_product == 0 && pn_inner_product == 0) {
        iftMatrix* vertex_p1_transformed_matrix = iftMultMatrices(
            pointwise_ops_point, vertexes[index_vertex_p1].vertex
        );
        iftMatrix* vertex_pn_transformed_matrix = iftMultMatrices(
            pointwise_ops_point, vertexes[index_vertex_pn].vertex
        );

        vertex_p1 = (iftPoint){
            .x = vertex_p1_transformed_matrix->val[0],
            .y = vertex_p1_transformed_matrix->val[1],
            .z = vertex_p1_transformed_matrix->val[2]
        };
        vertex_pn = (iftPoint){
            .x = vertex_pn_transformed_matrix->val[0],
            .y = vertex_pn_transformed_matrix->val[1],
            .z = vertex_pn_transformed_matrix->val[2]
        };
        visible_vertexes[0] = vertex_p1;
        visible_vertexes[1] = vertex_pn;

        iftDestroyMatrix(&vertex_p1_transformed_matrix);
        iftDestroyMatrix(&vertex_pn_transformed_matrix);
        return true;
    }
    else {
        return false;
    }
}

void WireFrame(
    iftImage* img, iftImage* mip_img, Face* faces, Vertex* vertexes,
    int tilt_alpha, int spin_beta
) {
    // Initializes necessary variables
    float volume_diagonal = sqrt(
        pow(img->xsize, 2) + pow(img->ysize, 2) + pow(img->zsize, 2)
    );
    float x_center, y_center, z_center;
    x_center = (float)img->xsize / 2.0;
    y_center = (float)img->ysize / 2.0;
    z_center = (float)img->zsize / 2.0;

    // Creates Matrixes
    iftMatrix* x_tilt_matrix = iftRotationMatrix(0, tilt_alpha);
    iftMatrix* y_spin_matrix = iftRotationMatrix(1, spin_beta);
    iftVector translation_vector;
    translation_vector = (iftVector){
        .x = (volume_diagonal/2.0), .y = (volume_diagonal/2.0),
        .z = (volume_diagonal/2.0)
    };
    iftMatrix* j_translation_matrix = iftTranslationMatrix(translation_vector);
    translation_vector = (iftVector){
        .x = -x_center, .y = -y_center,
        .z = -z_center
    };
    iftMatrix* j_c_translation_matrix = iftTranslationMatrix(translation_vector);

    iftMatrix* pointwise_ops_point = iftCreateMatrix(1, 4);
    pointwise_ops_point = iftMultMatrices(x_tilt_matrix, j_c_translation_matrix);
    pointwise_ops_point = iftMultMatrices(y_spin_matrix, pointwise_ops_point);
    pointwise_ops_point = iftMultMatrices(j_translation_matrix, pointwise_ops_point);
    iftMatrix* pointwise_ops_vector = iftCreateMatrix(1, 4);
    pointwise_ops_vector = iftMultMatrices(y_spin_matrix, x_tilt_matrix);

    iftMatrix* face_transformed = iftCreateMatrix(1, 4);
    iftPoint visible_vertexes[2];
    iftPoint* dda2d_points = NULL;
    int n_points;
    for (size_t idx_face = 0; idx_face < 6; idx_face++) {
        face_transformed = iftMultMatrices(
            pointwise_ops_vector, faces[idx_face].normal_vector
        );
        if (VerifyFaceVisibility(face_transformed)) {
            for (size_t idx_edge = 0; idx_edge < 4; idx_edge++) {
                if (
                    VerifyEdge(
                        faces[idx_face], vertexes, visible_vertexes, idx_edge,
                        pointwise_ops_point
                    )
                )
                {
                    dda2d_points = PerformDDA2D(mip_img, visible_vertexes, &n_points);
                    DrawWireframe(mip_img, dda2d_points, n_points, 0.5);
                }
            }
        }
        // Eeach iteration allocates a matrix, so we need to dealocate it.
        iftDestroyMatrix(&face_transformed);
    }


    // Destroy Allocated Resources
    iftDestroyMatrix(&x_tilt_matrix);
    iftDestroyMatrix(&y_spin_matrix);
    iftDestroyMatrix(&j_translation_matrix);
    iftDestroyMatrix(&j_c_translation_matrix);
    iftDestroyMatrix(&pointwise_ops_point);
    iftDestroyMatrix(&pointwise_ops_vector);
    free(dda2d_points);
}