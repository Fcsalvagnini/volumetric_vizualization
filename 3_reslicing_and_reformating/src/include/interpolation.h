#ifndef _INTERPOLATION_
#define _INTERPOLATION_

#include "ift.h"

void TrilinearInterpolation(
    iftImage* img, iftImage* resliced_img, iftPoint p_, iftVoxel u
);

#endif