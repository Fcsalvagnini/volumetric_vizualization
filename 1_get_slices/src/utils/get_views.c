#include "ift.h"

// Gets Axial Slice.
iftImage *GetAxialSlice(iftImage *img, int z, bool perspective)
{
    int q = 0;
    int p = 0;
    iftVoxel u;
    iftImage *slc = iftCreateImage(img->xsize, img->ysize, 1);

    if (perspective)
    {
        printf("[INFO] Neuroradiologist perspective!\n");
        u.z = (img->zsize - 1) - z;
        for (u.y = 0; u.y < img->ysize; u.y++)
        {
            for (u.x = img->xsize - 1; u.x >= 0; u.x--)
            {
                p = iftGetVoxelIndex(img,u);
                slc->val[q] = img->val[p];
                q++;
            }
        }

        return slc;
    }
    else
    {
        printf("[INFO] Radiologist perspective!\n");
        u.z = z;
        for (u.y = 0; u.y < img->ysize; u.y++)
        {
            for (u.x = 0; u.x < img->xsize; u.x++)
            {
                p = iftGetVoxelIndex(img,u);
                slc->val[q] = img->val[p];
                q++;
            }
        }

        return slc;
    }
}

// Gets Coronal Slice.
iftImage *GetCoronalSlice(iftImage *img, int y, bool perspective)
{
    int q = 0;
    int p = 0;
    iftVoxel u;
    iftImage *slc = iftCreateImage(img->xsize, img->zsize, 1);

    if (perspective)
    {
        printf("[INFO] Neuroradiologist perspective!\n");
        u.y = (img->ysize - 1) - y;
        for (u.z = img->zsize - 1; u.z >= 0; u.z--)
        {
            for (u.x = img->xsize - 1; u.x >= 0; u.x--)
            {
                p = iftGetVoxelIndex(img,u);
                slc->val[q] = img->val[p];
                q++;
            }
        }

        return slc;
    }
    else
    {
        printf("[INFO] Radiologist perspective!\n");
        u.y = y;
        for (u.z = img->zsize - 1; u.z >= 0; u.z--)
        {
            for (u.x = 0; u.x < img->xsize; u.x++)
            {
                p = iftGetVoxelIndex(img,u);
                slc->val[q] = img->val[p];
                q++;
            }
        }

        return slc;
    }
}

// Gets Sagittal Slice.
iftImage *GetSagittalSlice(iftImage *img, int x, bool perspective)
{
    int q = 0;
    int p = 0;
    iftVoxel u;
    iftImage *slc = iftCreateImage(img->ysize, img->zsize,1);

    if (perspective)
    {
        printf("[INFO] Neuroradiologist perspective!\n");
        u.x = x;
        for (u.z = img->zsize - 1; u.z >= 0; u.z--)
        {
            for (u.y = img->ysize - 1; u.y >= 0; u.y--)
            {
                p = iftGetVoxelIndex(img,u);
                slc->val[q] = img->val[p];
                q++;
            }
        }

        return slc;
    }
    else
    {
        printf("[INFO] Radiologist perspective!\n");
        u.x = (img->xsize - 1) - x;
        for (u.z = img->zsize - 1; u.z >= 0; u.z--)
        {
            for (u.y = 0; u.y < img->ysize; u.y++)
            {
                p = iftGetVoxelIndex(img,u);
                slc->val[q] = img->val[p];
                q++;
            }
        }

        return slc;
    }
}