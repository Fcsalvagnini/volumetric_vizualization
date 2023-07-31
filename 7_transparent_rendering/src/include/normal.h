#ifndef _NORMAL_
#define _NORMAL_

#include "ift.h"
#include "graphical_context.h"

void ComputesNormalScene(
    iftImage* img, iftImage* maskImg, GraphicalContext* graphicalContext,
    iftSet* S
);

void ComputesNormalObject(
    iftImage* img, iftImage* maskImg, GraphicalContext* graphicalContext,
    iftSet* S, int label
);

void ComputesObjectGradient(
    iftImage* cost, GraphicalContext* graphicalContext, iftSet* auxS
);

int GetClosestNormal(iftVector grad);

void GenerateNormalTable(GraphicalContext* graphicalContext);

#endif