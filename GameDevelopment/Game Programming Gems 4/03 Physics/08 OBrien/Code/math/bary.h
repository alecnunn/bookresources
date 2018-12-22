/*********************************************************************
 * bary.h
 * Authored by Kris Hauser 2002-2003
 *
 * Header for functions involving barycentric coordinates of tetrahedra.
 *
 * Copyright 2003, Regents of the University of California 
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



#ifndef BARYCENTRIC_MATH_H
#define BARYCENTRIC_MATH_H

#include "shared/primitives.h"

typedef vec3_t tetrahedron [4];
typedef float_t* tetrahedron_ptr [4];
typedef vec4_t bary_t;

float_t triangle_area(const vec3_t a, const vec3_t b, const vec3_t c);
float_t tet_volume(const vec3_t a, const vec3_t b, const vec3_t c, const vec3_t d);
float_t tet_volume(const tetrahedron);
float_t tet_volume(const tetrahedron_ptr);
float_t get_barycentric_coordinates(const vec3_t point, const tetrahedron e, bary_t bary);
float_t get_barycentric_coordinates(const vec3_t point, const tetrahedron_ptr e, bary_t bary);

void tet_centroid(const tetrahedron e, vec3_t centroid);
void tet_centroid(const tetrahedron_ptr e, vec3_t centroid);

#include "math/primitives.h"

//on the line L(t) = a+tb
//returns the point q s.t. q = L(t0) and |q-p| is minimized
void get_line_closest_point(const Vector3& a, const Vector3& b,
			    const Vector3& p,
			    Vector3& q,
			    double& t0);

//on the segment L(t) = a+tb, t in [0,1]
//returns the point q s.t. q = L(t0) and |q-p| is minimized
void get_segment_closest_point(const Vector3& a, const Vector3& b,
			       const Vector3& p,
			       Vector3& q,
			       double& t0);

//gets the closest point x on the triangle(a,b,c) to the point p
void get_triangle_closest_point(const Vector3& a, const Vector3& b, const Vector3& c,
				const Vector3& p,
				Vector3& x);

//gets the closest point x on the tetrahedron(a,b,c,d) to the point p
void get_tetrahedron_closest_point(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d,
				const Vector3& p,
				Vector3& x);



#endif

