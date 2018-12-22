/*******************************************************************\
| Types.h
|
|--------------------------------------------------------------------
| Copyright 2003. Martin Fleisz.
| All rights reserved.
|
|--------------------------------------------------------------------
| CREATED:		2003/02/09
| AUTHOR:		Martin Fleisz
|
|--------------------------------------------------------------------
| DESCRIPTION:
| 
| Implementation of the following data/math types:
|
| structs:
|   integer and floating point data types
|	vector
|   bounding box
|   plane
\********************************************************************/

#ifndef __MATHTYPES_H
#define __MATHTYPES_H

#include "Core.h"
#include <math.h>

// math defines
#define PI ((float32) 3.141592654f)
#define DEG2RAD(a)	((a) * (PI / 180.0f))
#define RAD2DEG(a)	((a) * (180.0f / PI))


typedef struct _Vertex
{
	float32	xyz[3];
	float32 nxyz[3];
} Vertex;


// vector class
class cVector
{
	public:
		float32 x, y, z;

	public:
		
		cVector() { }
		cVector(float32 a, float32 b, float32 c) : x(a), y(b), z(c) { }
		cVector(cVector &v) : x(v.x), y(v.y), z(v.z) { }
		cVector(Vertex &v) : x(v.xyz[0]), y(v.xyz[1]), z(v.xyz[2]) { }

		FORCEINLINE float32 getX() { return x; }
		FORCEINLINE float32 getY() { return y; }
		FORCEINLINE float32 getZ() { return z; }
		
		FORCEINLINE void	setX(float32 a) { x = a; }
		FORCEINLINE void	setY(float32 a) { y = a; }
		FORCEINLINE void	setZ(float32 a) { z = a; }

		inline float& operator[](int col)
		{
			if(col == 0)
				return x;
			if(col == 1)
				return y;
			if(col == 2)
				return z;	
			return z;
		}

		inline cVector&	operator+=(const cVector &v)
		{
            x += v.x;
			y += v.y;
			z += v.z;

			return (*this);
		}

		inline cVector&	operator-=(const cVector &v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;

			return (*this);
		}

		// dot product
		inline float32 operator*=(const cVector &v)
		{
            return ((x * v.x) + (y * v.y) + (z * v.z));
		}

		inline cVector&	operator*=(const float32 &f)
		{
			x *= f;
			y *= f;
			z *= f;

			return (*this);
		}

		// for *= TMatrix operations
		inline cVector&	operator*=(const float32 *m)
		{
			float32 tx = x;
			float32 ty = y;
			float32 tz = z;

			x = tx * m[0] + ty * m[1] + tz * m[2] + m[3];
			y = tx * m[4] + ty * m[5] + tz * m[6] + m[7];
			z = tx * m[8] + ty * m[9] + tz * m[10] + m[11];

			return (*this);
		}

		inline cVector&	operator/=(const float32 &f)
		{
			x /= f;
			y /= f;
			z /= f;

			return (*this);
		}

		// friend function for addition, substraction, dot and cross product
		inline friend cVector operator+(const cVector &a,const cVector &b)
		{
			cVector v;

			v.x = a.x + b.x;
			v.y = a.y + b.y;
			v.z = a.z + b.z;

			return v;
		}

		inline friend cVector operator-(const cVector &a,const cVector &b)
		{
			cVector v;

			v.x = a.x - b.x;
			v.y = a.y - b.y;
			v.z = a.z - b.z;

			return v;
		}

		// dot product
		inline friend float32 operator*(const cVector &a,const cVector &b)
		{
			return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
		}

		inline friend cVector operator*(const cVector &a,const float32 f)
		{
			return cVector((f * a.x), (f * a.y), (f * a.z));
		}
		
		inline friend cVector operator/(const cVector &a,const float32 f)
		{
			return cVector((a.x / f), ( a.y / f), (a.z / f));
		}

		inline cVector&	operator=(const cVector &v)
		{
            x = v.x;
			y = v.y;
			z = v.z;

			return (*this);
		}

