#include "assess.h"

/*
Execution sample:

./bin/main 3 data/output_labels/image_002_seg.nii.gz \
data/gt_labels/image_002_seg.nii.gz
*/

void AssessSegmentations(int argc, char *argv[]) {
    if (argc != 4) {
        iftError(
            "Usage: assess <...>\n"
            "[1] mode: generate kernels, convolve, segment, assess [0|1|2|3]. \n"
            "[2] predicted labels. \n"
            "[3] gt labels. \n",
            "assess"
        );
    }

    iftImage *predicted_labels = iftReadImageByExt(argv[2]);
    iftImage *gt_labels = iftReadImageByExt(argv[3]);

    iftDblArray *dice_metrics = iftDiceSimilarityMultiLabel(
        predicted_labels, gt_labels, 3
    );
    for (size_t idx = {1}; idx < dice_metrics->n; idx++) {
        printf("[Dice for label %ld]: %f\n", idx, dice_metrics->val[idx]);
    }
    printf("[Average Dice]: %f\n", dice_metrics->val[0]);

    // Releases allocated resources
    iftDestroyImage(&predicted_labels);
    iftDestroyImage(&gt_labels);
}