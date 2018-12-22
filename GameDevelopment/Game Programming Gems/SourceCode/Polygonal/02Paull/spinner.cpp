/* Copyright (C) David Paull, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) David Paull, 2000"
 */
#include "glut.h"
#include "spinner.h"
#include "vectorcamera.h"


#pragma warning ( disable : 4514)//unreferenced inline function has been removed

// This is the texture used by the spinner
extern bool UseVectorCamera;
extern unsigned long gpgtexture[128*128];
tMesh Mesh;


// Init()
//
// Initialize the state of the spinner
//
void Spinner::Init() 
{
	// Initialize spinner position	
	mRot = 0.0F;
	mRotMatrix = RotateRadMatrix44('z', DegToRad(mRot));


	// Initialize spinner texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, (GLuint*)&mTextureID);
	if (mTextureID == 0) {
		GLenum gle = glGetError();
	}
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 
		0, GL_RGBA, GL_UNSIGNED_BYTE, gpgtexture);
	Mesh.pV = new vector4[MAX_VERT];
	Mesh.pT = new unsigned int[MAX_TRI*3];
	Mesh.pN = new vector4[MAX_TRI];
	Mesh.pD = new float[MAX_TRI];

	Mesh.pV[0].x = -50.0;
	Mesh.pV[0].y = -50.0;
	Mesh.pV[0].z = 300.0;

	Mesh.pV[1].x = -50.0;
	Mesh.pV[1].y =  50.0;
	Mesh.pV[1].z = 300.0;

	Mesh.pV[2].x =  50.0;
	Mesh.pV[2].y =  50.0;
	Mesh.pV[2].z = 300.0;

	Mesh.pV[3].x =  50.0;
	Mesh.pV[3].y = -50.0;
	Mesh.pV[3].z = 300.0;

	Mesh.pN[0].x = 0.0;
	Mesh.pN[0].y = 0.0;
	Mesh.pN[0].z = 1.0;

	Mesh.pN[1].x = 0.0;
	Mesh.pN[1].y = 0.0;
	Mesh.pN[1].z = 1.0;

	Mesh.pD[0] = DotProduct3(Mesh.pN[0],Mesh.pV[1]);
	Mesh.pD[1] = DotProduct3(Mesh.pN[1],Mesh.pV[2]);

	Mesh.pT[0] = 0;
	Mesh.pT[1] = 1;
	Mesh.pT[2] = 2;

	Mesh.pT[3] = 0;
	Mesh.pT[4] = 2;
	Mesh.pT[5] = 3;

	Mesh.cV = 4;
	Mesh.cT = 2;
	InitVectorCamera();

}


// Display()
//
// Draw the spinner using open GL.
// see spinnervc.cpp for the vector camera version.
//
void Spinner::Display() 
{
  if(UseVectorCamera)
  {
	matrix44 temp;
	matrix44 temp2;
	matrix44 mPreMatrix;
	temp  = RotateRadMatrix44('y', DegToRad(180));
	temp2 = RotateRadMatrix44('z', DegToRad(mRot));
	mRotMatrix = temp*temp2;
    vcDisplayMesh(Mesh,mRotMatrix,mTextureID);
  }
  else
  {
    // Set up the spinner rotation
    glPushMatrix();
    glMultMatrixf((const float*)&mRotMatrix[0][0]);

    // Draw the spinner

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    glBegin(GL_QUADS);

    glColor4ub(0xff, 0, 0, 0xff);
    glTexCoord2f(0.0F, 1.0F);
    glVertex3f(-50, -50, 300);

    glColor4ub(0xff, 0, 0xff, 0xff);
    glTexCoord2f(1.0F, 1.0F);
    glVertex3f(-50,  50, 300);

    glColor4ub(0, 0xff, 0, 0xff);
    glTexCoord2f(1.0F, 0.0F);
    glVertex3f( 50,  50, 300);

    glColor4ub(0, 0, 0xff, 0xff);
    glTexCoord2f(0.0F, 0.0F);
    glVertex3f( 50, -50, 300);

    glEnd();

    glDisable(GL_TEXTURE_2D);

    // Pop the rotation matrix from the MODELVIEW stack
    glPopMatrix();
  }
}	


// Update()
//
// Update the state of the spinner each frame.
//
void Spinner::Update() 
{
	// Update rotation
	mRot += SPINNER_ROT_DELTA;
	mRotMatrix = RotateRadMatrix44('z', DegToRad(mRot));
}

