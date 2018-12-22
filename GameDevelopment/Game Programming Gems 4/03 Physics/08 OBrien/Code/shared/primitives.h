/*********************************************************************
 * primitives.h
 * Authored by Kris Hauser 2002-2003
 *
 * Declares some simple linear algebra primitives,
 * using C arrays.
 * NOTE: there's another primitives.h in math/.
 * This one doesn't use classes.
 *
 * Copyright (c) 2002,2003 SquireSoft, used with permission.
 *
 *********************************************************************/

/**************************************************************************
 * Copyright (c) 2003, University of California at Berkeley
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 * 
 *     * Neither the name of the University of California nor the names
 *       of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written
 *       permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************/



#ifndef DEFORM_PRIMITIVES_H
#define DEFORM_PRIMITIVES_H

#include <math.h>

#ifdef MATH_DOUBLE
typedef double float_t;
#else
typedef float float_t;
#endif //MATH_DOUBLE


 
typedef float_t vec3_t [3];
typedef float_t vec4_t [4];
typedef float_t matrix4_t [4][4];
typedef float_t* tet_ptr_element [4];
 
typedef float_t rgba_t [4];

/*****************************vec3 operations*******************************/

//x = 0
inline void vec3_zero(vec3_t x)
{
	x[0]=x[1]=x[2]=0.0;
}

//x = (a,b,c)
inline void vec3_make(vec3_t x, float_t a, float_t b, float_t c)
{
	x[0] = a;
	x[1] = b;
	x[2] = c;
}

//x = a
inline void vec3_equal(vec3_t x, const vec3_t a)
{
	x[0]=a[0];
	x[1]=a[1];
	x[2]=a[2];
}

//x = a+b
inline void vec3_add(vec3_t x, const vec3_t a, const vec3_t b)
{
	x[0] = a[0]+b[0];
	x[1] = a[1]+b[1];
	x[2] = a[2]+b[2];
}

//x = a-b
inline void vec3_sub(vec3_t x, const vec3_t a, const vec3_t b)
{
	x[0] = a[0]-b[0];
	x[1] = a[1]-b[1];
	x[2] = a[2]-b[2];
}

//x = -a
inline void vec3_negate(vec3_t x, const vec3_t a)
{
	x[0] = -a[0];
	x[1] = -a[1];
	x[2] = -a[2];
}

//x = a*c
inline void vec3_multiply(vec3_t x, const vec3_t a, float_t c)
{
	x[0] = a[0]*c;
	x[1] = a[1]*c;
	x[2] = a[2]*c;
}

//x = x + a*c
inline void vec3_madd(vec3_t x, const vec3_t a, float_t c)
{
  x[0] += a[0]*c;
  x[1] += a[1]*c;
  x[2] += a[2]*c;
}

//returns dot(a,b)
inline float_t vec3_dot(const vec3_t a, const vec3_t b)
{
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

//x = a cross b
inline void vec3_cross(vec3_t x, const vec3_t a, const vec3_t b)
{
	x[0] = a[1]*b[2] - a[2]*b[1];
	x[1] = a[2]*b[0] - a[0]*b[2];
	x[2] = a[0]*b[1] - a[1]*b[0];
}

//returns |x|
inline float_t vec3_length(const vec3_t x)
{
#ifdef USE_DOUBLE
  return sqrt(vec3_dot(x,x));
#else
  return sqrtf(vec3_dot(x,x));
#endif
}

//normalizes x
inline void vec3_normalize(vec3_t x)
{
	float_t l = (float_t)(1.0/vec3_length(x));
	vec3_multiply(x, x, l);
}

inline void vec3_modulate(vec3_t x, const vec3_t a, const vec3_t b)
{
  x[0] = a[0]*b[0];
  x[1] = a[1]*b[1];
  x[2] = a[2]*b[2];
}

//x = projection of a onto b = <a,b>/<b,b>*b
inline void vec3_project(vec3_t x, const vec3_t a, const vec3_t b)
{
	vec3_multiply(x, b, vec3_dot(a,b)/vec3_dot(b,b));
}


/*****************************vec4 operations*******************************/


//x = 0
inline void vec4_zero(vec4_t x)
{
	x[0]=x[1]=x[2]=x[3]=0.0f;
}

//x = (a,b,c,d)
inline void vec4_make(vec4_t x, float_t a, float_t b, float_t c, float_t d)
{
	x[0]=a;
	x[1]=b;
	x[2]=c;
	x[3]=d;
}

//x = a
inline void vec4_equal(vec4_t x, const vec4_t a)
{
	x[0]=a[0];
	x[1]=a[1];
	x[2]=a[2];
	x[3]=a[3];
}

//x = a+b
inline void vec4_add(vec4_t x, const vec4_t a, const vec4_t b)
{
	x[0] = a[0]+b[0];
	x[1] = a[1]+b[1];
	x[2] = a[2]+b[2];
	x[3] = a[3]+b[3];
}

//x = a-b
inline void vec4_sub(vec4_t x, const vec4_t a, const vec4_t b)
{
	x[0] = a[0]-b[0];
	x[1] = a[1]-b[1];
	x[2] = a[2]-b[2];
	x[3] = a[3]-b[3];
}

//x = a*c
inline void vec4_multiply(vec4_t x, const vec4_t a, float_t c)
{
	x[0] = a[0]*c;
	x[1] = a[1]*c;
	x[2] = a[2]*c;
	x[3] = a[3]*c;
}

//x = -a
inline void vec4_negate(vec4_t x, const vec4_t a)
{
	x[0] = -a[0];
	x[1] = -a[1];
	x[2] = -a[2];
	x[3] = -a[3];
}

//returns dot(a,b)
inline float_t vec4_dot(const vec4_t a, const vec4_t b)
{
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2] + a[3]*b[3];
}

