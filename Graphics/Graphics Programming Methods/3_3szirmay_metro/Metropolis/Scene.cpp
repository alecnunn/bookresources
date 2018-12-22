// Graphics Programming Methods
// Metropolis sampling in global illumination
// György Antal, Lászlo Szirmay-Kalos
// 2003.

// several classes used to build the virtual world

#include "Scene.hpp"
#include <windows.h>

// the Camera constructor set up the default camera
//-----------------------------------------------------------------
Camera::Camera () 
{
	eyep.Set (0.0, 1.000, 5.0);
	lookp.Set (0., 0., 0.);
	updir.Set (0., 1.0, 0.0);
	fov		= 22.5;

	hres	= DefaultScreenWidth;
	vres	= DefaultScreenHeight;

	CompleteCamera ();
}

// finalize camera data
//-----------------------------------------------------------------
void Camera::CompleteCamera ()
{
	// set up Z
	Z = lookp - eyep;
	viewdist = Z.Norm ();	// distance from virtual camera position to focus point
	if (viewdist < 1e-5F) {
		::MessageBox (NULL, "Camera", "eyepoint and look-point coincide", MB_OK);
		return;
	}
	Z *= 1.0 / viewdist;
	
	// set up X   Camera->X is a direction pointing to the right in the window 
	X =  Z % updir;
	double lengthX = X.Norm ();
	if (lengthX < 1e-5F) {
		MessageBox (NULL, "Camera", "up-direction and viewing direction coincide", MB_OK);
		return;
	}
	X *= 1.0 / lengthX;
	
	Y = Z % X;	// set up Y
	Y.Normalize ();
	
	// compute horizontal and vertical field of view angle from the specified one
	// if the vertical resolution is smaller, it is the specified fov = 45, the other is greater than 45
	if (hres < vres) {
		hfov = fov; 
		vfov = atan(tan(fov * M_PI/180.0) * (double)vres/(double)hres) * 180.0/M_PI;
	} else {
		vfov = fov; 
		hfov = atan(tan(fov * M_PI/180.0) * (double)hres/(double)vres) * 180.0/M_PI;
	}
	
	// Compute some extra frequently used quantities
	tanFovH = tan(hfov * M_PI / 180.0);
	tanFovV = tan(vfov * M_PI / 180.0);
	pixh = 2.0 * tanFovH / (double)(hres);
	pixv = 2.0 * tanFovV / (double)(vres);

}

const int MaxDistance = 1e+8;

