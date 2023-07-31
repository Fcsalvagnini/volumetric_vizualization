#include "segmentation.h"

/*
Execution sample:

./bin/main 2 data/feature_maps.txt data/input_images/image_002_FLAIR.nii.gz \
data/seeds/image_002_FLAIR-seeds.txt data/input_images/image_002_T1GD.nii.gz \
data/seeds/image_002_T1GD-seeds.txt data/output_labels/image_002_seg.nii.gz
*/

iftImage *SegmentByWatershed(
    iftImage *img, iftLabeledSet *seeds, iftImage *grad_map, float alpha,
    int mode
)
{
    // Defines necessary resources
    iftImage *path_cost = NULL, *label_img = NULL, *grad_input_img = NULL;
    iftGQueue *Q = NULL;
    // adj_idx = i, p_idx = p, q_idx = q, tmp_weight = tmp;
    int adj_idx, p_idx, q_idx, tmp_weight, grad;
    float weight, alpha_grad = 3, beta_grad = 0.8;
    iftVoxel u_voxel, v_voxel;
    iftAdjRel *adj_rel = iftSpheric(1.0);

    // Initializes necessary resources
    path_cost = iftCreateImage(img->xsize, img->ysize, img->zsize);
    label_img = iftCreateImage(img->xsize, img->ysize, img->zsize);
    grad_input_img = iftImageGradientMagnitude(img, adj_rel);
    Q = iftCreateGQueue(
        powf(iftMaximumValue(img), 1.5) + 1.0, // n buckets
        img->n, // n elements
        path_cost->val // Elements values
    );

    // Initializes path_cost with infinity cost
    for (p_idx = 0; p_idx < img->n; p_idx++) {
        path_cost->val[p_idx] = IFT_INFINITY_INT;
    }

    // Iniatilizes seeds path_cost (0) and labels
    while (seeds != NULL)
    {
        p_idx = seeds->elem;
        label_img->val[p_idx] = seeds->label;
        path_cost->val[p_idx] = 0;
        iftInsertGQueue(&Q, p_idx);
        seeds = seeds->next;
    }

    // Image Foresting Transform (IFT)
    while (!iftEmptyGQueue(Q))
    {
        p_idx = iftRemoveGQueue(Q);
        u_voxel = iftGetVoxelCoord(img, p_idx);

        for (adj_idx = 1; adj_idx < adj_rel->n; adj_idx++) {
            v_voxel = iftGetAdjacentVoxel(adj_rel, u_voxel, adj_idx);

            if (iftValidVoxel(img, v_voxel))
            {
                q_idx = iftGetVoxelIndex(img, v_voxel);
                // Verify if it was already added and removed from Queue
                if (Q->L.elem[q_idx].color != IFT_BLACK) {
                    if (mode == 0) {
                        grad = grad_map->val[q_idx];
                        weight = alpha * grad_input_img->val[q_idx]
                            + (1.0 - alpha) * grad;
                    }
                    else {
                        if (
                            grad_map->val[p_idx] > grad_map->val[q_idx]
                            && label_img->val[p_idx] == label_img->val[q_idx]
                        )
                        {
                            grad = (int)(pow(grad_map->val[q_idx], alpha_grad) + 0.5);
                        }
                        else if (
                            grad_map->val[p_idx] < grad_map->val[q_idx]
                            && label_img->val[p_idx] != label_img->val[q_idx]
                        )
                        {
                            grad = (int)(pow(grad_map->val[q_idx], alpha_grad) + 0.5);
                        }
                        else if (
                            grad_map->val[p_idx] == grad_map->val[q_idx]
                        ) {
                            grad = (int)(pow(grad_map->val[q_idx], beta_grad) + 0.5);
                        }
                        else {
                            grad = grad_map->val[q_idx];
                        }
                        weight = alpha * grad_input_img->val[q_idx]
                            + (1.0 - alpha) * grad;
                    }
                    tmp_weight = iftMax(
                        path_cost->val[p_idx], iftRound(weight)
                    );

                    // If offers a lesser weight, conquer adjacent voxel
                    if (tmp_weight < path_cost->val[q_idx]) {
                        // Verifies if its in the Queue and removes it
                        if (Q->L.elem[q_idx].color == IFT_GRAY) {
                            iftRemoveGQueueElem(Q, q_idx);
                        }
                        label_img->val[q_idx] = label_img->val[p_idx];
                        path_cost->val[q_idx] = tmp_weight;
                        iftInsertGQueue(&Q, q_idx);
                    }
                }
            }
        }
    }

    // Destroy allocated resources
    iftDestroyAdjRel(&adj_rel);
    iftDestroyGQueue(&Q);
    iftDestroyImage(&path_cost);
    iftDestroyImage(&grad_input_img);
    iftCopyVoxelSize(img, label_img);

    return label_img;
}

