#include "ift.h"

iftImage *GetBlue2RedImage(iftImage *img)
{
    float H = pow(2, 16) - 1;
    int p = 0;
    float voxelValue = 0.0;
    float V = 0;
    iftSetCbCr(img, 0);
    iftVoxel u;

    // Color Variables to Aid Conversion
    iftColor RGBColor;
    iftColor YCbCrColor;

    u.z = 0;
    for (u.x = 0; u.x < img->xsize; u.x++)
    {
        for (u.y = 0; u.y < img->ysize; u.y++)
        {
            p = iftGetVoxelIndex(img,u);
            voxelValue = img->val[p];

            V = (6.0 - 2.0) * (voxelValue / H) + 1.0;

            RGBColor.val[0] = H * iftMax(
                0.0, (3.0 - fabs(V - 4.0) - fabs(V - 5.0)) / 2.0
            );
            RGBColor.val[1] = H * iftMax(
                0.0, (4.0 - fabs(V - 2.0) - fabs(V - 4.0)) / 2.0
            );
            RGBColor.val[2] = H * iftMax(
                0.0, (3.0 - fabs(V - 1.0) - fabs(V - 2.0)) / 2.0
            );
            YCbCrColor = iftRGBtoYCbCr(
                RGBColor, H
            );

            img->val[p] = (int) YCbCrColor.val[0];
            img->Cb[p] = (unsigned short) YCbCrColor.val[1];
            img->Cr[p] = (unsigned short) YCbCrColor.val[2];
        }
    }

    return img;
}