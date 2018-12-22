/*

	Visual Terrain Grid Determination Using Frustrum
	written by Christopher Tremblay
	Last revision (26 Jan / 2003)

	This source code is a complement to the book:
	"Graphics Programming Methods" by 
	Charles River Media

	All rights reserved

	This is not the cleanest implementation of this because 
	it's not using SIMD, but it should be good enough
	to show what this source code wants to show.

	It's by no mean a complete set of operations nor usefull
*/
#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <math.h>
#include "Common.hpp"

#define sqr(x) ((x) * (x))


template <class Type> struct Vector2D {
	Vector2D(void) : X(0), Y(0) {};


	Vector2D(Type X, Type Y) : X(X), Y(Y) {}


	Vector2D(const Vector2D &A) : X(A.X), Y(A.Y) {}


	/// Assignment operators
	void operator =(const Vector2D &A)
	{
		this->X = A.X;
		this->Y = A.Y;
	}


	/// Vector Addition
	Vector2D operator +(const Vector2D &A)
	{
		Vector2D V(A);
		V += *this;
		return V;
	}


	void operator +=(const Vector2D &A) 
	{
		this->X += A.X;
		this->Y += A.Y;
	}


	/// Vector Substraction
	Vector2D operator -(const Vector2D &A)
	{
		Vector2D V(A);
		V -= *this;
		return V;
	}

	
	void operator -=(const Vector2D &A)
	{
		this->X -= A.X;
		this->Y -= A.Y;
	}


	/// Vector scale (forward)
	void operator *=(Type f)
	{
		this->X *= f;
		this->Y *= f;
	}


	/// Vetor scale (backward)
	void operator /=(Type f)
	{
		this->X /= f;
		this->Y /= f;
	}


	Type X, Y;
};



template <class Type> struct Vector3D : public Vector2D<Type> {
	// Explicit constructor
	Vector3D(void) : Z(0) {};


	Vector3D(Type X, Type Y, Type Z) : Z(Z), Vector2D<Type>(X, Y) {}


	Vector3D(const Vector3D &A) : Z(A.Z), Vector2D<Type>(A.X, A.Y) {}


	// Assignment operators
	void operator =(const Vector3D &A)
	{
		this->X = A.X;
		this->Y = A.Y;
		this->Z = A.Z;
	}


	// Vector Addition
	Vector3D operator +(const Vector3D &A)
	{
		Vector3D V(A);
		V += *this;
		return V;
	}


	void operator +=(const Vector3D &A) 
	{
		this->X += A.X;
		this->Y += A.Y;
		this->Z += A.Z;
	}


	// Vector Substraction
	Vector3D operator -(const Vector3D &A)
	{
		Vector3D V(*this);
		V -= A;
		return V;
	}

	
	void operator -=(const Vector3D &A)
	{
		this->X -= A.X;
		this->Y -= A.Y;
		this->Z -= A.Z;
	}


	// Vector scale (forward)
	void operator *=(Type f)
	{
		this->X *= f;
		this->Y *= f;
		this->Z *= f;
	}


	Vector3D operator *(Type f)
	{
		return Vector3D(this->X * f, this->Y * f, this->Z * f);
	}


	// Vetor scale (backward)
	void operator /=(Type f)
	{
		this->X /= f;
		this->Y /= f;
		this->Z /= f;
	}

	// Vector comparison
	bool operator ==(Vector3D &B)
	{
		return ((this->X == B.X) && (this->Y == B.Y) && (this->Z == B.Z));
	}

	// Dot Product
	Type operator *(const Vector3D &A)
	{
		return ((this->X * A.X) + (this->Y * A.Y) + (this->Z * A.Z));
	}


	// Cross Product
	Vector3D operator /(const Vector3D &A)
	{
		return Vector3D(this->Y * A.Z - A.Y * this->Z,
						this->Z * A.X - A.Z * this->X,
						this->X * A.Y - A.X * this->Y);
	}

	// Here we overload the / operator so we can divide by a scalar
	Vector3D operator/(Type num)
	{
		// Return the scale vector
		return Vector3D(X / num, Y / num, Z / num);
	}


	
	// Vector normalization
	void Normalize(void)
	{
		Type n = Norm();

		this->X /= n;
		this->Y /= n;
		this->Z /= n;
	}


	// Norm (length)
	Type Norm(void)
	{
		return (Type)sqrt(sqr(this->X) + sqr(this->Y) + sqr(this->Z));
	}


	Type Z;
};



template <class Type> struct Vector4D : public Vector3D<Type> {
	Vector4D() {};
	Vector4D(Type X, Type Y, Type Z, Type W) : W(W), Vector3D<Type>(X, Y, Z) {}

	Type W;
};


#undef sqr

#endif
