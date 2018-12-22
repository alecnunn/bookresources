/* Copyright (C) Miguel Gomez, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Miguel Gomez, 2000"
 */
#ifndef _VECTOR_H_
#define _VECTOR_H_


#include <cmath>


//define a 2 component vector template
template<class T>
struct T_VECTOR2
{
	T x,y;	//x,y coordinates

	//constructors
	//default
	T_VECTOR2()
		: x(0), y(0)
	{}

	//initialize
	T_VECTOR2( const T a, const T b )
		: x(a), y(b)
	{}

	//indexing (read-only)
	const T& operator [] ( const long i ) const
	{
		return *((&x) + i);
	}

	//indexing (write)
	T& operator [] ( const long i )
	{
		return *((&x) + i);
	}

	//assignment
	const T_VECTOR2& operator = ( const T_VECTOR2& b )
	{
		x = b.x;
		y = b.y;

		return *this;
	}

	//comparison
	const bool operator == ( const T_VECTOR2& b ) const
	{
		return( b.x==x && b.y==y );
	}

	const bool operator != ( const T_VECTOR2& b ) const
	{
		return !( b == *this );
	}

	const bool nearlyEquals( const T_VECTOR2& b, const T r ) const
	{
		//within a tolerance
		const T_VECTOR2 t = *this - b;//difference
		return t.dot(t) < r*r;//radius
	}

	//negation
	const T_VECTOR2 operator - () const
	{
		return T_VECTOR2( -x, -y );
	}

	//increment
	const T_VECTOR2& operator += ( const T_VECTOR2& b )
	{
		x += b.x;
		y += b.y;

		return *this;
	}

	//decrement
	const T_VECTOR2& operator -= ( const T_VECTOR2& b ) 
	{
		x -= b.x;
		y -= b.y;

		return *this;
	}

	//self-multiply
	const T_VECTOR2& operator *= ( const T s )
	{
		x *= s;
		y *= s;

		return *this;
	}

	//self-divide
	const T_VECTOR2& operator /= ( const T s )
	{
		x /= s;
		y /= s;

		return *this;
	}

	//add
	const T_VECTOR2 operator + ( const T_VECTOR2& b ) const
	{
		return T_VECTOR2( x + b.x, y + b.y );
	}

	//subtract
	const T_VECTOR2 operator - ( const T_VECTOR2& b ) const
	{
		return T_VECTOR2( x - b.x, y - b.y );
	}

	//post-multiply by a scalar
	const T_VECTOR2 operator * ( const T s ) const
	{
		return T_VECTOR2( x*s, y*s );
	}

	//pre-multiply by a scalar
	friend inline const T_VECTOR2 operator * ( const T s, const T_VECTOR2& v )
	{
		return v * s;
	}

	//divide
	const T_VECTOR2 operator / ( const T s ) const
	{
		return T_VECTOR2( x/s, y/s );
	}

	//NOTE:  The cross product is not defined for 2D vectors.

	//dot product
	const T dot( const T_VECTOR2& b ) const
	{
		return( x*b.x + y*b.y);
	}

	//length squared
	const T length_squared() const
	{
		return this->dot(*this);
	}

	//length
	const T length() const
	{
		//NOTE:  cast the return value of
		//sqrt() from a double to a T
		return (T)sqrt( this->length_squared() );
	}

	//unit vector
	const T_VECTOR2 unit() const
	{
		return (*this) / this->length();
	}

	//make this a unit vector
	const T_VECTOR2& normalize()
	{
		(*this) /= this->length();

		return *this;
	}
};



//
// A 3D vector template
//
template <class T>
struct T_VECTOR3
{
	T x,y,z;	//x,y,z coordinates

	//constructors
	//default
	T_VECTOR3()
		:	x(0), y(0), z(0)
	{}

	//initialize
	T_VECTOR3( const T a, const T b, const T c )
		:	x(a), y(b), z(c)
	{}

	//indexing (read-only)
	const T& operator [] ( const long i ) const
	{
		return *((&x) + i);
	}

	//indexing (write)
	T& operator [] ( const long i )
	{
		return *((&x) + i);
	}

	//compare
	const bool operator == ( const T_VECTOR3& b ) const
	{
		return (b.x==x && b.y==y && b.z==z);
	}

	const bool operator != ( const T_VECTOR3& b ) const
	{
		return !(b == *this);
	}

	const bool nearlyEquals( const T_VECTOR3& b, const T r ) const
	{
		//within a tolerance
		const T_VECTOR3 t = *this - b;//difference
		return t.dot(t) < r*r;//radius
	}

	//negate
	const T_VECTOR3 operator - () const
	{
		return T_VECTOR3( -x, -y, -z );
	}

	//assign
	const T_VECTOR3& operator = ( const T_VECTOR3& b )
	{
		x = b.x;
		y = b.y;
		z = b.z;

		return *this;
	}

	//increment
	const T_VECTOR3& operator += ( const T_VECTOR3& b ) 
	{
		x += b.x;
		y += b.y;
		z += b.z;

		return *this;
	} 

	//decrement
	const T_VECTOR3& operator -= ( const T_VECTOR3& b ) 
	{
		x -= b.x;
		y -= b.y;
		z -= b.z;

		return *this;
	} 

	//self-multiply
	const T_VECTOR3& operator *= ( const T s )
	{
		x *= s;
		y *= s;
		z *= s;

		return *this;
	}

