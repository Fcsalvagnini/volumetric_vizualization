#ifndef _SEGMENTATION_
#define _SEGMENTATION_

#include "ift.h"

typedef struct feature_maps
{
    int n_features;
    iftMImage **feature_maps;
    float *weights;
} FeatureMaps;

iftImage * MergeFeatureMaps(FeatureMaps features, int border);

iftImage *SegmentByWatershed(
    iftImage *img, iftLabeledSet *seeds, iftImage *grad_map, float alpha,
    int mode
);

void NormalizeFeatureMaps(FeatureMaps *feature_map);

iftImage *MergeLabels(iftImage *label_flair, iftImage *label_t1gd);

void RunSegmentation(int argc, char *argv[]);

#endif