/* General math Library for Graphics
 * Copyright (c) 2004 Takashi AMADA  All Rights Reserved
 */

//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>

#include "glg.h"



#define EPSILON (Real)0.0000001

#define DEG2RAD (Real)3.14159265358979323846/180.0
     

const Real IDENTITY[16] =
{
    GLG_ONE, GLG_ZERO, GLG_ZERO, GLG_ZERO,
    GLG_ZERO, GLG_ONE, GLG_ZERO, GLG_ZERO,
    GLG_ZERO, GLG_ZERO, GLG_ONE, GLG_ZERO,
    GLG_ZERO, GLG_ZERO, GLG_ZERO, GLG_ONE
};


void vec3i_from_string(vector3i* v, const char* str)
{
	int i;
    int n;
	int buf_i;
	int elem_index;
	char buf[200];

	n = (int)strlen(str);

	assert(n < 200);

	buf_i = 0;
	elem_index = 0;
	for (i = 0; i < n; i++)
	{
		char ch = str[i];
		if (isdigit(ch) || (ch == '-'))
		{
			buf[buf_i] = ch;
			buf_i++;
		}
		else if (ch == ',')
		{
			buf[buf_i] = 0;
			switch (elem_index)
			{
			case 0: v->x = atoi(buf); break;
			case 1: v->y = atoi(buf); break;
			//case 2: v->z = atof(buf); break;
			default: assert(GLG_FALSE);
			}
			buf_i = 0;
			elem_index++;
		}
		else if (ch == ';')
		{
			break;
		}
	}

	buf[buf_i] = 0;
	v->z = atoi(buf);
}

void vec3i_to_string(vector3i* v, char* str, int n)
{
	_snprintf(str, n, "%d, %d, %d;", v->x, v->y, v->z);
}


void vec3_from_string(vector3* v, const char* str)
{
	int i;
	int n;
	int buf_i;
	int elem_index;
	char buf[200];

	n = (int)strlen(str);

	assert(n < 200);

	buf_i = 0;
	elem_index = 0;
	for (i = 0; i < n; i++)
	{
		char ch = str[i];
		if (isdigit(ch) || (ch == '.') || (ch == '-'))
		{
			buf[buf_i] = ch;
			buf_i++;
		}
		else if (ch == ',')
		{
			buf[buf_i] = 0;
			switch (elem_index)
			{
			case 0: v->x = (Real)atof(buf); break;
			case 1: v->y = (Real)atof(buf); break;
			//case 2: v->z = atof(buf); break;
			default: assert(GLG_FALSE);
			}
			buf_i = 0;
			elem_index++;
		}
		else if (ch == ';')
		{
			break;
		}
	}

	buf[buf_i] = 0;
	v->z = (Real)atof(buf);
}

void vec3_to_string(vector3* v, char* str, int n)
{
	_snprintf(str, n, "%f, %f, %f;", v->x, v->y, v->z);
}

void mat3_set(matrix3* m,
			  Real e00, Real e01, Real e02,
			  Real e10, Real e11, Real e12,
			  Real e20, Real e21, Real e22)
{
    MAT3(m, 0, 0) = e00;
    MAT3(m, 0, 1) = e01;
    MAT3(m, 0, 2) = e02;

    MAT3(m, 1, 0) = e10;
    MAT3(m, 1, 1) = e11;
    MAT3(m, 1, 2) = e12;

    MAT3(m, 2, 0) = e20;
    MAT3(m, 2, 1) = e21;
    MAT3(m, 2, 2) = e22;
}

void mat3_copy(matrix3* dst, const matrix3* src)
{
    memcpy(dst, src, 9*sizeof(Real));
}

void mat3_set_zero(matrix3* m)
{
	memset(m, 0, 9*sizeof(Real));
}

void mat3_set_identity(matrix3* m)
{
    MAT3(m, 0, 0) = GLG_ONE;
    MAT3(m, 0, 1) = GLG_ZERO;
    MAT3(m, 0, 2) = GLG_ZERO;

    MAT3(m, 1, 0) = GLG_ZERO;
    MAT3(m, 1, 1) = GLG_ONE;
    MAT3(m, 1, 2) = GLG_ZERO;

    MAT3(m, 2, 0) = GLG_ZERO;
    MAT3(m, 2, 1) = GLG_ZERO;
    MAT3(m, 2, 2) = GLG_ONE;
}

void mat3_scale(matrix3* dst, Real s, matrix3* src)
{
	int row;
	int col;

    for (row = 0; row < 3; row++)
        for (col = 0; col < 3; col++)
			MAT3(dst, row, col) = s*MAT3(src, row, col);
}