	//self-divide
	const T_VECTOR3& operator /= ( const T s )
	{
		x /= s;
		y /= s;
		z /= s;

		return *this;
	}

	//add
	const T_VECTOR3 operator + ( const T_VECTOR3& b ) const
	{
		return T_VECTOR3( x + b.x, y + b.y, z + b.z );
	}

	//subtract
	const T_VECTOR3 operator - ( const T_VECTOR3& b ) const
	{
		return T_VECTOR3( x - b.x, y - b.y, z - b.z );
	}

	//post-multiply by a scalar
	const T_VECTOR3 operator * ( const T s) const
	{
		return T_VECTOR3( x*s, y*s, z*s );
	}

	//pre-multiply by a scalar
	friend inline const T_VECTOR3 operator * ( const T s, const T_VECTOR3& v )
	{
		return v * s;
	}

	//divide
	const T_VECTOR3 operator / ( const T s ) const
	{
		return T_VECTOR3( x/s, y/s, z/s );
	}

	//cross product
	const T_VECTOR3 cross( const T_VECTOR3& b ) const
	{
		return T_VECTOR3( y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x );
	}

	//scalar dot product
	const T dot( const T_VECTOR3& b ) const
	{
		return x*b.x + y*b.y + z*b.z;
	}

	//length squared
	const T length_squared() const
	{
		return this->dot(*this);
	}

	//length
	const T length() const
	{
		//NOTE:  cast the return value of
		//sqrt() from a double to a T
		return (T)sqrt( this->length_squared() );
	}

	//unit vector
	const T_VECTOR3 unit() const
	{
		return (*this) / this->length();
	}

	//make this a unit vector
	const T_VECTOR3& normalize()
	{
		(*this) /= this->length();

		return *this;
	}
};






//define a 4 component template
template<class T>
struct T_VECTOR4
{
	T x,y,z,w;	//x,y,z,w coordinates

	//default
	T_VECTOR4()
		:	x(0),
			y(0),
			z(0),
			w(0)
	{}

	//initialize
	T_VECTOR4( const T a, const T b, const T c, const T d )
		:	x (a),
			y (b),
			z (c),
			w (d)
	{}

	//indexing, read
	const T& operator [] ( const long i ) const
	{
		return *((&x) + i);
	}

	//indexing, write
	T& operator [] ( const long i )
	{
		return *((&x) + i);
	}

	//comparison
	const bool operator == ( const T_VECTOR4& b ) const
	{
		return( b.x==x && b.y==y && b.z==z && b.w==w );
	}

	const bool operator != ( const T_VECTOR4& b ) const
	{
		return !( b == *this );
	}

	const bool nearlyEquals( const T_VECTOR4& b, const T r ) const
	{
		//within a tolerance
		const T_VECTOR4 t = *this - b;//difference
		return t.dot(t) < r*r;//radius
	}

	//negate
	T_VECTOR4 operator - ( void ) const
	{
		return T_VECTOR4( -x, -y, -z, -w );
	}

	//assign
	const T_VECTOR4& operator = ( const T_VECTOR4& b )
	{
		x = b.x;
		y = b.y;
		z = b.z;
		w = b.w;

		return *this;
	}

	//increment
	const T_VECTOR4& operator += ( const T_VECTOR4& b ) 
	{
		x += b.x;
		y += b.y;
		z += b.z;
		w += b.w;

		return *this;
	} 

	//decrement
	const T_VECTOR4& operator -= ( const T_VECTOR4& b ) 
	{
		x -= b.x;
		y -= b.y;
		z -= b.z;
		w -= b.w;

		return *this;
	} 

	//self-multiply
	const T_VECTOR4& operator *= ( const T s )
	{
		x *= s;
		y *= s;
		z *= s;
		w *= s;

		return *this;
	} 

	//self-divide
	const T_VECTOR4& operator /= ( const T s )
	{
		x /= s;
		y /= s;
		z /= s;
		w /= s;

		return *this;
	}

	//add vectors, return sum
	const T_VECTOR4 operator + ( const T_VECTOR4& b ) const
	{
		return T_VECTOR4( x+b.x, y+b.y, z+b.z, w+b.w );
	}

	//subtract vectors
	const T_VECTOR4 operator - ( const T_VECTOR4& b ) const
	{
		return T_VECTOR4( x-b.x, y-b.y, z-b.z, w-b.w );
	}

	//multiply
	const T_VECTOR4 operator * ( const T s ) const
	{
		return T_VECTOR4( x*s, y*s, z*s, w*s );
	}

	//pre - multiply
	friend inline const T_VECTOR4 operator * ( const T s, const T_VECTOR4& v )
	{
		return v * s;
	}

	//divide
	const T_VECTOR4 operator / ( const T s ) const
	{
		return T_VECTOR4( x/s, y/s, z/s, w/s );
	}

	//the cross product needs two other vectors

	//dot product
	const T dot( const T_VECTOR4& b ) const
	{
		return( x*b.x + y*b.y + z*b.z + w*b.w );
	}

	//length squared
	const T length_squared() const
	{
		return this->dot(*this);
	}

	//length
	const T length() const
	{
		//NOTE:  cast the return value of
		//sqrt() from a double to a T
		return (T)sqrt( this->length_squared() );
	}

	//unit vector
	const struct T_VECTOR3 unit() const
	{
		return (*this) / this->length();
	}

	//make this a unit vector
	const T_VECTOR4& normalize()
	{
		(*this) /= this->length();

		return *this;
	}
};


#endif
//EOF
