#include "ift.h"
#include "get_views.h"
#include "linear_stretching.h"

void SaveSlices(
    iftImage* img, iftImage* maskImg, iftPoint slicesPoint, int perspective,
    float windowSize, float level, char* outputPath,
    GraphicalContext* graphicalContext
)
{
    // Verify point
    if (
        slicesPoint.x >= img->xsize
        || slicesPoint.y >= img->ysize
        || slicesPoint.z >= img->zsize
    )
    {
        iftError(
            "Slice coordinates are outside image domain",
            "save_slices"
        );
    }

    iftImage *sagittal = GetSagittalSlice(
        img, maskImg, (int)slicesPoint.x, perspective, graphicalContext
    );
    iftImage *coronal = GetCoronalSlice(
        img, maskImg, (int)slicesPoint.y, perspective, graphicalContext
    );
    iftImage *axial = GetAxialSlice(
        img, maskImg, (int)slicesPoint.z, perspective, graphicalContext
    );

    int iMin = iftMinimumValue(img);
    int iMax = iftMaximumValue(img);
    ApplyLinearStretching(
        sagittal, windowSize, level, iMin, iMax
    );
    ApplyLinearStretching(
        coronal, windowSize, level, iMin, iMax
    );
    ApplyLinearStretching(
        axial, windowSize, level, iMin, iMax
    );

    char outputImgPath[200];
    sprintf(
        outputImgPath, "%s/sagittal.png",
        outputPath
    );
    iftWriteImageByExt(sagittal, outputImgPath);
    sprintf(
        outputImgPath, "%s/coronal.png",
        outputPath
    );
    iftWriteImageByExt(coronal, outputImgPath);
    sprintf(
        outputImgPath, "%s/axial.png",
        outputPath
    );
    iftWriteImageByExt(axial, outputImgPath);

    iftDestroyImage(&sagittal);
    iftDestroyImage(&coronal);
    iftDestroyImage(&axial);
}