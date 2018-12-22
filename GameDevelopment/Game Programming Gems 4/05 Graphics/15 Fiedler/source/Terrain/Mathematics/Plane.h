// Plane primitive

#pragma once

#include "Vector.h"
#include "Matrix.h"


namespace Mathematics
{
    /// A plane in 3-space.
    /// Defined by a point and a normal.

    class Plane
    {
    public:

        /// default constructor.
        /// does nothing for speed.

	    Plane() {}

        /// construct plane from a point and normal.

	    Plane(const Vector &point, const Vector &normal)
	    {
            this->point = point;
            this->normal = normal;
	    }

        /// construct plane from a normal and d.

	    Plane(const Vector &normal, float d)
	    {
            this->point = normal * d;
            this->normal = normal;
	    }

        /// calculate minimum distance from point to plane

	    float distance(const Vector &point) const
	    {
		    Vector p = point - this->point;
		    return normal.dot(p);
	    }

        /// calculate nearest point on plane to point

        Vector nearest(const Vector &point) const
        {
            return point - (normal.dot(point) + this->point.dot(normal)) * normal;
        }

		/// calculate d for plane equation

		float d() const
		{
			return point.dot(normal);
		}

		/// transform plane by matrix.

		Plane transform(const Matrix &matrix, const Matrix &inverse) const
		{
			Plane plane;
			plane.point = matrix * point;
			plane.normal = normal * inverse;	// note: equivalent to inverse.transpose() * normal
			plane.normal.normalize();
			return plane;
		}

		/// get basis vectors for 

		void basis(Vector &u, Vector &v) const
		{
			u = normal.perpendicular();
			v = normal.cross(u);
		}

		/// get plane matrix that transforms parametric plane coordinates (u,v) into world (x,y,z) coordinates

		Matrix matrix() const
		{
			Vector u,v;
			Vector::orthonormal_basis(normal, u, v);

			Matrix matrix(u,v,normal);

            matrix.m14 = -point.x;
            matrix.m24 = -point.y;
            matrix.m34 = -point.z;

			return matrix;
		}

		/// project a point in three space into the plane

		Vector project(const Vector &u, const Vector &v, const Vector &point) const
		{
			return Vector(point.dot(u), point.dot(v), 0);
		}

		/// unproject a point in plane space to three space

		Vector unproject(const Vector &u, const Vector &v, const Vector &point) const
		{
			return u*point.x + v*point.y;
		}

        Vector point;           ///< a point on the plane
        Vector normal;          ///< normal of the plane
    };
}
