/* Copyright (C) John W. Ratcliff, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) John W. Ratcliff, 2001"
 */
#ifndef VECTOR_H

#define VECTOR_H

#include <math.h>

#include "stl.h"


/***********************************************************************/
/** VECTOR.H    : Template class to represent a 2d and 3d vector type. */
/**                                                                    */
/**               Written by John W. Ratcliff jratcliff@att.net        */
/***********************************************************************/

template <class Type> class Vector3d
{
public:
  Vector3d(void) { };  // null constructor, does not inialize point.

  Vector3d(const Vector3d &a) // constructor copies existing vector.
  {
    x = a.x;
    y = a.y;
    z = a.z;
  };

  Vector3d(Type a,Type b,Type c) // construct with initial point.
  {
    x = a;
    y = b;
    z = c;
  };

  bool operator==(const Vector3d<Type> &a) const
  {
    if ( a.x == x && a.y == y && a.z == z ) return true;
    return false;
  };

  bool operator!=(const Vector3d<Type> &a) const
  {
    if ( a.x != x || a.y != y || a.z != z ) return true;
    return false;
  };


// Operators
    Vector3d& operator = (const Vector3d& A)          // ASSIGNMENT (=)
      { x=A.x; y=A.y; z=A.z;
        return(*this);  };

    Vector3d operator + (const Vector3d& A) const     // ADDITION (+)
      { Vector3d Sum(x+A.x, y+A.y, z+A.z);
        return(Sum); };

    Vector3d operator - (const Vector3d& A) const     // SUBTRACTION (-)
      { Vector3d Diff(x-A.x, y-A.y, z-A.z);
        return(Diff); };

    Vector3d operator * (const float s) const       // MULTIPLY BY SCALAR (*)
      { Vector3d Scaled(x*s, y*s, z*s);
        return(Scaled); };

    Vector3d operator / (const float s) const       // DIVIDE BY SCALAR (/)
    { 
      float r = 1.0f / s;
        Vector3d Scaled(x*r, y*r, z*r);
        return(Scaled); 
    };

    void operator += (const Vector3d A)             // ACCUMULATED VECTOR ADDITION (+=)
      { x+=A.x; y+=A.y; z+=A.z; };
    void operator -= (const Vector3d A)             // ACCUMULATED VECTOR SUBTRACTION (+=)
      { x-=A.x; y-=A.y; z-=A.z; };
    void operator *= (const float s)        // ACCUMULATED SCALAR MULTIPLICATION (*=) (bpc 4/24/2000)
      {x*=s; y*=s; z*=s;}

    Vector3d operator - (void) const                // NEGATION (-)
      { Vector3d Negated(-x, -y, -z);
        return(Negated); };

    Type operator [] (const int i) const         // ALLOWS VECTOR ACCESS AS AN ARRAY.
      { return( (i==0)?x:((i==1)?y:z) ); };
    Type & operator [] (const int i)
      { return( (i==0)?x:((i==1)?y:z) ); };
//

  // accessor methods.
  Type GetX(void) const { return x; };
  Type GetY(void) const { return y; };
  Type GetZ(void) const { return z; };

  void SetX(Type t)   { x   = t; };
  void SetY(Type t)   { y   = t; };
  void SetZ(Type t)   { z   = t; };

  void Set(Type a,Type b,Type c)
  {
    x = a;
    y = b;
    z = c;
  };

  void Zero(void)
  {
    x = y = z = 0;
  };

// return -(*this).
	Vector3d negative(void) const
	{
		Vector3d result;
		result.x = -x;
		result.y = -y;
		result.z = -z;
		return result;
	}

	Type Magnitude(void) const
	{
		return Type(sqrtf(x * x + y * y + z * z));
	};

  void Lerp(const Vector3d<Type>& from,const Vector3d<Type>& to,float slerp)
  {
    *this = to-from; // delta on all 3 axis
    *this*=slerp;    // times interpolant distance.
    *this+=from;     // plus source
  };

  Type Length(void) const          // length of vector.
  {
    return Type(sqrt( x*x + y*y + z*z ));
  };

  Type Length2(void) const         // squared distance, prior to square root.
  {
    Type l2 = x*x+y*y+z*z;
    return l2;
  };

  Type Distance(const Vector3d<Type> &a) const   // distance between two points.
  {
    Vector3d<Type> d(a.x-x,a.y-y,a.z-z);
    return d.Length();
  }

  Type DistanceXY(const Vector3d<Type> &a) const
  {
	float dx = a.x - x;
	float dy = a.y - y;
    float dist = dx*dx + dy*dy;
	return dist;
  }

  inline Type Distance2(const Vector3d<Type> &a) const  // squared distance.
  {
    float dx = a.x - x;
    float dy = a.y - y;
    float dz = a.z - z;
    return dx*dx + dy*dy + dz*dz;
  };

  Type Normalize(void)       // normalize to a unit vector, returns distance.
  {
    Type l = Length(); // get length.
    if ( l != 0 )
    {
      x/=l;
      y/=l;
      z/=l;
    }
    else
    {
      x = y = z = 0;
    }
    return l;
  };

  Type Dot(const Vector3d<Type> &a) const        // computes dot product.
  {
		return (x * a.x + y * a.y + z * a.z );
  };

  void Cross(const Vector3d<Type> &a,const Vector3d<Type> &b)  // cross two vectors result in this one.
  {
    x = a.y*b.z - a.z*b.y;
    y = a.z*b.x - a.x*b.z;
    z = a.x*b.y - a.y*b.x;
  };

//private:

  Type x;
  Type y;
  Type z;
};


typedef std::vector< Vector3d<float> > Vector3dVector;

template <class Type> Vector3d<Type> operator * (Type s, const Vector3d<Type> &v )
      { Vector3d <Type> Scaled(v.x*s, v.y*s, v.z*s);
        return(Scaled); };


#endif
