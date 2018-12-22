#pragma once

#include <stdlib.h>
#include <float.h>
#include <math.h>

#include "vector.hpp"

//! Defining a ray with its origin and direction
class Ray {
public:
	//! origin of the ray
	Vector origin;
	
	//! must be normalised
	Vector dir;		

	//! must be initialised and unique for each search call to the KD tree data structure
	long id;		

	//! global static variable, aiding unique ID generation
	static long maxRayID;
};

//! Axis-aligned bounding box used in kd-tree
class BoundingBox {		
public:
	Vector minPoint;
	Vector maxPoint;

	// returns the center of the BoundingBox
	Vector Center (void) {
		Vector center;
		center = (minPoint + maxPoint) * 0.5;
		return center;
	}
};

class Intersectable;

//! ray - object intersection test result
class HitRec {
public:
	//! point of the intersection
	Vector	point;			

	//! distance from ray origin
	float	depth;			

	//! object found 
	Intersectable* object;	

	//! valid flag
	bool	isIntersect;	

	//! default constructor
	HitRec() { 
		depth=0.0f;
	}
};

class Material;

//! Defines the basic behaviour of an intersectable object to be stored in the KD tree
class Intersectable {
public:
	//! Material of the intersectable object
	const Material*	m_pMaterial;

	//! bounding box for the kd-tree
	BoundingBox	bbox;

	//! result of the last intersection test
	unsigned int	lastTestedRayId;		
	HitRec			lastTestedRayResult;

public:
	Intersectable(const Material* material);
	~Intersectable();
	
	//! pure virtual function, must be implemented to carry out the intersection test
	virtual bool intersect(const Ray& ray, float& depth)=0;
	
	//! pure virtual function, must be implemented to calculate the surface normal
	virtual Vector getShadingNormal(const Vector& point)=0;
  
	//! index of the object in the scene array
	unsigned long m_index;

	//! helper to initialize m_index 
	static unsigned long gMaxIndex;
}; 

class Triangle : public Intersectable {
protected:
	/*!@name Precomputed values for faster intersection testing */
	//@{
	DominantAxis	dominantAxis;
	float			hyperPlaneShiftOffset;
	double			abV1, abV2, abC, bcV1, bcV2, bcC, caV1, caV2, caC; 
	//@}

public:
	//! the 3 vertices
	const Vector	*a, *b, *c; 	

	//! surface normal (average of the vertices)
	Vector			normal;

	//! midpoint of the triangle
	Vector			midPoint;

	//! normal of the vertices
	Vector	*Na, *Nb, *Nc;	
	
	//! area of the triangle
	float			area;

public:
	Triangle() : Intersectable(NULL) {};
	Triangle(const Vector* a, const Vector* b, const Vector* c, const Material* material);
	bool intersect(const Ray& ray, float& depth);
	Vector getShadingNormal(const Vector& point);
 
	void CalculateArea() {
		Vector side1 = *a - *b;
		Vector side2 = *a - *c;
		Vector crossProd = side1 && side2;
		area = fabsf ((0.5f * crossProd.Norm ()));
	}

	//! Generate a random point in the triangle
	/*!	Point IN Triangle: barycentric parametrisation, u, v : barycentric coordinates
	barycentric coord: to define the barycentric coordinates in R^n ( n= 2 the triangle)
	one needs (n+1) points (3 points in 3D) that do not lie in a space of a lesser dimension. 
	From that we can find out the u,v barycentric coords.
	here do the reverse: from u, v barycentric coords, calculate the point */
	Vector PointInTriangle () {
		float u=RAND;
		float v=RAND;

		if (u + v > 1.0f) {
			u = 1.0f - u; 
			v = 1.0f - v;
		}

		return Vector(
			a->x + u * (b->x - a->x) + v * (c->x - a->x),
			a->y + u * (b->y - a->y) + v * (c->y - a->y),
			a->z + u * (b->z - a->z) + v * (c->z - a->z));
	}

	void Finish();
};

//! Application-specific derivation of Triangle,
//! variables specific to Stochastic Iteration are added
class Patch : public Triangle 
{
public:
	//! true, if the patch is visible from the eye
	bool visible;

	//! count the number of screen pixels this patch is visible from
	long nScreenPixel;

public:
	//! diffuse part of the radiance
	//! sum (I * f_d), where f_d is the main (diffuse) part of the BRDF function
	Color		Ld;

	//! scale factor for specular irradiance
	//! sum Lum(I*d_a), where d_a is the specular albedo
	float	C_I;

	//! last accepted irradiance
	Color		I;
	//! direction of the last accepted irradiance
	Vector	dirI;


	//! diffuse part of the importance
	//! sum (W * f_d), where f_d is the main (diffuse) part of the BRDF function
	float	Rd;

	//! scale factor for specular importance
	//! sum Lum(W*d_a), where d_a is the specular albedo
	float	C_W;

	//! last accepted importance
	float	W;

	//! direction of the last accepted importance
	Vector	dirW;


	//! precomputed value; patch --> eye direction
	Vector	m_VisDir;

	//! precomputed value; eye --> patch direction
	Vector	m_EyePatchDir;

	//! sum radiance towards the eye
	//! Leye = sum(I * f_eye)
	Color		Leye;

	Color		Leye_s;
	float		d;
	long		iteration;

public:
	void Reset() {
		visible=false;
		nScreenPixel=0;
		I = Ld = Leye = Color::BLACK;
		W = C_W = Rd = C_I = 0.0f;

		Leye_s = Color::BLACK;
		d = 0.0f;

		iteration = 0;
	}

	//! simple triangle plane -- ray intersection calculation
	bool CalculateIntersection (const Ray& ray, HitRec& hr);
};

class Light;

//! Precalclate and store a given number of point samples on area light sources
class LightSourceSample {
public:
	Light* m_pLight;
	int m_nSamples;
	Vector* samples;

public:
	LightSourceSample(Light* light=NULL, int nSamples=0) : samples(NULL) { Init(light, nSamples); };
	~LightSourceSample() { if (samples) delete samples; }

	//! Generate lightsource samples
	void Init(Light* light, int nSamples);
};

//! Assign emissive patches and LightSourceSample
class Light 
{
public:
	Patch*	m_pPatch;
	LightSourceSample*	m_pSamples;

public:
	Light() {};
	void Init(Patch* pPatch, int nSamples=2) { 
		m_pPatch = pPatch;
		m_pSamples = new LightSourceSample(this, nSamples);
	}
	~Light() { delete m_pSamples; }
};