//normalizes x
inline void vec4_normalize(vec4_t x)
{
	float_t l = 1.0f/sqrtf(vec4_dot(x,x));
	vec4_multiply(x, x, l);
}

//x = homogeneous orthogonal projection of a onto plane b:(n,d) = a - <a,b>/<n,n>*n
inline void vec4_plane_project(vec4_t x, const vec4_t a, const vec4_t b)
{
	vec4_t n;
	vec3_equal(n, b);
	n[4] = 0;
	vec4_multiply(x, n, vec4_dot(a,b)/vec3_dot(b,b));
	vec4_sub(x, a, x);
}

#define rgba_make vec4_make
#define rgba_zero vec4_zero

/*****************************matrix operations*******************************/

//x = 0
inline void matrix4_zero(matrix4_t x)
{
	int i,j;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			x[i][j] = 0.0;
}

//x = I
inline void matrix4_identity(matrix4_t x)
{
	matrix4_zero(x);
	x[0][0] = x[1][1] = x[2][2] = x[3][3] = 1.0;
}

//x = a
inline void matrix4_equal(matrix4_t x, const matrix4_t a)
{
	int i,j;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			x[i][j] = a[i][j];
}

//x = a+b
inline void matrix4_add(matrix4_t x, const matrix4_t a, const matrix4_t b)
{
	int i,j;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			x[i][j] = a[i][j] + b[i][j];
}

//x = a-b
inline void matrix4_sub(matrix4_t x, const matrix4_t a, const matrix4_t b)
{
	int i,j;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			x[i][j] = a[i][j] - b[i][j];
}

//x = a*b
inline void matrix4_multiply(matrix4_t x, const matrix4_t a, const matrix4_t b)
{
	int i,j,k;

	matrix4_t temp;
    matrix4_zero(temp);

    for(i=0; i<4; i++ ) 
        for(j=0; j<4; j++ ) 
            for(k=0; k<4; k++ ) 
                temp[i][j] += b[i][k] * a[k][j];

	matrix4_equal(x, temp);

}

//x = a^t
inline void matrix4_transpose(matrix4_t x, const matrix4_t a)
{
	int i,j;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			x[i][j] = a[j][i];
}

//x = A*v
inline void matrix4_vec3_multiply(vec3_t x, const matrix4_t a, const vec3_t v)
{
	x[0] = a[0][0]*v[0] + a[1][0]*v[1] + a[2][0]*v[2] + a[3][0];
	x[1] = a[0][1]*v[0] + a[1][1]*v[1] + a[2][1]*v[2] + a[3][1];
	x[2] = a[0][2]*v[0] + a[1][2]*v[1] + a[2][2]*v[2] + a[3][2];
}

//x = A*v (the 3*3 part)
inline void matrix4_normal_multiply(vec3_t x, const matrix4_t a, const vec3_t v)
{
	x[0] = a[0][0]*v[0] + a[1][0]*v[1] + a[2][0]*v[2];
	x[1] = a[0][1]*v[0] + a[1][1]*v[1] + a[2][1]*v[2];
	x[2] = a[0][2]*v[0] + a[1][2]*v[1] + a[2][2]*v[2];
}

//x = At*v
inline void matrix4_vec3_multiply_transpose(vec3_t x, const matrix4_t a, const vec3_t v)
{
  x[0] = a[0][0]*v[0] + a[0][1]*v[1] + a[0][2]*v[2];
  x[1] = a[1][0]*v[0] + a[1][1]*v[1] + a[1][2]*v[2];
  x[2] = a[2][0]*v[0] + a[2][1]*v[1] + a[2][2]*v[2];
}

#define matrix4_normal_multiply_transpose matrix4_vec3_multiply_transpose

