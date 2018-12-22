/* Copyright (C) Dante Treglia II, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dante Treglia II, 2000"
 */
#include "object.h"
#include "light.h"

//---------------------------------------------------------------------------
// Contstructor 
//---------------------------------------------------------------------------
Object::Object() {
	position.x = 0;
	position.y = 0;
	position.z = 0;
	orientation = IdentityMatrix44();
	modelView = IdentityMatrix44();
	normalMtx = IdentityMatrix44();
	lightsOn = 0;
	ambient.r = 0;
	ambient.g = 0;
	ambient.b = 0;
	ambient.a = 0;
	material.r = 0;
	material.g = 0;
	material.b = 0;
	material.a = 0;
	numLights = 0;
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
Object::~Object() {
}

//---------------------------------------------------------------------------
// Draw
//---------------------------------------------------------------------------
void Object::Draw(matrix44 &viewMtx) {
	if (!position.isZero()) {
		matrix44 tempMtx = 
			TranslateMatrix44(position.x, position.y, position.z)
			* orientation;
		modelView = viewMtx * tempMtx;
	} 
	else {
		modelView = viewMtx * orientation;
	}
	glPushMatrix();
	LoadPositionMatrix();
	if (lightsOn) {
	 	LoadNormalMatrix();
	 	EnableLights();
	}
	DrawPrimitives();
	if (lightsOn) {
	 	DisableLights();
	}
	glPopMatrix();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void Object::LoadPositionMatrix() {
	glLoadMatrixf((float *)&modelView);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void Object::LoadNormalMatrix() {
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void Object::EnableLights() {
	float temp[4];
	float white[] = {1.0, 1.0, 1.0, 1.0};
	float black[] = {0.0, 0.0, 0.0, 1.0};
	float grey[] = {0.2F, 0.2F, 0.2F, 1.0};
	vector4 lightPos;
	Light * litP;

	glEnable(GL_LIGHTING);
	for (int i = 0; i < numLights; i++) {
		litP = ((Light *)lights[i]);
		glEnable(litP->lightID);
		lightPos = litP->position;
		lightPos.w = 1;
		lightPos = normalMtx * lightPos;
		glLightfv(litP->lightID, GL_POSITION, (float *)&lightPos);
		temp[0] = (float)litP->ambient.r / 255.0;
		temp[1] = (float)litP->ambient.g / 255.0;
		temp[2] = (float)litP->ambient.b / 255.0;
		temp[3] = (float)litP->ambient.a / 255.0;
		glLightfv(litP->lightID, GL_AMBIENT, temp);
		temp[0] = (float)litP->material.r / 255.0;
		temp[1] = (float)litP->material.g / 255.0;
		temp[2] = (float)litP->material.b / 255.0;
		temp[3] = (float)litP->material.a / 255.0;
		glLightfv(litP->lightID, GL_DIFFUSE, temp);
		glLightfv(litP->lightID, GL_SPECULAR, black);
		glLightf(litP->lightID, GL_LINEAR_ATTENUATION, 0.001F);
	}

	// Set Objects Material
	temp[0] = (float)ambient.r / 255.0;
	temp[1] = (float)ambient.g / 255.0;
	temp[2] = (float)ambient.b / 255.0;
	temp[3] = (float)ambient.a / 255.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, temp);
	temp[0] = (float)material.r / 255.0;
	temp[1] = (float)material.g / 255.0;
	temp[2] = (float)material.b / 255.0;
	temp[3] = (float)material.a / 255.0;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, temp);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white); 
	glMaterialf(GL_FRONT, GL_SHININESS, 120);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void Object::DisableLights() {
	glDisable(GL_LIGHTING);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void Object::AddLight(void * light) {
	assert(numLights <= 8 && "Object::To many lights!");
	lights[numLights++] = (Light *)light;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void Object::SetMaterial(COLOR ambientIn, COLOR materialIn) {
	ambient.r = ambientIn.r;
	ambient.g = ambientIn.g;
	ambient.b = ambientIn.b;
	ambient.a = ambientIn.a;
	material.r = materialIn.r;
	material.g = materialIn.g;
	material.b = materialIn.b;
	material.a = materialIn.a;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void Object::SetMaterial(float ambientIn, COLOR materialIn) {
	ambient.r = (char)(materialIn.r * ambientIn);
	ambient.g = (char)(materialIn.g * ambientIn);
	ambient.b = (char)(materialIn.b * ambientIn);
	ambient.a = (char)(materialIn.a * ambientIn);
	material.r = materialIn.r;
	material.g = materialIn.g;
	material.b = materialIn.b;
	material.a = materialIn.a;
}
