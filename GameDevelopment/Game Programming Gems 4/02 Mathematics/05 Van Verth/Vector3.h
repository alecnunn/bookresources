//===============================================================================
// @ Vector3.h
// 
// Bare bones vector class
// Enough for covariance calculations, no more 
// ------------------------------------------------------------------------------
// Copyright (C) 2003  James M. Van Verth
//
// Change history:
//
// 01-Aug-03	JMV	First version
//
//===============================================================================

#pragma once

#ifndef __Vector3__h__
#define __Vector3__h__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <math.h>

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

struct Vector3
{
    // base constructors, destructor
    Vector3();
    Vector3( float _x, float _y, float _z );
    ~Vector3();

    // copy operations
    Vector3( const Vector3& other );
    Vector3& operator=( const Vector3& other );

    // set values
    void Set( float _x, float _y, float _z );

    // indexing
    float operator[](unsigned int i) const;
    float& operator[](unsigned int i);

    // unary minus
    Vector3& operator-();

    // addition, scale
    Vector3& operator+=( const Vector3& other );
    Vector3& operator*=( float scale );
    friend Vector3 operator*( float scale, const Vector3& vector );

    // subtraction
    Vector3 operator-( const Vector3& other ) const;

    // product operations
    float Dot( const Vector3& other ) const;
    Vector3 Cross( const Vector3& other ) const;

    // misc
    void Normalize();

    // member variables
    float x, y, z;
};

//-------------------------------------------------------------------------------
//-- Inlines --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Default constructor
//-------------------------------------------------------------------------------
inline 
Vector3::Vector3()
{
}

//-------------------------------------------------------------------------------
// Values constructor
//-------------------------------------------------------------------------------
inline
Vector3::Vector3( float _x, float _y, float _z )
{ 
    x = _x; y = _y; z = _z; 
}


//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
inline 
Vector3::~Vector3()
{
}


//-------------------------------------------------------------------------------
// Copy constructor
//-------------------------------------------------------------------------------
inline
Vector3::Vector3( const Vector3& other )
{ 
    if ( &other != this )
    {
        x = other.x; y = other.y; z = other.z;
    }
}


//-------------------------------------------------------------------------------
// Assignment operator
//-------------------------------------------------------------------------------
inline
Vector3&
Vector3::operator=( const Vector3& other )
{ 
    if ( &other != this )
    {
       x = other.x; y = other.y; z = other.z;
    }

    return *this;
}


//-------------------------------------------------------------------------------
// Set values
//-------------------------------------------------------------------------------
inline void 
Vector3::Set( float _x, float _y, float _z )
{ 
    x = _x; y = _y; z = _z; 
}


//-------------------------------------------------------------------------------
// Const index operator
//-------------------------------------------------------------------------------
inline float 
Vector3::operator[](unsigned int i) const
{
    return (&x)[i];
}


//-------------------------------------------------------------------------------
// Non-const index operator
//-------------------------------------------------------------------------------
inline float& 
Vector3::operator[](unsigned int i)
{
   return (&x)[i];
}


//-------------------------------------------------------------------------------
// Unary minus
//-------------------------------------------------------------------------------
inline Vector3& 
Vector3::operator-()
{
    x = -x; y = -y; z = -z;

    return *this;
}


//-------------------------------------------------------------------------------
// Add to self
//-------------------------------------------------------------------------------
inline Vector3& 
Vector3::operator+=( const Vector3& other )
{
    x += other.x; y += other.y; z += other.z;

    return *this;
}


//-------------------------------------------------------------------------------
// Scale self
//-------------------------------------------------------------------------------
inline Vector3& 
Vector3::operator*=( float scale )
{
    x *= scale; y *= scale; z *= scale;

    return *this;
}


//-------------------------------------------------------------------------------
// Scale
//-------------------------------------------------------------------------------
inline Vector3 
operator*( float scale, const Vector3& vector )
{
    Vector3 result( scale*vector.x, scale*vector.y, scale*vector.z );

    return result;

}


//-------------------------------------------------------------------------------
// Subtract
//-------------------------------------------------------------------------------
inline Vector3 
Vector3::operator-( const Vector3& other ) const
{
    return Vector3( x - other.x, y - other.y, z - other.z );
}


//-------------------------------------------------------------------------------
// Dot product
//-------------------------------------------------------------------------------
inline float 
Vector3::Dot( const Vector3& other ) const
{
    return x*other.x + y*other.y + z*other.z;
}


//-------------------------------------------------------------------------------
// Cross product
//-------------------------------------------------------------------------------
inline Vector3 
Vector3::Cross( const Vector3& other ) const
{
    Vector3 result( y*other.z - z*other.y,  z*other.x - x*other.z,  x*other.y - y*other.x );

    return result;
}


//-------------------------------------------------------------------------------
// Normalize
//-------------------------------------------------------------------------------
inline void 
Vector3::Normalize()
{
    float lengthsq = x*x + y*y + z*z;

    if ( lengthsq < 1.0e-6f )
    {
        x = 0.0f; y = 0.0f; z = 0.0f;
    }
    else
    {
        float recip = 1.0f/sqrtf( lengthsq );
        x *= recip; y *= recip; z *= recip;
    }
}

#endif