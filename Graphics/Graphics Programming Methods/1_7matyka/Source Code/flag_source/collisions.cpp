


#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#include "materialpoint.h"
#include "collisions.h"
#include "cloth.h"
#include "SoftBody.h"


#define epsilon 0.02f


extern Obszar *KorzenObszarow;

void collisions(MaterialPoint *korzenP, Obszar *korzenO)
{
	Obszar *roboczyO;

	Vector3 Vbak;			

	Vector3 Vn,Vs;		
	Vector3 Vn2,Vs2;	


	Vector3 n,n1,n2;			



for(roboczyO=korzenO;roboczyO != NULL; roboczyO=roboczyO->prawy)
if(roboczyO->MaterialPoint)	   				
	if(roboczyO->flaga & O_AKT)
	{				

		if(roboczyO->MaterialPoint->r.x > xmax - roboczyO->R)
		{
			roboczyO->MaterialPoint->r.x=xmax - roboczyO->R;
			roboczyO->MaterialPoint->v.x=roboczyO->MaterialPoint->v.x * (-tlumienie);
		}

		if(roboczyO->MaterialPoint->r.y > ymax - roboczyO->R)
		{
			roboczyO->MaterialPoint->r.y=ymax - roboczyO->R;
			roboczyO->MaterialPoint->v.y=roboczyO->MaterialPoint->v.y * (-tlumienie);
		}

		if(roboczyO->MaterialPoint->r.z > zmax + roboczyO->R )
		{
			roboczyO->MaterialPoint->r.z=zmax + roboczyO->R;
			roboczyO->MaterialPoint->v.z=roboczyO->MaterialPoint->v.z * (-tlumienie);
		}


		if(roboczyO->MaterialPoint->r.x < xmin + roboczyO->R )
		{
			roboczyO->MaterialPoint->r.x=xmin + roboczyO->R ;
			roboczyO->MaterialPoint->v.x=roboczyO->MaterialPoint->v.x * (-tlumienie);
		}

		if(roboczyO->MaterialPoint->r.y < ymin + roboczyO->R )
		{
			roboczyO->MaterialPoint->r.y=ymin + roboczyO->R;
			roboczyO->MaterialPoint->v.y=roboczyO->MaterialPoint->v.y * (-tlumienie);
		}

		if(roboczyO->MaterialPoint->r.z < zmin + roboczyO->R )
		{
			roboczyO->MaterialPoint->r.z=ymin + roboczyO->R;
			roboczyO->MaterialPoint->v.z=roboczyO->MaterialPoint->v.z * (-tlumienie);
		}


//		Kopiuj(roboczyO->MaterialPoint->r,&(roboczyO->r1));

		roboczyO->r1 = roboczyO->MaterialPoint->r;

	}

}



void collisions(Cloth &cloth)
{
	int i,j;

for(j=0;j<cloth.ny;j++)
for(i=0;i<cloth.nx;i++)
	if(!(cloth.Points[i][j]->flaga & P_ZAW))
	{
		if(cloth.Points[i][j]->r.x > xmax)
		{
			cloth.Points[i][j]->r.x=xmax;
			cloth.Points[i][j]->v.x=cloth.Points[i][j]->v.x * (-tlumienie);
		}

		if(cloth.Points[i][j]->r.y > ymax)
		{
			cloth.Points[i][j]->r.y=ymax;
			cloth.Points[i][j]->v.y=cloth.Points[i][j]->v.y * (-tlumienie);
		}

		if(cloth.Points[i][j]->r.z > zmax)
		{
			cloth.Points[i][j]->r.z=zmax;
			cloth.Points[i][j]->v.z=cloth.Points[i][j]->v.z * (-tlumienie);
		}


		if(cloth.Points[i][j]->r.x < xmin)
		{
			cloth.Points[i][j]->r.x=xmin;
			cloth.Points[i][j]->v.x=cloth.Points[i][j]->v.x * (-tlumienie);
		}

		if(cloth.Points[i][j]->r.y < ymin)
		{
			cloth.Points[i][j]->r.y=ymin;
			cloth.Points[i][j]->v.y=cloth.Points[i][j]->v.y * (-tlumienie);
		}

		if(cloth.Points[i][j]->r.z < zmin)
		{
			cloth.Points[i][j]->r.z=ymin;
			cloth.Points[i][j]->v.z=cloth.Points[i][j]->v.z * (-tlumienie);
		}
	}

}



