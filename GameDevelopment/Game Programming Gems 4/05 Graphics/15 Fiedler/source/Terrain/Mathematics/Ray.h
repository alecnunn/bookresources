// Ray class

#pragma once

#include "Vector.h"


namespace Mathematics
{
    /// %Ray from origin point in specified direction

    class Ray
    {
    public:

        /// default constructor.
        /// does nothing for speed.

        Ray() {};

		/// construct a ray from given origin and direction.
		/// note: the ray direction must be unit length!

        Ray(const Vector &origin, const Vector &direction)
        {
			assert(direction.normalized());

            this->origin = origin;
            this->direction = direction;
        }

        /// calculate point along ray.

        Vector point(float t) const
        {
            return origin + direction * t;
        }

        Vector origin;          ///< origin of ray
        Vector direction;       ///< direction of ray (always unit length)
    };
}
