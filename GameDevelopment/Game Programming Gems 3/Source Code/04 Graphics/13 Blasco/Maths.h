/*============================================================================
	Title: Maths.h
	Author: Oscar Blasco
	Description: Raytracing Math Functions for the plugin
	Changes:
============================================================================*/

#ifndef _MATHS_H
#define _MATHS_H

#include <math.h>

#define EPS 0.00000001f

class Point{
public:
	Point(){ x = y = z = 0.f; }
	Point(double _x, double _y, double _z){ x = _x;	y = _y;	z = _z;	}
	Point(const Point &p){ x = p.x; y = p.y; z = p.z; }

	inline double operator * (const Point &p){ return x*p.x + y*p.y + z*p.z; }
	inline Point operator -  (const Point &p){ 
		Point r;
		r.x = x - p.x;
		r.y = y - p.y;
		r.z = z - p.z;
		return r;
	}

	inline Point operator + (const Point &p){ 
		Point r;
		r.x = x + p.x;
		r.y = y + p.y;
		r.z = z + p.z;
		return r;
	}

	inline Point operator / (const int value){ 
		Point r;
		r.x = x / (double)value;
		r.y = y / (double)value;
		r.z = z / (double)value;
		return r;
	}

	inline Point operator / (const double value){ 
		Point r;
		r.x = x / value;
		r.y = y / value;
		r.z = z / value;
		return r;
	}

	inline Point operator * (const double value){ 
		Point r;
		r.x = x * value;
		r.y = y * value;
		r.z = z * value;
		return r;
	}

	inline Point operator * (const float value){ 
		Point r;
		r.x = x * value;
		r.y = y * value;
		r.z = z * value;
		return r;
	}

	inline Point operator * (const int value){ 
		Point r;
		r.x = x * value;
		r.y = y * value;
		r.z = z * value;
		return r;
	}

	inline Point operator ^ (const Point &p){ 
		Point r;
		r.x = y*p.z - z*p.y;
		r.y = z*p.x - x*p.z;
		r.z = x*p.y - y*p.x;
		return r;
	}

	inline void operator += (const Point &p){ 
		x += p.x;
		y += p.y;
		z += p.z;
	}

	inline void operator *= (const float value){ 
		x *= value;
		y *= value;
		z *= value;
	}

	inline void operator *= (const double value){ 
		x *= value;
		y *= value;
		z *= value;
	}

	inline void operator -= (const Point &p){ 
		x -= p.x;
		y -= p.y;
		z -= p.z;
	}

	void normalize(){
		double l = inverselength();
	
		x *= l;
		y *= l;
		z *= l;
	}

	double length(){ 
		double m = (*this) * (*this);
		return m? sqrt(m) : 0.f;
	}

	double inverselength(){ 
		double m = (*this) * (*this);
		return m? 1/sqrt(m) : 0.f;
	}

	/// Raytracing functions ///////////////////////////////////////////////////////////////////////

	/** 
	 * Computes if there is intersection and distance from Origin to the plane 
	 * Vector and PlaneNormal MUST be normalized!
	 */
	double intersectRayPlane(Point &Origin, Point &Vector, 
							 Point &PointOnPlane, Point &PlaneNormal);

	/** 
	 * Computes intersection point of a ray and a plane and stores the resulting point
	 * in this Point.
	 * Returns the distance if intersects, otherwise returns -1.f
	 */
	double computeIntersectionPoint(Point &Origin, Point &Vector,
								  Point &PointOnPlane, Point &PlaneNormal){

		double DistToPlane = intersectRayPlane(Origin, Vector, PointOnPlane, PlaneNormal);

		if(DistToPlane == 9999999.f) return(9999999.f);

		x = Origin.x + DistToPlane * Vector.x;
		y = Origin.y + DistToPlane * Vector.y;
		z = Origin.z + DistToPlane * Vector.z;

		return DistToPlane;
	}

	/** To check the point againts a bounding box of a face */
	bool checkPointInsideBBox(Point &Max, Point &Min){
		if(x - 0.00001 >= Max.x || x <= Min.x - 0.00001) return false;
		if(y - 0.00001 >= Max.y || y <= Min.y - 0.00001) return false;
		if(z - 0.00001 >= Max.z || z <= Min.z - 0.00001) return false;
		return TRUE;
	}

	/** Barycentric coordiantes intersection test */
	static bool TriIntersect(Point orig, Point dir, Point v0, Point v1, Point v2, Point &bar);

	/** 
	 * Distance from the origin of a ray to a triangle
	 * Ray -> Origin, Vector
	 * Plane -> PlaneNormal
	 * Triangle verts -> v1, v2, v3
	 * Returns distance if there is intersections or -1.f if not
	 */
	static double RayTriIntersectionDistance(Point &Origin, Point &Vector, Point &PlaneNormal, 
									 Point &v1, Point &v2, Point &v3, Point &Max, Point &Min, Point *Intersec);

	double x, y, z; /**< Point components */
};


#endif // _MATHS_H