#include "ift.h"
#include "scene_data.h"
#include "mip.h"
#include "wireframe.h"
#include "linear_stretching.h"
#include "blue2red_color.h"

/*
./bin/main ../0_data/input/thorax.scn 50 30 \
None data/max_intensity_projection_colored.png \
data/max_intensity_projection_gray.png 0.3 0.8

./bin/main ../0_data/input/thorax.scn 50 30 \
../0_data/input/left-lung.scn \
data/max_intensity_projection_colored.png \
data/max_intensity_projection_gray.png 0.3 0.8
*/


int main(int argc, char *argv[]) {
    timer *tStart = NULL;
    int MemDinStart, MemDinEnd;

    MemDinStart = iftMemoryUsed();

    if (argc != 9) {
        iftError(
            "Usage: main <...>\n"
            "[1] input image (.scn) \n"
            "[2] tilt angle alpha \n"
            "[3] spin angle beta \n"
            "[4] object mask (.scn) (Optional: None or Path) \n"
            "[5] gray output image of the maximum intensity projection (.png) \n"
            "[6] colored output image of the maximum intensity projection (.png) \n"
            "[7] (C) window size for linear stretching (percentage of the intensity range) \n"
            "[8] (B) level for linear stretching (percentage of the maximum intensity) \n",
            "main"
        );
    }

    tStart = iftTic();
    // If object mask is None reads the Whole Scene
    iftImage *img = NULL;
    if (strcmp("None", argv[4]) == 0) {
        printf("[INFO] Reading Whole Scene Image: %s \n", argv[1]);
        img = iftReadImageByExt(argv[1]);
    }
    else {
        printf("[INFO] Reading Object Mask: %s \n", argv[4]);
        img = iftReadImageByExt(argv[1]);
        iftImage *mask_img = iftReadImageByExt(argv[4]);
        iftVoxel v;
        int voxel_index;
        for (v.x = 0; v.x < mask_img->xsize; v.x++)
        {
            for (v.y = 0; v.y < mask_img->ysize; v.y++)
            {
                for (v.z = 0; v.z < mask_img->zsize; v.z++)
                {
                    voxel_index = iftGetVoxelIndex(mask_img, v);
                    if (mask_img->val[voxel_index] == 0) {
                        img->val[voxel_index] = 0;
                    }
                }
            }
        }
        iftDestroyImage(&mask_img);
    }

    // Plot Image Size
    printf(
        "[INFO IMAGE] : x_size: %d | y_size: %d | z_size: %d\n",
        img->xsize, img->ysize, img->zsize
    );

    Face *scene_faces = GetSceneFaces(img);
    Vertex *scene_vertexes = GetSceneVertexes(img);

    iftImage *mip_img = GetMaximumIntensityProjection(
        img, scene_faces, atoi(argv[2]), atoi(argv[3])
    );


    // Optimizes Image Visualization
    int i_min = iftMinimumValue(img);
    int i_max = iftMaximumValue(img);
    mip_img = ApplyLinearStretching(
        mip_img, atof(argv[7]), atof(argv[8]), i_min, i_max
    );
    // Draw WireFrame
    WireFrame(
        img, mip_img, scene_faces, scene_vertexes,
        atoi(argv[2]), atoi(argv[3])
    );
    iftWriteImageByExt(mip_img, argv[6]);
    mip_img = GetBlue2RedImage(mip_img);
    iftWriteImageByExt(mip_img, argv[5]);

    // Destroy Allocated Resources
    iftDestroyImage(&img);
    iftDestroyImage(&mip_img);
    DestroyFaces(scene_faces);
    DestroySceneVertex(scene_vertexes);

    // Verifies code execution time and correct memory usage
    puts("\nDone ...");
    puts(iftFormattedTime(iftCompTime(tStart, iftToc())));
    MemDinEnd = iftMemoryUsed();
    iftVerifyMemory(MemDinStart, MemDinEnd);

    return 0;
}