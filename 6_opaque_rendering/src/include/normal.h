#ifndef _NORMAL_
#define _NORMAL_

#include "ift.h"
#include "graphical_context.h"

void ComputesNormal(
    iftImage* img, iftImage* maskImg, GraphicalContext* graphicalContext,
    iftSet* S
);

int GetClosestNormal(iftVector grad);

void GenerateNormalTable(GraphicalContext* graphicalContext);

#endif