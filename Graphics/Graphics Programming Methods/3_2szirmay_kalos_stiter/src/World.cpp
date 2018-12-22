
// Graphics Programming Methods
// An Effective kd-tree Implementation
// László Szécsi
// 2003.

// several support classes used to build the virtual world
// - basic Vector class with overloaded operators (used in KDTree)
// - Ray and HitRec for ray-casting (used in KDTree)
// - BoundingBox, Intersectable classes (used in KDTree)
// - Triangle and Sphere descendants of Intersectable
// - Light and Material classes for the virtual world

#include <stdio.h>
#include <assert.h>

#include "World.hpp"
#include "Matrix.hpp"

unsigned long Intersectable::gMaxIndex = 0;
long Ray::maxRayID = 0;

Intersectable::Intersectable(const Material* material): m_pMaterial(material)
{
	lastTestedRayId = 0;
}

Intersectable::~Intersectable()
{
}

Triangle::Triangle(const Vector* _a, const Vector* _b, const Vector* _c, const Material* _material)
		:Intersectable(_material),a(_a),b(_b),c(_c)
{
}


void Triangle::Finish() 
{
	// 1. calculate bounding box
	bbox.minPoint = *a;
	bbox.minPoint <= *b;
	bbox.minPoint <= *c;

	bbox.maxPoint = *a;
	bbox.maxPoint >= *b;
	bbox.maxPoint >= *c;

	midPoint = (*a + *b + *c) * (1.0f /3.0f);

	assert(!_isnan(normal.x) && !_isnan(normal.y) && !_isnan(normal.z));
	double dx = fabs (normal.x);
	double dy = fabs (normal.y);
	double dz = fabs (normal.z);
	dominantAxis = (dx > dy) ? ((dx > dz) ? X_DOMINANT : Z_DOMINANT) : ((dy > dz) ? Y_DOMINANT : Z_DOMINANT);

	hyperPlaneShiftOffset = -1.0f * (normal * *a);

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

bool Triangle::intersect(const Ray& ray, float& depth)
{
	lastTestedRayId = ray.id;
	lastTestedRayResult.isIntersect = false;

	double cosa = normal * ray.dir;
	if (cosa > -1e-5F)	// back facing patch
		return false;

	double originDistOnNormal = normal * ray.origin;
	depth = (float)(-(hyperPlaneShiftOffset + originDistOnNormal) / cosa);
	if (depth < 0.0f)
		return false;

	double s, v;
	switch (dominantAxis) {
	case X_DOMINANT:
		s = ray.origin.y + depth * ray.dir.y;
		v = ray.origin.z + depth * ray.dir.z;
		break;
	case Y_DOMINANT:
		s = ray.origin.x + depth * ray.dir.x;
		v = ray.origin.z + depth * ray.dir.z;
		break;
	case Z_DOMINANT:
		s = ray.origin.x + depth * ray.dir.x;
		v = ray.origin.y + depth * ray.dir.y;
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
	lastTestedRayResult.depth		= depth;
	lastTestedRayResult.point		= ray.origin + ray.dir * depth;
	//hitRec->point	= ray.origin + ray.dir * t;	// calculate this just for the minimal Patch
	return true;
}


void LightSourceSample::Init(Light* light, int nSamples) 
{
	m_pLight=light;
	m_nSamples=nSamples;

	if (m_pLight) {
		if (samples) delete samples;
		samples = new Vector[m_nSamples];
		for (int j=0; j<m_nSamples; j++) {
			samples[j]=m_pLight->m_pPatch->PointInTriangle();
		}
	} else {
		m_pLight=NULL;
	}
}


//! simple triangle plane -- ray intersection calculation
/*! This is a simplified triangle-ray intersection calculation,
basically calculating the ray-plane intersection point and its distance
from the ray's origin.*/
bool Patch::CalculateIntersection (const Ray& ray, HitRec& hr)
{
	float cosa = normal * ray.dir;
	// back facing patch?
	if (cosa > -EPSILON) {
		hr.object=NULL;
		hr.isIntersect=false;
		return false;
	}

	float originDistOnNormal = normal * ray.origin;
	hr.depth = -(hyperPlaneShiftOffset + originDistOnNormal) / cosa;
	hr.point = ray.origin + ray.dir * hr.depth;
	hr.object= this;
	hr.isIntersect=true;
	return true;
}


Vector Triangle::getShadingNormal(const Vector& point) 
{
	Vector pointShadingNormal;

	Matrix3x3 m, mInv;
	Vector weights;

	m.Fill (a->x, b->x, c->x, a->y, b->y, c->y, a->z, b->z, c->z);
	mInv.InvertMatrix3x3 (m);
	if (_finite(mInv.m[0][0])) {	// if it is a full rank matrix
		mInv.MultiplyMatrixLeft (&point, &weights);
		pointShadingNormal = *Na * weights.x + *Nb * weights.y + *Nc * weights.z;
	} else {	
		// if the rank of m less than 3
		// or it is possible to measure the distances of the point to vertices, and make a
		// the weight according to the relative distances
		pointShadingNormal =  (*Na + *Nb+ *Nc)*(1.0f/3.0f);
	}
	pointShadingNormal.Normalize();

	return pointShadingNormal;
}
