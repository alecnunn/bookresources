/** 
 * Utility code header for GPU utility functions
 *
 * Copyright (c) Takashi AMADA  All Rights Reseved
 **/


#ifndef _GPU_UTIL_H_
#define _GPU_UTIL_H_

#include <windows.h>

#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/gl.h>
#include <assert.h>
#include <stdio.h>


#define GPUT_BOOL int
#define GPUT_TRUE 1
#define GPUT_FALSE 0

typedef struct
{
	DWORD start;
	DWORD elapsed;
	int count;
} gput_profile;

typedef struct
{
	GLfloat position[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
	GLfloat shininess;
} gput_mat;

typedef struct
{
	GLfloat position[4];
	GLfloat diffuse[4];
	GLfloat ambient[4];
} gput_light;

typedef struct
{
	GLint* array;
	int size;
	int cap;
} gput_intlist;


typedef struct
{
	HDC hdc;
	HGLRC hglrc;
} gput_context;

typedef struct
{
	HPBUFFERARB hpbuf;
	HDC hdc;
	HGLRC hglrc;
	int windowid;
	GLuint tex;
	GLenum target;
} gput_pbuffer;

typedef struct
{
	GLuint tex;
} gput_cubemap;

void gput_init();

_inline void gput_printf(const char* format, ...)
{
	va_list list;
	va_start(list, format);
	vprintf(format, list);
	va_end(list);
}

/** Shaders **/
//#define NO_NVCG
//#define SHADER_EMBEDDED

#ifndef NO_NVCG
#include <Cg/cg.h>
#include <Cg/cgGL.h>
CGcontext gput_cg_context();
#endif
#define SHADER_DIR "shaders\\"



#ifdef SHADER_EMBEDDED
#define FP_PROFILE CG_PROFILE_ARBFP1
#define VP_PROFILE CG_PROFILE_ARBVP1
#else
#define FP_PROFILE CG_PROFILE_ARBFP1
#define VP_PROFILE CG_PROFILE_ARBVP1
//#define FP_PROFILE CG_PROFILE_FP40
//#define VP_PROFILE CG_PROFILE_VP40
#endif


#ifdef SHADER_EMBEDDED
#define LOAD_FRAGMENT_PROGRAM(program, file) program = cgCreateProgram(\
	gput_cg_context(), CG_OBJECT, fp_str_##program, FP_PROFILE, NULL, NULL);\
	check_cg_error();\
	cgGLLoadProgram(program);\
	check_cg_error();
	//gput_printf("FP " #program " loaded\n")

#define LOAD_VERTEX_PROGRAM(program, file) program = cgCreateProgram(\
	gput_cg_context(), CG_OBJECT, vp_str_##program, VP_PROFILE, NULL, NULL);\
	check_cg_error();\
	cgGLLoadProgram(program);\
	check_cg_error();
	//gput_printf("VP " #program " loaded\n")

#define READ_FRAGMENT_PROGRAM(program, id, file) program = cgCreateProgram(\
	gput_cg_context(), CG_SOURCE, cg_str_##id, FP_PROFILE, NULL, NULL);\
	check_cg_error()

#else /* SHADER_EMBEDDED */

#define LOAD_FRAGMENT_PROGRAM(program, file) program = cgCreateProgramFromFile(\
	gput_cg_context(), CG_SOURCE, SHADER_DIR file, FP_PROFILE, NULL, NULL);\
	check_cg_error();\
	cgGLLoadProgram(program);\
	check_cg_error();\
	gput_printf("FP " #program " loaded\n")

#define LOAD_VERTEX_PROGRAM(program, file) program = cgCreateProgramFromFile(\
	gput_cg_context(), CG_SOURCE, SHADER_DIR file, VP_PROFILE, NULL, NULL);\
	check_cg_error();\
	cgGLLoadProgram(program);\
	check_cg_error();\
	gput_printf("VP " #program " loaded\n")

#define READ_FRAGMENT_PROGRAM(program, id, file) program = cgCreateProgramFromFile(\
	gput_cg_context(), CG_SOURCE, SHADER_DIR file, FP_PROFILE, NULL, NULL);\
	check_cg_error()
#endif /* SHADER_EMBEDDED */

#define CREATE_PARAM(x) cgCreateParameter(gput_cg_context(), x)


void gput_print_gpu_profile();

