#include "interpolation.h"
#include "math.h"

void TrilinearInterpolation(
    iftImage* src_img, iftImage* dst_img, iftPoint p_, iftVoxel u
)
{
    // Creates voxels for interpolation
    iftVoxel q_array[8] = {
        {.x = (int)floor(p_.x), .y = (int)floor(p_.y) + 1, .z = (int)floor(p_.z)},
        {.x = (int)floor(p_.x) + 1, .y = (int)floor(p_.y) + 1, .z = (int)floor(p_.z)},
        {.x = (int)floor(p_.x), .y = (int)floor(p_.y), .z = (int)floor(p_.z)},
        {.x = (int)floor(p_.x) + 1, .y = (int)floor(p_.y), .z = (int)floor(p_.z)},
        {.x = (int)floor(p_.x), .y = (int)floor(p_.y) + 1, .z = (int)floor(p_.z) + 1},
        {.x = (int)floor(p_.x) + 1, .y = (int)floor(p_.y) + 1, .z = (int)floor(p_.z) + 1},
        {.x = (int)floor(p_.x), .y = (int)floor(p_.y), .z = (int)floor(p_.z) + 1},
        {.x = (int)floor(p_.x) + 1, .y = (int)floor(p_.y), .z = (int)floor(p_.z) + 1}
    };

    iftPoint q_13 = {.x = floor(p_.x), .y = p_.y, .z = floor(p_.z)};
    iftPoint q_24 = {.x = floor(p_.x) + 1, .y = p_.y, .z = floor(p_.z)};
    iftPoint q_68 = {.x = floor(p_.x) + 1, .y = p_.y, .z = floor(p_.z) + 1};
    iftPoint q_57 = {.x = floor(p_.x), .y = p_.y, .z = floor(p_.z) + 1};
    iftPoint q_1357 = {.x = floor(p_.x), .y = p_.y , .z = p_.z};
    iftPoint q_2468 = {.x = floor(p_.x) + 1, .y = p_.y, .z = p_.z};

    // Comment about the problem (int)(x) for negative x
    iftVoxel v = {
        .x = round(p_.x),
        .y = round(p_.y),
        .z = round(p_.z),
    };

    int u_idx = 0;

    int iq_24, iq_68, iq_13, iq_57;
    int iq_2468, iq_1357;
    int intensity;

    if (iftValidVoxel(src_img, v)) {
        u_idx = iftGetVoxelIndex(dst_img, u);

        // Verify if adjacent voxels are outside image domain (y)
        if (q_array[1].y >= src_img->ysize) {
            iq_24 = (
                (p_.y - (float)q_array[3].y)
                * src_img->val[iftGetVoxelIndex(src_img, q_array[1])]
            );
        }
        else {
            iq_24 = (
                (p_.y - (float)q_array[3].y)
                * src_img->val[iftGetVoxelIndex(src_img, q_array[1])]
                + ((float)q_array[1].y - p_.y)
                * src_img->val[iftGetVoxelIndex(src_img, q_array[3])]
            );
        }
        if (q_array[5].y >= src_img->ysize) {
            iq_68 = (
                (p_.y - (float)q_array[7].y)
                * src_img->val[iftGetVoxelIndex(src_img, q_array[5])]
            );
        }
        else {
            iq_68 = (
                (p_.y - (float)q_array[7].y)
                * src_img->val[iftGetVoxelIndex(src_img, q_array[5])]
                + ((float)q_array[5].y - p_.y)
                * src_img->val[iftGetVoxelIndex(src_img, q_array[7])]
            );
        }
        if (q_array[0].y >= src_img->ysize) {
            iq_13 = (
                (p_.y - (float)q_array[2].y)
                * src_img->val[iftGetVoxelIndex(src_img, q_array[0])]
            );
        }
        else {
            iq_13 = (
                (p_.y - (float)q_array[2].y)
                * src_img->val[iftGetVoxelIndex(src_img, q_array[0])]
                + ((float)q_array[0].y - p_.y)
                * src_img->val[iftGetVoxelIndex(src_img, q_array[2])]
            );
        }
        if (q_array[4].y >= src_img->ysize) {
            iq_57 = (
                (p_.y - (float)q_array[6].y)
                * src_img->val[iftGetVoxelIndex(src_img, q_array[4])]
            );
        }
        else {
            iq_57 = (
                (p_.y - (float)q_array[6].y)
                * src_img->val[iftGetVoxelIndex(src_img, q_array[4])]
                + ((float)q_array[4].y - p_.y)
                * src_img->val[iftGetVoxelIndex(src_img, q_array[6])]
            );
        }

        // Verify if adjacent voxels are outside image domain (z)
        if (q_68.z >= src_img->zsize) {
            iq_2468 = (
                (p_.z - q_24.z) * iq_68
            );
        }
        else {
            iq_2468 = (
                (p_.z - q_24.z) * iq_68
                + (q_68.z - p_.z) * iq_24
            );
        }
        if (q_57.z >= src_img->zsize) {
            iq_1357 = (
                (p_.z - q_13.z) * iq_57
            );
        }
        else {
            iq_1357 = (
                (p_.z - q_13.z) * iq_57
                + (q_57.z - p_.z) * iq_13
            );
        }

        // Verify if adjacent voxels are outside image domain (x)
        if (q_2468.x >= src_img->xsize) {
            intensity = (int) (
                (p_.x - q_1357.x) * iq_2468
            ) + 0.5;
        }
        else {
            intensity = (int) (
                (p_.x - q_1357.x) * iq_2468
                + (q_2468.x - p_.x) * iq_1357
            ) + 0.5;
        }

        dst_img->val[u_idx] = intensity;
    }
}