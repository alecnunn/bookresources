// Cylinder primitive

#pragma once

#include "Vector.h"
#include "Matrix.h"


namespace Mathematics
{
    /// A cylinder solid.
    /// Defined as two points in 3-space and a radius.
    /// Radius squared is calculated and stored for later use.

    class Cylinder
    {
    public:

        /// default constructor.
        /// does nothing for speed

        Cylinder() {};

        /// construct a cylinder from end points and radius.

	    Cylinder(const Vector &a, const Vector &b, float radius)
	    {
		    this->a = a;
		    this->b = b;
		    this->radius = radius;
		    this->radiusSquared = radius * radius;
	    }

        /// construct a cylinder from end points, radius and radius squared.
        /// note: use this method if time is critical and you already have radius
        /// squared pre-calculated.

        Cylinder(const Vector &a, const Vector &b, float radius, float radiusSquared)
	    {
		    this->a = a;
		    this->b = b;
		    this->radius = radius;
		    this->radiusSquared = radiusSquared;
	    }

		/// calculate center of cylinder

		Vector center() const
		{
			return (a+b)*0.5f;
		}

        Vector a;				///< first end point
        Vector b;				///< second end point
        float radius;			///< radius of sphere
        float radiusSquared;    ///< radius of sphere squared
    };
}
