#include "ift.h"
#include "reslice.h"
#include "interpolation.h"
#include "reformating.h"

float GetAlpha(iftVector cut_plane_normal_vector) {
    float alpha = atan2(cut_plane_normal_vector.y, cut_plane_normal_vector.z);
    alpha *= (180 / IFT_PI);

    return alpha;
}

iftMatrix* GetRotationOps(iftVector cut_plane_normal_vector, float alpha) {
    iftMatrix* rx = iftRotationMatrix(IFT_AXIS_X, alpha);
    iftMatrix* rotation_ops = NULL;
    iftMatrix* cut_plane_normal_vector_matrix = iftCreateMatrix(1, 4);
    cut_plane_normal_vector_matrix->val[0] = cut_plane_normal_vector.x;
    cut_plane_normal_vector_matrix->val[1] = cut_plane_normal_vector.y;
    cut_plane_normal_vector_matrix->val[2] = cut_plane_normal_vector.z;

    cut_plane_normal_vector_matrix = iftMultMatrices(
        rx, cut_plane_normal_vector_matrix
    );

    float beta = atan2(
        cut_plane_normal_vector.x, cut_plane_normal_vector_matrix->val[2]
    );
    beta *= (180 / IFT_PI);

    iftDestroyMatrix(&rx);
    rx = iftRotationMatrix(IFT_AXIS_X, -alpha);
    iftMatrix* ry = iftRotationMatrix(IFT_AXIS_Y, beta);
    rotation_ops = iftMultMatrices(rx, ry);

    if (cut_plane_normal_vector.z < 0) {
        if (alpha != 0)
        {
            alpha -= 180;
        }
        if (beta != 0)
        {
            beta -= 180;
        }

        rx = iftRotationMatrix(IFT_AXIS_X, -alpha);
        ry = iftRotationMatrix(IFT_AXIS_Y, beta);
        iftDestroyMatrix(&rotation_ops);
        rotation_ops = iftMultMatrices(rx, ry);
    }

    if (cut_plane_normal_vector.z == 0) {
        if (cut_plane_normal_vector.x == 0 && cut_plane_normal_vector.y != 0)
        {
            beta = 0;
            alpha = 90 * iftSign(cut_plane_normal_vector.y);

            rx = iftRotationMatrix(IFT_AXIS_X, -alpha);
            ry = iftRotationMatrix(IFT_AXIS_Y, beta);
            iftDestroyMatrix(&rotation_ops);
            rotation_ops = iftMultMatrices(rx, ry);
        }
        else if (cut_plane_normal_vector.x != 0 && cut_plane_normal_vector.y == 0)
        {
            beta = 90 * iftSign(cut_plane_normal_vector.x);
            alpha = 0;

            rx = iftRotationMatrix(IFT_AXIS_X, -alpha);
            ry = iftRotationMatrix(IFT_AXIS_Y, beta);
            iftDestroyMatrix(&rotation_ops);
            rotation_ops = iftMultMatrices(rx, ry);
        }
        else if (cut_plane_normal_vector.x != 0 && cut_plane_normal_vector.y != 0)
        {
            float gamma = -iftSign(cut_plane_normal_vector.y)
                            * acos(cut_plane_normal_vector.x);
            gamma *= (180 / IFT_PI);

            iftMatrix* rz = iftRotationMatrix(IFT_AXIS_Z, -gamma);
            ry = iftRotationMatrix(IFT_AXIS_Y, 90);
            iftDestroyMatrix(&rotation_ops);
            rotation_ops = iftMultMatrices(rz, ry);
            iftDestroyMatrix(&rz);
        }
    }

    iftDestroyMatrix(&cut_plane_normal_vector_matrix);
    iftDestroyMatrix(&rx);
    iftDestroyMatrix(&ry);

    return rotation_ops;
}

