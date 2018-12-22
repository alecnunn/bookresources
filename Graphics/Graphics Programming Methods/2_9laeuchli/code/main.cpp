#include<gl\glut.h>
#include<gl\glext.h>
#include<fstream.h>
#include<iostream.h>
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include"ptable.h"
#include "matrix.h"
#include "bmp.h"
GLuint	texturest;
//Multi texture
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
bitmap_t TextureImage;


void setuptext()
{
	glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)
        wglGetProcAddress("glActiveTextureARB");
    glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)
        wglGetProcAddress("glMultiTexCoord2fARB");
}


double inline fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); }
double inline lerp(double t, double a, double b) { return a + t * (b - a); }
double inline grad(int hash, double x, double y, double z) {
	int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
	double u = h<8||h==12||h==13 ? x : y,   // INTO 12 GRADIENT DIRECTIONS.
		v = h<4||h==12||h==13 ? y : z;
	return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

double noise(double x, double y, double z) {
	
	int X = (int)floor(x) & 255,                  // FIND UNIT CUBE THAT
		Y = (int)floor(y) & 255,                  // CONTAINS POINT.
		Z = (int)floor(z) & 255;
	x -= floor(x);                                // FIND RELATIVE X,Y,Z
	y -= floor(y);                                // OF POINT IN CUBE.
	z -= floor(z);
	double u = fade(x),                                // COMPUTE FADE CURVES
		v = fade(y),                                // FOR EACH OF X,Y,Z.
		w = fade(z);
	int A = p[X  ]+Y, AA = p[A]+Z, AB = p[A+1]+Z,      // HASH COORDINATES OF
		B = p[X+1]+Y, BA = p[B]+Z, BB = p[B+1]+Z;      // THE 8 CUBE CORNERS,
	
	return lerp(w, lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ),  // AND ADD
		grad(p[BA  ], x-1, y  , z   )), // BLENDED
		lerp(u, grad(p[AB  ], x  , y-1, z   ),  // RESULTS
		grad(p[BB  ], x-1, y-1, z   ))),// FROM  8
		lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ),  // CORNERS
		grad(p[BA+1], x-1, y  , z-1 )), // OF CUBE
		lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
		grad(p[BB+1], x-1, y-1, z-1 ))));
}


void setup()
{
	

	glActiveTextureARB(0);

	
	LoadBMP("land.bmp",&TextureImage);
	glGenTextures(1,&texturest);
	glBindTexture(GL_TEXTURE_2D, texturest);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage.width, TextureImage.height, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage.data);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering*/
	//
}

struct vertex{
	double x,y,z,u,v;
};

vertex sphere[20000];
	int i3=0;
	int res=90;
	bool wire=false;
	bool LOD=false;
float MultiFractal(float x, float y,float z, float octaves, float offset,float freqchange,float h)
{
	float result;
	float signal;
	float weight;
	float freq=1;
	result=(noise(x,y,z)+offset)*pow(freq,-h);
	freq*=freqchange;
	weight=result;
	x*=freqchange;
	y*=freqchange;
	z*=freqchange;
	for(int i=0;i<octaves;i++)
	{
		if(weight>1)
			weight=1;
		signal=(noise(x,y,z)+offset)*pow(freq,-h);
		freq*=freqchange;
		result+=weight*signal;
		weight*=signal;
		x*=freqchange;
		y*=freqchange;
		z*=freqchange;

	}
return result;
}