iftImage *MergeLabels(iftImage *label_flair, iftImage *label_t1gd) {
    iftImage *label_image = iftCreateImage(
        label_flair->xsize, label_flair->ysize, label_flair->zsize
    );

    for (size_t n_idx = {0}; n_idx < label_flair->n; n_idx++) {
        if (label_t1gd->val[n_idx] == 1) {
            label_image->val[n_idx] = 1;
        }
        else if (label_t1gd->val[n_idx] == 3)
        {
            label_image->val[n_idx] = 3;
        }
        else if (label_flair->val[n_idx] == 2)
        {
            label_image->val[n_idx] = 2;
        }
    }

    return label_image;
}

iftImage * MergeFeatureMaps(FeatureMaps features, int border) {
    iftImage *merged_img = iftCreateImage(
        features.feature_maps[0]->xsize,
        features.feature_maps[0]->ysize,
        features.feature_maps[0]->zsize
    );
    for (size_t p = 0; p < features.feature_maps[0]->n; p++) {
        float pixel_value = 0;
        for (size_t f_idx = 0; f_idx < features.n_features; f_idx++) {
            pixel_value += features.feature_maps[f_idx]->val[p][0] * features.weights[f_idx];
        }
        if (pixel_value < 0) {
            pixel_value = 0;
        }
        merged_img->val[p] = (int)(pixel_value + 0.5);
    }

    iftImage *merged_img_with_boder = iftAddFrame(merged_img, border, 0);
    iftDestroyImage(&merged_img);

    return merged_img_with_boder;
}

void NormalizeFeatureMaps(FeatureMaps *feature_map)
{
    float k_max = 1.0, k_min = 0.0;
    float normalized_value, f_min, f_max;
    for (size_t idx = 0; idx < feature_map->n_features; idx++) {
        f_max = iftMMaximumValue(feature_map->feature_maps[idx], -1);
        f_min = iftMMinimumValue(feature_map->feature_maps[idx], -1);
        for (size_t p_idx = 0; p_idx < feature_map->feature_maps[idx]->n; p_idx++) {
            normalized_value = k_min +
                ( ((feature_map->feature_maps[idx]->val[p_idx][0] - f_min) * (k_max - k_min))
                    / (f_max - f_min)
                );
            feature_map->feature_maps[idx]->val[p_idx][0] = normalized_value;
        }
    }
}


