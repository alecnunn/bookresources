/* Copyright (C) Marco Tombesi, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Marco Tombesi, 2001"
 */
#pragma once

typedef float vect[3];

typedef int faceIdx[3];

typedef float quat[4];

typedef float matrix[16];

inline void CpyVect(vect vd, vect vs)
{   vd[0]=vs[0];
	vd[1]=vs[1];
	vd[2]=vs[2];
}
inline void VectAdd(vect vd, vect v1, vect v2) 
{
	vd[0]=v1[0]+v2[0];
	vd[1]=v1[1]+v2[1];
	vd[2]=v1[2]+v2[2];
}

inline void VectDelta(vect vd,vect v1,vect v2)
{
	vd[0]=v1[0]-v2[0];
	vd[1]=v1[1]-v2[1];
	vd[2]=v1[2]-v2[2];
}

inline void VectMul(vect vd,vect vo,float s)
{ 
	vd[0]=vo[0]*s;
	vd[1]=vo[1]*s;
	vd[2]=vo[2]*s;
}

float VectDist(vect from, vect to);

void MatXVect(vect vd, matrix tm, vect vec);

void CpyMat(matrix vd, const matrix src);
inline void Identity(matrix tm) 
{
	matrix identityTM =
	{	
		1.f, 0.f, 0.f, 0.f,	
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};
	CpyMat(tm, identityTM);
}
void GetTranslation(const matrix tm, vect trans);
void MatrixInv(matrix& b, matrix a);

void Slerp(quat from, quat to, float t, quat &res);
void QuatToMatrix(quat quat, matrix m);
inline void CpyQuat(quat a, quat b) 
{
	b[0]=a[0]; 
	b[1]=a[1];
	b[2]=a[2];
	b[3]=a[3];
}




//-------------------------------------------------------------------------
#include <windows.h>

// time is in millisecs
class Clock {
public:
	Clock();
	~Clock();

	int Update();
	
	LARGE_INTEGER	freq;
	double			res;
	double			lasttime;	

	double			tm;		
	double			time;	
	int				fcnt;		

private:
	int				ls;
	char			PTimerEnabled;

	unsigned int	oldTime;
	int				stc;	
};




