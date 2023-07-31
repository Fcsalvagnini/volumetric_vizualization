#ifndef _RENDERING_
#define _RENDERING_

#include "ift.h"
#include "graphical_context.h"

void RenderTransparentScene(
    iftImage* img, iftImage* maskImg, iftImage* renderedImg,
    GraphicalContext* graphicalContext, iftMatrix* pointwisePlaneOps,
    iftMatrix* planeNormalTransformed, iftMatrix* pMatrix, iftMatrix* p0Matrix
);

iftImage *Rendering(
    iftImage* img, iftImage* maskImg, GraphicalContext* graphicalContext,
    int tiltAlpha, int spinBeta
);

#endif