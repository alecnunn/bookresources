// Slab class

#pragma once

#include "Mathematics/Vector.h"


namespace Mathematics
{
    /// A solid formed from axis aligned extents in the xy plane and min/max planes with a common normal.

    class Slab
    {
    public:

        Slab() {};

        Slab(const Vector &_normal, float _minimum, float _maximum, float _x1, float _y1, float _x2, float _y2 )
			: normal(_normal), 
			  minimum(_minimum), 
			  maximum(_maximum), 
			  x1(_x1), 
			  y1(_y1), 
			  x2(_x2), 
			  y2(_y2)  {}

        Vector normal;          ///< normal of min/max planes (not required to be unit length)
        float maximum;          ///< plane d with respect to normal for maximum plane
        float minimum;          ///< plane d with respect to normal for minimum plane
        float x1;               ///< minimum x coordinate
        float y1;               ///< minimum y coordinate
        float x2;               ///< maximum x coordinate
        float y2;               ///< maximum y coordinate
    };
}