GLfloat gput_pack_2uint(GLuint i0, GLuint i1);

/** Material **/
/**
 * OpenGL material functions
 */
void gput_mat_init(gput_mat* mat);
void gput_mat_diffuse(gput_mat* mat, GLfloat r, GLfloat g, GLfloat b, GLfloat a);
void gput_mat_specular(gput_mat* mat, GLfloat r, GLfloat g, GLfloat b, GLfloat a);
void gput_mat_shininess(gput_mat* mat, GLfloat s);
void gput_mat_bind(gput_mat* mat);

/** Light **/
/**
 * OpenGL light functions
 */
void gput_light_init(gput_light* l);
void gput_light_position(gput_light* l, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void gput_light_diffuse(gput_light* l, GLfloat r, GLfloat g, GLfloat b, GLfloat a);
void gput_light_ambient(gput_light* l, GLfloat r, GLfloat g, GLfloat b, GLfloat a);
void gput_light_bind(gput_light* l, GLenum light);

void gput_write_screen(const char* filename, int width, int height);

/** Texture **/
GPUT_BOOL gput_load_texture(GLenum target, const char* filename);
//void gput_load_texture(const char* filename);

/** Profile **/
void gput_profile_init(gput_profile* p);
void gput_profile_begin(gput_profile* p);
void gput_profile_end(gput_profile* p);
void gput_profile_print(gput_profile* p, const char* name);

#ifndef NO_NVCG
void gput_set_transferfp(CGprogram fp);
#endif
//CGprogram gput_create_program_from_file(const char* program_file, CGprofile profile, CGcontext context);
//void gput_cache_program(CGprogram prog, CGprofile prof);
//CGprogram gput_create_program(const char* src, CGprofile profile, CGcontext context);

//void gput_cubemap_create_from_file(gput_cubemap* map, const char* filename[6]);
GLuint gput_cubemap_create_from_file(const char* filename[6]);

/** Cubemap **/
void gput_cubemap_enable();
void gput_cubemap_disable();
void gput_cubemap_skybox();


/** Error **/
void check_gl_error();
#ifndef NO_NVCG
void check_cg_error();
#endif


/** OpenGL vertex **/
void gput_vert2i_tex1f(int vx, int vy, GLfloat tx);
void gput_vert2i_tex2f(int vx, int vy,
					   GLfloat tx, GLfloat ty);
void gput_vert2i_2tex2f(int vx, int vy,
						GLfloat x0, GLfloat y0, 
						GLfloat x1, GLfloat y1);
void gput_vert2i_3tex2f(int vx, int vy,
						GLfloat x0, GLfloat y0, 
						GLfloat x1, GLfloat y1,
						GLfloat x2, GLfloat y2);

void gput_set_orthorenderview(int x, int y, int view_width, int view_height);

/** Array list functions for P-buffer **/
void gput_intlist_create(gput_intlist* l);
void gput_intlist_pushpair(gput_intlist* l, GLint i0, GLint i1);
void gput_intlist_push(gput_intlist* l, GLint i);
void gput_intlist_destroy(gput_intlist* l);

void gput_context_get_current(gput_context* c);
void gput_context_activate(gput_context* c);

/** P-Buffer **/
void gput_pbuffer_bind(gput_pbuffer* buf);
void gput_pbuffer_cubemap_create(gput_pbuffer* buf, int width, int height);

void gput_pbuffer_create(gput_pbuffer* buf, GLenum target, int width, int height, ...);

void gput_pbuffer_activate(gput_pbuffer* buf);
//void gput_pbuffer_end(gput_pbuffer* buf);
void gput_pbuffer_float_set(gput_pbuffer* buf, int width, int height, const void* pixels);
void gput_dump_frame(const char* name, int n_components, int x0, int y0, int width, int height);
void gput_pbuffer_float_create(gput_pbuffer* buf, int n_components, int width, int height);

GLuint gput_create_float_buffer(GLint internalformat, GLsizei width, GLsizei height, GLenum format, void* pixels);
void gput_bind_float_buffer(GLuint tex);


// Inline 

#define GPUT_INLINE _inline

GPUT_INLINE GLfloat gput_pack_2uint(GLuint i0, GLuint i1)
{
	return (GLfloat)((i0 & 0xFFFF) | (i1 << 16));
}


#endif