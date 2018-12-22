// Sphere primitive

#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "Plane.h"


namespace Mathematics
{
    /// A sphere solid.
    /// Defined as a point in 3-space and a radius.
    /// Radius squared is calculated and stored for later use.

    class Sphere
    {
    public:

        /// default constructor.
        /// does nothing for speed

        Sphere() {};

        /// construct a sphere from center point and radius.

	    Sphere(const Vector &center, float radius)
	    {
		    this->center = center;
		    this->radius = radius;
		    this->radiusSquared = radius * radius;
	    }

        /// construct a sphere from center point, radius and radius squared.
        /// note: use this method if time is critical and you already have radius
        /// square pre-calculated.

        Sphere(const Vector &center, float radius, float radiusSquared)
	    {
		    this->center = center;
		    this->radius = radius;
		    this->radiusSquared = radiusSquared;
	    }

        /// calculate distance from point to sphere surface.

	    float distance(const Vector &point) const
	    {
		    Vector d(point);
		    d -= center;
		    const float distance = d.length();
		    return distance - radius;
	    }

        /// calculate sphere normal at point.

	    Vector normal(const Vector &point) const
	    {
		    Vector normal = point - center;
		    normal.normalize();
		    return normal;
	    }

        /// test if point is inside sphere.

	    bool inside(const Plane &plane) const
	    {
		    float distance = plane.distance(center);

		    return distance > -radius;
	    }

        /// calculate sphere volume.

	    float volume() const
	    {
		    return 4.0f/3.0f * Float::pi * radius * radius * radius;
	    }

        /// calculate sphere inertia tensor.

	    Matrix tensor(float mass) const
	    {
		    const float A = 2.0f / 5.0f * mass * (radius * radius);
		    Matrix matrix;
		    matrix.diagonal(A, A, A);
		    return matrix;
	    }

        Vector center;          ///< center of sphere
        float radius;			///< radius of sphere
        float radiusSquared;    ///< radius of sphere squared
    };
}