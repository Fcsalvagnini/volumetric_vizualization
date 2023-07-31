#include "morph_ops.h"
#include "ift.h"

iftSet *MaskObjectBorder(iftImage *mask) {
    iftAdjRel *A = iftSpheric(1);
    iftSet *S = NULL;
    iftVoxel u, v;
    int q;

    // Iterates over all image pixels to get object border
    for (size_t p = 0; p < mask->n; p++) {
        // Only border values
        if (mask->val[p] == 1) {
            u = iftGetVoxelCoord(mask, p);
            // Iterates of the adjacent voxels
            for (size_t adj_idx = 1; adj_idx < A->n; adj_idx++) {
                v = iftGetAdjacentVoxel(A, u, adj_idx);
                // Verifies if it is a valid voxel
                if (iftValidVoxel(mask, v)) {
                    q = iftGetVoxelIndex(mask, v);
                    if (mask->val[q] == 0) {
                        iftInsertSet(&S, p);
                        break;
                    }
                }
            }
        }
    }

    // Release resources
    iftDestroyAdjRel(&A);

    return S;
}

iftImage *FastDilation(iftImage *mask, iftSet **S, int radius) {
    // Allocate necessary resources
    iftImage *dilated_mask = iftCopyImage(mask);
    // Adjacency relation of neighbourhood ?
    iftAdjRel *A = iftSpheric(sqrtf(3));
    int tmp = 0;
    iftGQueue *Q = NULL;
    /* Creates cost and root matix with all values as 0, and then set then
    as INF.*/
    iftImage *cost = iftCreateImageFromImage(mask);
    iftImage *root = iftCreateImageFromImage(mask);
    iftSetImage(cost, IFT_INFINITY_INT);
    iftSetImage(root, IFT_INFINITY_INT);

    Q = iftCreateGQueue(
        255, // n buckets
        dilated_mask->n, // n elements
        cost->val // Elements values
    );

    // Empties S set (Object Border), setting Cot and R images
    int p = 0;
    while (*S != NULL) {
        p = iftRemoveSet(S);
        cost->val[p] = 0;
        root->val[p] = p;
        iftInsertGQueue(&Q, p);
    }

    iftVoxel edge_voxel, u, v;
    int q = 0;
    while (!iftEmptyGQueue(Q)) {
        p = iftRemoveGQueue(Q);

        if (cost->val[p] <= pow(radius, 2)) {
            dilated_mask->val[p] = mask->val[root->val[p]];
            edge_voxel = iftGetVoxelCoord(mask, root->val[p]);

            // Iterates over the adjacent matrix
            u = iftGetVoxelCoord(mask, p);
            for (size_t adj_idx = 1; adj_idx < A->n; adj_idx++) {
                v = iftGetAdjacentVoxel(A, u, adj_idx);
                if (iftValidVoxel(mask, v)) {
                    q = iftGetVoxelIndex(mask, v);
                    if ( (cost->val[q] > cost->val[p]) && mask->val[q] == 0 ) {
                        // Computes the new cost value regarding the edge voxel
                        tmp = pow(v.x - edge_voxel.x, 2)
                                + pow(v.y - edge_voxel.y, 2)
                                + pow(v.z - edge_voxel.z, 2);
                        if (tmp < cost->val[q]) {
                            // Verifies if it is in the Queue and removes it
                            if (Q->L.elem[q].color == IFT_GRAY) {
                                iftRemoveGQueueElem(Q, q);
                            }
                            cost->val[q] = tmp;
                            root->val[q] = root->val[p];
                            iftInsertGQueue(&Q, q);
                        }
                    }
                }
            }
        }
        else {
            // Populates set with background border for erosion
            iftInsertSet(S, p);
        }
    }

    // Releases resources
    iftDestroyImage(&cost);
    iftDestroyImage(&root);
    iftDestroyAdjRel(&A);
    iftDestroyGQueue(&Q);

    return dilated_mask;
}

iftImage *FastErosion(iftImage *mask, iftSet **S, int radius) {
    iftImage *eroded_mask = iftCopyImage(mask);

    // Adjacency relation
    iftAdjRel *A = iftSpheric(sqrtf(3));
    int tmp = 0;
    iftGQueue *Q = NULL;
    /* Creates cost and root matix with all values as 0, and then set then
    as INF_INT.*/
    iftImage *cost = iftCreateImageFromImage(mask);
    iftImage *root = iftCreateImageFromImage(mask);
    iftSetImage(cost, IFT_INFINITY_INT);
    iftSetImage(root, IFT_INFINITY_INT);

    Q = iftCreateGQueue(
        255, // n buckets
        eroded_mask->n, // n elements
        cost->val // Elements values
    );

    // Empties S set (Background Border), setting Cot and R images
    int p = 0;
    while (*S != NULL) {
        p = iftRemoveSet(S);
        cost->val[p] = 0;
        root->val[p] = p;
        iftInsertGQueue(&Q, p);
    }

    iftVoxel edge_voxel, u, v;
    int q = 0;
    while (!iftEmptyGQueue(Q)) {
        p = iftRemoveGQueue(Q);

        if (cost->val[p] <= pow(radius, 2)) {
            eroded_mask->val[p] = mask->val[root->val[p]];
            edge_voxel = iftGetVoxelCoord(mask, root->val[p]);

            // Iterates over adjacent voxels
            u = iftGetVoxelCoord(mask, p);
            for (size_t adj_idx = 1; adj_idx < A->n; adj_idx++) {
                v = iftGetAdjacentVoxel(A, u, adj_idx);
                if (iftValidVoxel(mask, v)) {
                    q = iftGetVoxelIndex(mask, v);
                    if ( (cost->val[q] > cost->val[p]) && mask->val[q] == 1 ) {
                        // Computes the new cost value regarding the edge voxel
                        tmp = pow(v.x - edge_voxel.x, 2)
                                + pow(v.y - edge_voxel.y, 2)
                                + pow(v.z - edge_voxel.z, 2);
                        if (tmp < cost->val[q]) {
                            // Verifies if its in the Queue and removes it
                            if (Q->L.elem[q].color == IFT_GRAY) {
                                iftRemoveGQueueElem(Q, q);
                            }
                            cost->val[q] = tmp;
                            root->val[q] = root->val[p];
                            iftInsertGQueue(&Q, q);
                        }
                    }
                }
            }
        }
    }

    // Releases resources
    iftDestroyImage(&cost);
    iftDestroyImage(&root);
    iftDestroyAdjRel(&A);
    iftDestroyGQueue(&Q);

    return eroded_mask;
}

iftImage *FastClosing(iftImage *mask, int radius) {
    // Allocate necessary resources
    iftImage *padded_mask = iftAddFrame(mask, radius + 2, 0);
    iftSet *S = MaskObjectBorder(padded_mask);
    // iftImage *border = iftCreateImage(padded_mask->xsize, padded_mask->ysize, padded_mask->zsize);
    // iftSetImage(border, 0);
    // iftSetToImage(S, border, 1);
    // iftWriteImageByExt(border, "test_border.nii.gz");
    iftImage *dilated_mask = FastDilation(padded_mask, &S, radius);
    iftImage *eroded_mask = FastErosion(dilated_mask, &S, radius);
    iftImage *closed_mask = iftRemFrame(eroded_mask, radius + 2);

    // Free resources
    iftDestroyImage(&padded_mask);
    iftDestroyImage(&dilated_mask);
    iftDestroyImage(&eroded_mask);
    iftDestroySet(&S);

    return closed_mask;
}