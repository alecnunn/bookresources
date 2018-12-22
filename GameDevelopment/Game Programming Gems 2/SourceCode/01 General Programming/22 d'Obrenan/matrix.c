/* 
   Copyright (C) Nathan d'Obrenan, 2001. 
   All rights reserved worldwide.
   
   This software is provided "as is" without expressed or implied
   warranties. You may freely copy and compile this source into
   your applications, however this program is not in the public domain.
   Have fun with it!
   
   Game Programming Gems 2
   Web Cam in Video Games
   
   Nathan d'Obrenan
   Firetoad Software, Inc.
   http://www.firetoads.com/coding
   nathand@firetoads.com
*/ 

/////////////////////////////////////////////////////////////////////
// MATRIX.C
/////////////////////////////////////////////////////////////////////
#include "main_includes.h"

/////////////////////////////////////////////////////////////////////
// MATRIX44_INIT
/////////////////////////////////////////////////////////////////////
fs_void matrix44_init(matrix44f *m)
{ 
	fs_int i,j;
	for(i=0; i<4; i++)
	   for(j=0; j<4; j++)
	      m->mat[i][j] = (fs_float)(i==j?1.0f:0.0f);
} 

/////////////////////////////////////////////////////////////////////
// MATRIX33_INIT
/////////////////////////////////////////////////////////////////////
fs_void matrix33_init(matrix33f *m)
{ 
	fs_int i,j;
	for(i=0; i<3; i++)
	   for(j=0; j<3; j++)
	      m->mat[i][j] = (fs_float)(i==j?1.0f:0.0f);
} 

/////////////////////////////////////////////////////////////////////
// MATRIX44_COPY_MATRIX33
/////////////////////////////////////////////////////////////////////
fs_void matrix44_copy_matrix33(matrix44f *to, const matrix33f *from)
{ 
	to->mat[0][0] = from->mat[0][0];
	to->mat[0][1] = from->mat[0][1];
	to->mat[0][2] = from->mat[0][2];
  
	to->mat[1][0] = from->mat[1][0];
	to->mat[1][1] = from->mat[1][1];
	to->mat[1][2] = from->mat[1][2];
  
	to->mat[2][0] = from->mat[2][0];
	to->mat[2][1] = from->mat[2][1];
	to->mat[2][2] = from->mat[2][2];
} 

/////////////////////////////////////////////////////////////////////
// MATRIX33_COPY_MATRIX44
/////////////////////////////////////////////////////////////////////
fs_void matrix33_copy_matrix44(matrix33f *to, const matrix44f *from)
{ 
	to->mat[0][0] = from->mat[0][0];
	to->mat[0][1] = from->mat[0][1];
	to->mat[0][2] = from->mat[0][2];
  
	to->mat[1][0] = from->mat[1][0];
	to->mat[1][1] = from->mat[1][1];
	to->mat[1][2] = from->mat[1][2];
  
	to->mat[2][0] = from->mat[2][0];
	to->mat[2][1] = from->mat[2][1];
	to->mat[2][2] = from->mat[2][2];
} 

/////////////////////////////////////////////////////////////////////
// MATRIX44_MULT
// Copy the matrixs
/////////////////////////////////////////////////////////////////////
fs_void matrix44_mult(matrix44f *to, const matrix44f *from)
{ 
	fs_int i,j,k;
	matrix44f m;
  
	for(i=0; i<4; i++)
	{ 
    for(j=0; j<4; j++)
	  { 
      m.mat[i][j] = 0;
      for(k=0; k<4; k++)
         m.mat[i][j] += from->mat[i][k] * to->mat[k][j];
		} 
	} 
	*to = m;
} 

/////////////////////////////////////////////////////////////////////
// MATRIX33_MULT
// Function designed to compute multipication on 3x3 matricees
/////////////////////////////////////////////////////////////////////
fs_void matrix33_mult(matrix33f *to, const matrix33f *from)
{ 
	fs_int i,j,k;
	matrix33f m;
  
	for(i=0; i<3; i++)
	{ 
    for(j=0; j<3; j++)
    { 
      m.mat[i][j] = 0;
      for(k=0; k<3; k++)
         m.mat[i][j] += from->mat[i][k] * to->mat[k][j];
    } 
	} 
	*to = m;
} 