// precalculation for Green's ray-triangle itersection
//-----------------------------------------------------------------
void Triangle::FinishObject(void) 
{
	// 1. calculate bounding box
	bbox.minPoint = *a;
	bbox.minPoint <= *b;
	bbox.minPoint <= *c;

	bbox.maxPoint = *a;
	bbox.maxPoint >= *b;
	bbox.maxPoint >= *c;

	assert(!_isnan(normal.x) && !_isnan(normal.y) && !_isnan(normal.z));
	double dx = fabs (normal.x);
	double dy = fabs (normal.y);
	double dz = fabs (normal.z);
	dominantAxis = (dx > dy) ? ((dx > dz) ? X_DOMINANT : Z_DOMINANT) : ((dy > dz) ? Y_DOMINANT : Z_DOMINANT);

	hiperPlaneShiftOffset = -1.0 * (normal * *a);

	switch (dominantAxis) {
	case X_DOMINANT:
		abV1	= b->z - a->z;
		abV2	= a->y - b->y;
		abC		= abV1 * b->y + abV2 * b->z;
		if (c->y * abV1 + c->z * abV2 > abC) {
			abC		*= -1.0;
			abV1	*= -1.0;
			abV2	*= -1.0;
		}

		bcV1	= c->z - b->z;
		bcV2	= b->y - c->y;
		bcC		= bcV1 * c->y + bcV2 * c->z;
		if (a->y * bcV1 + a->z * bcV2 > bcC) {
			bcC		*= -1.0;
			bcV1	*= -1.0;
			bcV2	*= -1.0;
		}

		caV1	= a->z - c->z;
		caV2	= c->y - a->y;
		caC		= caV1 * a->y + caV2 * a->z;
		if (b->y * caV1 + b->z * caV2 > caC) {
			caC		*= -1.0;
			caV1	*= -1.0;
			caV2	*= -1.0;
		}

		break;
	case Y_DOMINANT:
		abV1	= b->z - a->z;
		abV2	= a->x - b->x;
		abC		= abV1 * b->x + abV2 * b->z;
		if (c->x * abV1 + c->z * abV2 > abC) {
			abC		*= -1.0;
			abV1	*= -1.0;
			abV2	*= -1.0;
		}

		bcV1	= c->z - b->z;
		bcV2	= b->x - c->x;
		bcC		= bcV1 * c->x + bcV2 * c->z;
		if (a->x * bcV1 + a->z * bcV2 > bcC){
			bcC		*= -1.0;
			bcV1	*= -1.0;
			bcV2	*= -1.0;
		}

		caV1	= a->z - c->z;
		caV2	= c->x - a->x;
		caC		= caV1 * a->x + caV2 * a->z;
		if (b->x * caV1 + b->z * caV2 > caC){
			caC		*= -1.0;
			caV1	*= -1.0;
			caV2	*= -1.0;
		}
		break;
	case Z_DOMINANT:
		abV1	= b->y - a->y;
		abV2	= a->x - b->x;
		abC		= abV1 * b->x + abV2 * b->y;
		if (c->x * abV1 + c->y * abV2 > abC) {
			abC		*= -1.0;
			abV1	*= -1.0;
			abV2	*= -1.0;
		}

		bcV1	= c->y - b->y;
		bcV2	= b->x - c->x;
		bcC		= bcV1 * c->x + bcV2 * c->y;
		if (a->x * bcV1 + a->y * bcV2 > bcC) {
			bcC		*= -1.0;
			bcV1	*= -1.0;
			bcV2	*= -1.0;
		}

		caV1	= a->y - c->y;
		caV2	= c->x - a->x;
		caC		= caV1 * a->x + caV2 * a->y;
		if (b->x * caV1 + b->y * caV2 > caC){
			caC		*= -1.0;
			caV1	*= -1.0;
			caV2	*= -1.0;
		}
		break;
	}
}

// Green's ray-triangle intersection
//-----------------------------------------------------------------
bool Triangle::Intersect(const Ray& ray, float* depth, float /*rayMin*/, float /*rayMax*/)
{
	lastTestedRayId = ray.id;
	lastTestedRayResult.isIntersect = false;

	double cosa = normal * ray.dir;
	if (cosa > -1e-5F)	// back facing patch
		return false;

	double originDistOnNormal = normal * ray.origin;
	*depth = -(hiperPlaneShiftOffset + originDistOnNormal) / cosa;
	if (*depth < 1e-4F || *depth > MaxDistance)
		return false;

	double s, v;
	switch (dominantAxis) {
	case X_DOMINANT:
		s = ray.origin.y + *depth * ray.dir.y;
		v = ray.origin.z + *depth * ray.dir.z;
		break;
	case Y_DOMINANT:
		s = ray.origin.x + *depth * ray.dir.x;
		v = ray.origin.z + *depth * ray.dir.z;
		break;
	case Z_DOMINANT:
		s = ray.origin.x + *depth * ray.dir.x;
		v = ray.origin.y + *depth * ray.dir.y;
		break;
	}

	if (abV1 * s + abV2 * v > abC)
		return false;
	else if (bcV1 * s + bcV2 * v > bcC)
		return false;
	else if (caV1 * s + caV2 * v > caC)
		return false;

	lastTestedRayResult.isIntersect	= true;
	lastTestedRayResult.object		= this;
	lastTestedRayResult.depth		= *depth;
	lastTestedRayResult.point		= ray.origin + ray.dir * *depth;
	return true;
}



