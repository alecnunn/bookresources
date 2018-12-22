

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/glut.h>

#include "Vector3.h"
#include "materialpoint.h"
#include "physics.h"
#include "view.h"


#include "collisions.h"
//#include "newton.h"


#include <iostream>
#include <list>



int szer=250,wys=250;

float xmin=-10.9,xmax=10.9,ymin=-0.9,ymax=0.9, zmin = -0.9, zmax = 0.9;


float dt=0.004;//5;//5;


float tlumienie = 0.02;


Vector3 Graw(0,-4.0,0);


MaterialPoint *KorzenMaterialPointow;
 *KorzenSprezyn;

Obszar *KorzenObszarow;





//#define fullscreen



// funkcja PrzesunNaKorzeniu - przesuwa wskaznik na n-ty element w liscie i zwraca jego a(dres)
// zwraca to co dostal +1, jesli blad
MaterialPoint *P(int n)
{
	MaterialPoint *Roboczy;
	int i;	 

	Roboczy = KorzenMaterialPointow;

	for(i=-1;i<n;i++)
	if(Roboczy->prawy)
		Roboczy=Roboczy->prawy;
	else
		return(KorzenMaterialPointow);
	  
	return Roboczy;
}

int motionchange=0;

void dodaj_MaterialPoint2(void)		// f
{
	motionchange=200;
//	P(7)->v = Vector3(-9,16,0);

}


void dodaj_MaterialPointS(void)
{

//Vector3 skok = Vector3(-0.7,1.8,(rand()/(float)RAND_MAX-0.5)*0.05);

/*	P(1)->v = P(1)->v + skok;
	P(2)->v = P(2)->v + skok;
	P(3)->v = P(3)->v + skok;
	P(4)->v = P(4)->v + skok;
*/

//	P(7)->v = P(7)->v + skok * 0.5;


	motionchange=1;
//	P(7)->r = P(7)->r - Vector3(-0.002,0.03,0); 
   //	  P(6)->r = P(6)->r - Vector3(0,0.1,0); 


}





double dlug4;		// length sprezyny ktora animujemy
double dlug3;		// length sprezyny ktora animujemy
double dlug2;		// length sprezyny ktora animujemy
double dlug;		// length sprezyny ktora animujemy
 *spre;		//  ktora animujemy
double st;

 

int main(int argc, char **argv)
{

	int i,j;
	MaterialPoint *roboczyP2;



	KorzenMaterialPointow=AlokujMaterialPoint(1,P_NRYS,Vector3(0,0,0),Vector3(0,0,0),0);
	KorzenSprezyn=AlokujSprezyne(0,0,0,KorzenMaterialPointow,KorzenMaterialPointow,0);
	KorzenObszarow=AlokujObszar(0,Vector3(0,0,0),0,O_NAKT,NULL);



	DodajMaterialPoint(KorzenMaterialPointow,10.0,P_NORM|P_NRYS,Vector3(0,0,0),Vector3(0,0,0),0);

#define a 0.23 
#define y -0.8





// podloze

  	DodajMaterialPoint(KorzenMaterialPointow,1.4,P_NORM,Vector3(-a*0.6,y,-a*0.6),Vector3(0,0,0),0);	// 1
  	DodajMaterialPoint(KorzenMaterialPointow,1.5,P_NORM,Vector3(a*0.6,y,-a*0.6),Vector3(0,0,0),0);		// 2
  	DodajMaterialPoint(KorzenMaterialPointow,1.5,P_NORM,Vector3(a*0.6,y,a*0.6),Vector3(0,0,0),0);		// 3
  	DodajMaterialPoint(KorzenMaterialPointow,1.4,P_NORM,Vector3(-a*0.6,y,a*0.6),Vector3(0,0,0),0);		// 4
  	DodajMaterialPoint(KorzenMaterialPointow,0.5,P_NORM,Vector3(0.0f,y,0),Vector3(0,0,0),0);		// 5

// szyja
  	DodajMaterialPoint(KorzenMaterialPointow,0.5,P_NORM,Vector3(-(float)a*0.2f,y+a*1.5,0),Vector3(0,0,0),0);		// 6

// glowa

  	DodajMaterialPoint(KorzenMaterialPointow,0.8,P_NORM,Vector3(-(float)a*0.6f,y+a*1.8,0),Vector3(0,0,0),0);		// 7
//	DodajObszar(KorzenObszarow,0.05,Vector3(0,0,0),0.0,O_AKT|O_RYS,P(7));

  	DodajMaterialPoint(KorzenMaterialPointow,0.2,P_NORM,Vector3(-(float)a*1.1f,y+a*1.4,0),Vector3(0,0,0),0);		// 7
	DodajObszar(KorzenObszarow,0.05,Vector3(0,0,0),0.0,O_AKT|O_RYS,P(8));


// poloczenia podloze

  	DodajSprezyne(KorzenSprezyn,14.0f,0,180.0f,P(1),P(3),S_OK|S_NRYS);				
  	DodajSprezyne(KorzenSprezyn,14.0f,0,180.0f,P(2),P(4),S_OK|S_NRYS);	   

  	DodajSprezyne(KorzenSprezyn,14.0f,0,180.0f,P(1),P(2),S_OK|S_NRYS);				
  	DodajSprezyne(KorzenSprezyn,14.0f,0,180.0f,P(2),P(3),S_OK|S_NRYS);	   
  	DodajSprezyne(KorzenSprezyn,14.0f,0,180.0f,P(3),P(4),S_OK|S_NRYS);	   
  	DodajSprezyne(KorzenSprezyn,14.0f,0,180.0f,P(4),P(1),S_OK|S_NRYS);	   


// polaczenia podloze/srodek

  	DodajSprezyne(KorzenSprezyn,14.0f,0,180.0f,P(1),P(5),S_OK|S_NRYS);				
  	DodajSprezyne(KorzenSprezyn,14.0f,0,180.0f,P(2),P(5),S_OK|S_NRYS);	   
  	DodajSprezyne(KorzenSprezyn,14.0f,0,180.0f,P(3),P(5),S_OK|S_NRYS);	   
  	DodajSprezyne(KorzenSprezyn,14.0f,0,180.0f,P(4),P(5),S_OK|S_NRYS);	   


// szyja podloze

  	DodajSprezyne(KorzenSprezyn,14.0f,0,280.0f,P(1),P(6),S_OK|S_NRYS);				
  	DodajSprezyne(KorzenSprezyn,14.0f,0,280.0f,P(2),P(6),S_OK|S_NRYS);	   
  	DodajSprezyne(KorzenSprezyn,14.0f,0,280.0f,P(3),P(6),S_OK|S_NRYS);	   
  	DodajSprezyne(KorzenSprezyn,14.0f,0,280.0f,P(4),P(6),S_OK|S_NRYS);	   
  	DodajSprezyne(KorzenSprezyn,14.0f,0,280.0f,P(5),P(6),S_OK|S_RYSL);	   



// glowa - podloze
  	DodajSprezyne(KorzenSprezyn,14.0f,0,290.0f,P(7),P(8),S_OK|S_RYSL);		
  	DodajSprezyne(KorzenSprezyn,2.0f,0,110.0f,P(1),P(8),S_OK|S_NRYS);		
  	DodajSprezyne(KorzenSprezyn,2.0f,0,110.0f,P(4),P(8),S_OK|S_NRYS);		
  	DodajSprezyne(KorzenSprezyn,6.0f,0,40.0f,P(6),P(8),S_OK|S_NRYS);		
  	DodajSprezyne(KorzenSprezyn,6.0f,0,40.0f,P(5),P(8),S_OK|S_NRYS);		


	DodajSprezyne(KorzenSprezyn,14.0f,0,290.0f,P(6),P(7),S_OK|S_RYSL);		
  	DodajSprezyne(KorzenSprezyn,12.0f,0,50.0f,P(4),P(7),S_OK|S_NRYS);	   
  	DodajSprezyne(KorzenSprezyn,12.0f,0,50.0f,P(1),P(7),S_OK|S_NRYS);
  	DodajSprezyne(KorzenSprezyn,12.0f,0,50.0f,P(2),P(7),S_OK|S_NRYS);	   
  	DodajSprezyne(KorzenSprezyn,12.0f,0,50.0f,P(3),P(7),S_OK|S_NRYS);
   


	spre=KorzenSprezyn->prawy;
	dlug=spre->length;
	dlug2=spre->prawy->length;
	dlug3=spre->prawy->prawy->prawy->length;

	dlug4=spre->prawy->prawy->prawy->prawy->prawy->length;
	st=0;





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

									   
    glutKeyboardFunc(Key);
    glutReshapeFunc(Reshape);
    glutDisplayFunc(Draw);
    glutIdleFunc(idle);
    glutMainLoop();
    
	return (0);

}