//x = A*v
inline void matrix4_vec4_multiply(vec4_t x, const matrix4_t a, const vec4_t v)
{
	x[0] = a[0][0]*v[0] + a[1][0]*v[1] + a[2][0]*v[2] + a[3][0]*v[3];
	x[1] = a[0][1]*v[0] + a[1][1]*v[1] + a[2][1]*v[2] + a[3][1]*v[3];
	x[2] = a[0][2]*v[0] + a[1][2]*v[1] + a[2][2]*v[2] + a[3][2]*v[3];
}

//x = A*v
inline void matrix4_vec4_multiply_transpose(vec4_t x, const matrix4_t a, const vec4_t v)
{
	x[0] = a[0][0]*v[0] + a[0][1]*v[1] + a[0][2]*v[2] + a[0][3]*v[3];
	x[1] = a[1][0]*v[0] + a[1][1]*v[1] + a[1][2]*v[2] + a[1][3]*v[3];
	x[2] = a[2][0]*v[0] + a[2][1]*v[1] + a[2][2]*v[2] + a[2][3]*v[3];
	x[3] = a[3][0]*v[0] + a[3][1]*v[1] + a[3][2]*v[2] + a[3][3]*v[3];
}

//scales the rotation part of the matrix by (s,s,s)
inline void matrix4_scale3(matrix4_t x, float_t scale)
{
	int i,j;

	for(i=0; i<3; i++)
		for(j=0; j<3; j++)
			x[i][j] *= scale;
}

//scales the rotation part of a matrix by (x,y,z)
inline void matrix4_vec3_scale(matrix4_t x, const vec3_t v_scale)
{
	int i,j;

	for(i=0; i<3; i++)
		for(j=0; j<3; j++)
			x[i][j] *= v_scale[i];
}


//x = matrix such that x*w = (v x w) for all w
inline void matrix4_cross_product(matrix4_t x, const vec3_t v)
{
	matrix4_zero(x);
	x[1][0] = -v[2];
	x[2][1] = -v[0];
	x[2][0] = v[1];

	x[0][1] = v[2];
	x[1][2] = v[0];
	x[0][2] = -v[1];
	x[3][3] = 1.0;
}

inline void matrix4_uncross(vec3_t x, const matrix4_t a)
{
	x[0] = a[1][2];
	x[1] = a[2][0];
	x[2] = a[0][1];
}

inline float_t matrix4_determinant(const matrix4_t a)
{
	return a[0][0] * (a[1][1]*a[2][2] - a[2][1]*a[1][2])
		+ a[1][0] * (a[2][1]*a[0][2] - a[2][2]*a[0][1])
		+ a[2][0] * (a[0][1]*a[1][2] - a[0][2]*a[1][1]);
}

//x = matrix such that x*w = proj (w on v) for all w
inline void matrix4_projection(matrix4_t x, const vec3_t v)
{
	vec3_t temp;
	matrix4_identity(x);
	vec3_make(temp, 1,0,0);
	vec3_project(x[0], temp, v);
	vec3_make(temp, 0,1,0);
	vec3_project(x[1], temp, v);
	vec3_make(temp, 0,0,1);
	vec3_project(x[2], temp, v);
}

//x = matrix such that x*w = proj (w on plane defined by v) for all w
inline void matrix4_plane_projection(matrix4_t x, const vec3_t v)
{
	matrix4_t temp;
	matrix4_identity(x);
	matrix4_projection(temp, v);
	matrix4_sub(x, x, temp);
}


bool matrix3_invert( matrix4_t x, const matrix4_t a );
bool matrix4_invert( matrix4_t x, const matrix4_t a );

void matrix4_rotation(matrix4_t x, const vec3_t r);
void matrix4_rotation_axis(vec3_t r, const matrix4_t R);


//x = R(r)*T(t)
inline void matrix4_rotation_translation(matrix4_t x, const matrix4_t r, const vec3_t t)
{
	int i,j;
	for(i=0; i<3; i++)
		for(j=0; j<3; j++)
			x[i][j] = r[i][j];

	for(i=0; i<3; i++)
		x[3][i]=t[i];
	x[0][3] = x[1][3] = x[2][3] = 0.0;
	x[3][3] = 1.0;
}

//x = a^-1 if a = rigid body transform
inline void matrix4_rb_inverse(matrix4_t x, const matrix4_t a)
{
  int i,j;
  //R(x) = R(a)^t
  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      x[i][j] = a[j][i];

  //T(x) = R(a)^-1 * T(-a)
  matrix4_vec3_multiply_transpose(x[3], a, a[3]);
  vec3_negate(x[3], x[3]);

  x[0][3] = x[1][3] = x[2][3] = 0.0;
  x[3][3] = 1.0;
}


#endif
