#ifndef _RESLICE_
#define _RESLICE_

#include "ift.h"

float GetAlpha(iftVector cut_plane_normal_vector);

iftMatrix* GetRotationOps(iftVector cut_plane_normal_vector, float alpha);

iftImage* ResliceScene(iftImage* img, iftPoint p0, iftPoint pn, int n);

#endif