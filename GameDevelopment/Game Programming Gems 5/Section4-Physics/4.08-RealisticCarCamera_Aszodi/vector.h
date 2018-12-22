//************************************************************************* //
// 3D Vector class
//
// Author: Szirmay-Kalos, László
// Created: November, 2002., Budapest, Hungary
//
// Budapest University of Technology and Economics,
// Department of Control Engineering and Information Technology (BME-IIT)
//************************************************************************* //

#ifndef M_PI
#define M_PI       3.14159265358979323846         // The value of PI
#endif


#ifndef VECTOR_H
#define VECTOR_H

// This class is a helper class.
// In this class we defined several useable function, which can help us in other part of the code
//===============================================================
class Vector3f {
//===============================================================
public:
    float x, y, z;                                // Descartes (Cartesian) coordinates
	Vector3f( ) { x = y = z = 0.0; }
	Vector3f( float x0, float y0, float z0, float w0 = 1.0 ) { x = x0; y = y0; z = z0; }

	void set( float x0, float y0, float z0 ) { x = x0; y = y0; z = z0; }

	// The sum of two vectors
	Vector3f operator+( const Vector3f& v ) {
		float X = x + v.x, Y = y + v.y, Z = z + v.z;
		return Vector3f(X, Y, Z);
	}
	// The difference of two vectors
	Vector3f operator-( const Vector3f& v ) { 
		float X = x - v.x, Y = y - v.y, Z = z - v.z;
		return Vector3f(X, Y, Z);
	}	
	// The vector is multiplied by a scalar
	Vector3f operator*( float f ) {
		return Vector3f( x * f, y * f, z * f );
	}
	// The dot product of two vectors
	float operator*( const Vector3f& v ) {
		return (x * v.x + y * v.y + z * v.z);
	}
	// The cross product of two vectors
	Vector3f operator%( const Vector3f& v ) {
		float X = y * v.z - z * v.y, Y = z * v.x - x * v.z, Z = x * v.y - y * v.x;
		return Vector3f(X, Y, Z);
	}
	// The length of the vector
	float Length( ) {
		return (float)sqrt( x * x + y * y + z * z );
	}
	// Incremental addition of two vectors
	void operator+=( const Vector3f& v ) {
		x += v.x; y += v.y; z += v.z; 
	}
	// Incremental subtraction of two vectors
	void operator-=( const Vector3f& v ) {
		x -= v.x; y -= v.y; z -= v.z; 
	}
	// Incremental multiplication of a vector and a scalar
	void operator*=( float f ) {
		x *= f; y *= f; z *= f; 
	}
	// Dividing the vector by a scalar 
	Vector3f operator/( float f ) {
		return Vector3f( x/f, y/f, z/f ); 
	}
	// Normalize the vector
	void Normalize( ) {
		float l = Length( );
		if ( l < 0.000001f) { x = 1; y = 0; z = 0; }
		else { x /= l; y /= l; z /= l; }
	}
	// A unit vector, which is parallel with the vector
	Vector3f UnitVector( ) {
		Vector3f r = * this;
		r.Normalize();
		return r;
	}
	// Rotate the vector around the given axis
	Vector3f Rotate( Vector3f& axis, float angleRAD ) {
		Vector3f iv = this -> UnitVector();
		Vector3f jv = axis.UnitVector() % this -> UnitVector();
		float radian = angleRAD * M_PI/180;
		return ((iv * cos(radian) +  jv * sin(radian)) * this -> Length());
	}
	// Return with the references of the coordinates
	float& X() { return x; }
	float& Y() { return y; }
	float& Z() { return z; }
};


#endif