#ifndef _PHONG_
#define _PHONG_

#include "ift.h"
#include "graphical_context.h"
#include "ray_casting.h"
#include "global.h"

iftColor GenerateRGBPhong(float r_p_, float *objectReflectance);

iftColor GetYcbCrFromPhong(
    int voxelIdx, int label, GraphicalContext* graphicalContext,
    iftVector planeNormal
);

#endif