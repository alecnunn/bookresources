/*********************************************************************
 * viewport.cpp
 * Authored by Kris Hauser 2002-2003
 *
 * Defines a viewport class.
 *
 * Copyright (c) 2002 SquireSoft, used with permission.
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




#include "viewport.h"
#include <stdio.h>
#include <GL/gl.h>



void GetCameraXBasisVector(const matrix4_t m, vec3_t v)
{
	v[0] = m[0][0];
	v[1] = m[1][0];
	v[2] = m[2][0];
}

void GetCameraYBasisVector(const matrix4_t m, vec3_t v)
{
	v[0] = m[0][1];
	v[1] = m[1][1];
	v[2] = m[2][1];
}

void GetCameraZBasisVector(const matrix4_t m, vec3_t v)
{
	v[0] = m[0][2];
	v[1] = m[1][2];
	v[2] = m[2][2];
}

//sets the rotation matrix that will rotate around the x axis r radians
void matrix4_x_rotation(matrix4_t m, float_t r)
{
	float_t c, s;
	c = cos(r);
	s = sin(r);
	matrix4_identity(m);
	m[1][1] = m[2][2] = c;
	m[1][2] = s;
	m[2][1] = -s;
}

//sets the rotation matrix that will rotate around the y axis xr radians
void matrix4_y_rotation(matrix4_t m, float_t r)
{
	float_t c, s;
	c = cos(r);
	s = sin(r);
	matrix4_identity(m);
	m[2][2] = m[0][0] = c;
	m[2][0] = s;
	m[0][2] = -s;
}

//sets the rotation matrix that will rotate around the z axis r radians
void matrix4_z_rotation(matrix4_t m, float_t r)
{
	float_t c, s;
	c = cos(r);
	s = sin(r);
	matrix4_identity(m);
	m[0][0] = m[1][1] = c;
	m[0][1] = s;
	m[1][0] = -s;
}

editor_viewport::editor_viewport()
  :show_grid(false), smooth(true), wire(false),
   scale(1),pitch(0),yaw(0),
   x(0),y(0),w(320),h(240), n(20), f(2000), dist(300)

{
	view = VIEW_PERSPECTIVE;
	set_view(VIEW_PERSPECTIVE);
}

void editor_viewport::set_view(int v)
//right is y->x z->y -x->z
//left is -y->x z->y x->z
//top is x->x y->y z->z
//bottom is x->x -y->y -z->z
//front is x->x z->y y->z
//back is -x->x z->y -y->z
{
	if(view == v)
	{
		scale = 1;
		vec3_zero(position);
	}
	view = v;
	switch(view)
	{
	case VIEW_RIGHT:
		yaw = -90;
		pitch = -90;
		break;
	case VIEW_LEFT:
		yaw = 90;
		pitch = -90;
		break;
	case VIEW_TOP:
		yaw = 0;
		pitch = 0;
		break;
	case VIEW_BOTTOM:
		yaw = 0;
		pitch = 180;
		break;
	case VIEW_FRONT:
		yaw = 0;
		pitch = -90;
		break;
	case VIEW_BACK:
		yaw = 180;
		pitch = -90;
		break;
	case VIEW_PERSPECTIVE:
		yaw = 0;
		pitch = 0;
		break;
	}
}


bool editor_viewport::clicked(int mx, int my) const 
{
	return (x<=mx && mx<=x+w && y<=my && my<=y+h);
}

void editor_viewport::zoom(float s)
{
	scale += s;
	if(scale < 0)
		scale = 0;
}

void editor_viewport::scroll(float x, float y)
{
	vec3_t v;
	get_movement_vector(x,y, v);
	vec3_multiply(v, v, scale);
	vec3_sub(position, position, v);
}

void editor_viewport::truck(float z)
{
	vec3_t v;
	get_view_vector(v);
	vec3_multiply(v,v,z*scale);
	vec3_sub(position, position, v);
}

void editor_viewport::push(float z)
{
  dist += z;
}

void editor_viewport::get_movement_vector(float x, float y, vec3_t v) const 
{
	vec3_t xb, yb;
	GetCameraXBasisVector(mat, xb);
	GetCameraYBasisVector(mat, yb);
	vec3_multiply(xb, xb, x/scale);
	vec3_multiply(yb, yb, y/scale);
	vec3_add(v, xb, yb);
}
/*
float editor_viewport::get_depth(int mx, int my) const
{
	float val;
	glReadPixels(mx,my, 1,1, GL_DEPTH_COMPONENT, GL_FLOAT, &val);

	//do the inverse of the perspective transform:
	//z -> zbuffer  = -b/z + a
	//b = n*f / (f-n)
	//a = f / (f-n)
	//do the inverse of that transformation
	if(view == VIEW_PERSPECTIVE)
	{
		val = (n*f) / (f - val * (f-n)) / scale;
	}
	else
	//the non-perspective transform is:
	//z -> zbuffer = (z-n)/(f-n)
	{
		val = (f-n)*val + n;
	}
	return val;
}*/


