/*********************************************************************
 * viewport.h
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




#ifndef EDITOR_VIEWPORT_H
#define EDITOR_VIEWPORT_H

enum { VIEW_FRONT = 0, VIEW_BACK, VIEW_LEFT, VIEW_RIGHT, VIEW_TOP, VIEW_BOTTOM, VIEW_PERSPECTIVE };

#include <primitives.h>

struct editor_viewport
{
	editor_viewport();
	void set_view(int);
	void set_mode();

	int view;
	bool show_grid;
	bool smooth;
	bool wire;

	vec3_t position;
	float scale;
	float pitch,yaw;

	matrix4_t mat;

	int x,y,w,h;
	float n, f;
	float dist;

	bool clicked(int mx, int my) const;
	void zoom(float s);
	void scroll(float x, float y);
	void truck(float z);
	void push( float z);
	void refresh_matrix();

	void get_view_vector(vec3_t) const;
	void get_movement_vector(float x, float y, vec3_t) const;
	float get_depth(int x, int y) const;
	
	void get_click_vector(int mx, int my, vec3_t) const;
	void get_click_source(int mx, int my, vec3_t) const;
};


#endif

