/* General math Library for Graphics
 * Copyright (c) 2004 Takashi AMADA  All Rights Reserved
 */


#ifndef _GLG_H_
#define _GLG_H_

//#define WITH_SSE

#define MAX_FLOAT 3.40282347e+38F

#ifdef WITH_SSE
#define _ALIGNED _declspec(align(16))
#else
#define _ALIGNED
#endif

#include <memory.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define _GLG_GEOM_STDOUT_
#define _GLG_REAL_AS_FLOAT_

#ifdef _GLG_REAL_AS_FLOAT_
#define Real float
#endif

#define GLG_BOOL  int
#define GLG_TRUE  1
#define GLG_FALSE 0

#ifndef M_PI
#define M_PI (Real)3.14159265358979323846
#endif

typedef struct
{
	int x;
	int y;
	int z;
} vector3i;

typedef struct
{
    Real x;
    Real y;
    Real z;
#ifdef WITH_SSE
	Real _w;
#endif
} vector3;



typedef struct
{
	Real x;
	Real y;
	Real z;
	Real w;
} vector4;

//typedef Real matrix4[16];
typedef struct
{
	Real m[16];
} matrix4;

typedef struct
{
	Real m[9];
} matrix3;

//#define MAT4(m, row, col) ((m)[(col)*4+(row)])
#define MAT3(mat, row, col) ((mat)->m[(col)*3+(row)])
#define MAT4(mat, row, col) ((mat)->m[(col)*4+(row)])


Real glg_clamp(Real x, Real mi, Real ma);

vector3* vec3_alloc(int size);
void vec3i_from_f(vector3i* dst, vector3* src);
void vec3i_set(vector3i* dst, int x, int y, int z);
void vec3i_from_string(vector3i* v, const char* str);
void vec3i_to_string(vector3i* v, char* str, int n);

void vec3_from_string(vector3* v, const char* str);
void vec3_to_string(vector3* v, char* str, int n);
void vec3_set(vector3* v, Real x, Real y, Real z);
void vec3_copy(vector3* dst, const vector3* v);
void vec3_negate(vector3* v);
void vec3_cross(vector3* dst, const vector3* v0, const vector3* v1);
Real vec3_dot(const vector3* v0, const vector3* v1);
void vec3_add(vector3* dst, const vector3* v0, const vector3* v1);
void vec3_sub(vector3* dst, const vector3* v0, const vector3* v1);
Real vec3_len(const vector3* v);
Real vec3_lensq(const vector3* v);
Real vec3_dist(const vector3* p0, const vector3* p1);
Real vec3_distsq(const vector3* p0, const vector3* p1);
void vec3_scale(vector3* r, Real s, const vector3* v);
void vec3_scaleadd(vector3* dst, const vector3* v0, Real s, const vector3* v1);
void vec3_normalize(vector3* n, const vector3* v);
void vec3_dump(vector3* v, const char* name);
void vec4_dump(vector4* v, const char* name);

void vec4_set(vector4* v, Real x, Real y, Real z, Real w);


void mat3_set(matrix3* m,
			  Real e00, Real e01, Real e02,
			  Real e10, Real e11, Real e12,
			  Real e20, Real e21, Real e22);
void mat3_copy(matrix3* dst, const matrix3* src);
void mat3_set_zero(matrix3* m);
void mat3_set_identity(matrix3* m);
void mat3_scale(matrix3* dst, Real s, matrix3* src);
void mat3_set_rotate(matrix3* m, Real angle, Real x, Real y, Real z);
void mat3_add(matrix3* result, const matrix3* m0, const matrix3* m1);

void mat3_set_rows(matrix3* m,
				   const vector3* r0,
				   const vector3* r1,
				   const vector3* r2);
void mat3_set_cols(matrix3* m,
				   const vector3* c0,
				   const vector3* c1,
				   const vector3* c2);
Real mat3_determinant(const matrix3* m);
void mat3_mulvec3(vector3* dst, const matrix3* m, const vector3* v);
void mat3_invert(matrix3* dst, const matrix3* m);
void mat3_mul(matrix3* dst, const matrix3* m0, const matrix3* m1);


void mat4_set(matrix4* m,
              Real e00, Real e01, Real e02, Real e03,
              Real e10, Real e11, Real e12, Real e13,
              Real e20, Real e21, Real e22, Real e23,
              Real e30, Real e31, Real e32, Real e33);
