// T-Junction Elimination and Retriangulation
// Game Programming Gems 3
// By Eric Lengyel


#include <math.h>


// Epsilon used in triangulation algorithm
const float epsilon = 0.001F;


// Triangle structure holds 3 indexes into a vertex array
struct Triangle
{
	unsigned short	index[3];
};


// Vector3D class represents a direction
class Vector3D
{
	public:
	
		float	x;
		float	y;
		float	z;
		
		Vector3D() {}
		
		Vector3D(float r, float s, float t)
		{
			x = r;
			y = s;
			z = t;
		}
		
		Vector3D& Set(float r, float s, float t)
		{
			x = r;
			y = s;
			z = t;
			return (*this);
		}
		
		Vector3D& operator +=(const Vector3D& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
			return (*this);
		}
		
		Vector3D& operator -=(const Vector3D& v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return (*this);
		}
		
		Vector3D& operator *=(float t)
		{
			x *= t;
			y *= t;
			z *= t;
			return (*this);
		}
		
		Vector3D& operator /=(float t)
		{
			float f = 1.0F / t;
			x *= f;
			y *= f;
			z *= f;
			return (*this);
		}
		
		// Cross product
		Vector3D& operator %=(const Vector3D& v)
		{
			float		r, s;
			
			r = y * v.z - z * v.y;
			s = z * v.x - x * v.z;
			z = x * v.y - y * v.x;
			x = r;
			y = s;
			
			return (*this);
		}
		
		Vector3D operator -(void) const
		{
			return (Vector3D(-x, -y, -z));
		}
		
		Vector3D operator +(const Vector3D& v) const
		{
			return (Vector3D(x + v.x, y + v.y, z + v.z));
		}
		
		Vector3D operator -(const Vector3D& v) const
		{
			return (Vector3D(x - v.x, y - v.y, z - v.z));
		}
		
		// Dot product
		Vector3D operator *(float t) const
		{
			return (Vector3D(x * t, y * t, z * t));
		}
		
		Vector3D operator /(float t) const
		{
			float f = 1.0F / t;
			return (Vector3D(x * f, y * f, z * f));
		}
		
		float operator *(const Vector3D& v) const
		{
			return (x * v.x + y * v.y + z * v.z);
		}
		
		// Cross product
		Vector3D operator %(const Vector3D& v) const
		{
			return (Vector3D(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x));
		}
		
		Vector3D& Normalize(void)
		{
			return (*this /= sqrt(x * x + y * y + z * z));
		}
};


// Point3D class represents a point
class Point3D : public Vector3D
{
	public:
		
		Point3D() {}
		
		Point3D(float r, float s, float t) : Vector3D(r, s, t) {}
		
		Point3D& operator =(const Vector3D& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			return (*this);
		}
		
		Point3D& operator *=(float t)
		{
			x *= t;
			y *= t;
			z *= t;
			return (*this);
		}
		
		Point3D& operator /=(float t)
		{
			float f = 1.0F / t;
			x *= f;
			y *= f;
			z *= f;
			return (*this);
		}
		
		Point3D operator -(void) const
		{
			return (Point3D(-x, -y, -z));
		}
		
		// Sum of point and vector (direction) is a point
		Point3D operator +(const Vector3D& v) const
		{
			return (Point3D(x + v.x, y + v.y, z + v.z));
		}
		
		// Difference of point and vector (direction) is a point
		Point3D operator -(const Vector3D& v) const
		{
			return (Point3D(x - v.x, y - v.y, z - v.z));
		}
		
		// Difference between to points is a vector (direction)
		Vector3D operator -(const Point3D& p) const
		{
			return (Vector3D(x - p.x, y - p.y, z - p.z));
		}
		
		Point3D operator *(float t) const
		{
			return (Point3D(x * t, y * t, z * t));
		}
		
		Point3D operator /(float t) const
		{
			float f = 1.0F / t;
			return (Point3D(x * f, y * f, z * f));
		}
		
		// Dot product
		float operator *(const Vector3D& v) const
		{
			return (x * v.x + y * v.y + z * v.z);
		}
};


inline float Norm(const Vector3D& v)
{
	return (v.x * v.x + v.y * v.y + v.z * v.z);
}

