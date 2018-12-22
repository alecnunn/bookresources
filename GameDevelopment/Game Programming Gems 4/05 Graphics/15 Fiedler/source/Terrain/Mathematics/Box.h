// Box primitive

#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "Plane.h"


namespace Mathematics
{
    /// An axis aligned box.

    class Box
    {
    public:

        /// default constructor.
        /// does nothing for speed.

        Box() {};

        /// construct an axis aligned box.

	    Box(const Vector &minimum, const Vector &maximum)
	    {
		    this->minimum = minimum;
		    this->maximum = maximum;
		    center = (maximum + minimum) * 0.5f;
		    delta = maximum - minimum;
		    extents = delta / 2;
	    }

        /// test if box is inside plane.
        /// inside is defined as being on the positive side of the plane,
        /// ie. the side on which the normal points out of.

	    bool inside(const Plane &plane) const
	    {
		    float radius = Float::abs(extents.x*plane.normal.x) +
			               Float::abs(extents.y*plane.normal.y) +
					       Float::abs(extents.z*plane.normal.z);

		    float distance = plane.distance(center);
    		
		    return distance > -radius;
	    }
          
        /// calculate volume of box.

	    float volume() const
	    {
		    return delta.x * delta.y * delta.z;
	    }

        /// calculate inertia tensor.

	    Matrix tensor(float mass) const
	    {
		    Matrix matrix;
            matrix.identity();
            return matrix;
	    }

		/// generate corner vertices of box

		void vertices(Vector vertex[]) const
		{
			vertex[0] = Vector(minimum.x, minimum.y, minimum.z);
			vertex[1] = Vector(minimum.x, maximum.y, minimum.z);
			vertex[2] = Vector(maximum.x, maximum.y, minimum.z);
			vertex[3] = Vector(maximum.x, minimum.y, minimum.z);
			vertex[4] = Vector(minimum.x, minimum.y, maximum.z);
			vertex[5] = Vector(minimum.x, maximum.y, maximum.z);
			vertex[6] = Vector(maximum.x, maximum.y, maximum.z);
			vertex[7] = Vector(maximum.x, minimum.y, maximum.z);
		}

		Vector maximum;         ///< maximum point of box
	    Vector minimum;         ///< minimum point of box
	    Vector center;          ///< center point of box
	    Vector delta;           ///< delta vector (width, height, depth)
	    Vector extents;         ///< extents vector (w/2, h/2, d/2)
    };
}