/////////////////////////////////////////////////////////////////////
// MATRIX44_SCALE
/////////////////////////////////////////////////////////////////////
fs_void matrix44_scale(matrix44f *m, fs_float scale)
{ 
	fs_int i,j;
  
	for(i=0; i<4; i++)
	{ 
		for(j=0; j<4; j++)
		{ 
			m->mat[i][j] *= scale;
		} 
	} 
} 

/////////////////////////////////////////////////////////////////////
// MATRIX33_SCALE
/////////////////////////////////////////////////////////////////////
fs_void matrix33_scale(matrix33f *m, fs_float scale)
{ 
	fs_int i,j;
  
	for(i=0; i<3; i++)
	{ 
		for(j=0; j<3; j++)
		{ 
			m->mat[i][j] *= scale;
		} 
	} 
} 

/////////////////////////////////////////////////////////////////////
// MATRIX44_XROT
/////////////////////////////////////////////////////////////////////
fs_void matrix44_xrot(fs_float angle, matrix44f *m)
{ 
	matrix44f r;
	fs_float c = (fs_float)cos(angle), s = (fs_float)sin(angle);
  
	matrix44_init(&r);
  
	r.mat[1][1] =  c;
	r.mat[1][2] =  s;
	r.mat[2][1] = -s;
	r.mat[2][2] =  c;
  
	matrix44_mult(m, &r);
} 

/////////////////////////////////////////////////////////////////////
// MATRIX33_XROT
// Calculate the matrix dependant upon the angle (x)
/////////////////////////////////////////////////////////////////////
fs_void matrix33_xrot(fs_float angle, matrix33f *m)
{ 
	matrix33f r;
	fs_float c = (fs_float)cos(angle), s = (fs_float)sin(angle);
  
	matrix33_init(&r);
  
	r.mat[1][1] =  c;
	r.mat[1][2] =  s;
	r.mat[2][1] = -s;
	r.mat[2][2] =  c;
  
	matrix33_mult(m, &r);
}

/////////////////////////////////////////////////////////////////////
// MATRIX44_YROT
// Multiplies the current matrix by the assigned angle
/////////////////////////////////////////////////////////////////////
fs_void matrix44_yrot(fs_float angle, matrix44f *m)
{
	matrix44f r;
	fs_float c = (fs_float)cos(angle), s = (fs_float)sin(angle);
  
	matrix44_init(&r);
  
	r.mat[0][0] =  c;
	r.mat[0][2] = -s;
	r.mat[2][0] =  s;
	r.mat[2][2] =  c;
  
	matrix44_mult(m, &r);
} 

/////////////////////////////////////////////////////////////////////
// MATRIX33_YROT
/////////////////////////////////////////////////////////////////////
fs_void matrix33_yrot(fs_float angle, matrix33f *m)
{ 
	matrix33f r;
	fs_float c = (fs_float)cos(angle), s = (fs_float)sin(angle);
  
	matrix33_init(&r);
  
	r.mat[0][0] =  c;
	r.mat[0][2] = -s;
	r.mat[2][0] =  s;
	r.mat[2][2] =  c;
  
	matrix33_mult(m, &r);
} 

/////////////////////////////////////////////////////////////////////
// MATRIX44_ZROT
/////////////////////////////////////////////////////////////////////
fs_void matrix44_zrot(fs_float angle, matrix44f *m)
{ 
	matrix44f r;
	
  fs_float c = (fs_float)cos(angle), s = (fs_float)sin(angle);
  
	matrix44_init(&r);
  
	r.mat[0][0] = c;
	r.mat[0][1] = s;
	r.mat[1][0] = -s;
	r.mat[1][1] = c;
  
	matrix44_mult(m, &r);
} 

/////////////////////////////////////////////////////////////////////
// MATRIX33_ZROT
/////////////////////////////////////////////////////////////////////
fs_void matrix33_zrot(fs_float angle, matrix33f *m)
{ 
	matrix33f r;
	fs_float c = (fs_float)cos(angle), s = (fs_float)sin(angle);
  
	matrix33_init(&r);
  
	r.mat[0][0] = c;
	r.mat[0][1] = s;
	r.mat[1][0] = -s;
	r.mat[1][1] = c;
  
	matrix33_mult(m, &r);
} 


