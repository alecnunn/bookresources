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
#include<fstream.h>
#include<math.h>
#define SIZEOF_LAND 64
struct vertex{
	double x,y,z,u,v;
};
float recallangle;
double eyeVX;
double eyeVZ;
float g_ViewAngle;
vertex lookDir;
vertex eye;
float heightmap[SIZEOF_LAND][SIZEOF_LAND];
vertex landscape[(SIZEOF_LAND*SIZEOF_LAND*2)*3];//memory allocated for landscape....
GLuint	texturest;
float ptexture[64][64][3];
void readfractal(int howx,int howy,char *whichhieghtmap)
{
	fstream file;
	file.open(whichhieghtmap,ios::in);
	for(int y=0;y<howx;y++)
		for(int x=0;x<howy;x++)
		{
			float it=0;
			file>>it;
			heightmap[y][x]=it;
		}
}
void landcolor(float color)
{
	if(color>4)
	{
		glColor3f(1,1,1);
	}
	else
		if(color<-1)
		{
			glColor3f(0,.3,0);
		}
		else
		{
			glColor3f(0,color/10+.2,0);
		}
}
void createground()
{
	for(int y=0;y<64;y++)
		for(int x=0;x<64;x++)
		{
			if(heightmap[y][x]<0)
			{
				ptexture[y][x][0]=.5;
				ptexture[y][x][1]=.5;
				ptexture[y][x][2]=.5;
			}
			else
			{
				ptexture[y][x][0]=heightmap[y][x];
				ptexture[y][x][1]=heightmap[y][x];
				ptexture[y][x][2]=heightmap[y][x];
			}
		}
}
void setuptexture(char *whichmap)
{	
	readfractal(64,64,whichmap);
	createground();
	glGenTextures(0,&texturest);
	glBindTexture(GL_TEXTURE_2D, texturest);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 64, 64, 0, GL_RGB,GL_FLOAT, ptexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering
}
void loadlandscape(int howbigx,int howbigy,char *whichhieghtmap)
{
	readfractal(howbigx,howbigy,whichhieghtmap);
	vertex test[6]={0,1,1,0,0, 1,0,1,0,0, 0,0,1,0,0, 1,1,1,0,0, 1,0,1,0,0, 0,1,1,0,0 };//Vertex that the flat plane will be built out of
	int starti=0;
	float addto=1;
	float addtoy=0;
	int heightmapix=0;
	int heightmapiy=0;
	//
	
	for(int i2=0;i2<SIZEOF_LAND*SIZEOF_LAND;i2++)
	{
		landscape[starti].x=test[0].x+addto;
		landscape[starti].y=test[0].y+addtoy;
		landscape[starti].z=heightmap[heightmapix][heightmapiy+1];//Create a the plane, but make in the hieghts.
		///
		starti++;
		landscape[starti].x=test[1].x+addto;
		landscape[starti].y=test[1].y+addtoy;
		landscape[starti].z=heightmap[heightmapix+1][heightmapiy];
		///
		starti++;
		landscape[starti].x=test[2].x+addto;
		landscape[starti].y=test[2].y+addtoy;
		landscape[starti].z=heightmap[heightmapix][heightmapiy];
		///
		starti++;
		landscape[starti].x=test[3].x+addto;
		landscape[starti].y=test[3].y+addtoy;
		landscape[starti].z=heightmap[heightmapix+1][heightmapiy+1];
		///
		starti++;
		landscape[starti].x=test[4].x+addto;
		landscape[starti].y=test[4].y+addtoy;
		landscape[starti].z=heightmap[heightmapix+1][heightmapiy];
		///
		starti++;
		landscape[starti].x=test[5].x+addto;
		landscape[starti].y=test[5].y+addtoy;
		landscape[starti].z=heightmap[heightmapix][heightmapiy+1];
		///
		starti++;
		addto+=1;
		heightmapix++;
		if(howbigx==addto)
		{
			addtoy+=1;
			heightmapix=0;
			heightmapiy++;
			addto=1;
		}
	}
	int q=0;
	glNewList(1,GL_COMPILE);
	glBegin(GL_TRIANGLES);
	for(int i=0;i<SIZEOF_LAND*SIZEOF_LAND*2;i++)
	{
		landcolor(landscape[q].z);
		glTexCoord2f(landscape[q].x,landscape[q].y);
		glVertex3f(landscape[q].x/10,landscape[q].y/10,landscape[q].z/10);
		q++;
		landcolor(landscape[q].z);
		glTexCoord2f(landscape[q].x,landscape[q].y);
		glVertex3f(landscape[q].x/10,landscape[q].y/10,landscape[q].z/10);
		q++;
		landcolor(landscape[q].z);
		glTexCoord2f(landscape[q].x,landscape[q].y);
		glVertex3f(landscape[q].x/10,landscape[q].y/10,landscape[q].z/10);
		q++;
	}
	glEnd();
	glEndList();
	//
	//
}
void renderScene(void) {

	//glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE );
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glRotatef(-90,1,0,0);
	glTranslatef(-1,-1,-2);
	glBegin(GL_TRIANGLES );
	glCallList(1);
	glEnd();	  
	glFlush();
	glPopMatrix();
}
void turn(float x)
{
g_ViewAngle+=(x*0.017453292519943295769236907684886);
eyeVX=-sin(g_ViewAngle);
eyeVZ=cos(g_ViewAngle);
lookDir.x=((eyeVX*2+eye.x));
lookDir.z=((eyeVZ*2+eye.z));
}
void move(float x)
{
eyeVX=(eyeVX*-1);
eye.x+=eyeVX*x;
eye.z-=eyeVZ*x;
lookDir.x+=eyeVX*x;
lookDir.z-=eyeVZ*x;
eyeVX=(eyeVX*-1);
}
void lookup(float x)
{
lookDir.y+=x;
}
void updatecamera()
{
glLoadIdentity();
gluLookAt(eye.x, eye.y, eye.z,
		  lookDir.x, lookDir.y, lookDir.z,
		  0.0, 1.0, 0.0);
}
void keyboard(int key,int x, int y)
{
	switch(key)
{
	case GLUT_KEY_UP:
	{
	move(-.5);
	updatecamera();
	renderScene();
	break;
	}
	
	case GLUT_KEY_DOWN:
	{
	move(.5);
	updatecamera();
	renderScene();
	break;
	}
	case GLUT_KEY_RIGHT:
	{
	turn(5);
	updatecamera();
	renderScene();
	break;
	}
	case GLUT_KEY_LEFT:
	{
	turn(-5);
	updatecamera();
	renderScene();
	break;
	}
	case GLUT_KEY_PAGE_UP:
	{
	lookup(-.5);
	updatecamera();
	renderScene();
	break;
	}
	case GLUT_KEY_PAGE_DOWN:
	{
	lookup(.5);
	updatecamera();
	renderScene();
	break;
	}
	}
}
void main()
{

	glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(512,512);
	glutCreateWindow("landscape");
	glutDisplayFunc(renderScene);
	glutSpecialFunc(keyboard);
	GLfloat fAspect=640/480;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60,fAspect,0.4,400.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	setuptexture("grass.raw");
	loadlandscape(64,64,"land.raw");//Generated with Oct:5 Amp:5 Freq:.21 H:1 
	glEnable(GL_TEXTURE_2D);
	glutMainLoop();
}