void mat3_set_rows(matrix3* m,
				   const vector3* r0,
				   const vector3* r1,
				   const vector3* r2)
{
	mat3_set(m,
		r0->x, r0->y, r0->z,
		r1->x, r1->y, r1->z,
		r2->x, r2->y, r2->z);
}

void mat3_add(matrix3* dst, const matrix3* m0, const matrix3* m1)
{
	int row;
	int col;

    for (row = 0; row < 3; row++)
        for (col = 0; col < 3; col++)
			MAT3(dst, row, col) = MAT3(m0, row, col) + MAT3(m1, row, col);
}

void mat3_mul(matrix3* dst, const matrix3* m0, const matrix3* m1)
{
    int row;
    int col;
    int i;
	matrix3 result;

    mat3_set_zero(&result);

    for (row = 0; row < 3; row++)
        for (col = 0; col < 3; col++)
            for (i = 0; i < 3; i++)
                MAT3(&result, row, col) += MAT3(m0, row, i)*MAT3(m1, i, col);
	mat3_copy(dst, &result);
}

void mat3_set_cols(matrix3* m,
				   const vector3* c0,
				   const vector3* c1,
				   const vector3* c2)
{
	mat3_set(m,
		c0->x, c1->x, c2->x,
		c0->y, c1->y, c2->y,
		c0->z, c1->z, c2->z);
}

void mat3_set_rotate(matrix3* m, Real angle, Real x, Real y, Real z)
{
    Real xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c, s, c;
//    Real m[16];
    GLG_BOOL optimized;

    s = (Real) sin( angle * DEG2RAD );
    c = (Real) cos( angle * DEG2RAD );

	mat3_set_identity(m);
    optimized = GLG_FALSE;

    if (x == GLG_ZERO)
    {
        if (y == GLG_ZERO)
        {
            if (z != GLG_ZERO)
            {
                optimized = GLG_TRUE;
                /* rotate only around z-axis */
                MAT3(m, 0, 0) = c;
                MAT3(m, 1, 1) = c;
                if (z < 0.0F)
                {
                    MAT3(m, 0, 1) = s;
                    MAT3(m, 1, 0) = -s;
                }
                else
                {
                    MAT3(m, 0, 1) = -s;
                    MAT3(m, 1, 0) = s;
                }
            }
        }
        else if (z == 0.0F)
        {
            optimized = GLG_TRUE;
            /* rotate only around y-axis */
            MAT3(m, 0, 0) = c;
            MAT3(m, 2, 2) = c;
            if (y < 0.0F)
            {
                MAT3(m, 0, 2) = -s;
                MAT3(m, 2, 0) = s;
            }
            else
            {
                MAT3(m, 0, 2) = s;
                MAT3(m, 2, 0) = -s;
            }
        }
    }
    else if (y == 0.0F)
    {
        if (z == 0.0F)
        {
            optimized = GLG_TRUE;
            /* rotate only around x-axis */
            MAT3(m, 1, 1) = c;
            MAT3(m, 2, 2) = c;
            if (x < 0.0F)
            {
                MAT3(m, 1, 2) = s;
                MAT3(m, 2, 1) = -s;
            }
            else
            {
                MAT3(m, 1, 2) = -s;
                MAT3(m, 2, 1) = s;
            }
        }
    }

    if (!optimized)
    {
        const Real mag = glg_sqrt(x * x + y * y + z * z);

        if (mag <= 1.0e-4)
        {
            /* no rotation, leave mat as-is */
            return;
        }

        x /= mag;
        y /= mag;
        z /= mag;

        xx = x * x;
        yy = y * y;
        zz = z * z;
        xy = x * y;
        yz = y * z;
        zx = z * x;
        xs = x * s;
        ys = y * s;
        zs = z * s;
        one_c = 1.0F - c;

        /* We already hold the identity-matrix so we can skip some statements */
        MAT3(m, 0,0) = (one_c * xx) + c;
        MAT3(m, 0,1) = (one_c * xy) - zs;
        MAT3(m, 0,2) = (one_c * zx) + ys;

        MAT3(m, 1, 0) = (one_c * xy) + zs;
        MAT3(m, 1,1) = (one_c * yy) + c;
        MAT3(m, 1,2) = (one_c * yz) - xs;

        MAT3(m, 2,0) = (one_c * zx) - ys;
        MAT3(m, 2,1) = (one_c * yz) + xs;
        MAT3(m, 2,2) = (one_c * zz) + c;
    }
}


Real mat3_determinant(const matrix3* m)
{
	return MAT3(m, 0, 0)*(MAT3(m, 1, 1)*MAT3(m, 2, 2) - MAT3(m, 1, 2)*MAT3(m, 2, 1))
		+ MAT3(m, 0, 1)*(MAT3(m, 1, 2)*MAT3(m, 2, 0) - MAT3(m, 1, 0)*MAT3(m, 2, 2))
		+ MAT3(m, 0, 2)*(MAT3(m, 1, 0)*MAT3(m, 2, 1) - MAT3(m, 1, 1)*MAT3(m, 2, 0));
}

