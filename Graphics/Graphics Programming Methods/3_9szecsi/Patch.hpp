#ifndef __PATCH_HPP__
#define __PATCH_HPP__

#include <float.h>
#include <math.h>

class Vector {
public:
	static const Vector RGBBLACK;
	static const Vector RGBWHITE;

	union{
		float v[3];
		struct{ float x; float y; float z; };
		struct{ float r; float g; float b; };
		};

	Vector(){};

	Vector(float x, float y, float z)
	{
		v[0] = x; v[1] = y; v[2] = z;
	}

	Vector operator-() const
	{
		return Vector(-v[0], -v[1], -v[2]);
	}

	Vector operator+(const Vector& addOperand) const
	{
		return Vector (	v[0] + addOperand.v[0],
						v[1] + addOperand.v[1],
						v[2] + addOperand.v[2]);
	}

	Vector operator-(const Vector& substractOperand) const
	{
		return Vector(  v[0] - substractOperand.v[0],
						v[1] - substractOperand.v[1],
						v[2] - substractOperand.v[2]);
	}

	void operator-=(const Vector& a) {
		v[0] -= a[0];
		v[1] -= a[1];
		v[2] -= a[2];
	}

	void operator+=(const Vector& a) {
		v[0] += a[0];
		v[1] += a[1];
		v[2] += a[2];
	}

	void operator%=(const Vector& a)
	{
		v[0] *= a[0];
		v[1] *= a[1];
		v[2] *= a[2];
	}

	void operator*=(const float scale)
	{
		v[0] *= scale;
		v[1] *= scale;
		v[2] *= scale;
	}

	Vector operator%(const Vector& blendOperand) const
	{
		return Vector(v[0] * blendOperand.v[0],
						v[1] * blendOperand.v[1],
						v[2] * blendOperand.v[2]);
	}

	Vector operator*(const float scale) const
	{
		return Vector(scale * v[0], scale * v[1], scale * v[2]);
	}

	float operator*(const Vector& dotProductOperand) const
	{
		return	v[0] * dotProductOperand[0] + 
				v[1] * dotProductOperand[1] +
				v[2] * dotProductOperand[2];
	}

	Vector operator&&(const Vector& crossProductOperand) const
	{
		return Vector(
			v[1] * crossProductOperand[2] - v[2] * crossProductOperand[1],
			v[2] * crossProductOperand[0] - v[0] * crossProductOperand[2],
			v[0] * crossProductOperand[1] - v[1] * crossProductOperand[0]);
	}

	float Normalize ()
	{
		float length = 1.0f / (float)sqrt (v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
		v[0] *= length;
		v[1] *= length;
		v[2] *= length;
		return 1.0f / length;
	}

	const Vector& operator=(const Vector& other)
	{
		v[0] = other.v[0];
		v[1] = other.v[1];
		v[2] = other.v[2];
		return *this;
	}

	float& operator[](const int index)
	{
		return v[index];
	}

	float operator[](const int index) const
	{
		return v[index];
	}

	void operator<=(const Vector& small)
	{
		if(v[0] > small[0]) v[0] = small[0];
		if(v[1] > small[1]) v[1] = small[1];
		if(v[2] > small[2]) v[2] = small[2];
	}

	void operator>=(const Vector& large)
	{
		if(v[0] < large[0]) v[0] = large[0];
		if(v[1] < large[1]) v[1] = large[1];
		if(v[2] < large[2]) v[2] = large[2];
	}

	void SetIdealReflectedDirection (const Vector& in, const Vector& normal)
	{
		*this = in - normal * (2.0f * (normal * in));
	}
};

class Ray{
public:
	Vector origin;
	Vector dir;
	int id;
};

class Light
{
public:
	Vector position;
	Vector radiance;
	Light() {};
	Light(const Vector& position, const Vector& radiance)
	{
		this->position = position;
		this->radiance = radiance;
	}
};

class Material
{
public: //private:
	Vector kd;	//diffuse coefficient
	Vector ks;	//specular energy reflected for perpendicular light
	float ns;	//specularity
public:
	static const Material DIFFUSEWHITE;
	static const Material SHINYBLACK;
	static const Material STEEL;

