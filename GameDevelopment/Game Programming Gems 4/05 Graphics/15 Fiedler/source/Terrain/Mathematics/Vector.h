// Vector class

#pragma once

#include <stdio.h>
#include "Float.h"


namespace Mathematics
{
    /// A vector in 3-space.
    /// Typically used to represent points and vectors in homogenous coordinates for
    /// use with 4x4 matrices (assumes w=1). This is less than ideal and in the future we intend
    /// to templatize this class, optimize it with template metaprogramming and
    /// offer a range of pre-fab vector classes Vector<2>, Vector<3>, Vector<4> etc.

    class Vector
    {
    public:

        /// default constructor.
        /// does nothing for speed.

        Vector() {}

        /// construct vector from x,y,z components.

        Vector(float x, float y, float z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        /// set vector to zero.

        void zero()
        {
            x = 0;
            y = 0;
            z = 0;
        }

        /// negate vector.

	    void negate()
	    {
		    x = -x;
		    y = -y;
		    z = -z;
	    }

        /// add another vector to this vector.

        void add(const Vector &vector)
        {
            x += vector.x;
            y += vector.y;
            z += vector.z;
        }

        /// subtract another vector from this vector.

        void subtract(const Vector &vector)
        {
            x -= vector.x;
            y -= vector.y;
            z -= vector.z;
        }

        /// multiply this vector by a scalar.

        void multiply(float scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
        }

        /// divide this vector by a scalar.

	    void divide(float scalar)
	    {
		    assert(scalar!=0);
		    const float inv = 1.0f / scalar;
		    x *= inv;
		    y *= inv;
		    z *= inv;
	    }

        /// calculate dot product of this vector with another vector.

        float dot(const Vector &vector) const
        {
            return x * vector.x + y * vector.y + z * vector.z;
        }

        /// calculate cross product of this vector with another vector.

	    Vector cross(const Vector &vector) const
        {
            return Vector(y * vector.z - z * vector.y, z * vector.x - x * vector.z, x * vector.y - y * vector.x);
        }

        /// calculate cross product of this vector with another vector, store result in parameter.

	    void cross(const Vector &vector, Vector &result) const
        {
            result.x = y * vector.z - z * vector.y;
            result.y = z * vector.x - x * vector.z;
            result.z = x * vector.y - y * vector.x;
        }

		/// calculate a unit length vector perpendicular to this one

		Vector perpendicular() const
		{
			if (Float::abs(x)>Float::abs(y))
			{
				const float length = Float::sqrt(x*x + z*z);		
				return Vector(z/length, 0.0f, -x/length);
			}
			else
			{
				const float length = Float::sqrt(y*y + z*z);		
				return Vector(0.0f, z/length, -y/length);
			}
		}

        /// calculate length of vector squared

        float length_squared() const
        {
            return x*x + y*y + z*z;
        }

        /// calculate length of vector.
        /// warning: requires slow sqrt!

        float length() const
        {
		    return Float::sqrt(x*x + y*y + z*z);
        }

        /// return unit length copy of vector

        Vector unit() const
        {
            const float magnitude = Float::sqrt(x*x + y*y + z*z);
            if (magnitude>Float::epsilon)
            {
                const float scale = 1.0f / magnitude;
				return Vector(x*scale, y*scale, z*scale);
            }
			else
				return Vector(0,0,1);
        }

        /// normalize vector and return length of vector before normalization

        float normalize()
        {
            const float magnitude = Float::sqrt(x*x + y*y + z*z);
            if (magnitude>Float::epsilon)
            {
                const float scale = 1.0f / magnitude;
                x *= scale;
                y *= scale;
                z *= scale;
            }
		    return magnitude;
        }

        /// test if vector is normalized.

	    bool normalized() const
	    {
		    return Float::equal(length(),1);
	    }

		// generate orthonormal basis, given single input vector u.
		// returns two other unit length basis vectors v,w.
		// note: u must be unit length!

		static void orthonormal_basis(const Vector &u, Vector &v, Vector &w)
		{
			assert(u.normalized());

			if (Float::abs(u.x)>=Float::abs(u.y))
			{
				// u.x or u.z is the largest magnitude component, snap them
				const float inverse_length = 1.0f / Float::sqrt(u.x*u.x+u.z*u.z);
				w.x = -u.z*inverse_length;
				w.y = 0.0f;
				w.z = +u.x*inverse_length;
			}
			else
			{
				// u.y or u.z is the largest magnitude component, snap them
				const float inverse_length = 1.0f / Float::sqrt(u.y*u.y+u.z*u.z);
				w.x = 0.0f;
				w.y = +u.z*inverse_length;
				w.z = -u.y*inverse_length;
			}

			v = u.cross(w);

			assert(w.normalized());
			assert(v.normalized());
			assert(Float::equal(w.dot(v),0));
			assert(Float::equal(v.dot(u),0));
			assert(Float::equal(u.dot(w),0));
		}

        /// equals operator

	    bool operator ==(const Vector &other) const
	    {
		    if (Float::equal(x,other.x) && Float::equal(y,other.y) && Float::equal(z,other.z)) return true;
		    else return false;
	    }

        /// not equals operator

	    bool operator !=(const Vector &other) const
	    {
		    return !(*this==other);
	    }

        /// print a basic representation of this vector to stdout

	    void print() const
	    {
		    printf("Vector[%f, %f, %f]\n", x, y, z);
	    }

	    friend inline Vector operator-(const Vector &a);
	    friend inline Vector operator+(const Vector &a, const Vector &b);
	    friend inline Vector operator-(const Vector &a, const Vector &b);
	    friend inline Vector operator*(const Vector &a, const Vector &b);
	    friend inline Vector& operator+=(Vector &a, const Vector &b);
	    friend inline Vector& operator-=(Vector &a, const Vector &b);
	    friend inline Vector& operator*=(Vector &a, const Vector &b);

	    friend inline Vector operator*(const Vector &a, float s);
	    friend inline Vector operator/(const Vector &a, float s);
	    friend inline Vector& operator*=(Vector &a, float s);
	    friend inline Vector& operator/=(Vector &a, float s);
	    friend inline Vector operator*(float s, const Vector &a);
	    friend inline Vector& operator*=(float s, Vector &a);

        float x;        ///< x component of vector
        float y;        ///< y component of vector
        float z;        ///< z component of vector
    };


