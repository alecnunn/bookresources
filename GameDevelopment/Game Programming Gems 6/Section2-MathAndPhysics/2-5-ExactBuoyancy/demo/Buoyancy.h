/*
 * Copyright (c) 2005 Erin Catto http://www.gphysics.com
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies.
 * Erin Catto makes no representations about the suitability 
 * of this software for any purpose.  
 * It is provided "as is" without express or implied warranty.
 */

#ifndef BUOYANCY_H
#define BUOYANCY_H

#include "MathUtils.h"

struct Face
{
	Face() {}
	Face(int i1, int i2, int i3) : i1(i1), i2(i2), i3(i3) {}
	int i1, i2, i3;
};

struct Polyhedron
{
	Vec3* verts;
	Face* faces;
	int numVerts;
	int numFaces;
	float length;
	float volume;
};

struct Plane
{
	Vec3 normal;
	float offset;
};

struct WaterVolume
{
	Plane plane;
	Vec3 velocity;
	float density;
	float linearDrag;
	float angularDrag;
};

// Compute the volume of the given polyhedron.
float ComputeVolume(Polyhedron& poly);

// Compute the buoyancy and drag forces.
void ComputeBuoyancy(RigidBody& body, Polyhedron& poly,
					 WaterVolume& water, float gravity);

#endif