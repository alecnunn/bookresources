/*********************************************************************
 * geometry.cpp
 * Authored by Kris Hauser 2002-2003
 *
 * Simple geometry objects.
 *
 * Copyright (c) 2002 SquireSoft, used and modified with permission.
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



#include "geometry.h"



#define AABB_RANGE_MAX FLT_MAX
#define AABB_RANGE_MIN -FLT_MAX
//#define AABB_RANGE_MAX 1000
//#define AABB_RANGE_MIN -1000

AABB::AABB()
{}

AABB::AABB(const Vector3& a, const Vector3& b)
:bmin(a), bmax(b)
{}

bool AABB::contains(const Vector3& v) const
{
	return (v.x>=bmin.x && v.x<=bmax.x &&
		v.y>=bmin.y && v.y<=bmax.y &&
		v.z>=bmin.z && v.z<=bmax.z);
}

bool AABB::intersects(const AABB& a) const
{
	return (bmin.x <= a.bmax.x && bmax.x >= a.bmin.x &&
		bmin.y <= a.bmax.y && bmax.y >= a.bmin.y &&
		bmin.z <= a.bmax.z && bmax.z >= a.bmin.z);
}

void AABB::minimize()
{
	bmin.x = bmin.y = bmin.z = AABB_RANGE_MAX;
	bmax.x = bmax.y = bmax.z = AABB_RANGE_MIN;
}

void AABB::maximize()
{
	bmin.x = bmin.y = bmin.z = AABB_RANGE_MIN;
	bmax.x = bmax.y = bmax.z = AABB_RANGE_MAX;
}

void AABB::set_intersect(const AABB& a, const AABB& b)
{
	operator = (a);
	GetMaxVector(b.bmin, bmin);
	GetMinVector(b.bmax, bmax);
}

void AABB::set_expand(const AABB& a, const AABB& b)
{
	operator = (a);
	GetMinVector(b.bmin, bmin);
	GetMaxVector(b.bmax, bmax);
}


void AABB::intersect(const AABB& b)
{
	GetMaxVector(b.bmin, bmin);
	GetMinVector(b.bmax, bmax);
}

void AABB::expand(const AABB& b)
{
	GetMinVector(b.bmin, bmin);
	GetMaxVector(b.bmax, bmax);
}

const AABB& AABB::operator = (const AABB& other)
{
	bmin = other.bmin;
	bmax = other.bmax;
	return *this;
}

#include <float.h>
#define min(a,b) (a < b ? a : b)
#define max(a,b) (a > b ? a : b)
#define opposing_signs(a,b) ((a<0) != (b<0))



//forms a plane from a point on the plane and two basis Vector3s
void Plane_FromPointBases(const Vector3& a, const Vector3& b1, const Vector3& b2, Plane& p)
{
	p.normal.setCross(b1,b2);
	normalize(p.normal);
	p.offset = dot(a, p.normal);
}

//forms a plane from three points on the plane
void Plane_FromPoints(const Vector3& a, const Vector3& b, const Vector3& c, Plane& p)
{
	Plane_FromPointBases(a, b-a, c-a, p);
}

//returns the orthogonal distance from the plane to the Vector3
float PlaneOffset(const Vector3& v, const Plane& p)
{
	return dot(v,p.normal) - p.offset;
}

//projects a Vector3 onto this plane
void PlaneProject(const Vector3& vin, const Plane& p, Vector3& vout)
{
	vout = vin - p.normal*(dot(vin,p.normal) - p.offset);
}

//transforms a plane
void PlaneTransform(const Plane& pin, const Matrix4& xform, Plane& pout)
{
	xform.mulVector(pin.normal, pout.normal);

	Vector3 v, v_out;
	v = pin.normal*pin.offset;
	xform.mulPoint(v, v_out);
	pout.offset = dot(v_out, pout.normal);
}

