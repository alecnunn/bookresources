/***********************************************
                                                                                
Demo for chapter "Gridless Controllable Fire"
in Games Programming Gems 5.
                                                                                
Author: Neeharika Adabala
                                                                                
Date: August 2004
                                                                                
************************************************/

#ifndef VECTOR_H
#define VECTOR_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>


#ifdef _MSC_VER
#  define INT2 short
#  define INT4 int
#  define FLOAT4 float
#  define double double
#  define BIGENDIAN 0
#endif


typedef unsigned char byte;
typedef unsigned int uint4;

class VECTOR
{
public:
	double x,y,z;
	
	VECTOR () {}
    
    VECTOR (double xc, double yc, double zc)
		:x(xc), y(yc), z(zc) {}

	~VECTOR() {}
 void Normalize ()
      {
      double l = 1.0/ sqrt (x*x + y*y + z*z);
      x*=l; y*=l; z*=l;
      }
    VECTOR Norm () const
      {
      float l = 1.0/sqrt (x*x + y*y + z*z);
      return VECTOR (x*l, y*l, z*l);
      }

    /*!
      returns the vector's length.
      */
    double Length () const
      {
      return sqrt (x*x + y*y + z*z);
      }
    /*!
      returns the square of the vector's length.
      Faster than Length()!
      */
    double SquaredLength () const
      {
      return (x*x + y*y + z*z);
      }

    /*! */
    VECTOR operator- () const
      {
      return VECTOR (-x, -y, -z);
      }

    /*! */
    void Flip ()
      {
      x=-x; y=-y; z=-z;
      }

    /*! */
    VECTOR &operator+= (const VECTOR &vec)
      {
      x+=vec.x; y+=vec.y; z+=vec.z;
      return *this;
      }
    /*! */
    VECTOR operator+ (const VECTOR &vec) const
      {
      return VECTOR (x+vec.x, y+vec.y, z+vec.z);
      }
    /*! */
    VECTOR &operator-= (const VECTOR &vec)
      {
      x-=vec.x; y-=vec.y; z-=vec.z;
      return *this;
      }
    /*! */
    VECTOR operator- (const VECTOR &vec) const
      {
      return VECTOR (x-vec.x, y-vec.y, z-vec.z);
      }

    /*! */
    VECTOR &operator*= (double factor)
      {
      x*=factor; y*=factor; z*=factor;
      return *this;
      }
    /*! */
    VECTOR operator* (double factor) const
      {
      return VECTOR (x*factor, y*factor, z*factor);
      }
    /*! */
    friend inline VECTOR operator* (double factor, const VECTOR &vec)
      {
      return VECTOR (vec.x*factor, vec.y*factor, vec.z*factor);
      }
    /*! */
    VECTOR &operator*= (const VECTOR &vec)
      {
      x*=vec.x; y*=vec.y; z*=vec.z;
      return *this;
      }
    /*! */
    VECTOR operator* (const VECTOR &vec) const
      {
      return VECTOR (x*vec.x, y*vec.y, z*vec.z);
      }
    /*! */
    VECTOR &operator/= (double divisor)
      {
      double mult = 1.0/divisor;
      x*=mult; y*=mult; z*=mult;
      return *this;
      }
    /*! */
    VECTOR operator/ (double divisor) const
      {
      double mult = 1.0/divisor;
      return VECTOR (x*mult, y*mult, z*mult);
      }
    /*! */
    VECTOR &operator/= (const VECTOR &vec)
      {
      x/=vec.x; y/=vec.y; z/=vec.z;
      return *this;
      }

    /*! */
    VECTOR operator/ (const VECTOR &vec) const
      {
      return VECTOR (x/vec.x, y/vec.y, z/vec.z);
      }
    /*! */
    void Minimize (const VECTOR &vec)
      {
      if (x > vec.x) x = vec.x;
      if (y > vec.y) y = vec.y;
      if (z > vec.z) z = vec.z;
      }
    /*! */
    void Maximize (const VECTOR &vec)
      {
      if (x < vec.x) x = vec.x;
      if (y < vec.y) y = vec.y;
      if (z < vec.z) z = vec.z;
      }

    /*! */
    friend inline double Dot (const VECTOR &a, const VECTOR &b)
      {
      return a.x*b.x + a.y*b.y + a.z*b.z;
      }
    /*! */
    friend inline VECTOR Cross (const VECTOR &a, const VECTOR &b)
      {
      return VECTOR (a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
      }

};

#endif

