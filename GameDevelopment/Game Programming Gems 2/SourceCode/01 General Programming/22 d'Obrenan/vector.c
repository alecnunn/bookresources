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
// VECTOR.C
/////////////////////////////////////////////////////////////////////
#include "main_includes.h"

// GLOBALS////////////////////////////////////////////////////////////
fs_float n,nn;

/////////////////////////////////////////////////////////////////////
// VECTOR4_INIT
// Assigns the designated vector its array
/////////////////////////////////////////////////////////////////////
fs_void vector4_init(vector4f *v)
{
	fs_int i;
  
	for(i=0; i<3; i++)
	  v->vec[i] = 0;
	
  v->vec[3] = 1;
}

/////////////////////////////////////////////////////////////////////
// VECTOR3_INIT
// Assigns the designated vector its array
/////////////////////////////////////////////////////////////////////
fs_void vector3_init(vector3f *v)
{
	fs_int i;
  
	for(i=0; i<2; i++)
	{
	  v->vec[i] = 0;
	}
	v->vec[2] = 1;
}

/////////////////////////////////////////////////////////////////////
// VECTOR4_SCALE
/////////////////////////////////////////////////////////////////////
fs_void vector4_scale(vector4f *from,fs_float scale,vector4f *to)
{
	to->vec[X] = from->vec[X] * scale;
	to->vec[Y] = from->vec[Y] * scale;
	to->vec[Z] = from->vec[Z] * scale;
	to->vec[W] = from->vec[W] * scale;
}

/////////////////////////////////////////////////////////////////////
// VECTOR4_ADD
// Add the other vector to this vector
/////////////////////////////////////////////////////////////////////
fs_void vector4_add(vector4f *from, vector4f *operand, vector4f *to)
{
	to->vec[X] = from->vec[X] + operand->vec[X];
	to->vec[Y] = from->vec[Y] + operand->vec[Y];
	to->vec[Z] = from->vec[Z] + operand->vec[Z];
	to->vec[W] = from->vec[W] + operand->vec[W];
}

/////////////////////////////////////////////////////////////////////
// VECTOR3_ADD
/////////////////////////////////////////////////////////////////////
fs_void vector3_add(vector3f *from, vector3f *operand, vector3f *to)
{ 
	to->vec[0] = from->vec[0] + operand->vec[0];
	to->vec[1] = from->vec[1] + operand->vec[1];
	to->vec[2] = from->vec[2] + operand->vec[2];
} 

/////////////////////////////////////////////////////////////////////
// VECTOR4_SUB
// Subtract the other vector from this vector.
/////////////////////////////////////////////////////////////////////
fs_void vector4_sub(vector4f *from, vector4f *operand, vector4f *to)
{
	to->vec[0] = from->vec[0] - operand->vec[0];
	to->vec[1] = from->vec[1] - operand->vec[1];
	to->vec[2] = from->vec[2] - operand->vec[2];
	to->vec[3] = from->vec[3] - operand->vec[3];
}

/////////////////////////////////////////////////////////////////////
// VECTOR3_SUB
/////////////////////////////////////////////////////////////////////
fs_void vector3_sub(vector3f *from, vector3f *operand, vector3f *to)
{ 
	to->vec[0] = from->vec[0] - operand->vec[0];
	to->vec[1] = from->vec[1] - operand->vec[1];
	to->vec[2] = from->vec[2] - operand->vec[2];
} 

/////////////////////////////////////////////////////////////////////
// VECTOR4_NORMALIZE
// Normalizes this vector, and returns the scalar value used to normalize the vector.
/////////////////////////////////////////////////////////////////////
__forceinline fs_float vector4_normalize(vector4f *to)
{ 
	nn = SQ(to->vec[0]) + SQ(to->vec[1]) + SQ(to->vec[2]);
  
	_ASSERT(nn!=0);
  
	n = 1.0f/(fs_float)sqrt(nn);
  
	to->vec[0] *= n;
	to->vec[1] *= n;
	to->vec[2] *= n;
	to->vec[3] *= n;

	return nn;
}

/////////////////////////////////////////////////////////////////////
// VECTOR3_NORMALIZE
// Normalizes this vector, and returns the scalar value used to normalize the vector.
/////////////////////////////////////////////////////////////////////
fs_void vector3_normalize(vector3f *to)
{ 
	nn = SQ(to->vec[0]) + SQ(to->vec[1]) + SQ(to->vec[2]);
  
  _ASSERT(nn!=0);
  
  n = 1.0f/(fs_float)sqrt(nn);  // NOTE:  You'll probably want to rewrite the square root funtion by yourself!!
  
  to->vec[0] *= n;
  to->vec[1] *= n;
  to->vec[2] *= n;
  
  //return nn;
} 

/////////////////////////////////////////////////////////////////////
// VECTOR3_COPY
/////////////////////////////////////////////////////////////////////
__inline fs_void vector3_copy(vector3f *v1, vector3f *v2)
{ 
  memcpy(v1, v2, sizeof(vector3f));
}

