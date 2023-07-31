#include "ift.h"
#include "dda3d.h"

iftPoint* PerformDDA3D(
    iftImage* img, iftPoint* intersectionPoints, int* nPoints
)
{
    float n = 0.0;
    float Dx = 0.0, Dy = 0.0, Dz = 0.0;
    float dx = 0.0, dy = 0.0, dz = 0.0;
    if (
        intersectionPoints[0].x == intersectionPoints[1].x
        && intersectionPoints[0].y == intersectionPoints[1].y
        && intersectionPoints[0].z == intersectionPoints[1].z
    )
    {
        // Insert only the intersection point
        n = 1.0;
    }
    else {
        Dx = intersectionPoints[1].x - intersectionPoints[0].x;
        Dy = intersectionPoints[1].y - intersectionPoints[0].y;
        Dz = intersectionPoints[1].z - intersectionPoints[0].z;

        if (fabs(Dx) >=fabs(Dy) && fabs(Dx) >= fabs(Dz)) {
            n = fabs(Dx) + 1;
            dx = (Dx > 0) - (Dx < 0);
            dy = (dx * Dy) / Dx;
            dz = (dx * Dz) / Dx;
        }
        else if (fabs(Dy) >= fabs(Dx) && fabs(Dy) >= fabs(Dz)) {
            n = fabs(Dy) + 1;
            dy = (Dy > 0) - (Dy < 0);
            dx = (dy * Dx) / Dy;
            dz = (dy * Dz) / Dy;
        }
        else {
            n = fabs(Dz) + 1;
            dz = (Dz > 0) - (Dz < 0);
            dx = (dz * Dx / Dz);
            dy = (dz * Dy / Dz);
        }
    }

    *nPoints = (int)ceil(n);
    // Index 0 stores array size
    iftPoint* dda3DPoints = malloc(*nPoints * sizeof(iftPoint));
    dda3DPoints[0] = intersectionPoints[0];

    for (size_t idx = 1; idx < *nPoints; idx++) {
        dda3DPoints[idx].x = dda3DPoints[idx - 1].x + dx;
        dda3DPoints[idx].y = dda3DPoints[idx - 1].y + dy;
        dda3DPoints[idx].z = dda3DPoints[idx - 1].z + dz;
    }

    return dda3DPoints;
}