void collisions(SoftBody &SoftBody, Obszar *korzenO)
{
	int i;

for(i=0;i<SoftBody.NUMP;i++)
	if(!(SoftBody.Points[i]->flaga & P_ZAW))
	{
		if(SoftBody.Points[i]->r.x > xmax)
		{
			SoftBody.Points[i]->r.x=xmax;
			SoftBody.Points[i]->v.x=SoftBody.Points[i]->v.x * (-1);
			SoftBody.Points[i]->v=SoftBody.Points[i]->v * (tlumienie);
//			SoftBody.Points[i]->v=Vector3(0,0,0);
//			SoftBody.Points[i]->dv=Vector3(0,0,0);
		}

		if(SoftBody.Points[i]->r.y > ymax)
		{
			SoftBody.Points[i]->r.y=ymax;
			SoftBody.Points[i]->v.y=SoftBody.Points[i]->v.y * (-1);
			SoftBody.Points[i]->v=SoftBody.Points[i]->v * (tlumienie);
//			SoftBody.Points[i]->v=Vector3(0,0,0);
//			SoftBody.Points[i]->dv=Vector3(0,0,0);
		}

		if(SoftBody.Points[i]->r.z > zmax)
		{
			SoftBody.Points[i]->r.z=zmax;
			SoftBody.Points[i]->v.z=SoftBody.Points[i]->v.z * (-1);
			SoftBody.Points[i]->v=SoftBody.Points[i]->v * (tlumienie);
//			SoftBody.Points[i]->v=Vector3(0,0,0);
//			SoftBody.Points[i]->dv=Vector3(0,0,0);
		}


		if(SoftBody.Points[i]->r.x < xmin)
		{
			SoftBody.Points[i]->r.x=xmin;
			SoftBody.Points[i]->v.x=SoftBody.Points[i]->v.x * (-1);
			SoftBody.Points[i]->v=SoftBody.Points[i]->v * (tlumienie);
//			SoftBody.Points[i]->v=Vector3(0,0,0);
//			SoftBody.Points[i]->dv=Vector3(0,0,0);
		}

		if(SoftBody.Points[i]->r.y < ymin)
		{
			SoftBody.Points[i]->r.y=ymin;
			SoftBody.Points[i]->v.y=SoftBody.Points[i]->v.y * (-1);
			SoftBody.Points[i]->v=SoftBody.Points[i]->v * (tlumienie);
//			SoftBody.Points[i]->v=Vector3(0,0,0);
//			SoftBody.Points[i]->dv=Vector3(0,0,0);
		}

		if(SoftBody.Points[i]->r.z < zmin)
		{
			SoftBody.Points[i]->r.z=ymin;
			SoftBody.Points[i]->v.z=SoftBody.Points[i]->v.z * (-1);
			SoftBody.Points[i]->v=SoftBody.Points[i]->v * (tlumienie);
//			SoftBody.Points[i]->v=Vector3(0,0,0);
//			SoftBody.Points[i]->dv=Vector3(0,0,0);
		}
	}





//
// Kolizja - MaterialPointy materialne + nieruchoma kula o promieniu R
// (po cholere ja pisze "O promieniu R ????"
//

	double d,z;
	Obszar *roboczyO;
	Vector3 n,Vn,Vs;

 		for(roboczyO=korzenO;roboczyO != NULL; roboczyO=roboczyO->prawy)
		for(i=0;i<SoftBody.NUMP;i++)
		if(!(SoftBody.Points[i]->flaga & P_ZAW) && !(roboczyO->flaga & O_NAKT))
		{
			d=(roboczyO->r1 - SoftBody.Points[i]->r).length();
			if(d - roboczyO->R < 0)
			{
				n = (roboczyO->r1 - SoftBody.Points[i]->r);
				n.norm();
	
				// przesun
				z = d - roboczyO->R;
			   	SoftBody.Points[i]->r = SoftBody.Points[i]->r + n * z;
	
				// v
				Vn = n * (n * SoftBody.Points[i]->v);
			  	Vs = SoftBody.Points[i]->v - Vn;
	
			   	SoftBody.Points[i]->v = Vs - Vn;
	  
			    SoftBody.Points[i]->v = SoftBody.Points[i]->v * (roboczyO->tlum);	
			}
		}
	
}




Obszar *AlokujObszar(float promien, Vector3 r,  float tlum, int flaga, MaterialPoint *p)
{
	Obszar *RoboczyObszar;


	if (!(RoboczyObszar = (Obszar *) malloc((unsigned) sizeof(Obszar)))) 
	{
		exit(0);
    }



	RoboczyObszar->flaga=flaga;
	RoboczyObszar->R=promien;
	RoboczyObszar->tlum=tlum;
	RoboczyObszar->MaterialPoint=p;

	RoboczyObszar->r1 = r;



	RoboczyObszar->r=(rand()/(float)RAND_MAX);
	RoboczyObszar->g=(rand()/(float)RAND_MAX);
	RoboczyObszar->b=(rand()/(float)RAND_MAX);

/*	RoboczyObszar->r=0.8;
	RoboczyObszar->g=0.7;
	RoboczyObszar->b=0.8;
*/

	RoboczyObszar->prawy=NULL;

	return (RoboczyObszar);
}



void DodajObszar(Obszar *korzen, float promien, Vector3 r,  float tlum, int flaga, MaterialPoint *p)
{
	Obszar *Roboczy;

	for(Roboczy=korzen;Roboczy->prawy != NULL; Roboczy=Roboczy->prawy);

	Roboczy->prawy=AlokujObszar(promien,r,tlum,flaga,p);

	if(p)
  	(*p).flaga |= P_OBSZ;

}

