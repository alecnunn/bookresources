/* Copyright (C) Jesse Laeuchli, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Jesse Laeuchli, 2001"
 */
#include<glut.h>
#include <gl\glaux.h>
#include<fstream.h>
#include<math.h>
GLuint	texturest;
#define csize 128
float ptexture[csize][csize][4];
float mf[csize][csize];

void readfractal(int howx,int howy,char *whichhieghtmap)
{
	fstream file;
	file.open(whichhieghtmap,ios::in);
	for(int y=0;y<howx;y++)
	for(int x=0;x<howy;x++)
	{
		float it=0;
		file>>it;
		mf[y][x]=it;
	}
}

void setupsky(bool day)
{
	for(int y=0;y<csize;y++)
	for(int x=0;x<csize;x++)
	{
		if(mf[y][x]>0)
		{
			ptexture[y][x][0]=(mf[y][x]);
			ptexture[y][x][1]=(mf[y][x]);
			ptexture[y][x][2]=(mf[y][x]);
			ptexture[y][x][3]=(mf[y][x]/10);
		}
		else
		{
			ptexture[y][x][3]=0;
		}
	}	
}


void setup()
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
	readfractal(csize,csize,"sky.raw");
	setupsky(true);
	glGenTextures(0,&texturest);
	glBindTexture(GL_TEXTURE_2D, texturest);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, csize, csize, 0, GL_RGBA, GL_FLOAT, ptexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering
}

void renderScene(void) {

	//
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	//glDisable(GL_DEPTH_TEST);
	glPushMatrix();
	glTranslatef(-1,-1,0);
	
	
	glBindTexture(GL_TEXTURE_2D, texturest);
	glBegin(GL_QUADS);
	//
	glColor3f(0,0,.7);
	//glColor4f(1,1,1,1);
	glTexCoord2f(0,0);
	glVertex3f(0,0,0);
	//
	glTexCoord2f(0,1);
	glVertex3f(0,2,0);
	//
	glTexCoord2f(1,1);
	glVertex3f(2,2,0);
	//
	glTexCoord2f(1,0);
	glVertex3f(2,0,0);		
	glEnd();
	glFlush();
	glPopMatrix();
}



void main() {

	glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(512,512);
	glutCreateWindow("Clouds");
	glutDisplayFunc(renderScene);
	setup();
	glutMainLoop();
}
