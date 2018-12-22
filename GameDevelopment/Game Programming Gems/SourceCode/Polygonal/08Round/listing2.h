/* Copyright (C) Tim Round, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Tim Round, 2000"
 */
typedef struct _VECTOR
{
	float x;
	float y;
	float z;
}VECTOR;

typedef struct _PLANE
{
	float a;
	float b;
	float c;
	float d;
}PLANE;

typedef struct _MATRIX
{
	float _11;
	float _12;
	float _13;
	float _14;
	float _21;
	float _22;
	float _23;
	float _24;
	float _31;
	float _32;
	float _33;
	float _34;
	float _41;
	float _42;
	float _43;
	float _44;
}MATRIX;

void PlaneFromPoints(VECTOR*, VECTOR*, VECTOR*, PLANE*);

