#ifndef _SAVE_SLICES_
#define _SAVE_SLICES_

#include "ift.h"

void SaveSlices(
    iftImage* img, iftImage* maskImg, iftPoint slicesPoint, int perspective,
    float windowSize, float level, char* outputPath,
    GraphicalContext* graphicalContext
);

#endif