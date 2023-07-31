#include "ift.h"
#include "include/get_views.h"
#include "include/linear_stretching.h"
#include "include/blue2red_color.h"

/*
./bin/main ../0_data/input/brain.scn 30 110 97 0 0.5 0.28
./bin/main ../0_data/input/brain.scn 30 110 97 1 0.5 0.28
*/

int main(int argc, char *argv[]) {
    timer *tStart = NULL;
    int MemDinStart, MemDinEnd;

    MemDinStart = iftMemoryUsed();

    if (argc != 8) {
        iftError(
            "Usage: main <...>\n"
            "[1] input image (.scn) \n"
            "[2] x coordinate to extract sagittal slice \n"
            "[3] y coordinate to extract coronal slice \n"
            "[4] z coordinate to extract axial slice \n"
            "[5] point of view (0: radiologists | 1: neuroradiologists) \n"
            "[6] (C) window size for linear stretching (percentage of the intensity range) \n"
            "[7] (B) level for linear stretching (percentage of the maximum intensity) \n",
            "main"
        );
    }

    tStart = iftTic();

    // Coding area
    iftImage *img = iftReadImageByExt(argv[1]);
    // Gets volume minimum and maximum value to guide linear stretching.
    int iMin = iftMinimumValue(img);
    int iMax = iftMaximumValue(img);

    /*Reasoning behind Radiologist (0) and Neuroradiologist (1)

    Radiologist:
    Expects see the patient always:
    - in the upright position
    - facing to the left side of the screen
    - with its left side on the right side of the screen
    Consequently, each slice will change as following:
    - Sagittal: left to right
    - Coronal: anterior to posterior
    - Axial: inferior to superior

    Neuroradiologist:
    Expects see the patient always:
    - in the upright position
    - facing to the right side of the screen
    - with its left side on the left side of the screen
    Consequently, each slice will change as following:
    - Sagittal: right to left
    - Coronal: posterior to anterior
    - Axial: superior to inferior

    */

    // Get slices -> Memory Leakage
    iftImage *sagittalImage = GetSagittalSlice(img, atoi(argv[2]), atoi(argv[5]));
    iftImage *coronalImage = GetCoronalSlice(img, atoi(argv[3]), atoi(argv[5]));
    iftImage *axialImage = GetAxialSlice(img, atoi(argv[4]), atoi(argv[5]));

    // Linear stretching
    sagittalImage = ApplyLinearStretching(
        sagittalImage,
        atof(argv[6]),
        atof(argv[7]),
        iMin,
        iMax
    );
    coronalImage = ApplyLinearStretching(
        coronalImage,
        atof(argv[6]),
        atof(argv[7]),
        iMin,
        iMax
    );
    axialImage = ApplyLinearStretching(
        axialImage,
        atof(argv[6]),
        atof(argv[7]),
        iMin,
        iMax
    );

    // ColloredImage
    sagittalImage = GetBlue2RedImage(sagittalImage);
    coronalImage = GetBlue2RedImage(coronalImage);
    axialImage = GetBlue2RedImage(axialImage);

    // Save image -> Memory Leakage
    char *output_folder = "data";
    char sagittal_path[100];
    char coronal_path[100];
    char axial_path[100];
    if (atoi(argv[5]))
    {
        sprintf(sagittal_path, "%s/neuroradiologist_sagittal.png", output_folder);
        sprintf(coronal_path, "%s/neuroradiologist_coronal.png", output_folder);
        sprintf(axial_path, "%s/neuroradiologist_axial.png", output_folder);
    }
    else
    {
        sprintf(sagittal_path, "%s/radiologist_sagittal.png", output_folder);
        sprintf(coronal_path, "%s/radiologist_coronal.png", output_folder);
        sprintf(axial_path, "%s/radiologist_axial.png", output_folder);
    }
    iftWriteImageByExt(sagittalImage, sagittal_path);
    iftWriteImageByExt(coronalImage, coronal_path);
    iftWriteImageByExt(axialImage, axial_path);


    // Releases resources
    iftDestroyImage(&img);
    iftDestroyImage(&sagittalImage);
    iftDestroyImage(&coronalImage);
    iftDestroyImage(&axialImage);

    // Verifies code execution time and correct memory usage
    puts("\nDone ...");
    puts(iftFormattedTime(iftCompTime(tStart, iftToc())));

    MemDinEnd = iftMemoryUsed();
    iftVerifyMemory(MemDinStart, MemDinEnd);

    return 0;
}