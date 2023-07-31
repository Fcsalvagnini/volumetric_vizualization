#include "ift.h"
#include "global.h"

void ApplyLinearStretching(iftImage *img, float windowSize, float level, int iMin, int iMax)
{
    int k1 = 0;
    int k2 = MAX_INT;

    int IWindowSize = windowSize * (iMax - iMin);
    int ILevel = level * iMax;
    int l1 = ILevel - (IWindowSize / 2);
    int l2 = ILevel + (IWindowSize / 2);

    iftVoxel u;
    int p = 0;
    int voxelValue = 0;

    u.z = 0;
    for (u.x = 0; u.x < img->xsize; u.x++)
    {
        for (u.y = 0; u.y < img->ysize; u.y++)
        {
            p = iftGetVoxelIndex(img,u);
            voxelValue = img->val[p];
            if (voxelValue < l1)
            {
                img->val[p] = k1;
            }
            else if (voxelValue >= l1 && voxelValue < l2)
            {
                img->val[p] = ( (k2 - k1) / (l2 - l1) ) * (voxelValue - l1) + k1;
            }
            else
            {
                img->val[p] = k2;
            }
        }
    }
}