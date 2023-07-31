#include "ift.h"
#include "dda3d.h"

iftPoint* PerformDDA3D(
    iftImage* img, iftPoint* intersection_points, int* n_points
)
{
    float n = 0.0;
    float Dx = 0.0, Dy = 0.0, Dz = 0.0;
    float dx = 0.0, dy = 0.0, dz = 0.0;
    if (
        intersection_points[0].x == intersection_points[1].x
        && intersection_points[0].y == intersection_points[1].y
        && intersection_points[0].z == intersection_points[1].z
    )
    {
        // Insert only the intersection point
        n = 1.0;
    }
    else {
        Dx = intersection_points[1].x - intersection_points[0].x;
        Dy = intersection_points[1].y - intersection_points[0].y;
        Dz = intersection_points[1].z - intersection_points[0].z;

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

    *n_points = (int)ceil(n);
    // Index 0 stores array size
    iftPoint* dda3d_points = malloc(*n_points * sizeof(iftPoint));
    dda3d_points[0] = intersection_points[0];

    for (size_t idx = 1; idx < *n_points; idx++) {
        dda3d_points[idx].x = dda3d_points[idx - 1].x + dx;
        dda3d_points[idx].y = dda3d_points[idx - 1].y + dy;
        dda3d_points[idx].z = dda3d_points[idx - 1].z + dz;
    }

    return dda3d_points;
}