/////////////////////////////////////////////////////////////////////
// VECTOR4_MULT_AS_MATRIX
/////////////////////////////////////////////////////////////////////
__inline fs_float vector4_mult_as_matrix(vector4f *v1,vector4f *v2)
{
	return v1->vec[0]*v2->vec[0]+
				 v1->vec[1]*v2->vec[1]+
				 v1->vec[2]*v2->vec[2]+
				 v1->vec[3]*v2->vec[3];
}

/////////////////////////////////////////////////////////////////////
// VECTOR4_DIV
/////////////////////////////////////////////////////////////////////
__inline fs_void vector4_div(vector4f *from, vector4f *divisor, vector4f *to)
{
	to->vec[0] = from->vec[0] / divisor->vec[0];
	to->vec[1] = from->vec[1] / divisor->vec[1];
	to->vec[2] = from->vec[2] / divisor->vec[2];
	to->vec[3] = from->vec[3] / divisor->vec[3];
}

/////////////////////////////////////////////////////////////////////
// VECTOR3_DIV
/////////////////////////////////////////////////////////////////////
__inline fs_void vector3_div(vector3f *from, vector3f *divisor, vector3f *to)
{
	to->vec[0] = from->vec[0] / divisor->vec[0];
	to->vec[1] = from->vec[1] / divisor->vec[1];
	to->vec[2] = from->vec[2] / divisor->vec[2];
}

/////////////////////////////////////////////////////////////////////
// VECTOR_MULT
/////////////////////////////////////////////////////////////////////
__inline fs_void vector4_mult(vector4f *from, vector4f *multiplicand, vector4f *to)
{
	to->vec[0] = from->vec[0] * multiplicand->vec[0];
	to->vec[1] = from->vec[1] * multiplicand->vec[1];
	to->vec[2] = from->vec[2] * multiplicand->vec[2];
	to->vec[3] = from->vec[3] * multiplicand->vec[3];
}

/////////////////////////////////////////////////////////////////////
// VECTOR3_MULT
/////////////////////////////////////////////////////////////////////
__inline fs_void vector3_mult(vector3f *from, vector3f *multiplicand, vector3f *to)
{ 
	to->vec[0] = from->vec[0] * multiplicand->vec[0];
	to->vec[1] = from->vec[1] * multiplicand->vec[1];
	to->vec[2] = from->vec[2] * multiplicand->vec[2];
} 

/////////////////////////////////////////////////////////////////////
// CROSS_PRODUCT
// Cross product of 2 vectors ('n' is a storage var)
/////////////////////////////////////////////////////////////////////
__forceinline fs_void cross_product(fs_float *u, fs_float *v, fs_float *n)
{ 
	/*
  n[X] = u[Y] * v[Z] - u[Z] * v[Y];
  n[Y] = u[Z] * v[X] - u[X] * v[Z]; // Compute the cross product
  n[Z] = u[X] * v[Y] - u[Y] * v[X];
  */
  
  //__asm		push eax
  __asm		push    ebx
  __asm		mov     eax, v
  __asm		mov     ebx, u
  __asm		fld     dword ptr [eax+4]	  // Starts & ends on cycle 0
  __asm		fmul    dword ptr [ebx+8]	  // Starts on cycle 1
  __asm		fld     dword ptr [eax+8]	  // Starts & ends on cycle 2
  __asm		fmul    dword ptr [ebx]		  // Starts on cycle 3
  __asm		fld     dword ptr [eax]		  // Starts & ends on cycle 4
  __asm		fmul    dword ptr [ebx+4]	  // Starts on cycle 5
  __asm		fld     dword ptr [eax+8]	  // Starts & ends on cycle 6
  __asm		fmul    dword ptr [ebx+4]	  // Starts on cycle 7
  __asm		fld     dword ptr [eax]		  // Starts & ends on cycle 8
  __asm		fmul    dword ptr [ebx+8]	  // Starts on cycle 9
  __asm		fld     dword ptr [eax+4]	  // Starts & ends on cycle 10
  __asm		fmul    dword ptr [ebx]		  // Starts on cycle 11
  __asm		fxch		st(2)							  // No cost
  __asm		fsubrp	st(5),st(0)				  // Starts on cycle 12
  __asm		fsubrp	st(3),st(0)				  // Starts on cycle 13
  __asm		fsubrp	st(1),st(0)				  // Starts on cycle 14
  __asm		fxch		st(2)							  // No cost
															        // Stalls for cycle 15

  __asm		mov ebx, n
  __asm	  fstp		dword ptr [ebx]	    // Starts on cycle 16,
															        // Ends on cycle 17
  __asm	  fstp		dword ptr [ebx+4]	  // Starts on cycle 18,
															        // Ends on cycle 19
  __asm    fstp    dword ptr [ebx+8]	// Starts on cycle 20,
															        // Ends on cycle 21
  __asm		pop ebx
}

/////////////////////////////////////////////////////////////////////
// FLOAT_TO_INT
// Doesn't take the pointer, is a bit faster
/////////////////////////////////////////////////////////////////////
__forceinline fs_int __stdcall float_to_int(fs_float  x)
{ 
	fs_int	   t;
	__asm  fld   x  
	__asm  fistp t
	return t;
} 