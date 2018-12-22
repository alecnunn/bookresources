/*********************************************************************
 * raytriangle.cpp
 * Authored by Kris Hauser 2002-2003
 *
 * Ray-triangle intersection tests
 *
 * Copyright (c) 2003, Regents of the University of California
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


#include "raytriangle.h"

#define EPSILON 2e-6

//u,v are the coordinates in the triangle's frame, t is the dist
//non-culling
int ray_triangle_intersect(const vec3_t orig, const vec3_t dir,
                   const vec3_t vert0, const vec3_t vert1, const vec3_t vert2,
                   float_t *t, float_t *u, float_t *v)
{
   vec3_t edge1, edge2, tvec, pvec, qvec;
   float_t det,inv_det;

   /* find vectors for two edges sharing vert0 */
   vec3_sub(edge1, vert1, vert0);
   vec3_sub(edge2, vert2, vert0);

   /* begin calculating determinant - also used to calculate U parameter */
   vec3_cross(pvec, dir, edge2);

   /* if determinant is near zero, ray lies in plane of triangle */
   det = vec3_dot(edge1, pvec);

   if (det > -EPSILON && det < EPSILON)
     return 0;
   inv_det = 1.0 / det;

   /* calculate distance from vert0 to ray origin */
   vec3_sub(tvec, orig, vert0);

   /* calculate U parameter and test bounds */
   *u = vec3_dot(tvec, pvec) * inv_det;
   if (*u < 0.0 || *u > 1.0)
     return 0;

   /* prepare to test V parameter */
   vec3_cross(qvec, tvec, edge1);

   /* calculate V parameter and test bounds */
   *v = vec3_dot(dir, qvec) * inv_det;
   if (*v < 0.0 || *u + *v > 1.0)
     return 0;

   /* calculate t, ray intersects triangle */
   *t = vec3_dot(edge2, qvec) * inv_det;

   return 1;
}

//u,v are the coordinates in the triangle's frame, t is the dist
//does backface culling
int ray_triangle_intersect_cull(const vec3_t orig, const vec3_t dir,
                   const vec3_t vert0, const vec3_t vert1, const vec3_t vert2,
                   float_t *t, float_t *u, float_t *v)
{
   vec3_t edge1, edge2, tvec, pvec, qvec;
   float_t det,inv_det;

   /* find vectors for two edges sharing vert0 */
   vec3_sub(edge1, vert1, vert0);
   vec3_sub(edge2, vert2, vert0);

   /* begin calculating determinant - also used to calculate U parameter */
   vec3_cross(pvec, dir, edge2);

   /* if determinant is near zero, ray lies in plane of triangle */
   det = vec3_dot(edge1, pvec);

   if (det < EPSILON)
      return 0;

   /* calculate distance from vert0 to ray origin */
   vec3_sub(tvec, orig, vert0);

   /* calculate U parameter and test bounds */
   *u = vec3_dot(tvec, pvec);
   if (*u < 0.0 || *u > det)
      return 0;

   /* prepare to test V parameter */
   vec3_cross(qvec, tvec, edge1);

    /* calculate V parameter and test bounds */
   *v = vec3_dot(dir, qvec);
   if (*v < 0.0 || *u + *v > det)
      return 0;

   /* calculate t, scale parameters, ray intersects triangle */
   *t = vec3_dot(edge2, qvec);
   inv_det = 1.0 / det;
   *t *= inv_det;
   *u *= inv_det;
   *v *= inv_det;

   return 1;
}


void intersect_to_intersection(float_t t, float_t u, float_t v,
		const vec3_t vert0, const vec3_t vert1, const vec3_t vert2,
		vec3_t pos)
{
  vec3_t edge1, edge2;
  vec3_sub(edge1, vert1, vert0);
  vec3_sub(edge2, vert2, vert0);

  vec3_equal(pos, vert0);
  vec3_madd(pos, edge1, u);
  vec3_madd(pos, edge2, v); 
}
