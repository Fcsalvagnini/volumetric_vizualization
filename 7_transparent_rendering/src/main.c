#include "ift.h"
#include "graphical_context.h"
#include "rendering.h"
#include "save_slices.h"

/*
./bin/main data/input/brain.scn data/input/brain_label.scn \
0 0 data/output 0.4,0.4,0.4 0 1,1,1 None 0 0.5 0.28

./bin/main data/input/brain.scn data/input/brain_label.scn \
10 110 data/output 0.4,0.4,0.4 0 1,1,1 30,110,97 0 0.5 0.28
*/

int main(int argc, char *argv[]) {
    timer *tStart = NULL;
    int menDinStart, menDinEnd;

    menDinStart = iftMemoryUsed();

    if (argc != 13) {
        iftError(
            "Usage: main <...>\n"
            "[1] Volumetric Input Image (.scn). \n"
            "[2] Volumetric Input Label (.scn). \n"
            "[3] tilt angle alpha. \n"
            "[4] spin angle beta. \n"
            "[5] path to save output images \n"
            "[6] objects opacity \n"
            "[7] normal estimation (0: scene-based | 1: object-based) \n"
            "[8] objects visibility \n"
            "[9] (Optional) Point to Get Slice \n"
            "[10] point of view (0: radiologists | 1: neuroradiologists) \n"
            "[11] (C) window size for linear stretching (percentage of the intensity range) \n"
            "[12] (B) level for linear stretching (percentage of the maximum intensity) \n",
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

    if (strcmp("None", argv[9]) == 0) {
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
        sscanf(argv[9], "%f,%f,%f",
            &slicesPoint.x, &slicesPoint.y, &slicesPoint.z
        );
        printf("x: %f | y: %f | z: %f\n",
            slicesPoint.x, slicesPoint.y, slicesPoint.z
        );
    }

    GraphicalContext *graphicalContext = GetGraphicalContext(
        img, maskImg
    );
    graphicalContext->normalMethod = atoi(argv[7]);
    // Sets visibility and opacity
    sscanf(
        argv[6], "%f,%f,%f",
        &graphicalContext->sceneObjects[0].opacity,
        &graphicalContext->sceneObjects[1].opacity,
        &graphicalContext->sceneObjects[2].opacity
    );
    sscanf(
        argv[8], "%d,%d,%d",
        &graphicalContext->sceneObjects[0].visibility,
        &graphicalContext->sceneObjects[1].visibility,
        &graphicalContext->sceneObjects[2].visibility
    );
    printf("[INFO] Objects Opacity: %f %f %f\n",
        graphicalContext->sceneObjects[0].opacity,
        graphicalContext->sceneObjects[1].opacity,
        graphicalContext->sceneObjects[2].opacity
    );
    printf("[INFO] Objects visibility: %d %d %d\n",
        graphicalContext->sceneObjects[0].visibility,
        graphicalContext->sceneObjects[1].visibility,
        graphicalContext->sceneObjects[2].visibility
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
    iftWriteImageByExt(renderedImg, outputImgPath);

    int perspective = atoi(argv[10]);
    float windowSize = atof(argv[11]);
    float level = atof(argv[12]);
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