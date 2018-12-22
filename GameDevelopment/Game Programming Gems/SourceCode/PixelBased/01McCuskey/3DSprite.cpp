/* Copyright (C) Mason McCuskey, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Mason McCuskey, 2000"
 */
// 3DSprite.cpp: implementation of the C3DSprite class.
//
//////////////////////////////////////////////////////////////////////
#include <GL/glut.h>
#include "3DSprite.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C3DSprite::C3DSprite()
{

}

C3DSprite::~C3DSprite()
{

}


// This is the texture used by the sprite
extern unsigned long gpgtexture[128*128];

// Init()
//
// Initialize the state of the sprite
//
void C3DSprite::Init() {
  // Initialize rotation	
	SetRotation(0);

	// Initialize sprite texture
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

  m_iWidth = m_iHeight = 100;
  m_iX = m_iY = 0;
}


// Display()
//
// Draw the sprite
//
void C3DSprite::Display() {
  // Set up the rotation and translation matrices
  glPushMatrix();
  glTranslatef(m_iX,m_iY,0);
  glRotatef(m_fRotation, 0, 0, 1);
  
  // Draw the sprite

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, mTextureID);

  glBegin(GL_QUADS);

  glColor4ub(0xff, 0xff, 0xff, m_iAlpha);
  glTexCoord2f(0.0F, 0.0F);
  glVertex3f(-m_iWidth/2, -m_iHeight/2, 0);

  glColor4ub(0xff, 0xff, 0xff, m_iAlpha);
  glTexCoord2f(1.0F, 0.0F);
  glVertex3f(-m_iWidth/2, m_iHeight/2, 0);

  glColor4ub(0xff, 0xff, 0xff, m_iAlpha);
  glTexCoord2f(1.0F, 1.0F);
  glVertex3f(m_iWidth/2, m_iHeight/2, 0);

  glColor4ub(0xff, 0xff, 0xff, m_iAlpha);
  glTexCoord2f(0.0F, 1.0F);
  glVertex3f(m_iWidth/2, -m_iHeight/2, 0);
  
  glEnd();
  

  glDisable(GL_TEXTURE_2D);

  // Pop the matrix we set up above
  glPopMatrix();
  
}	