void mat3_invert(matrix3* dst, const matrix3* m)
{
	Real d = mat3_determinant(m);
	Real r;

	if (d == 0)
	{
//		printf("invert error!\n");
	    return;
	}
	
	r = 1/d;

	MAT3(dst, 0, 0) = r*(MAT3(m, 1, 1)*MAT3(m, 2, 2) - MAT3(m, 1, 2)*MAT3(m, 2, 1));
	MAT3(dst, 0, 1) = r*(MAT3(m, 0, 2)*MAT3(m, 2, 1) - MAT3(m, 0, 1)*MAT3(m, 2, 2));
	MAT3(dst, 0, 2) = r*(MAT3(m, 0, 1)*MAT3(m, 1, 2) - MAT3(m, 0, 2)*MAT3(m, 1, 1));

	MAT3(dst, 1, 0) = r*(MAT3(m, 1, 2)*MAT3(m, 2, 0) - MAT3(m, 1, 0)*MAT3(m, 2, 2));
	MAT3(dst, 1, 1) = r*(MAT3(m, 0, 0)*MAT3(m, 2, 2) - MAT3(m, 0, 2)*MAT3(m, 2, 0));
	MAT3(dst, 1, 2) = r*(MAT3(m, 0, 2)*MAT3(m, 1, 0) - MAT3(m, 0, 0)*MAT3(m, 1, 2));

	MAT3(dst, 2, 0) = r*(MAT3(m, 1, 0)*MAT3(m, 2, 1) - MAT3(m, 1, 1)*MAT3(m, 2, 0));
	MAT3(dst, 2, 1) = r*(MAT3(m, 0, 1)*MAT3(m, 2, 0) - MAT3(m, 0, 0)*MAT3(m, 2, 1));
	MAT3(dst, 2, 2) = r*(MAT3(m, 0, 0)*MAT3(m, 1, 1) - MAT3(m, 0, 1)*MAT3(m, 1, 0));
}

void mat4_set(matrix4* m,
			  Real e00, Real e01, Real e02, Real e03,
			  Real e10, Real e11, Real e12, Real e13,
			  Real e20, Real e21, Real e22, Real e23,
			  Real e30, Real e31, Real e32, Real e33)
{
    MAT4(m, 0, 0) = e00;
    MAT4(m, 0, 1) = e01;
    MAT4(m, 0, 2) = e02;
    MAT4(m, 0, 3) = e03;

    MAT4(m, 1, 0) = e10;
    MAT4(m, 1, 1) = e11;
    MAT4(m, 1, 2) = e12;
    MAT4(m, 1, 3) = e13;

    MAT4(m, 2, 0) = e20;
    MAT4(m, 2, 1) = e21;
    MAT4(m, 2, 2) = e22;
    MAT4(m, 2, 3) = e23;

    MAT4(m, 3, 0) = e30;
    MAT4(m, 3, 1) = e31;
    MAT4(m, 3, 2) = e32;
    MAT4(m, 3, 3) = e33;
}

void mat4_copy(matrix4* dst, const matrix4* src)
{
    memcpy(dst, src, 16*sizeof(Real));
}

void mat4_set_zero(matrix4* m)
{
    int row;
    int col;
    for (row = 0; row < 4; row++)
        for (col = 0; col < 4; col++)
            MAT4(m, row, col) = GLG_ZERO;
}

void mat4_set_identity(matrix4* m)
{
    memcpy(m, IDENTITY, sizeof(Real)*16);
}

void mat4_set_translate(matrix4* m, Real x, Real y, Real z)
{
   memcpy(m, IDENTITY, sizeof(Real)*16);
   MAT4(m, 0, 3) = x;
   MAT4(m, 1, 3) = y;
   MAT4(m, 2, 3) = z;
}

