#ifndef _MORPH_OPS_
#define _MORPH_OPS_

#include "ift.h"

iftSet *MaskObjectBorder(iftImage *mask);

iftImage *FastDilation(iftImage *mask, iftSet **S, int radius);

iftImage *FastErosion(iftImage *mask, iftSet **S, int radius);

iftImage *FastClosing(iftImage *mask, int radius);

#endif