int k=0;
int kv=0;

#define prsf 0.05
#define dziel 9.5f

void dodaj_MaterialPoint(void)
{

	spre->length = dlug - dlug * sin(st)/1.5f;
   	spre->prawy->length = dlug2 - dlug2 * cos(st)/1.0f;
   	spre->prawy->prawy->length = dlug + dlug * sin(st*0.4)/1.5f;

   	spre->prawy->prawy->prawy->length = dlug3 - dlug3 * cos(st)/4.4f;			// reka - stopa
   	spre->prawy->prawy->prawy->prawy->length = dlug3 - dlug3 * sin(st)/4.4f;

   	spre->prawy->prawy->prawy->prawy->prawy->length = dlug4 - dlug4 * sin(st)/2.0f;
   	spre->prawy->prawy->prawy->prawy->prawy->prawy->length = dlug4 - dlug4 * sin(st)/2.0f;

//	st=st+(rand()/(float)RAND_MAX)*0.1;
	st=st+0.04;
	return;
}





//int motion = 0;



//#define Euler
#define MidPoint

void Motions(void)
{
	if(motionchange)
	if(motionchange>100)		// glowa
	{
		motionchange--;
		P(8)->v = Vector3(-0.72,1.2,0);
		P(7)->v = Vector3(-0.32,0.8,0);
	}
	else						
	if(motionchange>150)		// szyja
	{
		motionchange--;
		P(8)->v = Vector3(-0.52,0.94,0);
		P(7)->v = Vector3(-0.32,0.6,0);
	}
	else
	{
		motionchange--;
	}

}


void Timestep(void)
{
//	MaterialPoint *roboczyP;
// metoda Eulera

#ifdef Euler

	CalculateForces(KorzenMaterialPointow,KorzenSprezyn,KorzenObszarow);
	Solve_Euler(KorzenMaterialPointow,dt);

#endif


#ifdef MidPoint

	CalculateForces(KorzenMaterialPointow,KorzenSprezyn,KorzenObszarow);
	Solve_MidPoint_k1(KorzenMaterialPointow,dt);

 	collisions(KorzenMaterialPointow,KorzenObszarow);

 	CalculateForces(KorzenMaterialPointow,KorzenSprezyn,KorzenObszarow);
	Solve_MidPoint_k2(KorzenMaterialPointow,dt);

#endif


// rozpatruj collisions

	Motions();

	PrzesunObszary(KorzenObszarow);
 	collisions(KorzenMaterialPointow,KorzenObszarow);
	PrzesunObszary(KorzenObszarow);



	//PrzesunObszary(KorzenObszarow);




	//if(motion)
	//dodaj_MaterialPoint();
	
}