void mat4_set_rotate(matrix4* m, Real angle, Real x, Real y, Real z)
{
    Real xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c, s, c;
//    Real m[16];
    GLG_BOOL optimized;

    s = (Real) sin( angle * DEG2RAD );
    c = (Real) cos( angle * DEG2RAD );

    memcpy(m, IDENTITY, sizeof(Real)*16);
    optimized = GLG_FALSE;

    if (x == GLG_ZERO)
    {
        if (y == GLG_ZERO)
        {
            if (z != GLG_ZERO)
            {
                optimized = GLG_TRUE;
                /* rotate only around z-axis */
                MAT4(m, 0, 0) = c;
                MAT4(m, 1, 1) = c;
                if (z < 0.0F)
                {
                    MAT4(m, 0, 1) = s;
                    MAT4(m, 1, 0) = -s;
                }
                else
                {
                    MAT4(m, 0, 1) = -s;
                    MAT4(m, 1, 0) = s;
                }
            }
        }
        else if (z == 0.0F)
        {
            optimized = GLG_TRUE;
            /* rotate only around y-axis */
            MAT4(m, 0, 0) = c;
            MAT4(m, 2, 2) = c;
            if (y < 0.0F)
            {
                MAT4(m, 0, 2) = -s;
                MAT4(m, 2, 0) = s;
            }
            else
            {
                MAT4(m, 0, 2) = s;
                MAT4(m, 2, 0) = -s;
            }
        }
    }
    else if (y == 0.0F)
    {
        if (z == 0.0F)
        {
            optimized = GLG_TRUE;
            /* rotate only around x-axis */
            MAT4(m, 1, 1) = c;
            MAT4(m, 2, 2) = c;
            if (x < 0.0F)
            {
                MAT4(m, 1, 2) = s;
                MAT4(m, 2, 1) = -s;
            }
            else
            {
                MAT4(m, 1, 2) = -s;
                MAT4(m, 2, 1) = s;
            }
        }
    }

    if (!optimized)
    {
        const Real mag = glg_sqrt(x * x + y * y + z * z);

        if (mag <= 1.0e-4)
        {
            /* no rotation, leave mat as-is */
            return;
        }

        x /= mag;
        y /= mag;
        z /= mag;

        xx = x * x;
        yy = y * y;
        zz = z * z;
        xy = x * y;
        yz = y * z;
        zx = z * x;
        xs = x * s;
        ys = y * s;
        zs = z * s;
        one_c = 1.0F - c;

        /* We already hold the identity-matrix so we can skip some statements */
        MAT4(m, 0,0) = (one_c * xx) + c;
        MAT4(m, 0,1) = (one_c * xy) - zs;
        MAT4(m, 0,2) = (one_c * zx) + ys;
        /*    M(0,3) = 0.0F;
        */

        MAT4(m, 1, 0) = (one_c * xy) + zs;
        MAT4(m, 1,1) = (one_c * yy) + c;
        MAT4(m, 1,2) = (one_c * yz) - xs;
        /*    M(1,3) = 0.0F;
        */

        MAT4(m, 2,0) = (one_c * zx) - ys;
        MAT4(m, 2,1) = (one_c * yz) + xs;
        MAT4(m, 2,2) = (one_c * zz) + c;
        /*    M(2,3) = 0.0F;
        */

        /*
        MAT4(m, 3,0) = 0.0F;
        MAT4(m, 3,1) = 0.0F;
        MAT4(m, 3,2) = 0.0F;
        MAT4(m, 3,3) = 1.0F;
          */
    }
}

void mat4_mul(matrix4* dst, const matrix4* m0, const matrix4* m1)
{
    int row;
    int col;
    int i;
	matrix4 result;

    mat4_set_zero(&result);

    for (row = 0; row < 4; row++)
        for (col = 0; col < 4; col++)
            for (i = 0; i < 4; i++)
                MAT4(&result, row, col) += MAT4(m0, row, i)*MAT4(m1, i, col);
	mat4_copy(dst, &result);
}


void mat4_set_rotate_around(matrix4* dst, Real angle, const vector3* v)
{
	Real s;
	Real c;
	Real _c;
	Real x;
	Real y;
	Real z;

	s = (Real)sin(angle*DEG2RAD);
	c = (Real)cos(angle*DEG2RAD);
	_c = 1 - c;
	x = v->x;
	y = v->y;
	z = v->z;
	mat4_set(dst,
		c + x*x*_c, x*y*_c - z*s, x*z*_c + y*s, 0,
		x*y*_c + z*s, c + y*y*_c, y*z*_c - x*s, 0,
		x*z*_c - y*s, y*z*_c + x*s, c + z*z*_c, 0,
		0, 0, 0, 1);
}


void mat4_mulr_rotate(matrix4* dst, Real angle, Real x, Real y, Real z)
{
	matrix4 tmp;
	matrix4 result;
	mat4_set_rotate(&tmp, angle, x, y, z);
	mat4_mul(&result, dst, &tmp);
	mat4_copy(dst, &result);
}

void mat4_mull_rotate(matrix4* dst, Real angle, Real x, Real y, Real z)
{
	matrix4 tmp;
	matrix4 result;
	mat4_set_rotate(&tmp, angle, x, y, z);
	mat4_mul(&result, &tmp, dst);
	mat4_copy(dst, &result);
}

void mat4_translate(matrix4* dst, Real x, Real y, Real z)
{
	matrix4 tmp;
	matrix4 result;
	mat4_set_translate(&tmp, x, y, z);
	mat4_mul(&result, dst, &tmp);
	mat4_copy(dst, &result);
}

