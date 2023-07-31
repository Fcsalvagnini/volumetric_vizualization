#ifndef _GRAPHICAL_CONTEXT_
#define _GRAPHICAL_CONTEXT_

#include "ift.h"
#include "scene_data.h"

typedef struct phong_model {
    float      ka;     /* ambient reflection constant  (0.1 by default) */
    float      kd;     /* diffuse reflection constant  (0.7 by default) */
    float      ks;     /* specular refrection constant (0.2 by default) */
    float      ns;     /* exponent of the specular reflection component (5 by default) */
    float      dMin;   /* minimum distance from visualization plate to object surface*/
    float      dMax;   /* maximum distance from visualization plate to object surface*/
    int        ra;     /* Maximum intensity value */
    iftVector *normal; /* normal table used to speed up shading */
} PhongModel;

typedef struct object {
    int visibility; // 0 | 1 for object visibility
    bool viewed; // false | true for visualized object
    float opacity;
    float colorReflectance[3];
    iftColor colorReflectanceYCbCr;
} Object;

typedef struct graphical_context {
    Face *sceneFaces;
    iftImage *normalIndexes;
    int normalMethod; // normal estimation (0: scene-based | 1: object-based)
    iftImage *distances;
    Vertex *sceneVertexes;
    PhongModel *phongModel;
    int nObjects;
    Object *sceneObjects;


} GraphicalContext;

GraphicalContext *GetGraphicalContext(
    iftImage *img, iftImage *mask
);

void SetObjects(GraphicalContext *graphicalContext);

void SetPhongModel(GraphicalContext *graphicalContext);

void SetRandomRGB(GraphicalContext *graphicalContext, int objectIdx);

void DestroyGraphicalContext(GraphicalContext *graphicalContext);

iftSet *GetMaskObjectBorder(iftImage *mask, int label);

void UpdatesGraphicalContext(
    iftImage* img, iftImage* maskImg,  iftImage* renderedImg,
    GraphicalContext* graphicalContext, iftMatrix* pointwisePlaneOps,
    iftMatrix* planeNormalTransformed, iftMatrix* pMatrix, iftMatrix* p0Matrix
);

#endif