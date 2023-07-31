#include "phong.h"

iftColor GenerateRGBPhong(float r_p_, float *objectReflectance)
{
    iftColor RGB;
    RGB.val[0] = (int)(r_p_ * objectReflectance[0]);
    RGB.val[1] = (int)(r_p_ * objectReflectance[1]);
    RGB.val[2] = (int)(r_p_ * objectReflectance[2]);

    return RGB;
}

iftColor GetRGBFromPhong(
    int voxelIdx, int label, GraphicalContext* graphicalContext,
    float theta
)
{
    iftColor RGB;
    float r_p_, rd_p_, surfaceDistance;
    float ra_const = graphicalContext->phongModel->ka
                    * graphicalContext->phongModel->ra;

    //Computes reflected light
    if (theta >= 0 && theta < (IFT_PI / 2)) {
        // Do not comput specular component
        float ks;
        if (theta >= (IFT_PI / 4)) {
            ks = 0;
        }
        else {
            ks = graphicalContext->phongModel->ks;
        }

        surfaceDistance = graphicalContext->distances->val[voxelIdx];
        rd_p_ = MAX_INT
        * (
            1.0 - (surfaceDistance - graphicalContext->phongModel->dMin)
            / (graphicalContext->phongModel->dMax - graphicalContext->phongModel->dMin)
        );

        r_p_ = ra_const
        + rd_p_ * (
            graphicalContext->phongModel->kd * cos(theta)
            + ks * pow(cos(2 * theta), graphicalContext->phongModel->ns)
        );
    }
    else {
        r_p_ = ra_const;
    }

    RGB = GenerateRGBPhong(
        r_p_,
        graphicalContext->sceneObjects[label].colorReflectance
    );

    return RGB;
}