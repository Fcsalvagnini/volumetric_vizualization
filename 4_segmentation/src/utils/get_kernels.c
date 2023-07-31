#include "ift.h"
#include "get_kernels.h"
#include "stdlib.h"

/*
Execution Sample:
./bin/main 0 2 3 3 data/kernels
*/

void InitializeRandomKernels(iftMMKernel* kernel_bank) {
    float avg, std, random_number = 0;
    for (size_t k_idx = {0}; k_idx < kernel_bank->nkernels; k_idx++) {
        avg = 0;
        std = 0;
        // Creates each kernel band and calculates avg
        kernel_bank->bias[k_idx] = 0; // 0 Bias

        // Sets random values to weights
        for (size_t b_idx = {0}; b_idx < kernel_bank->nbands; b_idx++) {
            // Iterates over each kernel value
            for (size_t adj_idx = {0}; adj_idx < kernel_bank->A->n; adj_idx++)
            {
                random_number = ((float) rand()) / ((float) RAND_MAX);
                kernel_bank->weight[k_idx][b_idx].val[adj_idx] = random_number;
                avg += random_number;
            }
        }
        avg = avg / ((float)(kernel_bank->nbands) * (float)(kernel_bank->A->n));

        // Computes std
        for (size_t b_idx = {0}; b_idx < kernel_bank->nbands; b_idx++) {
            // Iterates over each kernel value
            for (size_t adj_idx = {0}; adj_idx < kernel_bank->A->n; adj_idx++)
            {
                std += pow(
                    (kernel_bank->weight[k_idx][b_idx].val[adj_idx] - avg), 2);
            }
        }
        std = std / ((float)(kernel_bank->nbands) * (float)(kernel_bank->A->n));

        // Normalizes each filter
        for (size_t b_idx = {0}; b_idx < kernel_bank->nbands; b_idx++) {
            // Iterates over each kernel value
            for (size_t adj_idx = {0}; adj_idx < kernel_bank->A->n; adj_idx++)
            {
                kernel_bank->weight[k_idx][b_idx].val[adj_idx] =
                    (kernel_bank->weight[k_idx][b_idx].val[adj_idx] - avg) / std;
            }
        }
    }
}

void GetKernels(int argc, char *argv[]) {
    if (argc != 6) {
        iftError(
            "Usage: get_kernels <...>\n"
            "[1] mode: generate kernels, convolve, segment, assess [0|1|2|3]. \n"
            "[2] n kernels. \n"
            "[3] n bands. \n"
            "[4] adjacency radius. \n"
            "[5] path to save output kernels. \n",
            "get_kernels"
        );
    }

    int n_kernels = atoi(argv[2]), n_bands = atoi(argv[3]);
    float adjacency_radius = atof(argv[4]);

    iftAdjRel* spheric_relationship = iftSpheric(adjacency_radius);

    iftMMKernel* random_kernel_bank = iftCreateMMKernel(
        spheric_relationship, n_bands, n_kernels
    );

    InitializeRandomKernels(random_kernel_bank);

    char kernel_bank_path[200];
    sprintf(
        kernel_bank_path, "%s/%d_kernels_%d_bands_%f_sph_adj.mmk",
        argv[5], atoi(argv[2]), atoi(argv[3]), adjacency_radius
    );
    iftWriteMMKernel(random_kernel_bank, kernel_bank_path);

    /*
        Fazer decoder manual. Dar peso para cada saida de filtro, positivo para os que detectam
        e negativo para os que detectam o fundo (-1). Atribuir um peso para cada canal.
        Implement the whole pipeline and then start to experiment with each step, documenting advancment on 
        metrics.
    */

    iftDestroyAdjRel(&spheric_relationship);
    iftDestroyMMKernel(&random_kernel_bank);
}