void mat4_invert(matrix4* dst, const matrix4* m)
{
	Real d = mat4_determinant(m);
	Real r;
	matrix4 ret;

	if (d == 0)
	    return;
	
	r = 1/d;

	MAT4(&ret, 0, 0) = r*(MAT4(m, 1, 1)*(MAT4(m, 2, 2)*MAT4(m, 3, 3) - MAT4(m, 2, 3)*MAT4(m, 3, 2)) + MAT4(m, 1, 2)*(MAT4(m, 2, 3)*MAT4(m, 3, 1) - MAT4(m, 2, 1)*MAT4(m, 3, 3)) + MAT4(m, 1, 3)*(MAT4(m, 2, 1)*MAT4(m, 3, 2) - MAT4(m, 2, 2)*MAT4(m, 3, 1)));
	MAT4(&ret, 0, 1) = r*(MAT4(m, 2, 1)*(MAT4(m, 0, 2)*MAT4(m, 3, 3) - MAT4(m, 0, 3)*MAT4(m, 3, 2)) + MAT4(m, 2, 2)*(MAT4(m, 0, 3)*MAT4(m, 3, 1) - MAT4(m, 0, 1)*MAT4(m, 3, 3)) + MAT4(m, 2, 3)*(MAT4(m, 0, 1)*MAT4(m, 3, 2) - MAT4(m, 0, 2)*MAT4(m, 3, 1)));
	MAT4(&ret, 0, 2) = r*(MAT4(m, 3, 1)*(MAT4(m, 0, 2)*MAT4(m, 1, 3) - MAT4(m, 0, 3)*MAT4(m, 1, 2)) + MAT4(m, 3, 2)*(MAT4(m, 0, 3)*MAT4(m, 1, 1) - MAT4(m, 0, 1)*MAT4(m, 1, 3)) + MAT4(m, 3, 3)*(MAT4(m, 0, 1)*MAT4(m, 1, 2) - MAT4(m, 0, 2)*MAT4(m, 1, 1)));
	MAT4(&ret, 0, 3) = r*(MAT4(m, 0, 1)*(MAT4(m, 1, 3)*MAT4(m, 2, 2) - MAT4(m, 1, 2)*MAT4(m, 2, 3)) + MAT4(m, 0, 2)*(MAT4(m, 1, 1)*MAT4(m, 2, 3) - MAT4(m, 1, 3)*MAT4(m, 2, 1)) + MAT4(m, 0, 3)*(MAT4(m, 1, 2)*MAT4(m, 2, 1) - MAT4(m, 1, 1)*MAT4(m, 2, 2)));
		     
	MAT4(&ret, 1, 0) = r*(MAT4(m, 1, 2)*(MAT4(m, 2, 0)*MAT4(m, 3, 3) - MAT4(m, 2, 3)*MAT4(m, 3, 0)) + MAT4(m, 1, 3)*(MAT4(m, 2, 2)*MAT4(m, 3, 0) - MAT4(m, 2, 0)*MAT4(m, 3, 2)) + MAT4(m, 1, 0)*(MAT4(m, 2, 3)*MAT4(m, 3, 2) - MAT4(m, 2, 2)*MAT4(m, 3, 3)));
	MAT4(&ret, 1, 1) = r*(MAT4(m, 2, 2)*(MAT4(m, 0, 0)*MAT4(m, 3, 3) - MAT4(m, 0, 3)*MAT4(m, 3, 0)) + MAT4(m, 2, 3)*(MAT4(m, 0, 2)*MAT4(m, 3, 0) - MAT4(m, 0, 0)*MAT4(m, 3, 2)) + MAT4(m, 2, 0)*(MAT4(m, 0, 3)*MAT4(m, 3, 2) - MAT4(m, 0, 2)*MAT4(m, 3, 3)));
	MAT4(&ret, 1, 2) = r*(MAT4(m, 3, 2)*(MAT4(m, 0, 0)*MAT4(m, 1, 3) - MAT4(m, 0, 3)*MAT4(m, 1, 0)) + MAT4(m, 3, 3)*(MAT4(m, 0, 2)*MAT4(m, 1, 0) - MAT4(m, 0, 0)*MAT4(m, 1, 2)) + MAT4(m, 3, 0)*(MAT4(m, 0, 3)*MAT4(m, 1, 2) - MAT4(m, 0, 2)*MAT4(m, 1, 3)));
	MAT4(&ret, 1, 3) = r*(MAT4(m, 0, 2)*(MAT4(m, 1, 3)*MAT4(m, 2, 0) - MAT4(m, 1, 0)*MAT4(m, 2, 3)) + MAT4(m, 0, 3)*(MAT4(m, 1, 0)*MAT4(m, 2, 2) - MAT4(m, 1, 2)*MAT4(m, 2, 0)) + MAT4(m, 0, 0)*(MAT4(m, 1, 2)*MAT4(m, 2, 3) - MAT4(m, 1, 3)*MAT4(m, 2, 2)));
	    
	MAT4(&ret, 2, 0) = r*(MAT4(m, 1, 3)*(MAT4(m, 2, 0)*MAT4(m, 3, 1) - MAT4(m, 2, 1)*MAT4(m, 3, 0)) + MAT4(m, 1, 0)*(MAT4(m, 2, 1)*MAT4(m, 3, 3) - MAT4(m, 2, 3)*MAT4(m, 3, 1)) + MAT4(m, 1, 1)*(MAT4(m, 2, 3)*MAT4(m, 3, 0) - MAT4(m, 2, 0)*MAT4(m, 3, 3)));
	MAT4(&ret, 2, 1) = r*(MAT4(m, 2, 3)*(MAT4(m, 0, 0)*MAT4(m, 3, 1) - MAT4(m, 0, 1)*MAT4(m, 3, 0)) + MAT4(m, 2, 0)*(MAT4(m, 0, 1)*MAT4(m, 3, 3) - MAT4(m, 0, 3)*MAT4(m, 3, 1)) + MAT4(m, 2, 1)*(MAT4(m, 0, 3)*MAT4(m, 3, 0) - MAT4(m, 0, 0)*MAT4(m, 3, 3)));
	MAT4(&ret, 2, 2) = r*(MAT4(m, 3, 3)*(MAT4(m, 0, 0)*MAT4(m, 1, 1) - MAT4(m, 0, 1)*MAT4(m, 1, 0)) + MAT4(m, 3, 0)*(MAT4(m, 0, 1)*MAT4(m, 1, 3) - MAT4(m, 0, 3)*MAT4(m, 1, 1)) + MAT4(m, 3, 1)*(MAT4(m, 0, 3)*MAT4(m, 1, 0) - MAT4(m, 0, 0)*MAT4(m, 1, 3)));
	MAT4(&ret, 2, 3) = r*(MAT4(m, 0, 3)*(MAT4(m, 1, 1)*MAT4(m, 2, 0) - MAT4(m, 1, 0)*MAT4(m, 2, 1)) + MAT4(m, 0, 0)*(MAT4(m, 1, 3)*MAT4(m, 2, 1) - MAT4(m, 1, 1)*MAT4(m, 2, 3)) + MAT4(m, 0, 1)*(MAT4(m, 1, 0)*MAT4(m, 2, 3) - MAT4(m, 1, 3)*MAT4(m, 2, 0)));
	    
	MAT4(&ret, 3, 0) = r*(MAT4(m, 1, 0)*(MAT4(m, 2, 2)*MAT4(m, 3, 1) - MAT4(m, 2, 1)*MAT4(m, 3, 2)) + MAT4(m, 1, 1)*(MAT4(m, 2, 0)*MAT4(m, 3, 2) - MAT4(m, 2, 2)*MAT4(m, 3, 0)) + MAT4(m, 1, 2)*(MAT4(m, 2, 1)*MAT4(m, 3, 0) - MAT4(m, 2, 0)*MAT4(m, 3, 1)));
	MAT4(&ret, 3, 1) = r*(MAT4(m, 2, 0)*(MAT4(m, 0, 2)*MAT4(m, 3, 1) - MAT4(m, 0, 1)*MAT4(m, 3, 2)) + MAT4(m, 2, 1)*(MAT4(m, 0, 0)*MAT4(m, 3, 2) - MAT4(m, 0, 2)*MAT4(m, 3, 0)) + MAT4(m, 2, 2)*(MAT4(m, 0, 1)*MAT4(m, 3, 0) - MAT4(m, 0, 0)*MAT4(m, 3, 1)));
	MAT4(&ret, 3, 2) = r*(MAT4(m, 3, 0)*(MAT4(m, 0, 2)*MAT4(m, 1, 1) - MAT4(m, 0, 1)*MAT4(m, 1, 2)) + MAT4(m, 3, 1)*(MAT4(m, 0, 0)*MAT4(m, 1, 2) - MAT4(m, 0, 2)*MAT4(m, 1, 0)) + MAT4(m, 3, 2)*(MAT4(m, 0, 1)*MAT4(m, 1, 0) - MAT4(m, 0, 0)*MAT4(m, 1, 1)));
	MAT4(&ret, 3, 3) = r*(MAT4(m, 0, 0)*(MAT4(m, 1, 1)*MAT4(m, 2, 2) - MAT4(m, 1, 2)*MAT4(m, 2, 1)) + MAT4(m, 0, 1)*(MAT4(m, 1, 2)*MAT4(m, 2, 0) - MAT4(m, 1, 0)*MAT4(m, 2, 2)) + MAT4(m, 0, 2)*(MAT4(m, 1, 0)*MAT4(m, 2, 1) - MAT4(m, 1, 1)*MAT4(m, 2, 0)));

	mat4_copy(dst, &ret);
}