void RunSegmentation(int argc, char *argv[]) {
    if (argc != 11) {
        iftError(
            "Usage: segmentation <...>\n"
            "[1] mode: generate kernels, convolve, segment, assess [0|1|2|3]. \n"
            "[2] feature maps txt. \n"
            "[3] input image (FLAIR). \n"
            "[4] seeds path (FLAIR). \n"
            "[5] input image (T1GD). \n"
            "[6] seeds path (T1GD). \n"
            "[7] output label path. \n"
            "[8] alpha [0,1]. \n"
            "[9] Watershed method for flair [0: Normal | 1: Oriented]. \n"
            "[10] Watershed method for t1gd [0: Normal | 1: Oriented]. \n",
            "segmentation"
        );
    }

    iftImage *input_image_flair = iftReadImageByExt(argv[3]);
    iftLabeledSet *seeds_flair = iftReadSeeds(input_image_flair, argv[4]);
    iftImage *input_image_t1gd = iftReadImageByExt(argv[5]);
    iftLabeledSet *seeds_t1gd = iftReadSeeds(input_image_t1gd, argv[6]);

    // Reads feature_maps.txt
    printf("[INFO] Reading feature_maps.txt\n");
    FILE *fp = fopen(argv[2],"r");
    int n_features, feature_id;
    char* file_name_t1gd = iftFilename(argv[5], ".nii.gz");
    char* file_name_flair = iftFilename(argv[3], ".nii.gz");
    printf("[INFO] Processing t1gd image: %s\n", file_name_t1gd);
    char feature_maps_path[100];
    char feature_map_path[200];
    fscanf(fp, "%s", feature_maps_path);
    fscanf(fp, "%d", &n_features);
    FeatureMaps t1gd_features;
    t1gd_features.n_features = n_features;
    t1gd_features.feature_maps = malloc(n_features * sizeof(iftMImage));
    t1gd_features.weights = malloc(n_features * sizeof(float));
    for (size_t idx = 0; idx < n_features; idx++) {
        fscanf(fp, "%d\t%f", &feature_id, &t1gd_features.weights[idx]);
        sprintf(
            feature_map_path, "%s/%s_%d.mimg",
            feature_maps_path, file_name_t1gd, feature_id
        );
        printf("[INFO] Reading feature_map %s with weight %f\n",
                feature_map_path, t1gd_features.weights[idx]
        );
        t1gd_features.feature_maps[idx] = iftReadMImage(feature_map_path);
    }
    printf("[INFO] Processing t2flair image: %s\n", file_name_flair);
    fscanf(fp, "%s", feature_maps_path);
    fscanf(fp, "%d", &n_features);
    FeatureMaps t2flair_features;
    t2flair_features.n_features = n_features;
    t2flair_features.feature_maps = malloc(n_features * sizeof(iftMImage));
    t2flair_features.weights = malloc(n_features * sizeof(float));
    for (size_t idx = 0; idx < n_features; idx++) {
        fscanf(fp, "%d\t%f", &feature_id, &t2flair_features.weights[idx]);
        sprintf(
            feature_map_path, "%s/%s_%d.mimg",
            feature_maps_path, file_name_flair, feature_id
        );
        printf("[INFO] Reading feature_map %s with weight %f\n",
                feature_map_path, t2flair_features.weights[idx]
        );
        t2flair_features.feature_maps[idx] = iftReadMImage(feature_map_path);
    }
    fclose(fp);

    // NormalizeFeatureMaps(&t1gd_features);
    // NormalizeFeatureMaps(&t2flair_features);

    // Merge feature maps
    iftAdjRel *At1gd = iftSpheric(2.0);
    iftAdjRel *At2flair = iftSpheric(1.0);
    iftImage *t1gd_merged = MergeFeatureMaps(t1gd_features, 3);
    iftImage *t2flair_merged = MergeFeatureMaps(t2flair_features, 11);
    iftImage *t1gd_merged_G = iftImageGradientMagnitude(t1gd_merged, At1gd);
    iftImage *t2flair_merged_G = iftImageGradientMagnitude(t2flair_merged, At2flair);
    char merged_path[200];
    sprintf(merged_path, "data/merged/%s_merged.scn", file_name_t1gd);
    iftWriteImageByExt(t1gd_merged, merged_path);
    sprintf(merged_path, "data/merged/%s_merged_G.scn", file_name_t1gd);
    iftWriteImageByExt(t1gd_merged_G, merged_path);
    sprintf(merged_path, "data/merged/%s_merged.scn", file_name_flair);
    iftWriteImageByExt(t2flair_merged, merged_path);
    sprintf(merged_path, "data/merged/%s_merged_G.scn", file_name_flair);
    iftWriteImageByExt(t2flair_merged_G, merged_path);

    iftImage *label_t1gd = NULL;
    iftImage *label_flair = NULL;

    label_t1gd = SegmentByWatershed(
        input_image_t1gd, seeds_t1gd, t2flair_merged_G, atof(argv[8]), atoi(argv[10])
    );
    label_flair = SegmentByWatershed(
        input_image_flair, seeds_flair, t1gd_merged_G, atof(argv[8]), atoi(argv[9])
    );

    // Merge labels
    /* Guidelines for correctly annotating images using FLIMBuilder:
    On T2-FLAIR:
        - Annotate Peritumoral edematous / invaded tissue with label 2;
    On T1-GD:
        - Annotate Necrotic Tumor Core with label 1;
        - Annotate Gd-enhancing tumor with label 3;
    */
    iftImage *label_img = MergeLabels(label_flair, label_t1gd);
    iftWriteImageByExt(label_img, argv[7]);

    // Releases allocated resources
    for (size_t idx = 0; idx < t1gd_features.n_features; idx++) {
        iftDestroyMImage(&t1gd_features.feature_maps[idx]);
    }
        for (size_t idx = 0; idx < t2flair_features.n_features; idx++) {
        iftDestroyMImage(&t2flair_features.feature_maps[idx]);
    }
    iftDestroyAdjRel(&At1gd);
    iftDestroyAdjRel(&At2flair);
    iftDestroyImage(&t1gd_merged);
    iftDestroyImage(&t2flair_merged);
    iftDestroyImage(&t1gd_merged_G);
    iftDestroyImage(&t2flair_merged_G);
    iftDestroyImage(&label_flair);
    iftDestroyImage(&input_image_flair);
    iftDestroyLabeledSet(&seeds_flair);
    iftDestroyImage(&label_t1gd);
    iftDestroyImage(&input_image_t1gd);
    iftDestroyLabeledSet(&seeds_t1gd);
    iftDestroyImage(&label_img);
}