#include "ift.h"
#include "graphical_context.h"

// Gets Axial Slice.
iftImage *GetAxialSlice(
    iftImage *img, iftImage* maskImg, int z, bool perspective,
    GraphicalContext* graphicalContext
)
{
    int q = 0;
    int p = 0;
    int label;
    iftColor YCbCr;
    iftVoxel u;
    iftImage *slc = iftCreateColorImage(img->xsize, img->ysize, 1, 16);

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
                label = maskImg->val[p];
                if(label != 0) {
                    label--;
                    YCbCr = graphicalContext->sceneObjects[label].colorReflectanceYCbCr;
                    slc->Cb[q] = YCbCr.val[1];
                    slc->Cr[q] = YCbCr.val[2];
                }

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
                label = maskImg->val[p];
                if(label != 0) {
                    label--;
                    YCbCr = graphicalContext->sceneObjects[label].colorReflectanceYCbCr;
                    slc->Cb[q] = YCbCr.val[1];
                    slc->Cr[q] = YCbCr.val[2];
                }

                q++;
            }
        }

        return slc;
    }
}

// Gets Coronal Slice.
iftImage *GetCoronalSlice(
    iftImage *img, iftImage* maskImg, int y, bool perspective,
    GraphicalContext* graphicalContext
)
{
    int q = 0;
    int p = 0;
    int label;
    iftColor YCbCr;
    iftVoxel u;
    iftImage *slc = iftCreateColorImage(img->xsize, img->zsize, 1, 16);

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
                label = maskImg->val[p];
                if(label != 0) {
                    label--;
                    YCbCr = graphicalContext->sceneObjects[label].colorReflectanceYCbCr;
                    slc->Cb[q] = YCbCr.val[1];
                    slc->Cr[q] = YCbCr.val[2];
                }

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
                label = maskImg->val[p];
                if(label != 0) {
                    label--;
                    YCbCr = graphicalContext->sceneObjects[label].colorReflectanceYCbCr;
                    slc->Cb[q] = YCbCr.val[1];
                    slc->Cr[q] = YCbCr.val[2];
                }

                q++;
            }
        }

        return slc;
    }
}

// Gets Sagittal Slice.
iftImage *GetSagittalSlice(
    iftImage *img, iftImage* maskImg, int x, bool perspective,
    GraphicalContext* graphicalContext
)
{
    int q = 0;
    int p = 0;
    int label;
    iftColor YCbCr;
    iftVoxel u;
    iftImage *slc = iftCreateColorImage(img->ysize, img->zsize, 1, 16);

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
                label = maskImg->val[p];
                if(label != 0) {
                    label--;
                    YCbCr = graphicalContext->sceneObjects[label].colorReflectanceYCbCr;
                    slc->Cb[q] = YCbCr.val[1];
                    slc->Cr[q] = YCbCr.val[2];
                }

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
                label = maskImg->val[p];
                if(label != 0) {
                    label--;
                    YCbCr = graphicalContext->sceneObjects[label].colorReflectanceYCbCr;
                    slc->Cb[q] = YCbCr.val[1];
                    slc->Cr[q] = YCbCr.val[2];
                }

                q++;
            }
        }

        return slc;
    }
}