    inline Vector operator-(const Vector &a)
    {
	    return Vector(-a.x, -a.y, -a.z);
    }

    inline Vector operator+(const Vector &a, const Vector &b)
    {
	    return Vector(a.x+b.x, a.y+b.y, a.z+b.z);
    }

    inline Vector operator-(const Vector &a, const Vector &b)
    {
	    return Vector(a.x-b.x, a.y-b.y, a.z-b.z);
    }

    inline Vector operator*(const Vector &a, const Vector &b)
    {
	    return Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }

    inline Vector& operator+=(Vector &a, const Vector &b)
    {
	    a.x += b.x;
	    a.y += b.y;
	    a.z += b.z;
	    return a;
    }

    inline Vector& operator-=(Vector &a, const Vector &b)
    {
	    a.x -= b.x;
	    a.y -= b.y;
	    a.z -= b.z;
	    return a;
    }

    inline Vector& operator*=(Vector &a, const Vector &b)
    {
        const float cx = a.y * b.z - a.z * b.y;
        const float cy = a.z * b.x - a.x * b.z;
        const float cz = a.x * b.y - a.y * b.x;
	    a.x = cx;
	    a.y = cy;
	    a.z = cz;
	    return a;
    }

    inline Vector operator*(const Vector &a, float s)
    {
	    return Vector(a.x*s, a.y*s, a.z*s);
    }

    inline Vector operator/(const Vector &a, float s)
    {
	    assert(s!=0);
	    return Vector(a.x/s, a.y/s, a.z/s);
    }

    inline Vector& operator*=(Vector &a, float s)
    {
	    a.x *= s;
	    a.y *= s;
	    a.z *= s;
	    return a;
    }

    inline Vector& operator/=(Vector &a, float s)
    {
	    assert(s!=0);
	    a.x /= s;
	    a.y /= s;
	    a.z /= s;
	    return a;
    }

    inline Vector operator*(float s, const Vector &a)
    {
	    return Vector(a.x*s, a.y*s, a.z*s);
    }

    inline Vector& operator*=(float s, Vector &a)
    {
	    a.x *= s;
	    a.y *= s;
	    a.z *= s;
	    return a;
    }
}
