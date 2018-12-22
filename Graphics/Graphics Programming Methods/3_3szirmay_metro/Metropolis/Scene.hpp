// Graphics Programming Methods
// Metropolis sampling in global illumination
// György Antal, Lászlo Szirmay-Kalos
// 2003.

// several support classes used to build the virtual world
// - Ray and HitRec for ray-casting (used in KDTree)
// - BoundingBox, Intersectable classes (used in KDTree)
// - Triangle and Sphere could be the descendants of Intersectable
// - Light class
#pragma once

#include <vector>
#include "Vector.hpp"
#include "Color.hpp"
#include "Material.hpp"
#include "Matrix.hpp"

const short DefaultScreenWidth	= 200;	// the width of the client area
const short DefaultScreenHeight	= 200;	// the height of the client area

// Camera represents a camera object
//---------------------------------------------------------------------------------------------
class Camera {
public:
	Vector	eyep;				// virtual camera position in 3D space
	Vector	lookp;				// focus point of camera
	Vector	updir;				// direction pointing upward

	double	viewdist;			// distance from eyepoint to focus point	
	double	fov, hfov, vfov;	// half of the field of view, horizontal and vertical, in degrees.

	long	hres, vres;			// horizontal and vertical resolution
		
	Vector	X, Y, Z;			// eye coordinate system (right-hand-orientation): X=right, Y=down, Z=viewing direction
	double	pixh, pixv;			// Width and height of a pixel

	double	tanFovH, tanFovV;	// when focus == 1, so, it is with/2

	Camera ();
	void CompleteCamera ();
};

// the Ray represents the ray
//-----------------------------------------------------------------
class Ray {
public:
	Vector	origin;				// origin of the ray
	Vector	dir;				// direction of the ray
	int		id;					// unique identifier of the ray
	Ray (const Vector& newOrigin,  const Vector& newDir) { origin = newOrigin; dir = newDir; };
};

class Intersectable;


// the HitRec class contains information on the intersection point returned by the kd-tree
//-----------------------------------------------------------------
class HitRec {
public:
	Intersectable*	object;			// the triangle of the hit point
	Vector			point;			// intersection point
	Vector			normal;			// the normal of the surface at the intersection point
	double			depth;			// ray coefficient: point = ray.orig + depth * ray.dir
	bool			isIntersect;	// valid flag
	HitRec () { object = NULL; isIntersect = false; depth = 0.0;}
};

// the Light class represents a point light source
//-----------------------------------------------------------------
class Light {
public:
	Color	emission;				// emission of the ligth in radiance
	Vector	position;				// the position of the point light source
};

// the BoundinbBox class is used by the kd-tree to speed-up computation
//-----------------------------------------------------------------
class BoundingBox {		
public:
	Vector minPoint;				// minimum point of the bounding box
	Vector maxPoint;				// minimum point of the bounding box
	Vector Center (void) {			// returns the center of the BoundingBox
		return (minPoint + maxPoint) * 0.5;
	}
};

// parent class for intersectable objects (e.g. the Triangle is a derived class)
//-----------------------------------------------------------------
class Intersectable {
public:
	BoundingBox		bbox;				// bounding box for space partitioning algorithms
	unsigned int	lastTestedRayId;	// unique identifier of the last tested rya. See GPM paper on kd-tree
	HitRec			lastTestedRayResult;// see GPM paper on kd-tree
	Material*		material;			// material of the object
	long			materialInd;		// the material index of the object (yes, it is redundant)

	Intersectable(long materialIndNew) { materialInd = materialIndNew; lastTestedRayId = 0; }
	~Intersectable() {}
	virtual void	FinishObject	(void) {};
	virtual bool	Intersect		(const Ray& ray, float* t, float rayMin, float rayMax)=0;
	virtual Vector	GetShadingNormal(const Vector& point)=0;
}; 

// the Triangle class represents a triangle patch by its 3 vertices
//-----------------------------------------------------------------
class Triangle : public Intersectable {
public:
	Vector			*a, *b, *c;		// defines the 3 vertices
	Vector			*aN, *bN, *cN;	// defines the vertex normals
	long			ai, bi, ci;		// indexes of the vertices
	Vector			normal;			// surface normal

	// precalculation datas for Green ray-triangle intersection
	DominantAxis	dominantAxis;
	double			hiperPlaneShiftOffset;
	double			abV1, abV2, abC, bcV1, bcV2, bcC, caV1, caV2, caC;
public:
	Triangle(long aiNew, long biNew, long ciNew, long materialIndNew)
		:Intersectable(materialIndNew) { ai = aiNew; bi = biNew; ci = ciNew; }
	void	FinishObject	(void);							// precalculate a lot before intersection calculations
	bool	Intersect		(const Ray& ray, float* t, float rayMin, float rayMax);		// ray-triangle intersection
	Vector	GetShadingNormal(const Vector& point) {			// shading normal is calculated by the vertex normals
		//return normal; 
		Vector shadingNormal;
		Matrix m(a->x, b->x, c->x, a->y, b->y, c->y, a->z, b->z, c->z);
		Matrix mInv = m.Invert();
		if (_finite(mInv.m[0][0])) {	// if it is a full rank matrix
			Vector weights = mInv.MultiplyMatrixLeft(point);
			shadingNormal = weights.x * (*aN) + weights.y * (*bN) + weights.z * (*cN);
		} else {	// if the rank of m less than 3 or it is possible to measure the distances 
			// of the point to vertices, and make a the weight according to the relative distances
			shadingNormal = 1/3.0 * (*aN) + 1/3.0 * (*bN) + 1/3.0 * (*cN);
		}
		shadingNormal.Normalize();
		return shadingNormal;
	}
};

class KDTree;

// the Scene class is the virtual scene
//-----------------------------------------------------------------
class Scene {
public:
	Camera						camera;			// camera
	std::vector<Material>		materials;		// materials for triangles
	std::vector<Vector>			vertices;		// vertices of the triangles
	std::vector<Vector>			vertexNormals;	// normals of the vertices for calculating shading normal
	std::vector<Intersectable*>	objects;		// objects are triangles in this application
	std::vector<Light>			lights;			// array of point light sources
	double						totalLightEmissionLum;	// sum of the emitted radiance of the lights
	KDTree*						kdtree;			// space partition structure

	void Read(const char* path);				// read the scene from a file and build it
};
