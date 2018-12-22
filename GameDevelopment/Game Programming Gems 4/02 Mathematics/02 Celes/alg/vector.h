// vector.h
// Represents triples.
// celes@tecgraf.puc-rio.br
// Jul 2003

/* This code is free software; you can redistribute it and/or modify it. 
** The software provided hereunder is on an "as is" basis, and 
** the author has no obligation to provide maintenance, support, updates,
** enhancements, or modifications. 
*/

#ifndef ALG_VECTOR_H
#define ALG_VECTOR_H

#include <math.h>
#include <stdio.h>

#define ALG_TOL 1.0e-7

class AlgVector
{
public:
  float x, y, z;
  AlgVector ()
  {
  }
  AlgVector (float vx, float vy, float vz)
    : x(vx), y(vy), z(vz)
  {
  }
  ~AlgVector ()
  {
  }
  void Set (float vx, float vy, float vz)
  {
    x = vx; y = vy; z = vz;
  }
  float Dot (const AlgVector& q) const
  {
    return x*q.x + y*q.y + z*q.z;
  }
  float Length () const
  {
    return (float)sqrt(x*x+y*y+z*z);
  }
  void Normalize ()
  {
    float l = Length();
    if (fabs(l) > ALG_TOL) {
      float d = 1.0f/l;
      x *= d; y *= d; z *= d;
    }
  }
  void Print (const char* label) const
  {
    printf("%s = {%g, %g, %g}\n",label,x,y,z);
  }
  friend AlgVector Cross (const AlgVector& a, const AlgVector& b)
  {
			AlgVector v;
   v.x = a.y*b.z - a.z*b.y;
   v.y = a.z*b.x - a.x*b.z;
   v.z = a.x*b.y - a.y*b.x;
			return v;
  }
};

#endif
