#include "ift.h"
#include "graphical_context.h"
#include "rendering.h"
#include "save_slices.h"

/*
./bin/main data/input/brain.scn data/input/brain_label.scn \
0 0 data/output None 0 0.5 0.28

./bin/main data/input/brain.scn data/input/brain_label.scn \
10 110 data/output 30,110,97 0 0.5 0.28
*/

int main(int argc, char *argv[]) {
    timer *tStart = NULL;
    int menDinStart, menDinEnd;

    menDinStart = iftMemoryUsed();

    if (argc != 11) {
        iftError(
            "Usage: main <...>\n"
            "[1] Volumetric Input Image (.scn). \n"
            "[2] Volumetric Input Label (.scn). \n"
            "[3] tilt angle alpha. \n"
            "[4] spin angle beta. \n"
            "[5] path to save output images \n"
            "[6] (Optional) Point to Get Slice \n"
            "[7] point of view (0: radiologists | 1: neuroradiologists) \n"
            "[8] (C) window size for linear stretching (percentage of the intensity range) \n"
            "[9] (B) level for linear stretching (percentage of the maximum intensity) \n"
            "[10] output image",
            "main"
        );
    }

    tStart = iftTic();

    // Reads input
    iftImage *img = iftReadImageByExt(argv[1]);
    iftImage *maskImg = iftReadImageByExt(argv[2]);
    printf("[INFO] Input Image Size: x: %d | y %d | z: %d \n",
        img->xsize, img->ysize, img->zsize
    );
    printf("[INFO] Input Image Label Size: x: %d | y %d | z: %d \n",
        maskImg->xsize, maskImg->ysize, maskImg->zsize
    );
    int tiltAlpha = atoi(argv[3]);
    int spinBeta = atoi(argv[4]);
    char *outputPath = argv[5];
    iftPoint slicesPoint = {.x = 0.0, .y = 0.0, .z = 0.0};

    if (strcmp("None", argv[6]) == 0) {
        printf("[INFO] Slice point at scene center: ");
        slicesPoint = (iftPoint) {
            .x = floor((float)(img->xsize) / 2.0),
            .y = floor((float)(img->ysize) / 2.0),
            .z = floor((float)(img->zsize) / 2.0)
        };
        printf("x: %f | y: %f | z: %f\n",
            slicesPoint.x, slicesPoint.y, slicesPoint.z
        );
    }
    else {
        printf("[INFO] Slice point provided by user: ");
        sscanf(argv[6], "%f,%f,%f",
            &slicesPoint.x, &slicesPoint.y, &slicesPoint.z
        );
        printf("x: %f | y: %f | z: %f\n",
            slicesPoint.x, slicesPoint.y, slicesPoint.z
        );
    }

    GraphicalContext *graphicalContext = GetGraphicalContext(
        img, maskImg
    );

    iftImage *renderedImg = Rendering(
        img, maskImg, graphicalContext, tiltAlpha, spinBeta
    );

    // Saves all images
    char outputImgPath[200];
    sprintf(
        outputImgPath, "%s/rendered_img.png",
        outputPath
    );
    iftWriteImageByExt(renderedImg, argv[10]);

    int perspective = atoi(argv[7]);
    float windowSize = atof(argv[8]);
    float level = atof(argv[9]);
    SaveSlices(
        img, maskImg, slicesPoint, perspective, windowSize, level, outputPath,
        graphicalContext
    );

    // Releases allocared resources
    iftDestroyImage(&img);
    iftDestroyImage(&maskImg);
    iftDestroyImage(&renderedImg);
    DestroyGraphicalContext(graphicalContext);

    // Verifies code execution time and correct memory usage
    puts("\nDone ...");
    puts(iftFormattedTime(iftCompTime(tStart, iftToc())));
    menDinEnd = iftMemoryUsed();
    iftVerifyMemory(menDinStart, menDinEnd);

    return 0;
}