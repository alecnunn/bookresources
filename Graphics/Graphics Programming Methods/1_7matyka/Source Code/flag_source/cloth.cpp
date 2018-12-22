

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include "Vector3.h"
#include "materialpoint.h"
#include "cloth.h"

using namespace std;


// NormalTriangle - zwraca normalna do trojkata
Vector3 NormalTriangle(Vector3 p1, Vector3 p2, Vector3 p3)
{
	Vector3 n=(p1-p2) % (p1-p3);
	n.norm();
	return n;
}

// funkcja wylicza normalne dla wszystkich MaterialPointow w scenie jako srednia 
// normalnych przylegajacych scian

// normalne przechowuje w polu MaterialPoint->dr
void Cloth::CalculateVertexNormals(void)
{
	Vector3 n;
	// na razie bez brzegow
	for(i=1;i<nx-1;i++)
	for(j=1;j<ny-1;j++)
	{
		n =     NormalTriangle(Points[i][j]->r,Points[i][j-1]->r,Points[i+1][j-1]->r);
		n = n + NormalTriangle(Points[i][j]->r,Points[i+1][j-1]->r,Points[i+1][j]->r);
		n = n + NormalTriangle(Points[i][j]->r,Points[i+1][j]->r,Points[i+1][j+1]->r);
		n = n + NormalTriangle(Points[i][j]->r,Points[i+1][j+1]->r,Points[i][j+1]->r);
		n = n + NormalTriangle(Points[i][j]->r,Points[i][j+1]->r,Points[i-1][j+1]->r);
		n = n + NormalTriangle(Points[i][j]->r,Points[i-1][j+1]->r,Points[i-1][j]->r);
		n = n + NormalTriangle(Points[i][j]->r,Points[i-1][j]->r,Points[i-1][j+1]->r);
		n = n + NormalTriangle(Points[i][j]->r,Points[i-1][j+1]->r,Points[i][j+1]->r);

		n.norm();
		
		Points[i][j]->n = n;
	}

	// brzegi bez rogow
	for(i=1;i<nx-1;i++)
		Points[i][0]->n = Points[i][1]->n;

	for(i=1;i<nx-1;i++)
		Points[i][ny-1]->n = Points[i][ny-2]->n;

	for(j=1;j<ny-1;j++)
		Points[0][j]->n = Points[1][j]->n;

	for(j=1;j<ny-1;j++)
		Points[nx-1][j]->n =  Points[nx-2][j]->n;


	// na rogach

	Points[0][0]->n = Points[1][0]->n + Points[0][1]->n;
	Points[0][0]->n.norm();

	Points[0][ny-1]->n = Points[0][ny-2]->n + Points[1][ny-1]->n;
	Points[0][ny-1]->n.norm();

	Points[nx-1][ny-1]->n = Points[nx-1][ny-2]->n + Points[nx-2][ny-1]->n;
	Points[nx-1][ny-1]->n.norm();

	Points[nx-1][0]->n = Points[nx-1][1]->n + Points[nx-2][0]->n;
	Points[nx-1][0]->n.norm();

}



void Cloth::MakeFlexSprings(void)
{
	for(i=0;i<nx-1;i++)
	for(j=0;j<ny-1;j++)
		Springs3[i][j] = new Sprezyna(Points[i][j],Points[i+1][j+1],ks,kd,0,S_RYSL,lenght_constrain);

	for(j=0;j<ny-1;j++)
	for(i=1;i<nx;i++)
		Springs4[i][j] = new Sprezyna(Points[i][j],Points[i-1][j+1],ks,kd,0,S_RYSL,lenght_constrain);
}


void Cloth::MakeStructuralSprings(void)
{
	for(i=0;i<nx;i++)
	for(j=0;j<ny-1;j++)
		Springs1[i][j] = new Sprezyna(Points[i][j],Points[i][j+1],ks,kd,0,S_RYSL,lenght_constrain);

	for(j=0;j<ny;j++)
	for(i=0;i<nx-1;i++)
		Springs2[i][j] = new Sprezyna(Points[i][j],Points[i+1][j],ks,kd,0,S_RYSL,lenght_constrain);

	// flex springs

	for(i=0;i<nx-2;i++)
	for(j=0;j<ny;j++)
		Springs5[i][j] = new Sprezyna(Points[i][j],Points[i+2][j],ks*3,kd,0,S_RYSL,lenght_constrain);

	for(j=0;j<ny-2;j++)
	for(i=0;i<nx;i++)
		Springs6[i][j] = new Sprezyna(Points[i][j],Points[i][j+2],ks*3,kd,0,S_RYSL,lenght_constrain);

}


/*

	for(i=0;i<NX-1;i++)
	for(j=0;j<NY;j++)
		StructuralSpringsX[i][j] = AlokujSprezyne(s_kd,0,s_ks,Surface[i][j],Surface[i+1][j],S_OK|S_RYSL);	
*/