void mat4_copy(matrix4* dst, const matrix4* src);
void mat4_set_zero(matrix4* m);
void mat4_set_identity(matrix4* m);
void mat4_set_translate(matrix4* m, Real x, Real y, Real z);
void mat4_set_rotate(matrix4* m, Real angle, Real x, Real y, Real z);
/*void mat4_set_rotate(matrix4 m);*/
void mat4_mul(matrix4* dst, const matrix4* m0, const matrix4* m1);
void mat4_mulvec3_as_mat3(vector3* dst, const matrix4* m, const vector3* v);
void mat4_mulvec3(vector3* dst, const matrix4* m, const vector3* v);
void mat4_set_rotate_around(matrix4* dst, Real angle, const vector3* v);
void mat4_mulr_rotate(matrix4* dst, Real angle, Real x, Real y, Real z);
void mat4_mull_rotate(matrix4* dst, Real angle, Real x, Real y, Real z);
void mat4_translate(matrix4* dst, Real x, Real y, Real z);
void mat4_invert(matrix4* dst, const matrix4* m);
void mat4_transpose(matrix4* dst, const matrix4* m);
Real mat4_determinant(const matrix4* m);
void mat4_dump(const matrix4* m);

Real distsq_tri_point(const vector3* v0,
					  const vector3* v1,
					  const vector3* v2,
					  const vector3* p);
Real dist_tri_point(const vector3* v0,
					const vector3* v1,
					const vector3* v2,
					const vector3* p);
Real dist_tri_point2(const vector3* v0,
					 const vector3* v1,
					 const vector3* v2,
					 const vector3* p);
void tri_normal(vector3* n, 
				const vector3* v0,
				const vector3* v1,
				const vector3* v2);


/**** inline *****/

#define GLG_INLINE _inline
#define GLG_ZERO 0.0f
#define GLG_ONE  1.0f


#ifdef _GLG_REAL_AS_FLOAT_
#define glg_sqrt(x) (Real)sqrt((x))
/*
GLG_INLINE Real glg_sqrt(Real x)
{
    float xhalf = 0.5f*x;
    int i = *(int*)&x;
    i = (((i - 0x3f800000) >> 1) + 0x3f800000);
    x = *(float*)&i;
    x = 0.5f*x + xhalf/x;

    return x;
}
*/
#endif

GLG_INLINE vector3* vec3_alloc(int size)
{
#ifdef WITH_SSE
	return (vector3*)_aligned_malloc(size*sizeof(vector3), 16);
#else
	return (vector3*)malloc(size*sizeof(vector3));
#endif
}
/*
#else
GLG_INLINE vector3* vec3_alloc(int size)
{
}
#endif
*/
GLG_INLINE Real glg_clamp(Real x, Real mi, Real ma)
{
	if (x <= mi)
		return mi;
	if (x >= ma)
		return ma;
	return x;
}

GLG_INLINE void vec3i_from_f(vector3i* dst, vector3* src)
{
	dst->x = (int)src->x;
	dst->y = (int)src->y;
	dst->z = (int)src->z;
}

GLG_INLINE void vec3i_set(vector3i* dst, int x, int y, int z)
{
	dst->x = x;
	dst->y = y;
	dst->z = z;
}

GLG_INLINE void vec3_set(vector3* dst, Real x, Real y, Real z)
{
    dst->x = x;
    dst->y = y;
    dst->z = z;
}


GLG_INLINE void vec3_copy(vector3* dst, const vector3* v)
{
	memcpy(dst, v, sizeof(vector3));
}

GLG_INLINE void vec3_negate(vector3* v)
{
	v->x = -v->x;
	v->y = -v->y;
	v->z = -v->z;
}

GLG_INLINE void vec3_cross(vector3* dst, const vector3* v0, const vector3* v1)
{
    Real new_x = v0->y*v1->z - v0->z*v1->y;
    Real new_y = v0->z*v1->x - v0->x*v1->z;
    dst->z = v0->x*v1->y - v0->y*v1->x;
	dst->x = new_x;
	dst->y = new_y;
}

GLG_INLINE Real vec3_dot(const vector3* v0, const vector3* v1)
{
    return v0->x*v1->x + v0->y*v1->y + v0->z*v1->z;
}

GLG_INLINE void vec3_add(vector3* dst, const vector3* v0, const vector3* v1)
{
#ifdef WITH_SSE
	_asm {
		mov eax, v0
		movaps xmm0, DWORD PTR [eax]
		mov ecx, v1
		movaps xmm1, DWORD PTR [ecx]
		addps xmm0, xmm1
		mov edx, dst
		movaps XMMWORD PTR [edx], xmm0
		//loadups DWORD PTR [edx], xmm0
	}
#else
    dst->x = v0->x + v1->x;
    dst->y = v0->y + v1->y;
    dst->z = v0->z + v1->z;
#endif

	/*
	_asm {
	
       mov     eax, v0
       movss   xmm0, DWORD PTR [eax]
       mov     ecx, v1 //DWORD PTR _v1$[esp-4]
       addss   xmm0, DWORD PTR [ecx]
       mov     edx, dst
       movss   DWORD PTR [edx], xmm0


       movss   xmm0, DWORD PTR [eax+4]
       addss   xmm0, DWORD PTR [ecx+4]
       movss   DWORD PTR [edx+4], xmm0


       movss   xmm0, DWORD PTR [eax+8]
       addss   xmm0, DWORD PTR [ecx+8]
       movss   DWORD PTR [edx+8], xmm0
	}
	*/
}

