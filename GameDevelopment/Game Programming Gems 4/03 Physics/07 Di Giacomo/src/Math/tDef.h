// **********************************************************************
// * FILE  : tDef.h
// *---------------------------------------------------------------------
// * PURPOSE : typedef, globals...
// *
// *---------------------------------------------------------------------
// * AUTHOR   : Thomas Di Giacomo . MIRALab (giacomo@miralab.unige.ch)
// *
// *---------------------------------------------------------------------
// * COPYRIGHT :
/*
 * This copyright notice must be included in all copies or derivative works.
 * (c) Copyright 2003, MIRALab, C.U.I. University of Geneva 
 * ALL RIGHTS RESERVED
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that
 * the name of MIRALab not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL MIRALAB
 * BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT MIRALAB  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef __TDEF_H__
#define __TDEF_H__

#include <vector>
#include <math.h>

// ****************************************
// * Typedef
// ****************************************

struct tVect
{
	float x, y, z;
};

struct tCollisionPlane
{
	tVect		normal;
  float		d;
};

// ****************************************
// * Variables
// ****************************************

const	float	worldSizeX = 2.6f;
const	float	worldSizeY = 2.6f;
const	float	worldSizeZ = 2.6f;

extern tCollisionPlane collisionPlane[];

// ****************************************
// * Functions
// ****************************************


static double DotVector(const tVect* v1, const tVect* v2)
{
	return ((v1->x * v2->x) + (v1->y * v2->y) + (v1->z * v2->z));
}

static void SubVector(const tVect* v1, const tVect* v2, tVect* result) 
{
	result->x = v1->x - v2->x;
	result->y = v1->y - v2->y;
	result->z = v1->z - v2->z;
}

static void SumVector(const tVect* v1, const tVect* v2, tVect* result) 
{
	result->x = v1->x + v2->x;
	result->y = v1->y + v2->y;
	result->z = v1->z + v2->z;
}

static void ScaleVector(const tVect* v, const float scale, tVect* result) 
{
	result->x = v->x * scale;
	result->y = v->y * scale;
	result->z = v->z * scale;
}

static void NullVector(tVect* v)
{
    v->x = 0.f;
    v->y = 0.f;
    v->z = 0.f;
}

static void SetVector(tVect* v, const float x, const float y, const float z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

static void CopyVector(const tVect* vs, tVect* vd)
{
    vd->x = vs->x;
    vd->y = vs->y;
    vd->z = vs->z;	
}

static void CrossVector(const tVect* v1, const tVect* v2, tVect* cross)
{
    cross->x = (v1->y * v2->z) - (v1->z * v2->y);
    cross->y = (v1->z * v2->x) - (v1->x * v2->z);
    cross->z = (v1->x * v2->y) - (v1->y * v2->x);
}

static float LengthVector(const tVect* v)
{
    return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
}

static void NormalVector(tVect* v)
{
    ScaleVector(v, 1.f/LengthVector(v), v);
}

static bool IsNullVector(const tVect* v)
{
	return((v->x==0) && (v->x==0) && (v->x==0));
}

static void ComputeOrtho(const tVect* norm, tVect* p1, tVect* p2, tVect* p3, tVect* p4)
{
	// find a first vector non-colinear with the normal
	if (norm->x!=0)
		SetVector(p1, -norm->y, norm->x, norm->z);
	else if (norm->y!=0)
		SetVector(p1, norm->y, -norm->x, norm->z);
	else if (norm->z!=0)
		SetVector(p1, norm->z, norm->x, -norm->y);

	// compute the three remaining vectors in the ortho plane
	CrossVector(norm, p1, p2);
	SetVector(p3, -p1->x, -p1->y, -p1->z);
	SetVector(p4, -p2->x, -p2->y, -p2->z);

	// base ortho a vecteur nul
	if (IsNullVector(norm))
	{
		SetVector(p1, -1.f, 0.f, 0.f);
		SetVector(p2, 0.f, 0.f, -1.f);
		SetVector(p3, 1.f, 0.f, 0.f);
		SetVector(p4, 0.f, 0.f, 1.0f);
	}

	NormalVector(p1);
	NormalVector(p2);
	NormalVector(p3);
	NormalVector(p4);
}

// Misc
static void InitWorldContainer()
{
	SetVector(&collisionPlane[0].normal, 0.f, -1.f, 0.f);
	collisionPlane[0].d = worldSizeY / 2.f;

	SetVector(&collisionPlane[1].normal, 0.f, 1.f, 0.f);
	collisionPlane[1].d = worldSizeY / 2.f;

	SetVector(&collisionPlane[2].normal, -1.f, 0.f, 0.f);
	collisionPlane[2].d = worldSizeX / 2.f;
	
	SetVector(&collisionPlane[3].normal, 1.f, 0.f, 0.f);
	collisionPlane[3].d = worldSizeX / 2.f;
	
	SetVector(&collisionPlane[4].normal, 0.f, 0.f, -1.f);
	collisionPlane[4].d = worldSizeZ / 2.f;
	
	SetVector(&collisionPlane[5].normal, 0.f, 0.f, 1.f);
	collisionPlane[5].d = worldSizeZ / 2.f;
}

#endif // __TDEF_H__