void editor_viewport::get_view_vector(vec3_t v) const 
{
	GetCameraZBasisVector(mat, v);
}

#define D_TO_R	0.0174532925f
#define R_TO_D	57.29577951f

inline float dtor(float f)
{
	return f*D_TO_R;
}
inline float rtod(float f)
{
	return f*R_TO_D;
}

void editor_viewport::refresh_matrix()
{
	matrix4_t yawrot, pitchrot, trans, zoom, flip;
	matrix4_identity(flip);
	matrix4_identity(trans);
	matrix4_identity(zoom);
	matrix4_identity(yawrot);
	matrix4_identity(pitchrot);
	vec3_multiply(trans[3], position, -1.0f);
	vec3_make(zoom[3], 0.f, 0.f, -dist);
	matrix4_z_rotation(yawrot, dtor(yaw));
	matrix4_x_rotation(pitchrot, dtor(pitch));

	matrix4_multiply(mat, pitchrot, yawrot);
	matrix4_multiply(mat, mat, trans);
	matrix4_multiply(mat, zoom, mat);
	matrix4_multiply(mat, flip, mat);
}

/*
void print_matrix(const matrix4_t& mat)
{
	printf("%f %f %f %f\n", mat._11, mat._12, mat._13, mat._14);
	printf("%f %f %f %f\n", mat._21, mat._22, mat._23, mat._24);
	printf("%f %f %f %f\n", mat._31, mat._32, mat._33, mat._34);
	printf("%f %f %f %f\n", mat._41, mat._42, mat._43, mat._44);
	printf("\n");
}*/


void print_matrix(const matrix4_t& mat)
{
	printf("%f %f %f %f\n", mat[0][0], mat[0][1], mat[0][2], mat[0][3]);
	printf("%f %f %f %f\n", mat[1][0], mat[1][1], mat[1][2], mat[1][3]);
	printf("%f %f %f %f\n", mat[2][0], mat[2][1], mat[2][2], mat[2][3]);
	printf("%f %f %f %f\n", mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
	printf("\n");
}

void editor_viewport::set_mode()
{
	refresh_matrix();
	/*	print_matrix(mat);
	printf("%d %d %d %d\n", x,y,w,h);
	printf("scale %f\n", scale);
	printf("dist %f\n", dist);
	*/
	glViewport(x,y,w,h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(view == VIEW_PERSPECTIVE)
	{
		float aspect = (float)w/h;
		//aspect = 1;
		float width = n*0.5/scale;
		float height = n*0.5/aspect/scale;
		glFrustum(-width, width, -height, height, n, f);
	}
	else
	{
		float width = (float)(w/2)/scale;
		float height = (float)(h/2)/scale;
		glOrtho(-width, width, -height, height, n, f);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0,0,-dist);
	glRotatef(pitch, 1,0,0);
	glRotatef(yaw, 0,0,1);
	glTranslatef(-position[0], -position[1], -position[2]);

	//glLoadMatrixf(&mat[0][0]);


	if(smooth)
		glShadeModel(GL_SMOOTH);
	else
		glShadeModel(GL_FLAT);

	if(wire)
		glPolygonMode(GL_FRONT, GL_LINE);
	else
		glPolygonMode(GL_FRONT, GL_FILL);
}




void editor_viewport::get_click_source(int mx, int my, vec3_t v) const 
{
	vec3_t vv;
	get_view_vector(vv);
	vec3_multiply(vv, vv, dist);

	if(view == VIEW_PERSPECTIVE)
	{
		vec3_add(v, position, vv);
	}
	else
	{
		mx = mx - x - w/2;
		my = my - y - h/2;
		vec3_t xb, yb;
		GetCameraXBasisVector(mat, xb);
		GetCameraYBasisVector(mat, yb);
		vec3_multiply(xb, xb, mx/scale);
		vec3_multiply(yb, yb, my/scale);

		vec3_add(v, position, vv);
		vec3_add(v, v, xb);
		vec3_add(v, v, yb);
	}
}

void editor_viewport::get_click_vector(int mx, int my, vec3_t v) const
{
	//world vector is
	//x*screen's x basis +
	//y*screen's y basis -
	//(screen width / 2 / tan lens angle) * screen's z basis
//	print_matrix(m);
	vec3_t vv;
	get_view_vector(vv);
	vec3_multiply(v,vv,-1.0f);
	if(view == VIEW_PERSPECTIVE)
	{
		int cx, cy;
		cx = x+w/2;
		cy = y+h/2;

		mx = mx - cx;
		my = my - cy;

		vec3_t xb, yb;
		GetCameraXBasisVector(mat, xb);
		GetCameraYBasisVector(mat, yb);
		vec3_multiply(xb, xb, (float)mx/w/scale);
		vec3_multiply(yb, yb, (float)my/w/scale);

		vec3_add(v, v, xb);
		vec3_add(v, v, yb);
	}
}