GLG_INLINE void vec3_sub(vector3* dst, const vector3* v0, const vector3* v1)
{
    dst->x = v0->x - v1->x;
    dst->y = v0->y - v1->y;
    dst->z = v0->z - v1->z;
}

GLG_INLINE Real vec3_len(const vector3* v)
{
    return glg_sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
}

GLG_INLINE Real vec3_lensq(const vector3* v)
{
    return v->x*v->x + v->y*v->y + v->z*v->z;
}

GLG_INLINE Real vec3_dist(const vector3* p0, const vector3* p1)
{
	return glg_sqrt(vec3_distsq(p0, p1));
}

GLG_INLINE Real vec3_distsq(const vector3* p0, const vector3* p1)
{
	Real dx;
	Real dy;
	Real dz;

	dx = p0->x - p1->x;
	dy = p0->y - p1->y;
	dz = p0->z - p1->z;

	return dx*dx + dy*dy + dz*dz;
}


GLG_INLINE void vec3_scale(vector3* r, Real s, const vector3* v)
{
    r->x = s*v->x;
    r->y = s*v->y;
    r->z = s*v->z;
}

GLG_INLINE void vec3_scaleadd(vector3* dst, const vector3* v0, Real s, const vector3* v1)
{
	dst->x = v0->x + s*v1->x;
	dst->y = v0->y + s*v1->y;
	dst->z = v0->z + s*v1->z;
}

GLG_INLINE void vec3_normalize(vector3* n, const vector3* v)
{
    Real invlen = GLG_ONE/vec3_len(v);
    n->x = v->x * invlen;
    n->y = v->y * invlen;
    n->z = v->z * invlen;
}


GLG_INLINE void mat4_mulvec3_as_mat3(vector3* dst, const matrix4* m, const vector3* v)
{
	float new_x;
	float new_y;
	float new_z;

	new_x = v->x*MAT4(m, 0, 0) + v->y*MAT4(m, 0, 1) + v->z*MAT4(m, 0, 2);
	new_y = v->x*MAT4(m, 1, 0) + v->y*MAT4(m, 1, 1) + v->z*MAT4(m, 1, 2);
	new_z = v->x*MAT4(m, 2, 0) + v->y*MAT4(m, 2, 1) + v->z*MAT4(m, 2, 2);
	dst->x = new_x;
	dst->y = new_y;
	dst->z = new_z;
}

GLG_INLINE void mat4_mulvec3(vector3* dst, const matrix4* m, const vector3* v)
{
	float new_x;
	float new_y;
	float new_z;
	float inv_w;

	new_x = v->x*MAT4(m, 0, 0) + v->y*MAT4(m, 0, 1) + v->z*MAT4(m, 0, 2) + MAT4(m, 0, 3);
	new_y = v->x*MAT4(m, 1, 0) + v->y*MAT4(m, 1, 1) + v->z*MAT4(m, 1, 2) + MAT4(m, 1, 3);
	new_z = v->x*MAT4(m, 2, 0) + v->y*MAT4(m, 2, 1) + v->z*MAT4(m, 2, 2) + MAT4(m, 2, 3);
	inv_w = GLG_ONE/(v->x*MAT4(m, 3, 0) + v->y*MAT4(m, 3, 1) + v->z*MAT4(m, 3, 2) + MAT4(m, 3, 3));
	dst->x = new_x*inv_w;
	dst->y = new_y*inv_w;
	dst->z = new_z*inv_w;
}

GLG_INLINE void mat3_mulvec3(vector3* dst, const matrix3* m, const vector3* v)
{
	float new_x;
	float new_y;

	new_x = v->x*MAT3(m, 0, 0) + v->y*MAT3(m, 0, 1) + v->z*MAT3(m, 0, 2);
	new_y = v->x*MAT3(m, 1, 0) + v->y*MAT3(m, 1, 1) + v->z*MAT3(m, 1, 2);
	dst->z = v->x*MAT3(m, 2, 0) + v->y*MAT3(m, 2, 1) + v->z*MAT3(m, 2, 2);
	dst->x = new_x;
	dst->y = new_y;
}

GLG_INLINE void vec3_dump(vector3* v, const char* name)
{
	printf("%s: x=%e, y=%e, z=%e\n", name, v->x, v->y, v->z);
}

GLG_INLINE void vec4_dump(vector4* v, const char* name)
{
	printf("%s: x=%e, y=%e, z=%e w=%e\n", name, v->x, v->y, v->z, v->w);
}

GLG_INLINE void vec4_set(vector4* v, Real x, Real y, Real z, Real w)
{
	v->x = x;
	v->y = y;
	v->z = z;
	v->w = w;
}
#endif