/////////////////////////////////////////////////////////////////////
// MATRIX33_INVERSE
// Taken from SGI's sample GL implementation, should work 
/////////////////////////////////////////////////////////////////////
fs_int matrix33_inverse(matrix33f *m)
{
  fs_int i, j, k, swap;
  fs_float t;
  matrix33f inv;
  matrix33f temp;
  
  temp = *m;
  matrix33_init(&inv);
   
  for(i=0; i<3; i++)
  {
    // Look for largest element in column
    swap = i;
    for(j=i+1; j<3; j++)
      if(fabs(temp.mat[j][i]) > fabs(temp.mat[i][i]))
        swap = j;
    
    if(swap != i)
    { 
      // Swap rows.
      for(k=0; k<3; k++)
      { 
        t = temp.mat[i][k];
        temp.mat[i][k] = temp.mat[swap][k];
        temp.mat[swap][k] = t;
        
        t = inv.mat[i][k];
        inv.mat[i][k] = inv.mat[swap][k];
        inv.mat[swap][k] = t;
      }
    }
    
    // No non-zero pivot.  The matrix is singular, which shouldn't
    // happen.  This means the user gave us a bad matrix.
    if(temp.mat[i][i] == 0)
       return 0;
    
    t = temp.mat[i][i];
    for(k=0; k<3; k++)
    { 
      temp.mat[i][k] /= t;
      inv.mat[i][k] /= t;
    } 
    for(j=0; j<3; j++)
    { 
      if(j != i)
      { 
        t = temp.mat[j][i];
        for(k=0; k<3; k++)
        { 
           temp.mat[j][k] -= temp.mat[i][k] * t;
           inv.mat[j][k] -= inv.mat[i][k] * t;
        } 
      }
    }
   }
   *m = inv;
   return 1;
}

/////////////////////////////////////////////////////////////////////
// MATRIX44_INVERSE
// Taken from SGI's sample GL implementation
/////////////////////////////////////////////////////////////////////
fs_int matrix44_inverse(matrix44f *m)
{
  fs_int i, j, k, swap;
  fs_float t;
  matrix44f inv;
  matrix44f temp;
  
  temp = *m;
  matrix44_init(&inv);
  
  for(i=0; i<4; i++)
  { 
    // Look for largest element in column
    swap = i;
    
    for(j=i+1; j<4; j++)
     if(fabs(temp.mat[j][i]) > fabs(temp.mat[i][i]))
       swap = j;
     
     if(swap != i)
     { 
       // Swap rows.
       for(k=0; k<4; k++)
       { 
         t = temp.mat[i][k];
         temp.mat[i][k] = temp.mat[swap][k];
         temp.mat[swap][k] = t;
         
         t = inv.mat[i][k];
         inv.mat[i][k] = inv.mat[swap][k];
         inv.mat[swap][k] = t;
       } 
     } 
     
     // No non-zero pivot.  The matrix is singular, which shouldn't
     // happen.  This means the user gave us a bad matrix.
     if(temp.mat[i][i] == 0)
       return 0;
     
     t = temp.mat[i][i];
     for(k=0; k<4; k++)
     { 
       temp.mat[i][k] /= t;
       inv.mat[i][k] /= t;
     } 
     for(j=0; j<4; j++)
     { 
       if(j != i)
       { 
         t = temp.mat[j][i];
         
         for(k=0; k<4; k++)
         { 
           temp.mat[j][k] -= temp.mat[i][k] * t;
           inv.mat[j][k] -= inv.mat[i][k] * t;
         } 
       } 
     } 
   } 
   
   *m = inv;
   return 1;
} 

/////////////////////////////////////////////////////////////////////
// MATRIX44_MULT_VECTOR4
// Multiplies the vector by the given matrix
/////////////////////////////////////////////////////////////////////
fs_void matrix44_mult_vector4(const matrix44f *m, vector4f *v)
{ 
	fs_int i,j;
	vector4f t;
  
	for(i=0; i<4; i++)
	{ 
    t.vec[i] = 0;
    for(j=0;j<4;j++)
      t.vec[i] += m->mat[j][i] * v->vec[j];
	} 
  
	*v = t;
}

/////////////////////////////////////////////////////////////////////
// MATRIX44_MULT_VECTOR3
// Multiplies the vector by the given matrix
/////////////////////////////////////////////////////////////////////
fs_void matrix44_mult_vector3(const matrix44f *m, vector3f *v)
{ 
	fs_int i,j;
	vector3f t;
  
	for(i=0; i<4; i++)
	{
     t.vec[i] = 0;
     for(j=0;j<3;j++)
        t.vec[i] += m->mat[j][i] * v->vec[j];
	}
	for(j=0; j<3; j++)
		t.vec[j] += m->mat[3][j];
  
	*v = t;
} 

