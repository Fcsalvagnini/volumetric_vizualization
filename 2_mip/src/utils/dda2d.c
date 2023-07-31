#include "ift.h"
#include "dda2d.h"

iftPoint* PerformDDA2D(
    iftImage* img, iftPoint* visible_vertexes, int* n_points
)
{

    float n = 0.0;
    float Du = 0.0, Dv = 0.0;
    float du = 0.0, dv = 0.0;
    if (
        visible_vertexes[0].x == visible_vertexes[1].x
        && visible_vertexes[0].y == visible_vertexes[1].y
    )
    {
        n = 1.0;
    }
    else {
        Du = visible_vertexes[1].x - visible_vertexes[0].x;
        Dv = visible_vertexes[1].y - visible_vertexes[0].y;

        if (fabs(Du) >= fabs(Dv)) {
            n = fabs(Du) + 1;
            du = (Du > 0) - (Du < 0);
            dv = (du * Dv) / Du;
        }
        else {
            n = fabs(Dv) + 1;
            dv = (Dv > 0) - (Dv < 0);
            du = (dv * Du) / Dv;
        }
    }

    *n_points = (int)ceil(n);
    iftPoint* dda2d_points = malloc(*n_points * sizeof(iftPoint));
    dda2d_points[0] = visible_vertexes[0];

    for (size_t idx = 1; idx < *n_points; idx++) {
        dda2d_points[idx].x = dda2d_points[idx - 1].x + du;
        dda2d_points[idx].y = dda2d_points[idx - 1].y + dv;
        dda2d_points[idx].z = 0;
    }

    return dda2d_points;
}