void Cloth::CreatePoints(Vector3 r0, Vector3 r1, float mass)
{
// nx,ny
// x0,y0,z0,x1,y1,z1

	Vector3 dr,r;

	dr = (r1-r0);
	dr.x /= (float)nx;
	dr.y /= (float)ny;
	dr.z /= (float)((nx+ny)/2);

	r=r0;
	for(j=0;j<ny;j++)
	{

		for(i=0;i<nx;i++)
		{
			r.x+=dr.x;
			Points[i][j] = new MaterialPoint(P_NORM, mass,r,Vector3(0,0,0));
		}

		r.x=r0.x;
		r.y=r.y+dr.y;
		r.z=r.z+dr.z;
	}


/* dump 

	FILE *fp;
	char work[100];
	fp=fopen("file.txt","w");

	j=10;
	for(i=0;i<nx;i++)
	{
		sprintf(work,"%f\n",Points[i][j]->r.x);
		fputs(work,fp);
	}
		fclose(fp);
*/

}


	Vector3 LightVector3 = Vector3(0.7,-1.9,-0.6);
	
// liczy oswietlenie deLamberta (ciekawe czy tak sie pisze to nazwisko?)
float deLambert(Vector3 n)
{

		return ((n * LightVector3)/2.0 + 0.63);	// + oswietlenie tla
}