/////////////////////////////////////////////////////////////////////
// MATRIX33_MULT_VECTOR_R
// Multiplies the vector by the given matrix
/////////////////////////////////////////////////////////////////////
fs_void matrix33_mult_vector3(const matrix33f *m, vector3f *v)
{
	__asm
	{
		push ebx
		mov eax, v	; eax is our vector
		mov ebx, m	; ebx is our matrix

		fld dword ptr[eax]
		fmul dword ptr[ebx]
		fld dword ptr[eax]
		fmul dword ptr[ebx + 4]
		fld dword ptr[eax]
		fmul dword ptr[ebx + 8]

		fld dword ptr[eax + 4]
		fmul dword ptr[ebx + 12]
		fld dword ptr[eax + 4]
		fmul dword ptr[ebx + 16]
		fld dword ptr[eax + 4]
		fmul dword ptr[ebx + 20]

		fxch st(2)
		faddp st(5),st(0)
		faddp st(3),st(0)
		faddp st(1),st(0)

		fld dword ptr[eax + 8]
		fmul dword ptr[ebx + 24]
		fld dword ptr[eax + 8]
		fmul dword ptr[ebx + 28]
		fld dword ptr[eax + 8]
		fmul dword ptr[ebx + 32]

		fxch st(2)
		faddp st(5),st(0)
		faddp st(3),st(0)
		faddp st(1),st(0)
		pop ebx

		fstp dword ptr[eax+8]
		fstp dword ptr[eax+4]
		fstp dword ptr[eax]
	}
/*	int i,j;
	vector3f t;

	for(i=0; i<3; i++)
	{
     t.vec[i] = 0;
     for(j=0; j<3; j++)
        t.vec[i] += m->mat[j][i] * v->vec[j];
	}

	*v = t;*/
}

/////////////////////////////////////////////////////////////////////
// MATRIX44_ORTHONORMALIZE
/////////////////////////////////////////////////////////////////////
fs_void matrix44_orthonormalize(matrix44f *Orientation)
{ 
	vector3f X = {Orientation->mat[0][0],Orientation->mat[0][1],Orientation->mat[0][2]};
	vector3f Y = {Orientation->mat[1][0],Orientation->mat[1][1],Orientation->mat[1][2]};
  vector3f Z;
  
  vector3_normalize(&X);
  cross_product(X.vec, Y.vec, Z.vec);
	vector3_normalize(&Z);
  cross_product(Z.vec, X.vec, Y.vec);
	vector3_normalize(&Y);
  
  Orientation->mat[0][0] = X.vec[0];	Orientation->mat[1][0] = Y.vec[0];	Orientation->mat[2][0] = Z.vec[0];
  Orientation->mat[0][1] = X.vec[1];	Orientation->mat[1][1] = Y.vec[1];	Orientation->mat[2][1] = Z.vec[1];
  Orientation->mat[0][2] = X.vec[2];	Orientation->mat[1][2] = Y.vec[2];	Orientation->mat[2][2] = Z.vec[2];
} 

/////////////////////////////////////////////////////////////////////
// MATRIX33_ORTHONORMALIZE
/////////////////////////////////////////////////////////////////////
fs_void matrix33_orthonormalize(matrix33f *Orientation)
{ 
	vector3f X = {Orientation->mat[0][0],Orientation->mat[0][1],Orientation->mat[0][2]};
	vector3f Y = {Orientation->mat[1][0],Orientation->mat[1][1],Orientation->mat[1][2]};
  vector3f Z;
  
  vector3_normalize(&X);
  cross_product(X.vec, Y.vec, Z.vec);
	vector3_normalize(&Z);
  cross_product(Z.vec, X.vec, Y.vec);
	vector3_normalize(&Y);
  
  Orientation->mat[0][0] = X.vec[0];	Orientation->mat[1][0] = Y.vec[0];	Orientation->mat[2][0] = Z.vec[0];
  Orientation->mat[0][1] = X.vec[1];	Orientation->mat[1][1] = Y.vec[1];	Orientation->mat[2][1] = Z.vec[1];
  Orientation->mat[0][2] = X.vec[2];	Orientation->mat[1][2] = Y.vec[2];	Orientation->mat[2][2] = Z.vec[2];
} 
