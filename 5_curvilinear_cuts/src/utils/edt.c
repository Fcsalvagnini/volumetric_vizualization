#include "morph_ops.h"
#include "ift.h"
#include "edt.h"

iftImage *GetEnvelopImage(iftImage *closed_img) {
    // Allocate necessary resources
    iftSet *S = MaskObjectBorder(closed_img);
    // Adjacency relation of neighbourhood
    iftAdjRel *A = iftSpheric(sqrtf(3));
    iftGQueue *Q = NULL;
    /* Creates cost and root matix with all values as 0, and then set then
    as INF.*/
    iftImage *cost = iftCreateImageFromImage(closed_img);
    iftImage *root = iftCreateImageFromImage(closed_img);
    iftSetImage(cost, -1);
    iftSetImage(root, IFT_INFINITY_INT);

    // Initializes cost on label regions
    int p;
    for (p = 0; p < cost->n; p++) {
        if (closed_img->val[p] == 1) {
            cost->val[p] = IFT_INFINITY_INT;
        }
    }


    Q = iftCreateGQueue(
        powf(2, 16) - 1.0, // n buckets
        closed_img->n, // n elements
        cost->val // Elements values
    );
    while (S != NULL) {
        p = iftRemoveSet(&S);
        cost->val[p] = 0;
        root->val[p] = p;
        iftInsertGQueue(&Q, p);
    }

    iftVoxel root_u, u, v;
    int q = 0;
    int tmp = 0;
    while (!iftEmptyGQueue(Q)) {
        p = iftRemoveGQueue(Q);
        u = iftGetVoxelCoord(closed_img, p);
        root_u = iftGetVoxelCoord(closed_img, root->val[p]);

        for (size_t adj_idx = 1; adj_idx < A->n; adj_idx++) {
            v = iftGetAdjacentVoxel(A, u, adj_idx);
            if (iftValidVoxel(closed_img, v)) {
                q = iftGetVoxelIndex(closed_img, v);
                if (cost->val[q] > cost->val[p] && closed_img->val[q] == 1) {
                    tmp = pow(v.x - root_u.x, 2)
                            + pow(v.y - root_u.y, 2)
                            + pow(v.z - root_u.z, 2);
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

    // Release resources
    iftDestroySet(&S);
    iftDestroyAdjRel(&A);
    iftDestroyGQueue(&Q);
    iftDestroyImage(&root);

    return cost;
}