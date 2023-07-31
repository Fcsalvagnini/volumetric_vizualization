#include "convolution.h"

/*
Execution sample:

./bin/main 1 data/kernels/2_kernels_3_bands_3.000000_sph_adj.mmk \
data/input_images/ data/image_list.txt data/feature_maps/2k_3b_3sph/
*/

CustomKernelBank *MMKernelToMKernels(iftMMKernel *kernel_bank) {
    int n_kernels = kernel_bank->nkernels;
    CustomKernelBank *kernels = malloc(n_kernels * sizeof(CustomKernelBank));
    for (size_t k_idx = {0}; k_idx < n_kernels; k_idx++) {
        kernels[k_idx].i_min = IFT_INFINITY_FLT;
        kernels[k_idx].i_max = IFT_INFINITY_FLT_NEG;
        kernels[k_idx].kernel = iftCreateMKernel(
            kernel_bank->A, kernel_bank->nbands
        );
        // Copies weights from MMKernels to each MKernel
        for (size_t b_idx = {0}; b_idx < kernel_bank->nbands; b_idx++) {
            for (size_t adj_idx = {0}; adj_idx < kernel_bank->A->n; adj_idx++) {
                kernels[k_idx].kernel->weight[b_idx].val[adj_idx] =
                    kernel_bank->weight[k_idx][b_idx].val[adj_idx];
            }
        }
    }

    return kernels;
}

void FreeCustomKernelBank(CustomKernelBank *kernel_bank, int n_kernels) {
    for (size_t k_idx = {0}; k_idx < n_kernels; k_idx++) {
        iftDestroyMKernel(&kernel_bank[k_idx].kernel);
    }
    free(kernel_bank);
}

void MinMaxNormalization(
    iftMImage *feature_map, float f_min, float f_max, float k_min, float k_max
)
{
    float normalized_value;
    for (size_t n = {0}; n < feature_map->n; n++) {
        normalized_value = k_min +
            ( ((feature_map->val[n][0] - f_min) * (k_max - k_min))
                / (f_max - f_min)
            );
        feature_map->val[n][0] = normalized_value;
    }
}

void ReLU(iftMImage *feature_map) {
    for (size_t n = {0}; n < feature_map->n; n++) {
        if (feature_map->val[n][0] < 0) {
            feature_map->val[n][0] = 0;
        }
    }
}

void GetFeatureMaps(int argc, char *argv[]) {
    if (argc != 6) {
        iftError(
            "Usage: convolution <...>\n"
            "[1] mode: generate kernels, convolve, segment, assess [0|1|2|3]. \n"
            "[2] filter bank path. \n"
            "[3] folder to input images. \n"
            "[4] list of images to use (.txt). \n"
            "[5] path to save output feature maps. \n",
            "convolution"
        );
    }
    iftMMKernel* kernel_bank = iftReadMMKernel(argv[2]);
    CustomKernelBank *custom_kernel_bank = MMKernelToMKernels(kernel_bank);

    char* input_images_folder = argv[3];
    char image_name[100];

    FILE *fp = fopen(argv[4],"r");
    int n_images;
    fscanf(fp, "%d", &n_images);
    printf("[INFO] Reading %d images.\n", n_images);
    char** image_path = malloc(n_images * sizeof(char*));
    for (size_t img_idx = {0}; img_idx < n_images; img_idx++) {
        image_path[img_idx] = calloc(200, sizeof(char));
        fscanf(fp, "%s", image_name);
        sprintf(image_path[img_idx], "%s/%s", input_images_folder, image_name);
    }
    fclose(fp);

    // Convolves verifying minimum and maximum for each filter (Across all images)
    iftImage *image = NULL;
    iftMImage *mimage = NULL, *feature_map = NULL;
    float f_max = 0, f_min = 0;
    for (size_t img_idx = {0}; img_idx < n_images; img_idx++) {
        image = iftReadImageByExt(image_path[img_idx]);
        mimage = iftImageToMImage(image, LABNorm2_CSPACE);
        for (size_t k_idx = {0}; k_idx < kernel_bank->nkernels; k_idx++) {
            printf(
                "[INFO|MIN_MAX] Convolving Image %ld by Filter %ld\n",
                img_idx, k_idx
            );
            feature_map = iftMLinearFilter(
                mimage, custom_kernel_bank[k_idx].kernel
            );
            ReLU(feature_map);
            f_max = iftMMaximumValue(feature_map, -1);
            f_min = iftMMinimumValue(feature_map, -1);
            if (f_max > custom_kernel_bank[k_idx].i_max) {
                custom_kernel_bank[k_idx].i_max = f_max;
            }
            if (f_min < custom_kernel_bank[k_idx].i_min) {
                custom_kernel_bank[k_idx].i_min = f_min;
            }
            iftDestroyMImage(&feature_map);
        }
        iftDestroyImage(&image);
        iftDestroyMImage(&mimage);
    }

    // Convolves saving feature maps
    char* output_folder = argv[5];
    char feature_maps_folder[100];
    char filename_to_write[200];
    for (size_t img_idx = {0}; img_idx < n_images; img_idx++) {
        iftMakeDir(output_folder);
        sprintf(
            feature_maps_folder, "%s/%s",
            output_folder, iftFilename(image_path[img_idx], ".nii.gz")
        );
        image = iftReadImageByExt(image_path[img_idx]);
        mimage = iftImageToMImage(image, LABNorm2_CSPACE);
        for (size_t k_idx = {0}; k_idx < kernel_bank->nkernels; k_idx++) {
            printf(
                "[INFO|SAVING] Convolving Image %ld by Filter %ld\n",
                img_idx, k_idx
            );
            feature_map = iftMLinearFilter(
                mimage, custom_kernel_bank[k_idx].kernel
            );
            ReLU(feature_map);

            // Saves image for watershed usage
            sprintf(
                filename_to_write,"%s_%ld.mimg", feature_maps_folder,
                k_idx
            );
            iftWriteMImage(feature_map,filename_to_write);

            // Saves image for visualization purposes
            MinMaxNormalization(
                feature_map, custom_kernel_bank[k_idx].i_min,
                custom_kernel_bank[k_idx].i_max, 0,
                65535
            );
            sprintf(
                filename_to_write,"%s_%ld_vis.mimg", feature_maps_folder,
                k_idx
            );
            iftWriteMImage(feature_map,filename_to_write);
            iftDestroyMImage(&feature_map);
        }
        iftDestroyImage(&image);
        iftDestroyMImage(&mimage);
    }

    FreeCustomKernelBank(custom_kernel_bank, kernel_bank->nkernels);
    iftDestroyMMKernel(&kernel_bank);
}