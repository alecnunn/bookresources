/*============================================================================
	Title: Point.cpp
	Author: Oscar Blasco
	Description: Point class implementation
	Changes:
============================================================================*/

#include "NormalMapGen.h"

/** Triangle - Ray intersection test using barycentric coordinates */
bool Point::TriIntersect(Point orig, Point dir, Point v0, Point v1, Point v2, Point &bar){
		double det;
		Point a, b;

		a = v1 - v0;
		b = v2 - v0;

		Point pVector = dir ^ b;
		det = a * pVector;

		if(det > - EPS && det < EPS)
			return 0;

		det = 1.f / det;
		Point tVector = orig - v0;
		bar.x = (tVector * pVector) * det;

		if(bar.x < 0.f || bar.x > 1.f)
			return 0;

		pVector = tVector ^ a;
		bar.y = (dir * pVector) * det;

		if(bar.y < 0.f || bar.x + bar.y > 1.f)
			return 0;

		bar.z = (b * pVector) * det;

		return 1;
}


/** 
 * Returns the distance fron origin to intersection point and places that point in 'Intersec'
 * If there is not hit at all, returns 9999999.f
 */
double Point::RayTriIntersectionDistance(Point &Origin, Point &Vector, Point &PlaneNormal, 
									 Point &v1, Point &v2, Point &v3, Point &Max, Point &Min, Point *Intersec){ 
	Point IPoint; // Our intersection point of the ray and the plane

	// Compute the point and the distance from the origin and the plane
	double DistToPlane = IPoint.computeIntersectionPoint(Origin, Vector, v1, PlaneNormal);		

	// Check if there is intersection at all...
	if(DistToPlane == 9999999.f) return(9999999.f); 

	// Check against the bbox of the given face
	if(!IPoint.checkPointInsideBBox(Max, Min)) 
		return(9999999.f);

	Point barcoords;

	// Check if intersection point is inside the triangle
	if(TriIntersect(Origin, Vector, v1, v2, v3, barcoords)){
		*Intersec = IPoint;
		return DistToPlane; // Return the distance
	}

	return(9999999.f); 
		
}


/** returns distance to intersection point on plane*/
double Point::intersectRayPlane(Point &Origin, Point &Vector, Point &PointOnPlane, Point &PlaneNormal){
	double denom = PlaneNormal * Vector;
  
	if (denom == 0)  // normal is orthogonal to vector, cant intersect
		return (9999999.f);

	double d = -(PlaneNormal * PointOnPlane);
	double numer = -((PlaneNormal * Origin) + d);
	
	return (numer / denom);	
}
