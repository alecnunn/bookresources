

#include <stdio.h>
#include <stdlib.h>

#include "Vector3.h"
#include "materialpoint.h"
#include "physics.h"
#include "view.h"
#include "collisions.h"
 
#include "cloth.h"
#include "SoftBody.h"


extern Vector3 Wind;

void ConstrainedSizeCorrection(Sprezyna *spring, int PossiblyChange)
{
	double diff = (spring->MaterialPoint2->r-spring->MaterialPoint1->r).length() - spring->length;
	Vector3 n;
#define MAX_ODCHYL ((float)(PossiblyChange*spring->length)/100.0)
	
	if(diff > MAX_ODCHYL )
	{

		diff = (spring->MaterialPoint2->r-spring->MaterialPoint1->r).length() - (spring->length + MAX_ODCHYL);
		
		n = spring->MaterialPoint2->r - spring->MaterialPoint1->r;
		n.norm();
		n = n * (diff/2.0f);

		if(!(spring->MaterialPoint1->flaga & P_ZAW))
			spring->MaterialPoint1->r = spring->MaterialPoint1->r + n;
		
		if(!(spring->MaterialPoint2->flaga & P_ZAW))
			spring->MaterialPoint2->r = spring->MaterialPoint2->r - n;


	}
}

void CCC_Springs(SoftBody &SoftBody)
{
	int i;

	for(i=0;i<SoftBody.S1_num;i++)
		ConstrainedSizeCorrection(SoftBody.S1[i],SoftBody.S1[0]->const_lenght);

	for(i=0;i<SoftBody.S2_num;i++)
		ConstrainedSizeCorrection(SoftBody.S2[i],SoftBody.S2[0]->const_lenght);

	for(i=0;i<SoftBody.S3_num;i++)
		ConstrainedSizeCorrection(SoftBody.S3[i],SoftBody.S3[0]->const_lenght);

//	for(i=0;i<SoftBody.S4_num;i++)
//		ConstrainedSizeCorrection(SoftBody.S4[i],SoftBody.S4[0]->const_lenght);

	for(i=0;i<SoftBody.S5_num;i++)
		ConstrainedSizeCorrection(SoftBody.S5[i],SoftBody.S5[0]->const_lenght);

	for(i=0;i<SoftBody.S6_num;i++)
		ConstrainedSizeCorrection(SoftBody.S6[i],SoftBody.S6[0]->const_lenght);

  
}



// Constrained Cloth Correction on a Springs 
void CCC_Springs(Cloth &cloth)
{
	int i,j;

	for(j=0;j<cloth.ny-1;j++)
	for(i=0;i<cloth.nx;i++)
		ConstrainedSizeCorrection(cloth.Springs1[i][j],cloth.lenght_constrain);
	
	for(i=0;i<cloth.nx-1;i++)
	for(j=0;j<cloth.ny;j++)
		ConstrainedSizeCorrection(cloth.Springs2[i][j],cloth.lenght_constrain);


	for(i=0;i<cloth.nx-1;i++)
	for(j=0;j<cloth.ny-1;j++)
		ConstrainedSizeCorrection(cloth.Springs3[i][j],cloth.lenght_constrain);

	for(j=0;j<cloth.ny-1;j++)
	for(i=1;i<cloth.nx;i++)
		ConstrainedSizeCorrection(cloth.Springs4[i][j],cloth.lenght_constrain);
}



// przesuwa srodek obszaru sferycznego  do MaterialPointu przypisanego do niego, jesli pole -> MaterialPoint != NULL
void PrzesunObszary(Obszar *korzenO)
{
	Obszar *roboczyO;

		for(roboczyO=korzenO;roboczyO != NULL; roboczyO=roboczyO->prawy)
			if(roboczyO->flaga & O_AKT)
				if(roboczyO->MaterialPoint)
//					Kopiuj(roboczyO->MaterialPoint->r,&(roboczyO->r1));						
					roboczyO->r1=roboczyO->MaterialPoint->r;

}



Vector3 Force_Gravity(MaterialPoint *p)
{
	return Graw * (*p).masa;
}

