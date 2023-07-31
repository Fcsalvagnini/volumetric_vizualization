#ifndef _LINEAR_STRETCHING_
#define _LINEAR_STRETCHING_

#include "ift.h"

/*[Document]
Document better each method.
*/

iftImage *ApplyLinearStretching(
    iftImage *img, float window_size, float level, int iMin, int iMax
);

#endif