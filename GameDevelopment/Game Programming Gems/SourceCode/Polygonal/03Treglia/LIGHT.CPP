/* Copyright (C) Dante Treglia II, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dante Treglia II, 2000"
 */
#include "light.h"

//---------------------------------------------------------------------------
// Contstructor 
//---------------------------------------------------------------------------
Light::Light() {
	COLOR white = {255, 255, 255, 255};

	Initialize(white, white, GL_LIGHT0);
}

//---------------------------------------------------------------------------
// Contstructor 
//---------------------------------------------------------------------------
Light::Light(COLOR ambIn, COLOR matIn, int lightIDIn) {
	Initialize(ambIn, matIn, lightIDIn);
}

//---------------------------------------------------------------------------
// Contstructor 
//---------------------------------------------------------------------------
void Light::Initialize(COLOR ambIn, COLOR matIn, int lightIDIn) {
	ambient.r = ambIn.r;
	ambient.g = ambIn.g;
	ambient.b = ambIn.b;
	ambient.a = ambIn.a;
	material.r = matIn.r;
	material.g = matIn.g;
	material.b = matIn.b;
	material.a = matIn.a;
	lightID = lightIDIn;
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
Light::~Light() {
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void Light::DrawPrimitives() {
	glPointSize(4.0);
	glBegin(GL_POINTS);
		glColor4ub(material.r, material.g, material.b, material.a);
		glVertex3f(0.0, 0.0, 0.0);
	glEnd();
	glPointSize(1.0);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void Light::Load() {
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void Light::SetDirection(vector3 &dirIn) {
	direction = dirIn;
	direction.normalize();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void Light::SetPosition(vector3 &posIn) {
	position = posIn;
}