void mat4_transpose(matrix4* dst, const matrix4* m)
{
	matrix4 result;
    int row;
    int col;
    for (row = 0; row < 4; row++)
        for (col = 0; col < 4; col++)
            MAT4(&result, row, col) = MAT4(m, col, row);
	mat4_copy(dst, &result);
}

Real mat4_determinant(const matrix4* m)
{
    return (MAT4(m, 0, 0)*MAT4(m, 1, 1) - MAT4(m, 0, 1)*MAT4(m, 1, 0))
           *(MAT4(m, 2, 2)*MAT4(m, 3, 3) - MAT4(m, 2, 3)*MAT4(m, 3, 2))
           -(MAT4(m, 0, 0)*MAT4(m, 1, 2) - MAT4(m, 0, 2)*MAT4(m, 1, 0))
           *(MAT4(m, 2, 1)*MAT4(m, 3, 3) - MAT4(m, 2, 3)*MAT4(m, 3, 1))
           +(MAT4(m, 0, 0)*MAT4(m, 1, 3) - MAT4(m, 0, 3)*MAT4(m, 1, 0))
           *(MAT4(m, 2, 1)*MAT4(m, 3, 2) - MAT4(m, 2, 2)*MAT4(m, 3, 1))
           +(MAT4(m, 0, 1)*MAT4(m, 1, 2) - MAT4(m, 0, 2)*MAT4(m, 1, 1))
           *(MAT4(m, 2, 0)*MAT4(m, 3, 3) - MAT4(m, 2, 3)*MAT4(m, 3, 0))
           -(MAT4(m, 0, 1)*MAT4(m, 1, 3) - MAT4(m, 0, 3)*MAT4(m, 1, 1))
           *(MAT4(m, 2, 0)*MAT4(m, 3, 2) - MAT4(m, 2, 2)*MAT4(m, 3, 0))
           +(MAT4(m, 0, 2)*MAT4(m, 1, 3) - MAT4(m, 0, 3)*MAT4(m, 1, 2))
           *(MAT4(m, 2, 0)*MAT4(m, 3, 1) - MAT4(m, 2, 1)*MAT4(m, 3, 0));
}


