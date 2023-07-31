#include "ift.h"
#include "scene_data.h"
#include "dda3d.h"
#include "interpolation.h"

iftPoint *GetCutPoint(
    iftImage* img, iftImage* envelop_img, iftPoint* dda3d_points, int n_points,
    int depth_cut
) {
    int voxel_index;
    iftVoxel u;
    for (size_t idx = 0; idx < n_points; idx++)
    {
        // Adds 0.5 to round
        iftVoxel q_array[8] = {
            {
                .x = (int)floor(dda3d_points[idx].x),
                .y = (int)floor(dda3d_points[idx].y) + 1,
                .z = (int)floor(dda3d_points[idx].z)
            },
            {
                .x = (int)floor(dda3d_points[idx].x) + 1,
                .y = (int)floor(dda3d_points[idx].y) + 1,
                .z = (int)floor(dda3d_points[idx].z)
            },
            {
                .x = (int)floor(dda3d_points[idx].x),
                .y = (int)floor(dda3d_points[idx].y),
                .z = (int)floor(dda3d_points[idx].z)
            },
            {
                .x = (int)floor(dda3d_points[idx].x) + 1,
                .y = (int)floor(dda3d_points[idx].y),
                .z = (int)floor(dda3d_points[idx].z)
            },
            {
                .x = (int)floor(dda3d_points[idx].x),
                .y = (int)floor(dda3d_points[idx].y) + 1,
                .z = (int)floor(dda3d_points[idx].z) + 1
            },
            {
                .x = (int)floor(dda3d_points[idx].x) + 1,
                .y = (int)floor(dda3d_points[idx].y) + 1,
                .z = (int)floor(dda3d_points[idx].z) + 1
            },
            {
                .x = (int)floor(dda3d_points[idx].x),
                .y = (int)floor(dda3d_points[idx].y),
                .z = (int)floor(dda3d_points[idx].z) + 1
            },
            {
                .x = (int)floor(dda3d_points[idx].x) + 1,
                .y = (int)floor(dda3d_points[idx].y),
                .z = (int)floor(dda3d_points[idx].z) + 1
            }
        };

        for (size_t q_idx = 0; q_idx < 8; q_idx++) {
            u = q_array[q_idx];
            voxel_index = iftGetVoxelIndex(img, u);
            if (envelop_img->val[voxel_index] != -1) {
                if (
                    sqrt(envelop_img->val[voxel_index]) > ((float)depth_cut) - 0.5
                    && sqrt(envelop_img->val[voxel_index]) < ((float)depth_cut) + 0.5
                )
                {
                    return &dda3d_points[idx];
                }
            }
        }
    }

    return NULL;
}

iftPoint GetPoint(
    iftMatrix* p0_matrix, float lambda, iftMatrix* plane_normal_vector
)
{
    iftPoint point;
    point.x = p0_matrix->val[0] + lambda * plane_normal_vector->val[0];
    point.y = p0_matrix->val[1] + lambda * plane_normal_vector->val[1];
    point.z = p0_matrix->val[2] + lambda * plane_normal_vector->val[2];

    return point;
}

float CalculateLambda(
    Face face, iftMatrix* plane_normal_vector, iftMatrix* p0_matrix
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
                p0_matrix->val[0] * face.normal_vector->val[0]
                + p0_matrix->val[1] * face.normal_vector->val[1]
                + p0_matrix->val[2] * face.normal_vector->val[2]
            )
        )
        /
        (
            plane_normal_vector->val[0] * face.normal_vector->val[0]
            + plane_normal_vector->val[1] * face.normal_vector->val[1]
            + plane_normal_vector->val[2] * face.normal_vector->val[2]
        );

    return lambda;
}

bool VerifyIntersection(
    iftImage* img, Face* faces, iftPoint* intersection_points,
    iftMatrix* plane_normal_vector, iftMatrix* p0_matrix
)
{
    float lambda = 0;
    float lambda_min = IFT_INFINITY_FLT, lambda_max = IFT_INFINITY_FLT_NEG;
    iftPoint tmp_point;
    iftVoxel u;
    bool intersection = false;
    for(size_t idx=0; idx < 6; idx++) {
        lambda = CalculateLambda(faces[idx], plane_normal_vector, p0_matrix);

        /*If lambda greater than 0 computes image point.
        If valid point (Inside image domain), updates lambda maximum and minimum.
        */
        if (lambda > 0) {
            tmp_point = GetPoint(p0_matrix, lambda, plane_normal_vector);
            u.x = (int)(tmp_point.x + 0.5);
            u.y = (int)(tmp_point.y + 0.5);
            u.z = (int)(tmp_point.z + 0.5);
            if(iftValidVoxel(img, u))
            {
                if(!intersection) {
                    intersection = true;
                }
                if(lambda < lambda_min) {
                    lambda_min = lambda;
                }
                if(lambda > lambda_max) {
                    lambda_max = lambda;
                }
            }
        }
    }

    if(intersection) {
        intersection_points[0] = GetPoint(
            p0_matrix, lambda_min, plane_normal_vector
        );
        intersection_points[1] = GetPoint(
            p0_matrix, lambda_max, plane_normal_vector
        );
        return intersection;
    }
    else {
        return intersection;
    }
}