		inline int	operator==(const cVector &v)
		{
            if(x == v.x && y == v.y && z == v.z)
				return TRUE;

			return FALSE;
		}
	
		inline int	operator!=(const cVector &v)
		{
			if(x != v.x || y != v.y || z != v.z)
				return TRUE;

			return FALSE;
		}

        inline float32 length()
		{
			return (float32)(sqrt((x * x) + (y * y) + (z * z)));
		}

		inline void normalize()
		{
			float32 dom =length();
			float32 magnitude;

			// preventing zero division
			if(dom) 
			{
				magnitude = 1.0f / dom;

				x = x * magnitude;
				y = y * magnitude;
				z = z * magnitude;

				return;
			}

            z = 1.0f;			
		}

		// friend function for normalize
		inline friend cVector normalize(cVector &v)
		{
			float32 dom = v.length();
			float32 magnitude;

			// preventing zero division
			if(dom) 
			{
				magnitude = 1.0f / dom;

				return cVector(v.x * magnitude, v.y * magnitude, v.z * magnitude);
			}

			return v;
		}


		inline cVector&	cross(cVector &v)
		{
			x = (y * v.z) - (z * v.y);
			y = (z * v.x) - (x * v.z);
			z = (x * v.y) - (y * v.x);

			return (*this);
		}



		// friend function for cross product
		inline friend cVector cross(const cVector &a,const cVector &b)
		{
			cVector v;

			v.x = (a.y * b.z) - (a.z * b.y);
			v.y = (a.z * b.x) - (a.x * b.z);
			v.z = (a.x * b.y) - (a.y * b.x);

			return v;
		}

		inline void lerp(const cVector &s, const cVector &e, float32 v)
		{
			float32 t = 1.0f - v;

			x = (t * s.x) + (v * e.x);
			y = (t * s.y) + (v * e.y);
			z = (t * s.z) + (v * e.z);
		}

		inline void lerp(const cVector &e, float32 v)
		{
			float32 t = 1.0f - v;

			x = (t * x) + (v * e.x);
			y = (t * y) + (v * e.y);
			z = (t * z) + (v * e.z);
		}
};

class cPlane
{
	public:

		cVector	n;	// norm vector of the plane
		cVector	p;	// point on the plane

		cPlane() { }
		cPlane(cVector &vn, cVector &vp) : n(vn), p(vp) { }

		// calcs the intersectoin point of this plane and a given line (AB)
		// returns TRUE if a result was found
		// For a description of this formular look at http://astronomy.swin.edu.au/~pbourke/geometry/planeline/
		inline uint32 getPlaneLineIntersection(cVector &lineA, cVector &lineB, cVector &intersection)
		{
            float32 a = n * (p - lineA);
			float32 b = n * (lineB - lineA);

			// if b is 0.0f the line is either completely on the plane (infinit results) or 
			// is parallel to the plane (no solution)
            if(b == 0.0f)
				return FALSE;

			float32 u = a / b;

			// check if intersection is really inside the line's length
			if(u >= 1.0f || u <= 0.0f)
				return FALSE;

            intersection = lineA + ((lineB - lineA) * u);

			// we have a result
			return TRUE;
		}

		// cals the distance from the point to the plane
		// Taken from http://astronomy.swin.edu.au/~pbourke/geometry/pointplane/
        inline float32 getDistance(cVector &pt)
		{
            return (((pt - p) * n) / n.length());
		}
};


// axis-alligned bounding box
class cAABoundingBox
{
	public:
		cVector minPt;
		cVector centerPt;
		cVector maxPt;


		float32 getAxisLen(uint32 i)
		{
			// get the projection of the distance from min to max 
			// onto our requested axis
			cVector axis(0.0f, 0.0f, 0.0f);
			axis[i] = 1.0f;
			return (axis * (maxPt - minPt));
		}

		float32 getSplitPercent(cPlane &p_Plane)
		{
			return ((p_Plane.n * (p_Plane.p - minPt)) / (p_Plane.n * (maxPt - minPt)));
		}
};





#endif // FYP_MATHTYPES_H