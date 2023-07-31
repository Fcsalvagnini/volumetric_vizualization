#include "reformating.h"

iftImage* ReformatImage(iftImage* resliced_img)
{
    printf("[INFO RESLICED IMAGE] dsize: X -> %f | Y -> %f | Z -> %f\n",
    resliced_img->dx, resliced_img->dy, resliced_img->dz);

    float new_dx, new_dy, scale_x, scale_y;
    int new_xsize, new_ysize;
    new_dx = new_dy = resliced_img->dz;
    scale_x = resliced_img->dx / new_dx;
    scale_y = resliced_img->dy / new_dy;
    new_xsize = (int)(scale_x * resliced_img->xsize + 0.5);
    new_ysize = (int)(scale_y * resliced_img->ysize + 0.5);

    iftImage* i_ = iftCreateImage(
        new_xsize, resliced_img->ysize, resliced_img->zsize
    );

    // Interpolate allong X
    iftVoxel v, qk, qk_p_1;
    iftPoint p1, pk_p_1;
    int intensity;
    for (v.z = 0; v.z < i_->zsize; v.z++) {
        for (v.y = 0; v.y < i_->ysize; v.y++) {
            p1 = (iftPoint){.x = 0, .y = v.y, .z = v.z};
            qk = iftPointToVoxel(p1);
            qk_p_1 = (iftVoxel){.x = 1, .y = v.y, .z = v.z};
            for (v.x = 0; v.x < i_->xsize; v.x++) {
                pk_p_1 = (iftPoint){
                    .x = p1.x + (int)(v.x) * new_dx,
                    .y = p1.y,
                    .z = p1.z
                };
                qk.x = (int)(floor(pk_p_1.x));
                qk_p_1.x = (int)(floor(pk_p_1.x) + 1);
                intensity = (int) (
                    (pk_p_1.x - qk.x) * resliced_img->val[iftGetVoxelIndex(resliced_img, qk_p_1)]
                    + (qk_p_1.x - pk_p_1.x) * resliced_img->val[iftGetVoxelIndex(resliced_img, qk)]
                ) + 0.5;

                i_->val[iftGetVoxelIndex(i_, v)] = intensity;
            }
        }
    }

    iftImage* reformated_img = iftCreateImage(
        new_xsize, new_ysize, resliced_img->zsize
    );
    reformated_img->dx = new_dx;
    reformated_img->dy = new_dy;
    reformated_img->dz = resliced_img->dz;

    printf("[INFO REFORMATED IMAGE] dsize: X -> %f | Y -> %f | Z -> %f\n",
    reformated_img->dx, reformated_img->dy, reformated_img->dz);
    printf("[INFO REFORMATED IMAGE] size: X -> %d | Y -> %d | Z -> %d\n",
    reformated_img->xsize, reformated_img->ysize, reformated_img->zsize);

    // Interpolate allong Y
    for (v.z = 0; v.z < reformated_img->zsize; v.z++) {
        for (v.x = 0; v.x < reformated_img->xsize; v.x++) {
            p1 = (iftPoint){.x = v.x, .y = 0, .z = v.z};
            qk = iftPointToVoxel(p1);
            qk_p_1 = (iftVoxel){.x = v.x, .y = 1, .z = v.z};
            for (v.y = 0; v.y < reformated_img->ysize; v.y++) {
                pk_p_1 = (iftPoint){
                    .x = p1.x,
                    .y = p1.y + (int)(v.y) * new_dy,
                    .z = p1.z
                };
                qk.y = (int)(floor(pk_p_1.y));
                qk_p_1.y = (int)(floor(pk_p_1.y) + 1);
                intensity = (int) (
                    (pk_p_1.y - qk.y) * i_->val[iftGetVoxelIndex(i_, qk_p_1)]
                    + (qk_p_1.y - pk_p_1.y) * i_->val[iftGetVoxelIndex(i_, qk)]
                ) + 0.5;

                reformated_img->val[iftGetVoxelIndex(reformated_img, v)] = intensity;
            }
        }
    }

    iftDestroyImage(&i_);

    return reformated_img;
}