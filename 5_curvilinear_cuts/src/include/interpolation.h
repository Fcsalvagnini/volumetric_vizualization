#ifndef _INTERPOLATION_
#define _INTERPOLATION_

#include "ift.h"

void TrilinearInterpolation(
    iftImage* src_img, iftImage* dst_img, iftPoint p_, iftVoxel u
);

#endif