void Force_Hooke(Sprezyna *spring)
{
	Vector3 r12;		
	double r12d;		
	Vector3 v12;		
	float f;
	Vector3 F;

	r12 = (spring->MaterialPoint1->r - spring->MaterialPoint2->r);
	r12d=r12.length();

	if(r12d != 0)
	{	
		v12 = (spring->MaterialPoint1->v - spring->MaterialPoint2->v);	 // diff vi


		f = (r12d -  spring->length) * spring->ks + (v12 * r12) * spring->kd / r12d;
	
		F = (r12 * (1.0f/r12d));
		F = (F * f);
		
		spring->MaterialPoint2->f = F + spring->MaterialPoint2->f;
		spring->MaterialPoint1->f = F * (-1) + spring->MaterialPoint1->f;
	}
}

void CalculateForces(SoftBody &SoftBody, float dt)
{
	int i,j;
	Vector3 force13;

	for(i=0;i<SoftBody.NUMP;i++)
		if(!(SoftBody.Points[i]->flaga & P_ZAW))
			SoftBody.Points[i]->f= Force_Gravity(SoftBody.Points[i]);

		for(i=0;i<SoftBody.S1_num;i++)
			Force_Hooke(SoftBody.S1[i]);
		for(i=0;i<SoftBody.S2_num;i++)
			Force_Hooke(SoftBody.S2[i]);
		for(i=0;i<SoftBody.S3_num;i++)
			Force_Hooke(SoftBody.S3[i]);
	//	for(i=0;i<SoftBody.S4_num;i++)
	//		Force_Hooke(SoftBody.S4[i]);
		for(i=0;i<SoftBody.S5_num;i++)
			Force_Hooke(SoftBody.S5[i]);
		for(i=0;i<SoftBody.S6_num;i++)
			Force_Hooke(SoftBody.S6[i]);
}

void CalculateForces(Cloth &cloth, float dt)
{
	int i,j;


// grawitacja, dla wszystkich MaterialPointow struktury "class Cloth"

	for(j=0;j<cloth.ny;j++)
	for(i=0;i<cloth.nx;i++)
		if(!(cloth.Points[i][j]->flaga & P_ZAW))
			cloth.Points[i][j]->f= Force_Gravity(cloth.Points[i][j]);

	for(j=0;j<cloth.ny-1;j++)
	for(i=0;i<cloth.nx;i++)
		Force_Hooke(cloth.Springs1[i][j]);


	for(i=0;i<cloth.nx-1;i++)
	for(j=0;j<cloth.ny;j++)
		Force_Hooke(cloth.Springs2[i][j]);


	for(i=0;i<cloth.nx-1;i++)
	for(j=0;j<cloth.ny-1;j++)
		Force_Hooke(cloth.Springs3[i][j]);

	for(j=0;j<cloth.ny-1;j++)
	for(i=1;i<cloth.nx;i++)
		Force_Hooke(cloth.Springs4[i][j]);

	// flex springs
	for(i=0;i<cloth.nx-2;i++)		// x
	for(j=0;j<cloth.ny;j++)
		Force_Hooke(cloth.Springs5[i][j]);

	for(j=0;j<cloth.ny-2;j++)			// y
	for(i=0;i<cloth.nx;i++)
		Force_Hooke(cloth.Springs6[i][j]);


	cloth.CalculateVertexNormals();		// trzymane w ->n

	for(j=0;j<cloth.ny;j++)				// wiatr na trojkaty 1
	for(i=0;i<cloth.nx;i++)
		if(!(cloth.Points[i][j]->flaga & P_ZAW))
		cloth.Points[i][j]->f = cloth.Points[i][j]->f + cloth.Points[i][j]->n * ((Wind - cloth.Points[i][j]->v)*cloth.Points[i][j]->n);
	

}

void Solve_Euler(Cloth &cloth, float dt)
{
	int i,j;

	for(j=0;j<cloth.ny;j++)
	for(i=0;i<cloth.nx;i++)
		if(!(cloth.Points[i][j]->flaga & P_ZAW))
		{
			cloth.Points[i][j]->dv = (cloth.Points[i][j]->f * (1/cloth.Points[i][j]->masa))*dt; 
			cloth.Points[i][j]->v = cloth.Points[i][j]->v + cloth.Points[i][j]->dv; 
			cloth.Points[i][j]->dr = cloth.Points[i][j]->v * dt; 
			cloth.Points[i][j]->r=cloth.Points[i][j]->r+cloth.Points[i][j]->dr;
		}

}