void mat4_dump(const matrix4* m)
{
    printf(" [ %f %f %f %f ]\n", m->m[0], m->m[4], m->m[8], m->m[12]);
    printf(" [ %f %f %f %f ]\n", m->m[1], m->m[5], m->m[9], m->m[13]);
    printf(" [ %f %f %f %f ]\n", m->m[2], m->m[6], m->m[10], m->m[14]);
    printf(" [ %f %f %f %f ]\n", m->m[3], m->m[7], m->m[11], m->m[15]);
}


Real distsq_tri_point(const vector3* v0,
					  const vector3* v1,
					  const vector3* v2,
					  const vector3* p)
{
	/* refer to http://www.magic-software.com/Distance.html */
	vector3 e0;
	vector3 e1;
	vector3 temp;
	Real a;
	Real b;
	Real c;
	Real d;
	Real e;
	Real f;
	Real det;
	Real inv_det;
	Real s;
	Real t;
	Real numer;
	Real denom;
	Real tmp0;
	Real tmp1;

	vec3_sub(&e0, v1, v0);
	vec3_sub(&e1, v2, v0);

	a = vec3_dot(&e0, &e0);
	b = vec3_dot(&e0, &e1);
	c = vec3_dot(&e1, &e1);
	vec3_sub(&temp, v0, p);
	d = vec3_dot(&e0, &temp);
	e = vec3_dot(&e1, &temp);
	f = vec3_dot(&temp, &temp);


	det = a*c - b*b;
	s = b*e - c*d;
	t = b*d - a*e;

	if ((s + t) <= det)
	{
		if (s < 0)
		{
			if (t < 0)
			{
				/* region 4 */
				if (d < 0.0f)
				{
					t = 0;
					s = ((-d >= a) ? 1 : -d/a);
				}
				else
				{
					s = 0;
					t = ((e >= 0) ? 0 : ((-e >= c) ? 1 : -e/c));
				}

			}
			else
			{
				/* region 3 */
				s = 0;
				t = (e >= 0 ? 0 : (-e >= c ? 1 : -e/c));
			}
		}
		else if (t < 0)
		{
			/* region 5 */
			s = (d >= 0 ? 0 : (-d >= a ? 1 : -d/a));
			t = 0;
		}
		else
		{
			/* region 0 */
			inv_det = 1/det;
			s *= inv_det;
			t *= inv_det;
		}
	}
	else
	{
		if (s < 0)
		{
			/* region 2 */
			tmp0 = b + d;
			tmp1 = c + e;
			if (tmp1 > tmp0)
			{
				numer = tmp1 - tmp0;
				denom = a - 2*b + c;
				s = (numer >= denom ? 1 : numer/denom);
				t = 1 - s;
			}
			else
			{
				s = 0;
				t = (tmp1 <= 0 ? 1 : (e >= 0 ? 0 : -e/c));
			}
		}
		else if (t < 0)
		{
			/* region 6 */
			tmp0 = b + e;
			tmp1 = a + d;
			if (tmp1 > tmp0)
			{
				numer = tmp1 - tmp0;
				denom = a - 2*b + c;
				t = (numer >= denom ? 1 : numer/denom);
				s = 1 - t;
			}
			else
			{
				t = 0;
				s = (tmp1 <= 0 ? 1 : (d >= 0 ? 0 : -d/a));
			}
		}
		else
		{
			/* region 1 */
			numer = (c + e - b - d);
			if (numer <= 0)
			{
				s = 0;
			}
			else
			{
				denom = a - 2*b + c;
				s = (numer >= denom ? 1 : numer/denom);
			}
			t = 1 - s;
		}
	}


	vec3_copy(&temp, v0);
	vec3_scaleadd(&temp, &temp, s, &e0);
	vec3_scaleadd(&temp, &temp, t, &e1);

	return vec3_distsq(&temp, p);
}