	Material(const Vector& kd, const Vector& ks, float ns)
	{
		this->kd = kd;
		this->ks = ks;
		this->ns = ns;
	}
	bool isMirror() const
	{
		return (ns > 0.0f) && (ks.r > 0.0f) && (ks.g > 0.0f) && (ks.b > 0.0f);
	}

	void EvalReflectivity(Vector& result) const
	{
		result = ks;
	}

	void EvalBrdfCosTheta (const Vector& in, Vector& out, const Vector& normal, Vector& result) const
	{
		float nDotOut = normal * out;
		float nDotIn = - (normal * in);
		if ( nDotOut < 0) {
			result = Vector::RGBBLACK;
			return;
		}

		result	= kd; 	// add diffuse part
		// add specular
		Vector idealReflected;
		idealReflected.SetIdealReflectedDirection (in, normal);
		float dotProduct = idealReflected* out;
	    if(dotProduct > 0)
		{
			float scale = (float)pow (dotProduct, ns) * (ns + 2.0f) * 0.5f / 3.14159265358979323846f ;
			if(nDotOut > nDotIn)
				scale /= nDotOut;
			else
				scale /= nDotIn;
			result += ks * scale;
		}
		result *= nDotOut;
	}
};


//! Axes-aligned bounding box used by space partitioning algorithms
/*@ note RX is the tag for RenderX, because CyberVrml also define this class */
class BoundingBox {		
public:
	Vector minPoint;
	Vector maxPoint;

	//! returns the center of the BoundingBox
	/*! @return the center of the BoundingBox */
	Vector Center (void) {
		Vector center;
		center = (minPoint + maxPoint) * 0.5;
		return center;
	}
};

class Triangle;
class HitRec;

class Intersectable {
public: //protected:
	const Material*		material;
public:
	Intersectable(const Material* material);
	~Intersectable();
	//! bounding box for space partitioning algorithms
	BoundingBox	bbox;

	unsigned int	lastTestedRayId;
	HitRec*			lastTestedRayResult;
	virtual bool intersect(const Ray& ray, float& depth, float rayMin, float rayMax)=0;
	virtual Vector getShadingNormal(const Vector& point)=0;
}; 

class Triangle : public Intersectable {
	//! defines the 3 vertices
	const Vector*			a, *b, *c;

	Vector			normal;
	enum DominantAxis{X_DOMINANT, Y_DOMINANT, Z_DOMINANT}	dominantAxis;
	float			hiperPlaneShiftOffset;
	float			d1, d2, d3, d4, d5, d6;		// pre-computation for Intersect() speed-up
public:
	void finishTriangle (void);
	Triangle(const Vector* a, const Vector* b, const Vector* c, const Material* material)
		:Intersectable(material),a(a),b(b),c(c)
	{
		finishTriangle();
	}
	bool intersect(const Ray& ray, float& depth, float rayMin, float rayMax);
	Vector getShadingNormal(const Vector& point)
	{
		return normal;
	}
};

class Sphere : public Intersectable {
	//! defines the 3 vertices
	const Vector	centre;
	const float		radius;

public:
	Sphere(const Vector& centre, const float radius, const Material* material)
		:Intersectable(material),centre(centre),radius(radius)
	{
		Vector diagonal(radius, radius, radius);
		bbox.minPoint = centre - diagonal;
		bbox.maxPoint = centre + diagonal;
	}
	bool intersect(const Ray& ray, float& depth, float rayMin, float rayMax);
	Vector getShadingNormal(const Vector& point)
	{
		Vector normal = point - centre;
		normal *= 1.0 / radius;
		return normal;
	}
};

//! Ray - Triangle intersection test result is stored in HitRec
class HitRec {
public:
	Vector	point;			// point of the intersection
	float	depth;			// distance from ray origin
	Intersectable* object;	// object found 
	bool	isIntersect;	//valid flag
	HitRec() { 
		depth=0.0f;
	}
};

#endif //__PATCH_HPP__