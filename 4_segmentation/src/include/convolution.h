#ifndef _CONVOLUTION_
#define _CONVOLUTION_

#include "ift.h"

typedef struct custom_kernel_bank
{
    float i_min, i_max;
    iftMKernel* kernel;
} CustomKernelBank;

CustomKernelBank *MMKernelToMKernels(iftMMKernel *kernel_bank);

void FreeCustomKernelBank(CustomKernelBank *kernel_bank, int n_kernels);

void MinMaxNormalization(
    iftMImage *feature_map, float f_min, float f_max, float k_min, float k_max
);

void GetFeatureMaps(int argc, char *argv[]);

void ReLU(iftMImage *feature_map);

#endif