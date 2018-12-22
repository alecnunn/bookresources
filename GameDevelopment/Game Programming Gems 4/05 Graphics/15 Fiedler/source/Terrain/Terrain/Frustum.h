// Frustum class

#pragma once

#include "Mathematics/Vector.h"
#include "Mathematics/Matrix.h"


/// Stores camera transform and clip plane information used for frustum culling, occlusion and screen space error metrics.

class Frustum
{
public:

    Mathematics::Vector eye;                ///< the eye position
    Mathematics::Vector base;               ///< the base position below the eye of the screen (eye - up * some constant)
    Mathematics::Vector direction;          ///< view direction from eye position.
    Mathematics::Matrix matrix;             ///< concatenated world, view, projection, viewport matrix for transforms
    float width;                            ///< screen width in pixels
    float height;                           ///< screen height in pixels
    float k;                                ///< k = screen width / (2 * tan(fov/2));
};

