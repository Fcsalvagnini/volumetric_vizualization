#ifndef _GET_VIEWS_
#define _GET_VIEWS_

#include "ift.h"
#include "graphical_context.h"

/*[Document]
Document better each method.
*/

// Gets Axial Slice.
iftImage *GetAxialSlice(
    iftImage *img, iftImage* maskImg, int z, bool perspective,
    GraphicalContext* graphicalContext
);

// Gets Coronal Slice.
iftImage *GetCoronalSlice(
    iftImage *img, iftImage* maskImg, int y, bool perspective,
    GraphicalContext* graphicalContext
);

// Gets Sagittal Slice.
iftImage *GetSagittalSlice(
    iftImage *img, iftImage* maskImg, int x, bool perspective,
    GraphicalContext* graphicalContext
);

#endif