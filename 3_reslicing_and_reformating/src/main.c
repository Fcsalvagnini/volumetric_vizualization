#include "ift.h"
#include "reslice.h"

/* ./bin/main ../0_data/input/thorax.scn \
0,0,0 277,277,272 400 \
data/output_scene.scn data/output_scene.nii.gz */

int main(int argc, char *argv[]) {
    timer *tStart = NULL;
    int MemDinStart, MemDinEnd;

    MemDinStart = iftMemoryUsed();

    if (argc != 7) {
        iftError(
            "Usage: main <...>\n"
            "[1] input image (.scn) \n"
            "[2] p0 (x0, y0, z0) coordinates \n"
            "[3] p(n - 1) (x(n-1), y(n-1), z(n-1) coordinates \n"
            "[4] the number (n) of axial slices of the new scene \n"
            "[5] output volumetric scene (.scn) \n"
            "[6] output volumetric scene (.nii.gz) \n",
            "main"
        );
    }

    tStart = iftTic();

    iftImage *img = iftReadImageByExt(argv[1]);
    printf(
        "[INFO IMAGE] : x_size: %d | y_size: %d | z_size: %d\n",
        img->xsize, img->ysize, img->zsize
    );
    iftPoint p0 = {.x=0.0, .y=0.0, .z=0.0}, pn = {.x=0.0, .y=0.0, .z=0.0};
    sscanf(argv[2], "%f,%f,%f", &p0.x, &p0.y, &p0.z);
    sscanf(argv[3], "%f,%f,%f", &pn.x, &pn.y, &pn.z);
    printf(
        "[INFO] : p0 -> x: %f | y: %f | z: %f\n",
        p0.x, p0.y, p0.z
    );
    printf(
        "[INFO] : p(n - 1) -> x: %f | y: %f | z: %f\n",
        pn.x, pn.y, pn.z
    );
    int n = atoi(argv[4]);

    iftImage *resliced_img = ResliceScene(img, p0, pn, n);

    iftWriteImageByExt(resliced_img, argv[5]);
    iftWriteImageByExt(resliced_img, argv[6]);

    // Release alocated resources
    iftDestroyImage(&img);
    iftDestroyImage(&resliced_img);

    // Verifies code execution time and correct memory usage
    puts("\nDone ...");
    puts(iftFormattedTime(iftCompTime(tStart, iftToc())));
    MemDinEnd = iftMemoryUsed();
    iftVerifyMemory(MemDinStart, MemDinEnd);

    return 0;
}