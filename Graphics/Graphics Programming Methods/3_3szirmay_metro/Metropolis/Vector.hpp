// Graphics Programming Methods
// Metropolis sampling in global illumination
// György Antal, Lászlo Szirmay-Kalos
// 2003.

// implementation of the Vector class

#pragma once
#define _USE_MATH_DEFINES		// for math.h to define M_PI
#include <math.h>

// the Vector class represents the vectors
//-----------------------------------------------------------------
class Vector {
public:
	float x, y, z;

	Vector () {}
	Vector (float xx, float yy, float zz) {
		x = xx; y = yy; z = zz;
	}
	
	void Set (float xx, float yy, float zz) {
		x = xx; y = yy; z = zz;
	}

	// binary operators
	Vector operator+(const Vector& c) const {
		return Vector (x + c.x, y + c.y, z + c.z);
	}

	Vector operator-(const Vector& c) const {
		return Vector (x - c.x, y - c.y, z - c.z);
	}

	Vector operator*(float f) const {
		return Vector (x * f, y * f, z * f);
	}

	Vector operator/(float f) const {
		return Vector (x / f, y / f, z / f);
	}

	// unary operators
	void operator+=(float f) { 
		x += f; y += f; z += f; 
	}

	void operator+=(const Vector& c) { 
		x += c.x; y += c.y; z += c.z; 
	}

	void operator*=(float f) { 
		x *= f; y *= f; z *= f; 
	}

	Vector operator-(void) const { 
		return Vector (-x, -y, -z);
	}

	// other methods
	float operator*(const Vector& v) const {		// DotProduct
		return x * v.x + y * v.y + z * v.z; 
	}

	Vector operator%(const Vector& v) const {		// CrossProduct
		return Vector (y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}    

	void operator<=(const Vector& small) {
		if (x > small.x) x = small.x;
		if (y > small.y) y = small.y;
		if (z > small.z) z = small.z;
	}

	void operator>=(const Vector& large) {
		if (x < large.x) x = large.x;
		if (y < large.y) y = large.y;
		if (z < large.z) z = large.z;
	}

	float Norm (void) const {
		return sqrt (x*x + y*y + z*z);
	}

	void Normalize () {
		float norm = Norm();
		x /= norm;
		y /= norm;
		z /= norm;
	}

	// other methods
	friend Vector operator*(float f, const Vector& v);
};

inline Vector operator*(float f, const Vector& v) {
	return Vector (f * v.x, f * v.y, f * v.z);
}

enum DominantAxis {
	X_DOMINANT,
	Y_DOMINANT,
	Z_DOMINANT
};