iftImage *GetCurvilinearCut(
        iftImage* img, iftImage* envelop_img, Face* faces, int tilt_alpha,
        int spin_beta, int depth_cut
) {
    // Initializes necessary variables
    float volume_diagonal = sqrt(
        pow(img->xsize, 2) + pow(img->ysize, 2) + pow(img->zsize, 2)
    );
    float x_center, y_center, z_center;
    x_center = (float)img->xsize / 2.0;
    y_center = (float)img->ysize / 2.0;
    z_center = (float)img->zsize / 2.0;

    // Initializes image to hold Curvilinear Cut
    iftImage* curvilinear_cut_img = iftCreateImage(
        (int)ceil(volume_diagonal),
        (int)ceil(volume_diagonal),
        1
    );
    printf(
        "[INFO IMAGE] : x_size: %d | y_size: %d | z_size: %d\n",
        img->xsize, img->ysize,
        img->zsize
    );
    printf(
        "[INFO CURVILINEAR_CUT_IMAGE] : x_size: %d | y_size: %d | z_size: %d\n",
        curvilinear_cut_img->xsize, curvilinear_cut_img->ysize,
        curvilinear_cut_img->zsize
    );

    // Creates Matrixes
    iftMatrix* p_matrix = iftCreateMatrix(1, 4);
    p_matrix->val[2] = -(volume_diagonal/2.0);
    p_matrix->val[3] = 1.0;
    iftMatrix* p0_matrix = iftCreateMatrix(1, 4);
    iftMatrix* x_tilt_matrix = iftRotationMatrix(0, -tilt_alpha);
    iftMatrix* y_spin_matrix = iftRotationMatrix(1, -spin_beta);
    iftVector translation_vector;
    translation_vector = (iftVector){
        .x = -(volume_diagonal/2.0), .y = -(volume_diagonal/2.0),
        .z = -(volume_diagonal/2.0)
    };
    iftMatrix* j_translation_matrix = iftTranslationMatrix(translation_vector);
    translation_vector = (iftVector){
        .x = x_center, .y = y_center,
        .z = z_center
    };
    iftMatrix* j_c_translation_matrix = iftTranslationMatrix(translation_vector);

    // Iterates over the plane casting rays
    // Tmp matrix to allocate calculations
    // Fazer transformação do vetor normal do plano (phi_r) => Duas Rotações

    /* First, let us define the matrix to project the visualization plane
    (for each voxel)
    */
    iftMatrix* pointwise_plane_ops = iftCreateMatrix(1, 4);
    pointwise_plane_ops = iftMultMatrices(y_spin_matrix, j_translation_matrix);
    pointwise_plane_ops = iftMultMatrices(x_tilt_matrix, pointwise_plane_ops);
    pointwise_plane_ops = iftMultMatrices(j_c_translation_matrix, pointwise_plane_ops);
    // Then, transform the normal vector of our visualization plane.
    iftMatrix* plane_normal_vector = iftCreateMatrix(1, 4);
    plane_normal_vector->val[2] = 1.0;
    plane_normal_vector = iftMultMatrices(y_spin_matrix, plane_normal_vector);
    plane_normal_vector = iftMultMatrices(x_tilt_matrix, plane_normal_vector);

    iftVoxel u = (iftVoxel){.z=0};
    iftPoint intersection_points[2];
    iftPoint* dda3d_points = NULL;
    int n_points;
    iftPoint *cut_point;
    for (u.x = 0; u.x <= curvilinear_cut_img->xsize; u.x++) {
        for (u.y = 0; u.y <= curvilinear_cut_img->ysize; u.y++) {
            // Apply Transformations on visualization plane
            p_matrix->val[0] = u.x;
            p_matrix->val[1] = u.y;
            p0_matrix = iftMultMatrices(pointwise_plane_ops, p_matrix);
            // Verify Intersection (If it Happens runs FindIsoSurfacePoint)
            if (VerifyIntersection(
                img, faces, intersection_points,
                plane_normal_vector, p0_matrix
            ))
            {
                // Projects DDA3D (Comment about the decision to use pointer)
                dda3d_points = PerformDDA3D(img, intersection_points, &n_points);
                // Gets cut intensity across DDA3D path
                cut_point = GetCutPoint(
                    img, envelop_img, dda3d_points, n_points, depth_cut
                );
                if (cut_point != NULL) {
                    TrilinearInterpolation(
                        img, curvilinear_cut_img, *cut_point, u
                    );
                }

                free(dda3d_points);
            }
            // Eeach iteration allocates a matrix, so we need to dealocate it.
            iftDestroyMatrix(&p0_matrix);
        }
    }

    // Destroy Allocated Resources
    iftDestroyMatrix(&p_matrix);
    iftDestroyMatrix(&x_tilt_matrix);
    iftDestroyMatrix(&y_spin_matrix);
    iftDestroyMatrix(&j_translation_matrix);
    iftDestroyMatrix(&j_c_translation_matrix);
    iftDestroyMatrix(&pointwise_plane_ops);
    iftDestroyMatrix(&plane_normal_vector);

    return curvilinear_cut_img;
}