iftImage* ResliceScene(iftImage* img, iftPoint p0, iftPoint pn, int n) {
    // pow is slower than multiplication (x**2)
    float D = sqrt(pow(img->xsize, 2) + pow(img->ysize, 2) + pow(img->zsize, 2));
    iftImage *resliced_img = iftCreateImage(
        (int)ceil(D),
        (int)ceil(D),
        n
    );
    printf(
        "[INFO RESLICED IMAGE] : x_size: %d | y_size: %d | z_size: %d\n",
        resliced_img->xsize, resliced_img->ysize, resliced_img->zsize
    );

    iftVector cut_plane_normal_vector = {.x = 0, .y = 0, .z = 0};
    float vector_norm =
        sqrt(
            pow((pn.x - p0.x), 2) + pow((pn.y - p0.y), 2) + pow((pn.z - p0.z), 2)
        );
    cut_plane_normal_vector.x =
        (pn.x - p0.x) / vector_norm;
    cut_plane_normal_vector.y =
        (pn.y - p0.y) / vector_norm;
    cut_plane_normal_vector.z =
        (pn.z - p0.z) / vector_norm;
    float lambda = vector_norm / n;
    resliced_img->dz = lambda * img->dz;

    // Creates graphical context
    float alpha = GetAlpha(cut_plane_normal_vector);
    iftMatrix* rotation_ops = GetRotationOps(cut_plane_normal_vector, alpha);
    iftVector translation_vector;
    translation_vector = (iftVector){
        .x = -(D/2.0), .y = -(D/2.0),
        .z = (D/2.0)
    };
    iftMatrix* j_D_translation_matrix = iftTranslationMatrix(translation_vector);
    translation_vector = (iftVector){
        .x = p0.x, .y = p0.y,
        .z = p0.z
    };
    iftMatrix* j_p0_translation_matrix = iftTranslationMatrix(translation_vector);
    iftMatrix* pointwise_ops = iftMultMatrices(rotation_ops, j_D_translation_matrix);
    pointwise_ops = iftMultMatrices(j_p0_translation_matrix, pointwise_ops);

    // Creates Matrix for Transformations Reference
    iftMatrix* transformed_x = iftCreateMatrix(D, D);
    iftMatrix* transformed_y = iftCreateMatrix(D, D);
    iftMatrix* transformed_z = iftCreateMatrix(D, D);

    // Pre-computes x, y, and z
    iftVoxel u = {.x = 0, .y = 0, .z = 0};
    iftMatrix* plane_points = iftCreateMatrix(1, 4);
    iftMatrix* p_origin_points = iftCreateMatrix(1, 4);
    plane_points->val[2] = -(D / 2);
    plane_points->val[3] = 1;
    for (u.x = 0; u.x < resliced_img->xsize; u.x++) {
        for (u.y = 0; u.y < resliced_img->ysize; u.y++) {
            plane_points->val[0] = u.x;
            plane_points->val[1] = u.y;
            p_origin_points = iftMultMatrices(pointwise_ops, plane_points);
            iftMatrixElem(transformed_x, u.x, u.y) = p_origin_points->val[0];
            iftMatrixElem(transformed_y, u.x, u.y) = p_origin_points->val[1];
            iftMatrixElem(transformed_z, u.x, u.y) = p_origin_points->val[2];
            iftDestroyMatrix(&p_origin_points);
        }
    }

    iftPoint p_ = {.x = 0.0, .y = 0.0, .z = 0.0};
    for (u.z = 0; u.z < resliced_img->zsize; u.z++) {
        for (u.x = 0; u.x < resliced_img->xsize; u.x++) {
            for (u.y = 0; u.y < resliced_img->ysize; u.y++) {
                p_ = (iftPoint) {
                    .x = iftMatrixElem(transformed_x, u.x, u.y)
                        + u.z * lambda  * cut_plane_normal_vector.x,
                    .y = iftMatrixElem(transformed_y, u.x, u.y)
                        + u.z * lambda  * cut_plane_normal_vector.y,
                    .z = iftMatrixElem(transformed_z, u.x, u.y)
                        + u.z * lambda  * cut_plane_normal_vector.z,
                };
                TrilinearInterpolation(img, resliced_img, p_, u);
            }
        }
    }

    iftImage* reformated_img = ReformatImage(resliced_img);

    iftDestroyImage(&resliced_img);
    iftDestroyMatrix(&rotation_ops);
    iftDestroyMatrix(&j_D_translation_matrix);
    iftDestroyMatrix(&j_p0_translation_matrix);
    iftDestroyMatrix(&pointwise_ops);
    iftDestroyMatrix(&transformed_x);
    iftDestroyMatrix(&transformed_y);
    iftDestroyMatrix(&transformed_z);

    return reformated_img;
}