//************************************************************************* //
// 3D Environment class inherited from 3D Movable Object class for OpenGL scenes
//
// Authors: Aszódi, Barnabás
//          Czuczor, Szabolcs
// Created: July, 2004., Budapest, Hungary
//
// Budapest University of Technology and Economics,
// Department of Control Engineering and Information Technology (BME-IIT)
//************************************************************************* //
#include "object.h"

// This class defines the stationary surroundings, however it is inherited
// from the MovableObject class to have the drawInScene() function.
//===============================================================
class Environment : public MovableObject {
//===============================================================
private:
	void draw() {
		glPushMatrix();
			glLoadIdentity();
			for (int i = 0; i < 3; i++)	{
				glTranslatef(-1.0f + 3 * i, 1.0, 0.0f + 3 * i);
				glBegin(GL_QUADS);					
					glColor3f(0.0f,1.0f,0.0f);		
					glVertex3f( 1.0f, 1.0f,-1.0f);	
					glVertex3f(-1.0f, 1.0f,-1.0f);	
					glVertex3f(-1.0f, 1.0f, 1.0f);	
					glVertex3f( 1.0f, 1.0f, 1.0f);	
					glColor3f(1.0f,0.5f,0.0f);		
					glVertex3f( 1.0f,-1.0f, 1.0f);	
					glVertex3f(-1.0f,-1.0f, 1.0f);	
					glVertex3f(-1.0f,-1.0f,-1.0f);	
					glVertex3f( 1.0f,-1.0f,-1.0f);	
					glColor3f(1.0f,0.0f,0.0f);		
					glVertex3f( 1.0f, 1.0f, 1.0f);	
					glVertex3f(-1.0f, 1.0f, 1.0f);	
					glVertex3f(-1.0f,-1.0f, 1.0f);	
					glVertex3f( 1.0f,-1.0f, 1.0f);	
					glColor3f(1.0f,1.0f,0.0f);			
					glVertex3f( 1.0f,-1.0f,-1.0f);		
					glVertex3f(-1.0f,-1.0f,-1.0f);		
					glVertex3f(-1.0f, 1.0f,-1.0f);		
					glVertex3f( 1.0f, 1.0f,-1.0f);		
					glColor3f(0.0f,0.0f,1.0f);			
					glVertex3f(-1.0f, 1.0f, 1.0f);		
					glVertex3f(-1.0f, 1.0f,-1.0f);		
					glVertex3f(-1.0f,-1.0f,-1.0f);		
					glVertex3f(-1.0f,-1.0f, 1.0f);		
					glColor3f(1.0f,0.0f,1.0f);			
					glVertex3f( 1.0f, 1.0f,-1.0f);		
					glVertex3f( 1.0f, 1.0f, 1.0f);		
					glVertex3f( 1.0f,-1.0f, 1.0f);		
					glVertex3f( 1.0f,-1.0f,-1.0f);		
				glEnd();	
			}
		glPopMatrix();
		glPushMatrix();
			glLoadIdentity();
			glTranslatef(1.5f,0.0f,-7.0f);				

			glColor3f(1.0f,1.0f,1.0f);
			glBegin(GL_QUADS);
			for (float z = -20.0; z < 20.0; z += 5)
				for (float x = -20.0; x < 20.0; x += 5) {
						glTexCoord2f(0.0, 0.0);
						glVertex3f(x, 0.0, z);
						glTexCoord2f(0.0, 1.0);
						glVertex3f(x, 0.0, z + 5);
						glTexCoord2f(1.0, 1.0);
						glVertex3f(x + 5, 0.0, z + 5);
						glTexCoord2f(1.0, 0.0);
						glVertex3f(x + 5, 0.0, z);
				}
			glEnd();
		glPopMatrix();
	}


public:
	Environment(float posX = 0.0f, float posY = 0.0f, float posZ = 0.0f) : MovableObject(posX, posY, posZ) {
	}
};