inline float Magnitude(const Vector3D& v)
{
	return (sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
}


// Returns index of next vertex in the working polygon
static long GetNextActive(long x, long vertexCount, const bool *active)
{
	for (;;)
	{
		if (++x == vertexCount) x = 0;
		if (active[x]) return (x);
	}
}

// Returns index of previous vertex in the working polygon
static long GetPrevActive(long x, long vertexCount, const bool *active)
{
	for (;;)
	{
		if (--x == -1) x = vertexCount - 1;
		if (active[x]) return (x);
	}
}

static long TriangulatePolygon(
	long vertexCount,				// Number of vertices in polygon
	const Point3D *vertex,			// Counterclockwise-wound vertex list
	const Vector3D& normal,			// Normal to plane of polygon
	Triangle *triangle)				// Output triangle list -- must be room for (vertexCount - 2) triangles
{
	// Allocate an array to hold boolean values indicating whether
	// a vertex is still part of the working polygon.
	bool *active = new bool[vertexCount];
	for (long a = 0; a < vertexCount; a++) active[a] = true;
	
	long triangleCount = 0;
	long start = 0;
	
	long p1 = 0;
	long p2 = 1;
	long m1 = vertexCount - 1;
	long m2 = vertexCount - 2;
	
	bool lastPositive = false;
	for (;;)
	{
		if (p2 == m2)
		{
			// Only three vertices remain -- make a triangle out of them and exit
			triangle->index[0] = m1;
			triangle->index[1] = p1;
			triangle->index[2] = p2;
			triangleCount++;
			break;
		}
		
		// Get four vertices to work with
		const Point3D& vp1 = vertex[p1];
		const Point3D& vp2 = vertex[p2];
		const Point3D& vm1 = vertex[m1];
		const Point3D& vm2 = vertex[m2];
		
		bool positive = false;
		bool negative = false;
		
		// Determine whether vp1, vp2, and vm1 form a valid triangle
		Vector3D n1 = normal % (vm1 - vp2).Normalize();
		if (n1 * (vp1 - vp2) > epsilon)
		{
			positive = true;
			
			Vector3D n2 = (normal % (vp1 - vm1).Normalize());
			Vector3D n3 = (normal % (vp2 - vp1).Normalize());
			
			for (long a = 0; a < vertexCount; a++)
			{
				// Look for other vertices inside the triangle
				if ((active[a]) && (a != p1) && (a != p2) && (a != m1))
				{
					const Vector3D& v = vertex[a];
					if ((n1 * (v - vp2).Normalize() > -epsilon) && (n2 * (v - vm1).Normalize() > -epsilon) && (n3 * (v - vp1).Normalize() > -epsilon))
					{
						positive = false;
						break;
					}
				}
			}
		}
		
		// Determine whether vm1, vm2, and vp1 form a valid triangle
		n1 = normal % (vm2 - vp1).Normalize();
		if (n1 * (vm1 - vp1) > epsilon)
		{
			negative = true;
			
			Vector3D n2 = (normal % (vm1 - vm2).Normalize());
			Vector3D n3 = (normal % (vp1 - vm1).Normalize());
			
			for (long a = 0; a < vertexCount; a++)
			{
				// Look for other vertices inside the triangle
				if ((active[a]) && (a != m1) && (a != m2) && (a != p1))
				{
					const Vector3D& v = vertex[a];
					if ((n1 * (v - vp1).Normalize() > -epsilon) && (n2 * (v - vm2).Normalize() > -epsilon) && (n3 * (v - vm1).Normalize() > -epsilon))
					{
						negative = false;
						break;
					}
				}
			}
		}
		
		// If both triangles valid, choose the one having the larger smallest angle
		if ((positive) && (negative))
		{
			float pd = (vp2 - vm1).Normalize() * (vm2 - vm1).Normalize();
			float md = (vm2 - vp1).Normalize() * (vp2 - vp1).Normalize();
			
			if (fabs(pd - md) < epsilon)
			{
				if (lastPositive) positive = false;
				else negative = false;
			}
			else
			{
				if (pd < md) negative = false;
				else positive = false;
			}
		}
		
		if (positive)
		{
			// Output the triangle m1, p1, p2
			active[p1] = false;
			triangle->index[0] = m1;
			triangle->index[1] = p1;
			triangle->index[2] = p2;
			triangleCount++;
			triangle++;
			
			p1 = GetNextActive(p1, vertexCount, active);
			p2 = GetNextActive(p2, vertexCount, active);
			
			lastPositive = true;
			start = -1;
		}
		else if (negative)
		{
			// Output the triangle m2, m1, p1
			active[m1] = false;
			triangle->index[0] = m2;
			triangle->index[1] = m1;
			triangle->index[2] = p1;
			triangleCount++;
			triangle++;
			
			m1 = GetPrevActive(m1, vertexCount, active);
			m2 = GetPrevActive(m2, vertexCount, active);
			
			lastPositive = false;
			start = -1;
		}
		else
		{
			// Safety -- if we've gone all the way around the polygon
			// without finding a valid triangle, then bail.
			if (start == -1) start = p2;
			else if (p2 == start) break;
			
			// Advance working set of vertices
			m2 = m1;
			m1 = p1;
			p1 = p2;
			p2 = GetNextActive(p2, vertexCount, active);
		}
	}
	
	delete[] active;
	return (triangleCount);
}
