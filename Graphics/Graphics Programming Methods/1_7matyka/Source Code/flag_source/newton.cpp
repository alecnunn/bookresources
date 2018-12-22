#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include "Vector3.h"
#include "materialpoint.h"
#include "physics.h"
#include "view.h"
#include "collisions.h"
#include "cloth.h"
#include "object.h"

#include "SoftBody.h"


#include "Bitmap.h"
#include "Texture.h"

const int szer=400,wys=250;

float xmin=-1.9,xmax=1.9,ymin=-1.9,ymax=1.9, zmin = -1.9, zmax = 1.9;

float dt=0.01;

float tlumienie = 0.0;

Vector3 Graw(0,0.0,0);
Vector3 GrawBAK(0,0.0,0);
Vector3 Wind(2.0,0.0,0.1);



#ifndef PI
#define PI 3.141592654
#endif

unsigned int texture1;
unsigned int texture2;
unsigned int texture3;
void LoadTextures(void);


// parzyste nx,ny

//Cloth cloth3(16,20, 80,0.1,   3,  Vector3(-0.4,0.3,0), Vector3(0.4,-0.3,0),     0.3);		// flaga
//Cloth cloth1(2,2, 22, 0.15,  5,   Vector3(-0.3,0.3,0), Vector3(0.3,-0.3,0),   0.45);		// zaw
//Cloth cloth2(2,2, 122, 0.15,  5,   Vector3(-0.9,0.3,0), Vector3(-0.25,-0.3,0),   0.45);		// zaw

// last parameter - 1 == new, 0 == old type of data object

//SoftBody TheSoftBody(Vector3(0.7,0.7,0),0.11,"SoftBody3.dat",1.1);
//SoftBody TheSoftBody2(Vector3(-0.7,-0.3,0),0.11,"SoftBody3.dat",1.1);


Cloth cloth1(16,18, 180,3,   3,  Vector3(-0.4,0.3,0), Vector3(0.4,-0.3,0),     0.3);		// flaga



Obszar *KorzenObszarow;	// ze starego enginu

//#define fullscreen

int main(int argc, char **argv)
{
	int i,j;



#define ROZ 0.46	// rozstaw osi
#define PRM 1.15	// promien

	KorzenObszarow=AlokujObszar(0,Vector3(0,0,0),0,O_NAKT,NULL);
//	KorzenObszarow->prawy=AlokujObszar(0.4,Vector3(-ROZ,-0.6,-ROZ),PRM,O_AKT,NULL);
//	KorzenObszarow->prawy->prawy=AlokujObszar(0.4,Vector3(-ROZ,-0.6,ROZ),PRM,O_AKT,NULL);
//	KorzenObszarow->prawy->prawy->prawy=AlokujObszar(0.4,Vector3(ROZ,-0.6,-ROZ),PRM,O_AKT,NULL);
//	KorzenObszarow->prawy->prawy->prawy->prawy=AlokujObszar(0.4,Vector3(ROZ,-0.6,ROZ),PRM,O_AKT,NULL);



    // fix 2 cloth points
	
	for(j=0;j<cloth1.ny;j+=1)
	{
		if(j<=0 || j>=cloth1.ny-1)
		cloth1.Points[0][j]->flaga |=P_ZAW;
	}



	glutInit(&argc, argv);

#ifndef fullscreen

    glutInitWindowPosition(150, 150); glutInitWindowSize( szer, wys);
    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
    if (glutCreateWindow("Newton") == GL_FALSE) {exit(1);}

#else
 	glutGameModeString("640x480:16@60");
    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
    glutEnterGameMode();
    glutFullScreen();
#endif

    Init();
	LoadTextures();


    glutKeyboardFunc(Key);
    glutReshapeFunc(Reshape);
    glutDisplayFunc(Draw);
    glutIdleFunc(idle);
    glutMainLoop();
    
	return (0);
}


//#define Euler
#define MidPoint




float t=0;
void Timestep(void)
{

#ifdef Euler

//	CalculateForces(cloth1,dt);
//	Solve_Euler(cloth1,dt);
//	CCC_Springs(cloth1);

//	CalculateForces(cloth2,dt);
//	Solve_Euler(cloth2,dt);
//	CCC_Springs(cloth2);

#endif
#ifdef MidPoint


	CalculateForces(cloth1,dt);
//	WyznaczSily(cloth2,dt);
//	WyznaczSily(cloth3,dt);

	Solve_MidPoint_k1(cloth1,dt);
//	RozwiazMidPoint_k1(cloth2,dt);
//	RozwiazMidPoint_k1(cloth3,dt);

	CCC_Springs(cloth1);
//	CCC_Springs(cloth2);
//	CCC_Springs(cloth3);

//	Kolizje(cloth1);


	CalculateForces(cloth1,dt);
//	WyznaczSily(cloth2,dt);
//	WyznaczSily(cloth3,dt);
	Solve_MidPoint_k2(cloth1,dt);
//	RozwiazMidPoint_k2(cloth2,dt);
//	RozwiazMidPoint_k2(cloth3,dt);

	CCC_Springs(cloth1);
//	CCC_Springs(cloth2);
//	CCC_Springs(cloth3);
//	Kolizje(cloth1);



#endif	

}

void LoadTextures(void)
{
	glGenTextures(1,&texture1);

	glBindTexture(GL_TEXTURE_2D,texture1);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	TextureLoadMipmap("uwr.bmp");


	cloth1.BindTextureOnIt(texture1);



}