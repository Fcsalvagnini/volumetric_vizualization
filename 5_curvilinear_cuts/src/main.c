#include "ift.h"
#include "morph_ops.h"
#include "edt.h"
#include "curvilinear_cut.h"
#include "scene_data.h"
#include "linear_stretching.h"
#include "wireframe.h"

/*
./bin/main data/input_images/image_002_T1GD.nii.gz \
data/brain_mask/image_002_T1GD.nii.gz 2 0 50 50 \
data/output_images 1 0.5
*/

int main(int argc, char *argv[]) {
    timer *tStart = NULL;
    int MemDinStart, MemDinEnd;

    MemDinStart = iftMemoryUsed();

    if (argc != 10) {
        iftError(
            "Usage: main <...>\n"
            "[1] MRI input image (.nii.gz). \n"
            "[2] 3D brain mask (.nii.gz). \n"
            "[3] Radius to Generate Envelop Image. \n"
            "[4] the depth of the cut in mm. \n"
            "[5] tilt angle alpha. \n"
            "[6] spin angle beta. \n"
            "[7] path to save output images \n"
            "[8] (C) window size for linear stretching (percentage of the intensity range) \n"
            "[9] (B) level for linear stretching (percentage of the maximum intensity) \n",
            "main"
        );
    }

    tStart = iftTic();
    // If object mask is None reads the Whole Scene
    iftImage *img = iftReadImageByExt(argv[1]);
    iftImage *mask_img = iftReadImageByExt(argv[2]);
    int closing_radius = atoi(argv[3]);
    int depth_cut = atoi(argv[4]); // in mm
    int tilt_alpha = atoi(argv[5]);
    int spin_beta = atoi(argv[6]);

    char output_img_path[200];
    sprintf(
        output_img_path, "%s/mask.nii.gz",
        argv[7]
    );
    iftWriteImageByExt(mask_img, output_img_path);

    iftImage *closed_img = FastClosing(mask_img, closing_radius);
    sprintf(
        output_img_path, "%s/closed_img.nii.gz",
        argv[7]
    );
    iftWriteImageByExt(closed_img, output_img_path);
    iftImage *envelop_img = GetEnvelopImage(closed_img);
    sprintf(
        output_img_path, "%s/envelop_img.nii.gz",
        argv[7]
    );
    iftWriteImageByExt(envelop_img, output_img_path);

    // Gets curvlinear cut images
    Face *scene_faces = GetSceneFaces(img);
    Vertex *scene_vertexes = GetSceneVertexes(img);
    iftImage *curvilinear_cut = GetCurvilinearCut(
        img, envelop_img, scene_faces, tilt_alpha, spin_beta, depth_cut
    );
    sprintf(
        output_img_path, "%s/curvilinear_cut_img.png",
        argv[7]
    );
    iftWriteImageByExt(curvilinear_cut, output_img_path);
    // Optimizes Image Visualization
    int i_min = iftMinimumValue(img);
    int i_max = iftMaximumValue(img);
    curvilinear_cut = ApplyLinearStretching(
        curvilinear_cut, atof(argv[8]), atof(argv[9]), i_min, i_max
    );
    sprintf(
        output_img_path, "%s/curvilinear_cut_img_enhanced.png",
        argv[7]
    );
    WireFrame(
        img, curvilinear_cut, scene_faces, scene_vertexes,
        tilt_alpha, spin_beta
    );
    iftWriteImageByExt(curvilinear_cut, output_img_path);

    // Destroy Allocated Resources
    iftDestroyImage(&img);
    iftDestroyImage(&mask_img);
    iftDestroyImage(&closed_img);
    iftDestroyImage(&envelop_img);
    iftDestroyImage(&curvilinear_cut);
    DestroyFaces(scene_faces);
    DestroySceneVertex(scene_vertexes);

    // Verifies code execution time and correct memory usage
    puts("\nDone ...");
    puts(iftFormattedTime(iftCompTime(tStart, iftToc())));
    MemDinEnd = iftMemoryUsed();
    iftVerifyMemory(MemDinStart, MemDinEnd);

    return 0;
}