void Solve_MidPoint_k1(Cloth &cloth, float dt)
{
	Vector3 przech;
	int i,j;

	for(j=0;j<cloth.ny;j++)
	for(i=0;i<cloth.nx;i++)
		if(!(cloth.Points[i][j]->flaga & P_ZAW))
		{

			cloth.Points[i][j]->dv = (cloth.Points[i][j]->f * (1/cloth.Points[i][j]->masa))*dt; 
			cloth.Points[i][j]->dr = (cloth.Points[i][j]->v + cloth.Points[i][j]->dv * 0.5f) * dt; 

	// przechowaj r w dr i v w dv
			przech = cloth.Points[i][j]->v;
			cloth.Points[i][j]->v = cloth.Points[i][j]->v + cloth.Points[i][j]->dv * 0.5;
			cloth.Points[i][j]->dv = przech;
 
			przech = cloth.Points[i][j]->r;

			cloth.Points[i][j]->r = cloth.Points[i][j]->r + cloth.Points[i][j]->dr * 0.5;
			cloth.Points[i][j]->dr = przech;
		}
}



/* 

w polach dr i dv struktury MaterialPoint funkcja MidPoint_k1 zostawila stare wartosci v i r, do ktorych dodamy
przyrosty obliczone z wartosci nowych wyliczonych przez k2 */


void Solve_MidPoint_k2(Cloth &cloth, float dt)
{

	Vector3 dr,dv;
	int i,j;

	for(j=0;j<cloth.ny;j++)
	for(i=0;i<cloth.nx;i++)
		if(!(cloth.Points[i][j]->flaga & P_ZAW))
		{
			dv = (cloth.Points[i][j]->f * (1/cloth.Points[i][j]->masa))*dt; 
			cloth.Points[i][j]->v = dv + cloth.Points[i][j]->dv; 
			dr = cloth.Points[i][j]->v * dt; 
			cloth.Points[i][j]->r = dr;
			cloth.Points[i][j]->r = cloth.Points[i][j]->r + cloth.Points[i][j]->dr;
		}
}





// to samo dla kuli


void Solve_MidPoint_k1(SoftBody &SoftBody, float dt)
{
	Vector3 przech;
	int i,j;

	
	for(i=0;i<SoftBody.NUMP;i++)
		if(!(SoftBody.Points[i]->flaga & P_ZAW))
		{

			SoftBody.Points[i]->dv = (SoftBody.Points[i]->f * (1/SoftBody.Points[i]->masa))*dt; 
			SoftBody.Points[i]->dr = (SoftBody.Points[i]->v + SoftBody.Points[i]->dv * 0.5f) * dt; 

	// przechowaj r w dr i v w dv
			przech = SoftBody.Points[i]->v;
			SoftBody.Points[i]->v = SoftBody.Points[i]->v + SoftBody.Points[i]->dv * 0.5;
			SoftBody.Points[i]->dv = przech;
 
			przech = SoftBody.Points[i]->r;

			SoftBody.Points[i]->r = SoftBody.Points[i]->r + SoftBody.Points[i]->dr * 0.5;
			SoftBody.Points[i]->dr = przech;
		}
}




void Solve_MidPoint_k2(SoftBody &SoftBody, float dt)
{

	Vector3 dr,dv;
	int i,j;

	for(i=0;i<SoftBody.NUMP;i++)
		if(!(SoftBody.Points[i]->flaga & P_ZAW))
		{
			dv = (SoftBody.Points[i]->f * (1/SoftBody.Points[i]->masa))*dt; 
			SoftBody.Points[i]->v = dv + SoftBody.Points[i]->dv; 
			dr = SoftBody.Points[i]->v * dt; 
			SoftBody.Points[i]->r = dr;
			SoftBody.Points[i]->r = SoftBody.Points[i]->r + SoftBody.Points[i]->dr;
		}
}
