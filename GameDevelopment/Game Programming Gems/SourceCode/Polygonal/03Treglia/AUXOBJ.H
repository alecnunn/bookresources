/* Copyright (C) Dante Treglia II, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dante Treglia II, 2000"
 */
#ifndef _AUXOBJ_H
#define _AUXOBJ_H
#include "def.h"
#include <cstdlib>
#include <gl/glaux.h>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class Sphere : public Object {
public:
	Sphere(double r): radius(r) {};
	void DrawPrimitives() {auxSolidSphere(radius);}

public:
	double radius;
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class Box : public Object {
public:
	Box(double w, double h, double d): width(w), height(h), depth(d) {};
	void DrawPrimitives() {auxSolidBox(width, height, depth);}

public:
	double width;
	double height;
	double depth;
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class Torus : public Object {
public:
	Torus(double ir, double or): innerRadius(ir), outerRadius(or) {};
	void DrawPrimitives() {auxSolidTorus(innerRadius, outerRadius);}

public:
	double innerRadius;
	double outerRadius;
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class Cylinder : public Object {
public:
	Cylinder(double r, double h): radius(r), height(h) {};
	void DrawPrimitives() {auxSolidCylinder(radius, height);}

public:
	double radius;
	double height;
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class Cone : public Object {
public:
	Cone(double r, double h): radius(r), height(h) {};
	void DrawPrimitives() {auxSolidCone(radius, height);}

public:
	double radius;
	double height;
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class Icosahedron : public Object {
public:
	Icosahedron(double r): radius(r) {};
	void DrawPrimitives() {auxSolidIcosahedron(radius);}

public:
	double radius;
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class Tetrahedron : public Object {
public:
	Tetrahedron(double r): radius(r) {};
	void DrawPrimitives() {auxSolidTetrahedron(radius);}

public:
	double radius;
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class Octahedron : public Object {
public:
	Octahedron(double r): radius(r) {};
	void DrawPrimitives() {auxSolidOctahedron(radius);}

public:
	double radius;
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class Docecahedron : public Object {
public:
	Docecahedron(double r): radius(r) {};
	void DrawPrimitives() {auxSolidDodecahedron(radius);}

public:
	double radius;
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class Teapot : public Object {
public:
	Teapot(double s): size(s) {};
	void DrawPrimitives() {auxSolidTeapot(size);}

public:
	double size;
};

#endif
