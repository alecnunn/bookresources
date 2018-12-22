// Ellipsoid primitive

#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"

#include "Color.h"


namespace Mathematics
{
    /// An oriented ellipsoid solid

    class Ellipsoid
    {
    public:

        /// default constructor.
        /// does nothing for speed.

        Ellipsoid() {}

        /// construct ellipsoid from center position, axis lengths (a,b,c) and a quaternion orientation

	    Ellipsoid(const Vector &center, float a, float b, float c, const Quaternion &rotation)
	    {
		    this->center = center;
		    this->a = a;
		    this->b = b;
		    this->c = c;
		    this->rotation = rotation;

		    Matrix S,R,T;
		    S.diagonal(a, b, c);
		    R = rotation.matrix();
		    T.translate(center.x, center.y, center.z);

		    matrix = T * R * S;
		    inverse = matrix.inverse();
	    }

        /// calculate distance from point to ellipsoid surface.
        /// note: i have reason to believe that this routine may be incorrect.

	    float distance(const Vector &point) const
	    {
		    Vector p = inverse * point;
		    Vector s = p;
		    s.normalize();
		    float sign = 1;
		    if (p.length_squared()<s.length_squared()) sign = -1;
		    s *= matrix;
		    float length = (point - s).length();
		    return length * sign;
	    }
    	
        /// calculate normal at point on surface.
        /// note: i need to verify that this method is correct.

	    Vector normal(const Vector &point) const
	    {
		    Vector p = inverse * point;
		    Vector s = p;
		    s.normalize();
		    return s;
	    }

        /// calculate volume of ellipsoid

	    float volume() const
	    {
		    return 4.0f/3.0f * Float::pi * a * b * c;
	    }
    	
        /// calculate inertia tensor

	    Matrix tensor(float mass) const
	    {
		    const float A = 1.0f / 5.0f * mass * (b*b + c*c);
		    const float B = 1.0f / 5.0f * mass * (a*a + c*c);
		    const float C = 1.0f / 5.0f * mass * (a*a + b*b);
		    Matrix matrix;
		    matrix.diagonal(A, B, C);
		    return matrix;
	    }

	    Vector center;              ///< center of ellipsoid
	    float a;                    ///< extent of ellipsoid along x axis
        float b;                    ///< extent of ellipsoid along y axis
        float c;                    ///< extent of ellipsoid along z axis
	    Quaternion rotation;        ///< orientation of ellipsoid

	    Matrix matrix;              ///< ellipsoid matrix
	    Matrix inverse;             ///< inverse of ellipsoid matrix
    };
}