void Cloth::DisplayGL(int mode)
{

		Vector3 n;
		double nasycenie;



	switch(mode)
	{
	case 0:
		glDisable(GL_LIGHTING);
		glColor3f(1,1,1);

	// rysuj MaterialPointy
		glBegin(GL_POINTS);
		for(j=0;j<ny;j++)
		for(i=0;i<nx;i++)
		{
				glVertex3f(Points[i][j]->r.x,Points[i][j]->r.y,Points[i][j]->r.z);
		}
		glEnd();
	
	
	// rysuj sprezyny
		glLineWidth(1);
		
		glBegin(GL_LINES);
		for(i=0;i<nx;i++)
		for(j=0;j<ny-1;j++)
		{
			glVertex3f(Springs1[i][j]->MaterialPoint1->r.x,Springs1[i][j]->MaterialPoint1->r.y,Springs1[i][j]->MaterialPoint1->r.z);
			glVertex3f(Springs1[i][j]->MaterialPoint2->r.x,Springs1[i][j]->MaterialPoint2->r.y,Springs1[i][j]->MaterialPoint2->r.z);
		}
	
		
		for(j=0;j<ny;j++)
		for(i=0;i<nx-1;i++)
		{
			glVertex3f(Springs2[i][j]->MaterialPoint1->r.x,Springs2[i][j]->MaterialPoint1->r.y,Springs2[i][j]->MaterialPoint1->r.z);
			glVertex3f(Springs2[i][j]->MaterialPoint2->r.x,Springs2[i][j]->MaterialPoint2->r.y,Springs2[i][j]->MaterialPoint2->r.z);
		}
	
		for(i=0;i<nx-1;i++)
		for(j=0;j<ny-1;j++)
		{
			glVertex3f(Springs3[i][j]->MaterialPoint1->r.x,Springs3[i][j]->MaterialPoint1->r.y,Springs3[i][j]->MaterialPoint1->r.z);
			glVertex3f(Springs3[i][j]->MaterialPoint2->r.x,Springs3[i][j]->MaterialPoint2->r.y,Springs3[i][j]->MaterialPoint2->r.z);
		}
	
		for(j=0;j<ny-1;j++)
		for(i=1;i<nx;i++)
		{
			glVertex3f(Springs4[i][j]->MaterialPoint1->r.x,Springs4[i][j]->MaterialPoint1->r.y,Springs4[i][j]->MaterialPoint1->r.z);
			glVertex3f(Springs4[i][j]->MaterialPoint2->r.x,Springs4[i][j]->MaterialPoint2->r.y,Springs4[i][j]->MaterialPoint2->r.z);
		}
	
		glEnd();
		
		break;	

	case 1:
			// rysuj sprezyny
		glLineWidth(1);
		glDisable(GL_LIGHTING);
		glBegin(GL_LINES);
		for(i=0;i<nx-2;i++)		// x
		for(j=0;j<ny;j++)
		{
			glVertex3f(Springs5[i][j]->MaterialPoint1->r.x,Springs5[i][j]->MaterialPoint1->r.y,Springs5[i][j]->MaterialPoint1->r.z);
			glVertex3f(Springs5[i][j]->MaterialPoint2->r.x,Springs5[i][j]->MaterialPoint2->r.y,Springs5[i][j]->MaterialPoint2->r.z);
		}

		for(j=0;j<ny-2;j++)			// y
		for(i=0;i<nx;i++)
		{
			glVertex3f(Springs6[i][j]->MaterialPoint1->r.x,Springs6[i][j]->MaterialPoint1->r.y,Springs6[i][j]->MaterialPoint1->r.z);
			glVertex3f(Springs6[i][j]->MaterialPoint2->r.x,Springs6[i][j]->MaterialPoint2->r.y,Springs6[i][j]->MaterialPoint2->r.z);
		}
		
		glEnd();

		break;

	case 2:
		// oswietlenie d'Lamberta, korzysta z CalculateVertexNormal

//		CalculateVertexNormals();


		glDisable(GL_LIGHTING);

		glBegin(GL_TRIANGLES);
			for(j=0;j<ny-1;j++)				// wiatr na trojkaty 1
			for(i=0;i<nx-1;i++)
			{

				nasycenie =deLambert(Points[i][j]->n);
				glColor3f(nasycenie,nasycenie,nasycenie);
				glVertex3f(Points[i][j]->r.x,Points[i][j]->r.y,Points[i][j]->r.z);

				nasycenie = deLambert(Points[i+1][j]->n);
				glColor3f(nasycenie,nasycenie,nasycenie);
				glVertex3f(Points[i+1][j]->r.x,Points[i+1][j]->r.y,Points[i+1][j]->r.z);

				nasycenie = deLambert(Points[i][j+1]->n);
				glColor3f(nasycenie,nasycenie,nasycenie);
				glVertex3f(Points[i][j+1]->r.x,Points[i][j+1]->r.y,Points[i][j+1]->r.z);
			}


			for(j=0;j<ny;j++)				// wiatr na trojkaty 2
			for(i=1;i<nx;i++)
			{
				nasycenie = deLambert(Points[i][j]->n);
				glColor3f(nasycenie,nasycenie,nasycenie);
				glVertex3f(Points[i][j]->r.x,Points[i][j]->r.y,Points[i][j]->r.z);

				nasycenie = deLambert(Points[i][j+1]->n);
				glColor3f(nasycenie,nasycenie,nasycenie);
				glVertex3f(Points[i][j+1]->r.x,Points[i][j+1]->r.y,Points[i][j+1]->r.z);

				nasycenie = deLambert(Points[i-1][j+1]->n);
				glColor3f(nasycenie,nasycenie,nasycenie);
				glVertex3f(Points[i-1][j+1]->r.x,Points[i-1][j+1]->r.y,Points[i-1][j+1]->r.z);
			}
		glEnd();

		break;



	case 3:		// teksturowanie

		CalculateVertexNormals();

		glShadeModel(GL_SMOOTH);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		glColor4f(0.5,0.5,0.5,0.5);
		glBindTexture(GL_TEXTURE_2D,cloth_txt);	

		glBegin(GL_TRIANGLES);
			for(j=0;j<ny-1;j++)				// wiatr na trojkaty 1
			for(i=0;i<nx-1;i++)
			{
				nasycenie =deLambert(Points[i][j]->n);
				glColor4f(nasycenie,nasycenie,nasycenie,1);
				glTexCoord2f((float)i/(float)(nx-1),1-(float)j/(float)(ny-1));
				glVertex3f(Points[i][j]->r.x,Points[i][j]->r.y,Points[i][j]->r.z);

				nasycenie = deLambert(Points[i+1][j]->n);
				glColor4f(nasycenie,nasycenie,nasycenie,1);
				glTexCoord2f((float)(i+1)/(float)(nx-1),1-(float)j/(float)(ny-1));
				glVertex3f(Points[i+1][j]->r.x,Points[i+1][j]->r.y,Points[i+1][j]->r.z);

				nasycenie = deLambert(Points[i][j+1]->n);
				glColor4f(nasycenie,nasycenie,nasycenie,1);
				glTexCoord2f((float)(i)/(float)(nx-1),1-(float)(j+1)/(float)(ny-1));
				glVertex3f(Points[i][j+1]->r.x,Points[i][j+1]->r.y,Points[i][j+1]->r.z);
			}


			for(j=0;j<ny-1;j++)				// wiatr na trojkaty 2
			for(i=1;i<nx;i++)
			{
				nasycenie = deLambert(Points[i][j]->n);
				glColor4f(nasycenie,nasycenie,nasycenie,1);
				glTexCoord2f((float)(i)/(float)(nx-1),1-(float)(j)/(float)(ny-1));
				glVertex3f(Points[i][j]->r.x,Points[i][j]->r.y,Points[i][j]->r.z);

				nasycenie = deLambert(Points[i][j+1]->n);
				glColor4f(nasycenie,nasycenie,nasycenie,1);
				glTexCoord2f((float)(i)/(float)(nx-1),1-(float)(j+1)/(float)(ny-1));
				glVertex3f(Points[i][j+1]->r.x,Points[i][j+1]->r.y,Points[i][j+1]->r.z);

				nasycenie = deLambert(Points[i-1][j+1]->n);
				glColor4f(nasycenie,nasycenie,nasycenie,1);
				glTexCoord2f((float)(i-1)/(float)(nx-1),1-(float)(j+1)/(float)(ny-1));
				glVertex3f(Points[i-1][j+1]->r.x,Points[i-1][j+1]->r.y,Points[i-1][j+1]->r.z);
			}
		glEnd();

		glDisable(GL_BLEND);
		glDepthFunc(GL_LESS);
		glDisable(GL_TEXTURE_2D);

		break;



	default:
		break;
	}

	
}


