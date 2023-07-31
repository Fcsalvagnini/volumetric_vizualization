#ifndef _GET_VIEWS_
#define _GET_VIEWS_

#include "ift.h"

/*[Document]
Document better each method.
*/

// Gets Axial Slice.
iftImage *GetAxialSlice(iftImage *img, int z, bool perspective);

// Gets Coronal Slice.
iftImage *GetCoronalSlice(iftImage *img, int y, bool perspective);

// Gets Sagittal Slice.
iftImage *GetSagittalSlice(iftImage *img, int x, bool perspective);

#endif