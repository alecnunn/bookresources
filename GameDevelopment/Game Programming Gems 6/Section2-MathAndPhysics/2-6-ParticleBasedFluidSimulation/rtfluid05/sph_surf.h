#ifndef _VOLUME_RENDERING_H
#define _VOLUME_RENDERING_H

#include "gpu_util.h"

typedef struct
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
} vector3f;

typedef struct
{
    vector3f p;
    vector3f n;
    GLfloat val;
} cellvertex;

typedef struct
{
//    cellvertex v[8];
    cellvertex* v[8];
	cellvertex vmem[8];
} gridcell;

typedef struct
{
	GLuint width;
	GLuint height;
	GLuint depth;
	GLfloat voxelsize;
	GLfloat* density;
	gridcell g;
} implicit_surface;

void implicit_surface_create(implicit_surface* s);
//void render_volume(GLfloat* density, GLfloat stride, GLuint width, GLuint height, GLuint depth, float threshold);
void render_marching_cubes(const implicit_surface* v,
						   int x0, int x1,
						   int y0, int y1,
						   int z0, int z1,
						   float threshold);
#endif