#include "stdafx.h"
#include "Patch.hpp"

const Vector Vector::RGBBLACK(0.0, 0.0, 0.0);
const Vector Vector::RGBWHITE(0.9, 0.9, 0.9);

const Material Material::DIFFUSEWHITE(Vector::RGBWHITE, Vector::RGBBLACK, 0.0f);
const Material Material::SHINYBLACK(Vector::RGBBLACK, Vector::RGBWHITE, 100.0f);
const Material Material::STEEL(Vector(0.8f, 0.8f, 0.8f), Vector(0.5f, 0.5f, 0.8f), 10.0f);

/*
void Triangle::CalculateArea (void)
{
	Vector side1, side2, crossProd;
	side1.Subtract (*a, *b);
	side2.Subtract (*a, *c);
	crossProd.CrossProduct (side1, side2);
	area = fabs (0.5 * crossProd.Norm ());

}
*/
void Triangle::finishTriangle (void) 
{
	normal = ((*a) - (*b)) && ((*a) - (*c));
	normal.Normalize();

	float x = fabsf (normal.x);
	float y = fabsf (normal.y);
	float z = fabsf (normal.z);
	dominantAxis = (x > y) ? ((x > z) ? X_DOMINANT : Z_DOMINANT) : ((y > z) ? Y_DOMINANT : Z_DOMINANT);
	
	// it doens't matter which point to choose for hiperPlaneShiftOffset
	hiperPlaneShiftOffset = -(normal * (*a));
	
	switch (dominantAxis) {
	case X_DOMINANT:
		d1 = b->z - a->z;
		d2 = b->y - a->y;
		d3 = c->z - b->z;
		d4 = c->y - b->y;
		d5 = a->z - c->z;
		d6 = a->y - c->y;
		break;
	case Y_DOMINANT:
		d1 = b->z - a->z;
		d2 = b->x - a->x;
		d3 = c->z - b->z;
		d4 = c->x - b->x;
		d5 = a->z - c->z;
		d6 = a->x - c->x;		
		break;
	case Z_DOMINANT:
		d1 = b->y - a->y;
		d2 = b->x - a->x;
		d3 = c->y - b->y;
		d4 = c->x - b->x;
		d5 = a->y - c->y;
		d6 = a->x - c->x;			
		break;
	}
	
	bbox.minPoint = *a;
	bbox.minPoint <= *b;
	bbox.minPoint <= *c;

	bbox.maxPoint = *a;
	bbox.maxPoint >= *b;
	bbox.maxPoint >= *c;
}

bool Triangle::intersect (const Ray& ray, float& depth, float rayMin, float rayMax)
{	
	lastTestedRayId = ray.id;
	lastTestedRayResult->isIntersect = false;

	float cosa = normal * ray.dir;
	if (cosa > -0.00001f)	// back facing triangle
		return false;
	
	float originDistOnNormal = normal * ray.origin;
	depth = -(hiperPlaneShiftOffset + originDistOnNormal) / cosa;
	if (depth < 0.00001f || depth > 10000000.0f)
		return false;
	
	float s, t;
	switch (dominantAxis)
	{
	case X_DOMINANT:
		// project to YZ plane than
		// test that tg (P2->P1) > tg (P2->Pintersect),
		// so if Pintersect is on the wrong side, drop it
		s = ray.origin.y + depth * ray.dir.y;
		t = ray.origin.z + depth * ray.dir.z;
		
		if ((b->y - s) * (d1) < (b->z - t) * (d2))	// speed up: e->g store: (b->z - a->z)
			return false;
		if ((c->y - s) * (d3) < (c->z - t) * (d4))
			return false;
		if ((a->y - s) * (d5) < (a->z - t) * (d6))
			return false;

		lastTestedRayResult->isIntersect	= true;
		lastTestedRayResult->object			= this;
		lastTestedRayResult->depth			= depth;
		lastTestedRayResult->point = ray.origin + ray.dir * depth;
		return true;
		
	case Y_DOMINANT:
		s = ray.origin.x + depth * ray.dir.x;
		t = ray.origin.z + depth * ray.dir.z;

		if ((b->x - s) * (d1) < (b->z - t) * (d2))
			return false;
		if ((c->x - s) * (d3) < (c->z - t) * (d4))
			return false;
		if ((a->x -s) * (d5) < (a->z - t) * (d6))
			return false;
		lastTestedRayResult->isIntersect	= true;
		lastTestedRayResult->object			= this;
		lastTestedRayResult->depth			= depth;
		lastTestedRayResult->point = ray.origin + ray.dir * depth;
		return true;
		
	case Z_DOMINANT:
		s = ray.origin.x + depth * ray.dir.x;
		t = ray.origin.y + depth * ray.dir.y;

		if ((b->x - s) * (d1) < (b->y - t) * (d2))
			return false;
		if ((c->x - s) * (d3) < (c->y - t) * (d4))
			return false;
		if ((a->x - s) * (d5) < (a->y - t) * (d6))
			return false;
		lastTestedRayResult->isIntersect	= true;
		lastTestedRayResult->object			= this;
		lastTestedRayResult->depth			= depth;
		lastTestedRayResult->point = ray.origin + ray.dir * depth;
		return true;
	}
	return false;
}

bool Sphere::intersect (const Ray& ray, float& depth, float rayMin, float rayMax)
{
	lastTestedRayId = ray.id;
	lastTestedRayResult->isIntersect = false;
	Vector diff = (ray.origin - centre);
	float b = 2.0f * (ray.dir * diff);
	float c = diff * diff - radius * radius;

	float discriminant = b * b - 4.0 * c;
	if(discriminant < 0.0f)
		return false;
	else
	{
		depth = (-b - sqrtf(discriminant)) * 0.5f;
		lastTestedRayResult->isIntersect	= true;
		lastTestedRayResult->object			= this;
		lastTestedRayResult->depth			= depth;
		lastTestedRayResult->point = ray.origin + ray.dir * depth;
		return true;
	}
}

Intersectable::Intersectable(const Material* material):material(material)
{
	lastTestedRayId = 0;
	lastTestedRayResult = new HitRec;
}

Intersectable::~Intersectable()
{
	delete lastTestedRayResult;
}

