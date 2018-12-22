// frustum.cpp
// Tecgraf/PUC-Rio
// celes@tecgraf.puc-rio.br
// Jul 2003


/* This code is free software; you can redistribute it and/or modify it. 
** The software provided hereunder is on an "as is" basis, and 
** the author has no obligation to provide maintenance, support, updates,
** enhancements, or modifications. 
*/


#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include "frustum.h"

#define VGL_PI  3.1415926f

static void multmatrix (float* a, float *b, float* c)
{
 int i,j,k,l;
 for (i=0; i<4; ++i)
 {
  for (j=0; j<16; j+=4)
  {
   c[j+i] = 0.0f;
   for (k=0,l=0; k<4; ++k,l+=4)
    c[j+i] += a[l+i] * b[j+k];
  }
 }
}

VglFrustum::VglFrustum (float* Mmv, float* Mp)
: AlgFrustum(6)
{
	InitCanonicalPlanes();
 Update(Mmv,Mp);
}

VglFrustum::VglFrustum (float* Mmvp)
: AlgFrustum(6)
{
	InitCanonicalPlanes();
 Update(Mmvp);
}

VglFrustum::VglFrustum ()
: AlgFrustum(6)
{
	InitCanonicalPlanes();
 Update();
}

void VglFrustum::Update (float* Mmv, float* Mp)
{
	float Mmvp[16];
 multmatrix(Mp,Mmv,Mmvp);
	Update(Mmvp);
}

void VglFrustum::Update (float* Mmvp)
{
	SetVertexMatrix(Mmvp);
}

void VglFrustum::Update ()
{
 float Mmv[16], Mp[16];
 glGetFloatv(GL_MODELVIEW_MATRIX,Mmv);
 glGetFloatv(GL_PROJECTION_MATRIX,Mp);
	Update(Mmv,Mp);
}

AlgPlane VglFrustum::GetPlane (int id)
{
	return AlgFrustum::GetPlane(id);  // implemented for documentation
}

AlgVector VglFrustum::GetEyePos ()
{
 AlgPlane l = GetPlane(VGL_PLANE_LEFT);
 AlgPlane r = GetPlane(VGL_PLANE_RIGHT);
 AlgPlane t = GetPlane(VGL_PLANE_TOP);
	return Intersect(l,r,t);
}

AlgVector VglFrustum::GetViewDir ()
{
	AlgPlane n = GetPlane(VGL_PLANE_NEAR);
	return AlgVector(n.a,n.b,n.c);
}

AlgVector VglFrustum::GetViewUp ()
{
	AlgPlane l = GetPlane(VGL_PLANE_LEFT);
	AlgPlane r = GetPlane(VGL_PLANE_RIGHT);
	AlgVector lv(l.a,l.b,l.c);
	AlgVector rv(r.a,r.b,r.c);
	return Cross(rv,lv);
}

float VglFrustum::GetNearDist ()
{
 AlgVector p = GetEyePos();
 AlgPlane n = GetPlane(VGL_PLANE_NEAR);
	n.Normalize();
 return -n.Distance(p);
}

float VglFrustum::GetFarDist ()
{
 AlgVector p = GetEyePos();
 AlgPlane f = GetPlane(VGL_PLANE_FAR);
	f.Normalize();
 return f.Distance(p);
}

float VglFrustum::GetFovx ()
{
 AlgPlane l = GetPlane(VGL_PLANE_LEFT);
 AlgPlane r = GetPlane(VGL_PLANE_RIGHT);
	l.Normalize();
	r.Normalize();
 return 180.0f - (float)acos(l.a*r.a+l.b*r.b+l.c*r.c)*180.0f/VGL_PI;
}

float VglFrustum::GetFovy ()
{
 AlgPlane b = GetPlane(VGL_PLANE_BOTTOM);
 AlgPlane t = GetPlane(VGL_PLANE_TOP);
	b.Normalize();
	t.Normalize();
 return 180.0f - (float)acos(b.a*t.a+b.b*t.b+b.c*t.c)*180.0f/VGL_PI;
}

void VglFrustum::InitCanonicalPlanes ()
{
	SetCanonicalPlane(VGL_PLANE_LEFT,1,0,0,1);
	SetCanonicalPlane(VGL_PLANE_RIGHT,-1,0,0,1);
	SetCanonicalPlane(VGL_PLANE_BOTTOM,0,1,0,1);
	SetCanonicalPlane(VGL_PLANE_TOP,0,-1,0,1);
	SetCanonicalPlane(VGL_PLANE_NEAR,0,0,1,1);
	SetCanonicalPlane(VGL_PLANE_FAR,0,0,-1,1);
}