Real dist_tri_point(const vector3* v0,
					const vector3* v1,
					const vector3* v2,
					const vector3* p)
{
	return glg_sqrt(distsq_tri_point(v0, v1, v2, p));
}

Real dist_tri_point2(const vector3* v0,
					 const vector3* v1,
					 const vector3* v2,
					 const vector3* p)
{
	
	/*
	vector3 e0;
	vector3 e1;
	vector3 tri_n;
	vector3 p_tri;
	vector3 org_p;


	vec3_sub(&e0, v1, v0);
	vec3_sub(&e1, v2, v0);
	vec3_cross(&tri_n, &e0, &e1);
	vec3_normalize(&tri_n);
	vec3_sub(&org_p, p, v0);
	vec3_set(&p_tri, vec3_dot(&e0, &org_p), vec3_dot(&e1, &org_p), vec3_dot(&tri_n, &org_p));

	if ((p_tri.x > 0.0) && (p_tri.y > 0.0) && ((p_tri.x + p_tri.y) < (vec3_lensq(&e0) + vec3_lensq(&e1))))
//	if ((p_tri.x > 0.0) && (p_tri.y > 0.0) && ((p_tri.x + p_tri.y) < 1.0))
	{
		return p_tri.z;
	}
	else return p_tri.z + p_tri.x*p_tri.x + p_tri.y*p_tri.y;//length(p_tri);
*/

	/**** original ****/
	
	vector3 e0;
	vector3 e1;
	vector3 tri_n;
	vector3 p_tri;
	matrix4 m;
	matrix4 m_tri;


	vec3_sub(&e0, v1, v0);
	vec3_sub(&e1, v2, v0);

	tri_normal(&tri_n, v0, v1, v2);

	mat4_set_identity(&m);
	MAT4(&m, 0, 0) = e0.x;
	MAT4(&m, 1, 0) = e0.y;
	MAT4(&m, 2, 0) = e0.z;

	MAT4(&m, 0, 1) = e1.x;
	MAT4(&m, 1, 1) = e1.y;
	MAT4(&m, 2, 1) = e1.z;

	MAT4(&m, 0, 2) = tri_n.x;
	MAT4(&m, 1, 2) = tri_n.y;
	MAT4(&m, 2, 2) = tri_n.z;

	MAT4(&m, 0, 3) = v0->x;
	MAT4(&m, 1, 3) = v0->y;
	MAT4(&m, 2, 3) = v0->z;

	mat4_invert(&m_tri, &m);

	mat4_mulvec3(&p_tri, &m_tri, p);

	//return p_tri.z;

//	if ((0.0f < p_tri.x) && (p_tri.x < 1.0f) && (0.0f < p_tri.y) && (p_tri.y < 1.0f))
	if ((0.0f <= p_tri.x) && (0.0f <= p_tri.y) && ((p_tri.x + p_tri.y) <= 1.0f))
		return p_tri.z;
	
	return p_tri.z + p_tri.x*p_tri.x + p_tri.y*p_tri.y;
	
}

void tri_normal(vector3* n, 
				const vector3* v0,
				const vector3* v1,
				const vector3* v2)
{
	vector3 e0;
	vector3 e1;

	vec3_sub(&e0, v2, v0);
	vec3_sub(&e1, v1, v0);

	vec3_cross(n, &e0, &e1);
	vec3_normalize(n, n);
}