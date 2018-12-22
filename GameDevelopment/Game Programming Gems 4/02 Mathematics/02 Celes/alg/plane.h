// plane.h
// Represents plane
// celes@tecgraf.puc-rio.br
// Jul 2003

/* This code is free software; you can redistribute it and/or modify it. 
** The software provided hereunder is on an "as is" basis, and 
** the author has no obligation to provide maintenance, support, updates,
** enhancements, or modifications. 
*/

#ifndef ALG_PLANE_H
#define ALG_PLANE_H

#include <math.h>
#include <stdio.h>

#include "vector.h"

class AlgPlane
{
public:
 float a,b,c,d;

 AlgPlane ()
 {
 }
 AlgPlane (float a, float b, float c, float d)
 : a(a),b(b),c(c),d(d)
 {
 }
 AlgPlane (const AlgVector& n, float d)
 : a(n.x),b(n.y),c(n.z),d(d)
 {
 }
 ~AlgPlane ()
 {
 }
 void Set (float a, float b, float c, float d)
 {
		this->a = a;
		this->b = b;
		this->c = c;
		this->d = d;
	}

 AlgVector GetNormal ()
	{
		return AlgVector(a,b,c);
	}

 void Normalize ()
	{
  float l = (float)sqrt(a*a+b*b+c*c);
  if (fabs(l) > ALG_TOL) {
		 float il = 1.0f/l;
		 a *= il; b *= il; c *= il; d *= il;
  }
	}
 float Distance (const AlgVector& p) const
	{
		return a*p.x+b*p.y+c*p.z+d;
	}
	void Print (const char* label=0) const
	{
	 printf("%s = {%g, %g, %g, %g}\n",label?label:"plane",a,b,c,d);
	}

 friend AlgVector Intersect 
		(const AlgPlane& p1, const AlgPlane& p2, const AlgPlane& p3)
	{
		AlgVector v(0.0f,0.0f,0.0f);
		float d = AlgPlane::Det(p1.a,p1.b,p1.c,p2.a,p2.b,p2.c,p3.a,p3.b,p3.c);
		if (d!=0.0)
		{
			v.Set(Det(-p1.d,p1.b,p1.c,-p2.d,p2.b,p2.c,-p3.d,p3.b,p3.c)/d,
									Det(p1.a,-p1.d,p1.c,p2.a,-p2.d,p2.c,p3.a,-p3.d,p3.c)/d,
									Det(p1.a,p1.b,-p1.d,p2.a,p2.b,-p2.d,p3.a,p3.b,-p3.d)/d
								);
		}
		return v;
	}
private:
	static float Det(float m11, float m12, float m13,
			               float m21, float m22, float m23,
																		float m31, float m32, float m33
																	)
	{
		return ((m11)*(m22)*(m33)-(m11)*(m23)*(m32)-(m21)*(m12)*(m33)+
			   	   (m21)*(m13)*(m32)+(m31)*(m12)*(m23)-(m31)*(m13)*(m22));
	}
};


#endif
