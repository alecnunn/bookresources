/* Copyright (C) Nathan d'Obrenan, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Nathan d'Obrenan, 2001"
 */
#ifndef __main_includes_h__
#define __main_includes_h__

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////
// INCLUDES
/////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <crtdbg.h>
#include <vfw.h>

#include <GL/gl.h>
#include <GL/glaux.h>

//////////////////////////////////////
// FIRETOAD's TYPEDEFS
//////////////////////////////////////
typedef float fs_float;
typedef double fs_double;

typedef void fs_void;

typedef unsigned char fs_bool;

typedef unsigned int fs_uint;
typedef signed int fs_int;

typedef unsigned char fs_uchar;
typedef signed char fs_char;

typedef unsigned short fs_ushort;
typedef signed short fs_short;

typedef unsigned long fs_ulong;
typedef signed long fs_long;

/////////////////////////////////////////////////////////////////////
// DEFINES
/////////////////////////////////////////////////////////////////////
#define FS_TRUE   (1)
#define FS_FALSE  (0)

#define M_PI 3.14159265359f

#define SQ(x) ((x)*(x))

#define ONE_OVER_180 0.00555555f
#define RAD(x) ((x)*ONE_OVER_180*M_PI)
#define DEG(x) ((x)*M_PI1*180.0f)

#define MAKEVECTOR3(a,vx,vy,vz)	{a[0] = vx; a[1] = vy; a[2] = vz;}

#define CAM_DEGRADE_SPEED  (0.5f)

/////////////////////////////////////////////////////////////////////
// STRUCTS
/////////////////////////////////////////////////////////////////////
typedef struct opengl_matrix_struct
{
	fs_float mat[16];
}opengl_matrix;

typedef struct matrix44f_struct
{ 
  fs_float mat[4][4];
} matrix44f;

typedef struct matrix33f_struct
{ 
  fs_float mat[3][3];
} matrix33f;

typedef struct vector3f_struct
{ 
  fs_float vec[3];
}vector3f;

typedef struct vector4f_struct
{ 
  fs_float vec[4];
}vector4f;

typedef struct camera_struct
{ 
  matrix44f info;       // (matrix44f) This is the cameras main matrix
  matrix33f rotation;   // (matrix33f) This is the rotational part of the camera's matrix
  
  vector4f  view_pos;   // (vector3f) Camera's world coords this frame
  vector4f  targ_pos;   // (vector3f) Camera's target's position
  vector4f    up_pos;   // (vector3f) Camera's up vector orientation
  
  vector3f motion;      // (fs_float) This vector is its x (strafe), y (height), and z (speed) vectors
}camera;

enum
{ 
	HELICAM_STRAFE = 0,
	HELICAM_HEIGHT,
	HELICAM_SPEED,
	HELICAM_PITCH,
	HELICAM_HEADING,
	HELICAM_ROLL,
  HELICAM_STOP,
  HELICAM_180
};

// EXTERNALS/////////////////////////////////////////////////////////
extern camera game_cam;

enum { X=0, Y, Z, W };
enum { R=0, G, B, A };

// MAIN.C -- PROTOTYPES//////////////////////////////////////////////
void init(void);
void draw_screen(void);
void settings_init(char *filename);

void move_person(void);

// MATRIX.C -- PROTOTYPES////////////////////////////////////////////
fs_void matrix44_init(matrix44f *m);
fs_void matrix33_init(matrix33f *m);
fs_void matrix44_copy_matrix33(matrix44f *to, const matrix33f *from);
fs_void matrix33_copy_matrix44(matrix33f *to,const matrix44f *from);
fs_void matrix44_mult(matrix44f *to, const matrix44f *from);
fs_void matrix33_mult(matrix33f *to,const matrix33f *from);
fs_void matrix44_scale(matrix44f *m,fs_float scale);
fs_void matrix33_scale(matrix33f *m,fs_float scale);
fs_void matrix44_xrot(fs_float angle, matrix44f *m);
fs_void matrix33_xrot(fs_float angle, matrix33f *m);
fs_void matrix44_yrot(fs_float angle, matrix44f *m);
fs_void matrix33_yrot(fs_float angle, matrix33f *m);
fs_void matrix44_zrot(float angle, matrix44f *m);
fs_void matrix33_zrot(float angle, matrix33f *m);
fs_int matrix44_inverse(matrix44f *m);
fs_int matrix33_inverse(matrix33f *m);
fs_void matrix44_mult_vector4(const matrix44f *m, vector4f *v);
fs_void matrix33_t_mult_vector3(const matrix33f *m, vector3f *v);
fs_void matrix33_mult_vector3(const matrix33f *m, vector3f *v);
fs_void matrix44_mult_vector3(const matrix44f *m, vector3f *v);
fs_void matrix44_transpose(matrix44f *m);
fs_void matrix33_transpose(matrix33f *m);
fs_void matrix44_copy(matrix44f *m1, matrix44f *m2);
fs_void matrix33_copy(matrix33f *m1, matrix33f *m2);

fs_void matrix44_orthonormalize(matrix44f *Orientation);

// CAMERA.C -- PROTOTYPES////////////////////////////////////////////
fs_void cam_init(camera *cam);
fs_void cam_update(camera *cam, fs_char move_type, fs_float amount);
fs_void cam_position(camera *cam);
fs_void cam_move(camera *cam);

// VECTOR.C -- PROTOTYPES////////////////////////////////////////////
fs_void vector4_init(vector4f *v);
fs_void vector3_init(vector3f *v);
fs_void vector4_scale(vector4f *from,fs_float scale,vector4f *to);
fs_void vector4_add(vector4f *from, vector4f *operand, vector4f *to);
fs_void vector3_add(vector3f *from, vector3f *operand, vector3f *to);
fs_void vector4_sub(vector4f *from, vector4f *operand, vector4f *to);
fs_void vector3_sub(vector3f *from, vector3f *operand, vector3f *to);
fs_float vector4_normalize(vector4f *to);
fs_void vector3_normalize(vector3f *to);
fs_void vector3_copy(vector3f *v1, vector3f *v2);
fs_float vector4_mult_as_matrix(vector4f *v1,vector4f *v2);
fs_void vector4_div(vector4f *from, vector4f *divisor, vector4f *to);
fs_void vector3_div(vector3f *from, vector3f *divisor, vector3f *to);
fs_void vector4_mult(vector4f *from, vector4f *multiplicand, vector4f *to);
fs_void vector3_mult(vector3f *from, vector3f *multiplicand, vector3f *to);

extern __forceinline fs_void cross_product(fs_float *u, fs_float *v, fs_float *n);
extern __forceinline fs_int __stdcall float_to_int(fs_float x);


#ifdef __cplusplus
}
#endif

#endif