void generatesphere(int numbofpoints)
{
	/////////////
	double horizontal=0;
	if(numbofpoints>90)
	{
		res=numbofpoints=90;
	}
	if(numbofpoints<27)
	{
		res=numbofpoints=27;
	}
	float n, p, nStep = 6.283185307179586476925286766559/numbofpoints;
	i3=0;
	float fx=0;
	float fy=0;
	int num=0;
	int x=0,y=0;
	for( n = 0; n  <= numbofpoints/2; n++ )	
	{
	
	
		for ( p = 0; p <= numbofpoints; p++ )	
		{		
			horizontal	=  sin(fy);	
			float nmod=MultiFractal((horizontal * sin(fx)),(horizontal * cos(fx)),(cos(fy)),5,.8,2,.15)/10;
			sphere[i3].x = (horizontal * sin(fx))*((nmod)+1);	
			sphere[i3].y = (horizontal * cos(fx))*((nmod)+1);
			sphere[i3].z = (cos(fy))*((nmod)+1);
			sphere[i3].u=p/numbofpoints;
			sphere[i3].v=2*n/(numbofpoints);
	
		
			i3++;
			fy-=nStep;

			horizontal	=  sin(fy);	
			nmod=MultiFractal((horizontal * sin(fx)),(horizontal * cos(fx)),(cos(fy)),5,.8,2,.15)/10;
			sphere[i3].x = (horizontal * sin(fx))*((nmod)+1);	
			sphere[i3].y = (horizontal * cos(fx))*((nmod)+1);
			sphere[i3].z = (cos(fy))*((nmod)+1);
			sphere[i3].u=p/numbofpoints;
			sphere[i3].v=2*(n+1)/(numbofpoints);
			
		
			fx-=nStep;
			i3++;
			fy+=nStep;
	

		}
		fx=0;
		fy-=nStep;
	
	
		
	}
	cout<<"Number of Triangles "<<i3;
	cout<<endl;
}

void RenderScene(void) {
	
	setup();
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);
	glPushMatrix();
	updatecamera();
	////////

	/////
	glEnable(GL_TEXTURE_2D);
	/////
	glTranslatef(0,0,-3);
	glRotatef(90,1,0,0);

	if(wire==true)
	{
		glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE );
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL );
	}

	glBegin(GL_TRIANGLE_STRIP);
		for(int i=0;i<i3;i++)
		{
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,sphere[i].u,sphere[i].v);
		glVertex3f(sphere[i].x,sphere[i].y,sphere[i].z);
	
		}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glColor3f(0,0,1);
	glutSolidSphere(1.16,30,30);
	glColor3f(1,1,1);
	glEnable(GL_TEXTURE_2D);
	glPopMatrix();
	glutSwapBuffers();
	
}
void setres()
{
float x=fabs(translationm[0][3])*20;
float y=fabs(translationm[1][3])*20;
float z=fabs(translationm[2][3])*20;
float ress=(x+y+z)/10;
res=90-ress;
}

void Key( unsigned char key, int x, int y )
{
	
	if(key=='l')
	{
		if(LOD==false)
		{
			LOD=true;
		}
		else
		{
			LOD=false;
		}
	}
	if(key=='p')
	{
		wire=true;
		RenderScene();
	}
	if(key=='o')
	{
		wire=false;
		RenderScene();
	}

		if(key=='d')
	{
		rotatey(.1);
		RenderScene();
		
		
	}
	if(key=='a')
	{	
		rotatey(-.1);
		RenderScene();
		
		
	}
	if(key=='r')
	{
		rotatex(.1);
		RenderScene();
		
		
	}
	if(key=='t')
	{	
		rotatex(-.1);
		RenderScene();
		
	}
	if(key=='w')
	{	
	
		moveforward(.1);
		if(LOD==true)
		{
			setres();//This is for the simple LOD system. You probably will not want to update the LOD every time you move forward, as done here.
			generatesphere(res);
		}
		RenderScene();
		
		
	}
	if(key=='s')
	{	
	
		moveforward(-.1);
		if(LOD==true)
		{
			setres();//This is for the simple LOD system. You probably will not want to update the LOD every time you move forward, as done here.
			generatesphere(res);
		}
		RenderScene();
		
	}
	if(key=='z')
	{
		moveup(-1);
		RenderScene();
	}
	if(key=='x')
	{
		moveup(1);
		RenderScene();
	}
	
}  

void main() {
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(512,512);
	glutCreateWindow("Planet");
	glClearColor(0,0,0,0);
	GLfloat fAspect=512/512;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60,fAspect,0.1,400.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutDisplayFunc(RenderScene);
	glutKeyboardFunc(Key);
	for (int i2=0; i2 < 256 ; i2++) p[256+i2] = p[i2] = permutation[i2];
	setuptext();
	setres();
	generatesphere(res);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
	FreeBMP(&TextureImage);
}
