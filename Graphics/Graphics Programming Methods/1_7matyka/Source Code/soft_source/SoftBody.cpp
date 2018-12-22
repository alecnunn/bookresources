


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include "Vector3.h"
#include "materialpoint.h"
#include "SoftBody.h"

using namespace std;

extern unsigned int texture1;
extern unsigned int texture2;


int step=0;


void changecol(void)
{
	if(step==0)
	{
		glColor3f(1,0,0);
		step=1;
	} else
	{
		glColor3f(1,1,1);
		step=0;
	}
}



// draw normal vector
//
// assumption of placing between glbegin(gl_lines), glend()
//

void show_normal3f(MaterialPoint *p, Vector3 &n)
{
	glVertex3f(p->r.x,p->r.y,p->r.z);
	glVertex3f(p->r.x+n.x*0.2,p->r.y+n.y*0.2,p->r.z+n.z*0.2);
}

void show_normal3f2(MaterialPoint *p, Vector3 &n)
{
	glVertex3f(p->r.x,p->r.y,p->r.z);
	glVertex3f(p->r.x+n.x*0.2,p->r.y+n.y*0.2,p->r.z+n.z*0.2);
}


// return non-normalized normal  :)  to a triangle defined by p1,p2,p3
//
//
//

Vector3 norm3(MaterialPoint *p1,MaterialPoint *p2,MaterialPoint *p3)
{
	Vector3 n=(p1->r - p2->r) % (p1->r - p3->r);
//	n.norm();
	return n;
}


// recalculate normals for all SoftBody points
//
//
// average of neighboor triangle normals

void SoftBody::CalculateNormals(void)
{
	int i,j;
	Vector3 n;

	glDisable(GL_LIGHTING);
	glColor3f(1,0,0);
	glLineWidth(2);
	glBegin(GL_LINES);



		for(i=1;i<nx-1;i++)			//1 horizontal
		{	
			for(j=3;j<ny;j++)
			{

				n=  norm3(Points[i*ny+j],Points[(i+1)*ny+j],Points[(i+1)*ny+j+1]);	
				n=n+norm3(Points[i*ny+j],Points[(i+1)*ny+j+1],Points[(i)*ny+j+1]);	
				n=n+norm3(Points[i*ny+j],Points[(i)*ny+j+1],Points[(i-1)*ny+j+1]);	
				n=n+norm3(Points[i*ny+j],Points[(i-1)*ny+j+1],Points[(i-1)*ny+j]);	
				n=n+norm3(Points[i*ny+j],Points[(i-1)*ny+j],Points[(i-1)*ny+j-1]);	
				n=n+norm3(Points[i*ny+j],Points[(i-1)*ny+j-1],Points[(i)*ny+j-1]);	
				n=n+norm3(Points[i*ny+j],Points[(i)*ny+j-1],Points[(i+1)*ny+j-1]);	
				n=n+norm3(Points[i*ny+j],Points[(i+1)*ny+j-1],Points[(i+1)*ny+j]);	
				
				n.norm();
				Normals[j+i*ny]=n;				
		
	//			show_normal3f(Points[j+i*ny],Normals[j+i*ny]);
			}
		}

		// 2 vertical
		i=0;
//		i=nx-1;
		for(j=3;j<ny;j++)
		{
			n=  norm3(Points[i*ny+j],Points[(i+1)*ny+j],Points[(i+1)*ny+j+1]);	
			n=n+norm3(Points[i*ny+j],Points[(i+1)*ny+j+1],Points[(i)*ny+j+1]);	
			n=n+norm3(Points[i*ny+j],Points[(i)*ny+j+1],Points[(nx-1)*ny+j+1]);	
			n=n+norm3(Points[i*ny+j],Points[(nx-1)*ny+j+1],Points[(nx-1)*ny+j]);	
			n=n+norm3(Points[i*ny+j],Points[(nx-1)*ny+j],Points[(nx-1)*ny+j-1]);	
			n=n+norm3(Points[i*ny+j],Points[(nx-1)*ny+j-1],Points[(i)*ny+j-1]);	
			n=n+norm3(Points[i*ny+j],Points[(i)*ny+j-1],Points[(i+1)*ny+j-1]);	
			n=n+norm3(Points[i*ny+j],Points[(i+1)*ny+j-1],Points[(i+1)*ny+j]);	
		
			n.norm();
			Normals[j+i*ny]=n;				
		
	//		show_normal3f(Points[j+i*ny],Normals[j+i*ny]);
		}


		// 2 vertical
//		i=0;
		i=nx-1;
		for(j=3;j<ny;j++)
		{
			n=  norm3(Points[i*ny+j],Points[j],Points[j+1]);	
			n=n+norm3(Points[i*ny+j],Points[j+1],Points[(i)*ny+j+1]);	
			n=n+norm3(Points[i*ny+j],Points[(i)*ny+j+1],Points[(i-1)*ny+j+1]);	
			n=n+norm3(Points[i*ny+j],Points[(i-1)*ny+j+1],Points[(i-1)*ny+j]);	
			n=n+norm3(Points[i*ny+j],Points[(i-1)*ny+j],Points[(i-1)*ny+j-1]);	
			n=n+norm3(Points[i*ny+j],Points[(i-1)*ny+j-1],Points[(i)*ny+j-1]);	
			n=n+norm3(Points[i*ny+j],Points[(i)*ny+j-1],Points[j-1]);	
			n=n+norm3(Points[i*ny+j],Points[j-1],Points[j]);	
		
			n.norm();
			Normals[j+i*ny]=n;				
		
	//		show_normal3f(Points[j+i*ny],Normals[j+i*ny]);
		}

		j=ny;
		for(i=1;i<nx-1;i++)			//1 horizontal bottom
		{
			n=norm3(Points[i*ny+j],Points[(i-1)*ny+j],Points[(i-1)*ny+j-1]);	
			n=n+norm3(Points[i*ny+j],Points[(i-1)*ny+j-1],Points[(i)*ny+j-1]);	
			n=n+norm3(Points[i*ny+j],Points[(i)*ny+j-1],Points[(i+1)*ny+j-1]);	
			n=n+norm3(Points[i*ny+j],Points[(i+1)*ny+j-1],Points[(i+1)*ny+j]);	
			
			n=n+norm3(Points[i*ny+j],Points[(i+1)*ny+j],Points[(i+1)*ny+(j-1)]);	
			n=n+norm3(Points[i*ny+j],Points[(i)*ny+j-1],Points[(i-1)*ny+(j)]);	

			n.norm();
			Normals[j+i*ny]=n;				
		
	//		show_normal3f(Points[j+i*ny],Normals[j+i*ny]);
		}


		j=2;
		for(i=1;i<nx-1;i++)			//1 horizontal top
		{
			n=norm3(Points[i*ny+j],Points[(i-1)*ny+j],Points[(i-1)*ny+j+1]);	
			n=n+norm3(Points[i*ny+j],Points[(i-1)*ny+j+1],Points[(i)*ny+j+1]);	
			n=n+norm3(Points[i*ny+j],Points[(i)*ny+j+1],Points[(i+1)*ny+j+1]);	
			n=n+norm3(Points[i*ny+j],Points[(i+1)*ny+j+1],Points[(i+1)*ny+j]);	
			
			n=n+norm3(Points[i*ny+j],Points[(i+1)*ny+j],Points[(i+1)*ny+(j+1)]);	
			n=n+norm3(Points[i*ny+j],Points[(i)*ny+j+1],Points[(i-1)*ny+(j)]);	

			n.norm();
			Normals[j+i*ny]=n*(-1);			// backwards				
		
	//		show_normal3f(Points[j+i*ny],Normals[j+i*ny]);
		}


		// top
		//
		//

		i=nx-1;
		j=2;

		n=norm3(Points[i*ny+j],Points[(i-1)*ny+j],Points[(i-1)*ny+j+1]);	
		n=n+norm3(Points[i*ny+j],Points[(i-1)*ny+j+1],Points[(i)*ny+j+1]);	
		n=n+norm3(Points[i*ny+j],Points[(i)*ny+j+1],Points[j+1]);	
		n=n+norm3(Points[i*ny+j],Points[j+1],Points[j]);	
		
		n=n+norm3(Points[i*ny+j],Points[j],Points[(j+1)]);	
		n=n+norm3(Points[i*ny+j],Points[(i)*ny+j+1],Points[(i-1)*ny+(j)]);	

		n.norm();
		Normals[j+i*ny]=n*(-1);			// backwards				
		
	//	show_normal3f(Points[j+i*ny],Normals[j+i*ny]);

		//
		i=0;
		j=2;
	
		n=norm3(Points[i*ny+j],Points[(nx-1)*ny+j],Points[(nx-1)*ny+j+1]);	
		n=n+norm3(Points[i*ny+j],Points[(nx-1)*ny+j+1],Points[(i)*ny+j+1]);	
		n=n+norm3(Points[i*ny+j],Points[(i)*ny+j+1],Points[(i+1)*ny+j+1]);	
		n=n+norm3(Points[i*ny+j],Points[(i+1)*ny+j+1],Points[(i+1)*ny+j]);	
		
		n=n+norm3(Points[i*ny+j],Points[(i+1)*ny+j],Points[(i+1)*ny+(j+1)]);	
		n=n+norm3(Points[i*ny+j],Points[(i)*ny+j+1],Points[(nx-1)*ny+(j)]);	

		n.norm();
		Normals[j+i*ny]=n*(-1);			// backwards				
		
	//	show_normal3f(Points[j+i*ny],Normals[j+i*ny]);






		// bottom

		i=0;
		j=ny;

		n=norm3(Points[i*ny+j],Points[(nx-1)*ny+j],Points[(nx-1)*ny+nx-1]);	
		n=n+norm3(Points[i*ny+j],Points[(nx-1)*ny+j-1],Points[(i)*ny+nx-1]);	
		n=n+norm3(Points[i*ny+j],Points[(i)*ny+j-1],Points[(i+1)*ny+nx-1]);	
		n=n+norm3(Points[i*ny+j],Points[(i+1)*ny+j-1],Points[(i+1)*ny+j]);	
		
		n=n+norm3(Points[i*ny+j],Points[(i+1)*ny+j],Points[(i+1)*ny+(nx-1)]);	
		n=n+norm3(Points[i*ny+j],Points[(i)*ny+j-1],Points[(nx-1)*ny+(j)]);	

		n.norm();
		Normals[j+i*ny]=n;				
		
	//	show_normal3f(Points[j+i*ny],Normals[j+i*ny]);

		i=nx-1;
		j=ny;

		n=norm3(Points[i*ny+j],Points[(i-1)*ny+j],Points[(i-1)*ny+j-1]);	
		n=n+norm3(Points[i*ny+j],Points[(i-1)*ny+j-1],Points[(i)*ny+j-1]);	
		n=n+norm3(Points[i*ny+j],Points[(i)*ny+j-1],Points[j-1]);	
		n=n+norm3(Points[i*ny+j],Points[j-1],Points[j]);	
		
		n=n+norm3(Points[i*ny+j],Points[j],Points[(j-1)]);	
		n=n+norm3(Points[i*ny+j],Points[(i)*ny+j-1],Points[(i-1)*ny+(j)]);	

		n.norm();
		Normals[j+i*ny]=n;				
		
	//	show_normal3f(Points[j+i*ny],Normals[j+i*ny]);




	
		
		
		// noormalna top kombinacja z y-1

		n=Normals[2];
		for(i=1;i<=nx-1;i++)
		{
			n=n+Normals[2+i*ny];
		}
		n.norm();
		Normals[1]=n;
	//	show_normal3f(Points[1],Normals[1]);

	//	show_normal3f(Points[0],Normals[0]);
	//	show_normal3f(Points[nx*ny+1],Normals[nx*ny+1]);

		n=Normals[nx*ny];
		for(i=1;i<=nx-1;i++)
		{
			n=n+Normals[nx*ny-i*ny];
		}
		n.norm();
		Normals[nx*ny+1]=n;
	//	show_normal3f(Points[nx*ny+1],Normals[nx*ny+1]);

	glEnd();
	glEnable(GL_LIGHTING);


}



void SoftBody::DisplayNormals(void)
{
	int i,k;

	glColor4f(1,1,1,1);
	glLineWidth(2);
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);


		for(k=0;k<nx;k++)			//1 horizontal
		{	
			for(i=1;i<=ny;i++)
			{
				show_normal3f(Points[i+k*ny],Normals[i+k*ny]);
			}
		}
		show_normal3f(Points[nx*ny+1],Points[nx*ny+1]->v * 0.05);
	glEnd();
	glColor4f(1,1,1,0.5);
	glEnable(GL_LIGHTING);
}


void SoftBody::DisplayVelocities(void)
{
	int i,k;

	glColor3f(1,0.4,0.4);
	glLineWidth(2);
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);

		for(k=0;k<nx-1;k++)			//1 horizontal
		{	
			for(i=2;i<ny;i++)
			{
				show_normal3f(Points[i+k*ny],Points[i+k*ny]->v * 0.05);
			}
		}

	glEnd();
	glEnable(GL_LIGHTING);
glColor4f(1,1,1,0.5);
}

void SoftBody::DisplayGL(int mode)
{
	int i,k;


	// rysuj MaterialPointy
/*	glBegin(GL_POINTS);
	for(i=0;i<NUMP;i++)
	{
		glVertex3f(Points[i]->r.x,Points[i]->r.y,Points[i]->r.z);
	}
	glEnd();

*/
/*	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glColor4f(1,1,1,1);	
	glLineWidth(3);
	glBegin(GL_LINES);
	for(i=0;i<S1_num;i++)
	{
		glVertex3f(S1[i]->MaterialPoint1->r.x,S1[i]->MaterialPoint1->r.y,S1[i]->MaterialPoint1->r.z);
		glVertex3f(S1[i]->MaterialPoint2->r.x,S1[i]->MaterialPoint2->r.y,S1[i]->MaterialPoint2->r.z);
	}
	glEnd();
*/

		

	switch(mode)
	{
	case 0:
	//	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

		glColor4f(1,1,1,1);	
	//	glEnable(GL_LIGHTING);
//		glEnable(GL_BLEND);
		glBindTexture(GL_TEXTURE_2D,texture2);
	glEnable(GL_LIGHTING);
	

  /* Enable environment mapping */

		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
 
		step=0;
		glBegin(GL_QUADS);
		for(k=0;k<nx-1;k++)			//1 horizontal
		{	
			for(i=2;i<ny;i++)
			{
//			glTexCoord2f(0,0);
				glNormal3f(Normals[i+k*ny].x,Normals[i+k*ny].y,Normals[i+k*ny].z);
				glVertex3f(Points[i+k*ny]->r.x,Points[i+k*ny]->r.y,Points[i+k*ny]->r.z);
	
//			glTexCoord2f(1,0);
				glNormal3f(Normals[i+1+k*ny].x,Normals[i+1+k*ny].y,Normals[i+1+k*ny].z);
				glVertex3f(Points[i+1+k*ny]->r.x,Points[i+1+k*ny]->r.y,Points[i+1+k*ny]->r.z);
	
//			glTexCoord2f(0,1);
				glNormal3f(Normals[i+1+ny+k*ny].x,Normals[i+1+ny+k*ny].y,Normals[i+1+ny+k*ny].z);
				glVertex3f(Points[i+1+ny+k*ny]->r.x,Points[i+1+ny+k*ny]->r.y,Points[i+1+ny+k*ny]->r.z);
	
//			glTexCoord2f(1,1);
				glNormal3f(Normals[i+ny+k*ny].x,Normals[i+ny+k*ny].y,Normals[i+ny+k*ny].z);
				glVertex3f(Points[i+ny+k*ny]->r.x,Points[i+ny+k*ny]->r.y,Points[i+ny+k*ny]->r.z);

	//			changecol();
			}
	//			changecol();
		}

		for(i=2;i<ny;i++)
		{
			glNormal3f(Normals[i].x,Normals[i].y,Normals[i].z);
			glVertex3f(Points[i]->r.x,Points[i]->r.y,Points[i]->r.z);

			glNormal3f(Normals[i+nx*ny-ny].x,Normals[i+nx*ny-ny].y,Normals[i+nx*ny-ny].z);
			glVertex3f(Points[i+nx*ny-ny]->r.x,Points[i+nx*ny-ny]->r.y,Points[i+nx*ny-ny]->r.z);

			glNormal3f(Normals[i+1+nx*ny-ny].x,Normals[i+1+nx*ny-ny].y,Normals[i+1+nx*ny-ny].z);
			glVertex3f(Points[i+1+nx*ny-ny]->r.x,Points[i+1+nx*ny-ny]->r.y,Points[i+1+nx*ny-ny]->r.z);

			glNormal3f(Normals[i+1].x,Normals[i+1].y,Normals[i+1].z);
			glVertex3f(Points[i+1]->r.x,Points[i+1]->r.y,Points[i+1]->r.z);
		}
		glEnd();
	
		glBegin(GL_TRIANGLES);
		
		for(k=0;k<nx-1;k++)			//up and bottom
		{	
			glNormal3f(Normals[1].x,Normals[1].y,Normals[1].z);
			glVertex3f(Points[1]->r.x,Points[1]->r.y,Points[1]->r.z);

			glNormal3f(Normals[1+1+k*ny].x,Normals[1+1+k*ny].y,Normals[1+1+k*ny].z);
			glVertex3f(Points[1+1+k*ny]->r.x,Points[1+1+k*ny]->r.y,Points[1+1+k*ny]->r.z);

			glNormal3f(Normals[1+1+ny+k*ny].x,Normals[1+1+ny+k*ny].y,Normals[1+1+ny+k*ny].z);
			glVertex3f(Points[1+1+ny+k*ny]->r.x,Points[1+1+ny+k*ny]->r.y,Points[1+1+ny+k*ny]->r.z);
		}
			glNormal3f(Normals[1].x,Normals[1].y,Normals[1].z);
			glVertex3f(Points[1]->r.x,Points[1]->r.y,Points[1]->r.z);


			glNormal3f(Normals[1+1+ny+(nx-2)*ny].x,Normals[1+1+ny+(nx-2)*ny].y,Normals[1+1+ny+(nx-2)*ny].z);	
			glVertex3f(Points[1+1+ny+(nx-2)*ny]->r.x,Points[1+1+ny+(nx-2)*ny]->r.y,Points[1+1+ny+(nx-2)*ny]->r.z);	

			glNormal3f(Normals[1+1].x,Normals[1+1].y,Normals[1+1].z);
			glVertex3f(Points[1+1]->r.x,Points[1+1]->r.y,Points[1+1]->r.z);

		
		for(k=0;k<nx-1;k++)			//up and bottom
		{	
			glNormal3f(Normals[ny*nx+1].x,Normals[ny*nx+1].y,Normals[ny*nx+1].z);
			glVertex3f(Points[ny*nx+1]->r.x,Points[ny*nx+1]->r.y,Points[ny*nx+1]->r.z);


			glNormal3f(Normals[2*ny+ny*k].x,Normals[2*ny+ny*k].y,Normals[2*ny+ny*k].z);
			glVertex3f(Points[2*ny+ny*k]->r.x,Points[2*ny+ny*k]->r.y,Points[2*ny+ny*k]->r.z);

			glNormal3f(Normals[ny+ny*k].x,Normals[ny+ny*k].y,Normals[ny+ny*k].z);
			glVertex3f(Points[ny+ny*k]->r.x,Points[ny+ny*k]->r.y,Points[ny+ny*k]->r.z);

		}

			glNormal3f(Normals[ny*nx+1].x,Normals[ny*nx+1].y,Normals[ny*nx+1].z);
			glVertex3f(Points[ny*nx+1]->r.x,Points[ny*nx+1]->r.y,Points[ny*nx+1]->r.z);

			glNormal3f(Normals[ny].x,Normals[ny].y,Normals[ny].z);
			glVertex3f(Points[ny]->r.x,Points[ny]->r.y,Points[ny]->r.z);

			glNormal3f(Normals[2*ny+ny*(nx-2)].x,Normals[2*ny+ny*(nx-2)].y,Normals[2*ny+ny*(nx-2)].z);
			glVertex3f(Points[2*ny+ny*(nx-2)]->r.x,Points[2*ny+ny*(nx-2)]->r.y,Points[2*ny+ny*(nx-2)]->r.z);

		glEnd();
	


		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T); 

		break;
		
	case 1:
		

		glDisable(GL_TEXTURE_2D);
		//	glDisable(GL_LIGHTING);

		glColor4f(1,1,1,1);	
	//	glEnable(GL_LIGHTING);
//		glEnable(GL_BLEND);
		glBindTexture(GL_TEXTURE_2D,texture2);
	glEnable(GL_LIGHTING);
	

  /* Enable environment mapping */

		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
 
		step=0;
		glBegin(GL_QUADS);
		for(k=0;k<nx-1;k++)			//1 horizontal
		{	
			for(i=2;i<ny;i++)
			{
//			glTexCoord2f(0,0);
				glNormal3f(Normals[i+k*ny].x,Normals[i+k*ny].y,Normals[i+k*ny].z);
				glVertex3f(Points[i+k*ny]->r.x,Points[i+k*ny]->r.y,Points[i+k*ny]->r.z);
	
//			glTexCoord2f(1,0);
				glNormal3f(Normals[i+1+k*ny].x,Normals[i+1+k*ny].y,Normals[i+1+k*ny].z);
				glVertex3f(Points[i+1+k*ny]->r.x,Points[i+1+k*ny]->r.y,Points[i+1+k*ny]->r.z);
	
//			glTexCoord2f(0,1);
				glNormal3f(Normals[i+1+ny+k*ny].x,Normals[i+1+ny+k*ny].y,Normals[i+1+ny+k*ny].z);
				glVertex3f(Points[i+1+ny+k*ny]->r.x,Points[i+1+ny+k*ny]->r.y,Points[i+1+ny+k*ny]->r.z);
	
//			glTexCoord2f(1,1);
				glNormal3f(Normals[i+ny+k*ny].x,Normals[i+ny+k*ny].y,Normals[i+ny+k*ny].z);
				glVertex3f(Points[i+ny+k*ny]->r.x,Points[i+ny+k*ny]->r.y,Points[i+ny+k*ny]->r.z);

	//			changecol();
			}
	//			changecol();
		}

		for(i=2;i<ny;i++)
		{
			glNormal3f(Normals[i].x,Normals[i].y,Normals[i].z);
			glVertex3f(Points[i]->r.x,Points[i]->r.y,Points[i]->r.z);

			glNormal3f(Normals[i+nx*ny-ny].x,Normals[i+nx*ny-ny].y,Normals[i+nx*ny-ny].z);
			glVertex3f(Points[i+nx*ny-ny]->r.x,Points[i+nx*ny-ny]->r.y,Points[i+nx*ny-ny]->r.z);

			glNormal3f(Normals[i+1+nx*ny-ny].x,Normals[i+1+nx*ny-ny].y,Normals[i+1+nx*ny-ny].z);
			glVertex3f(Points[i+1+nx*ny-ny]->r.x,Points[i+1+nx*ny-ny]->r.y,Points[i+1+nx*ny-ny]->r.z);

			glNormal3f(Normals[i+1].x,Normals[i+1].y,Normals[i+1].z);
			glVertex3f(Points[i+1]->r.x,Points[i+1]->r.y,Points[i+1]->r.z);
		}
		glEnd();
	
		glBegin(GL_TRIANGLES);
		
		for(k=0;k<nx-1;k++)			//up and bottom
		{	
			glNormal3f(Normals[1].x,Normals[1].y,Normals[1].z);
			glVertex3f(Points[1]->r.x,Points[1]->r.y,Points[1]->r.z);

			glNormal3f(Normals[1+1+k*ny].x,Normals[1+1+k*ny].y,Normals[1+1+k*ny].z);
			glVertex3f(Points[1+1+k*ny]->r.x,Points[1+1+k*ny]->r.y,Points[1+1+k*ny]->r.z);

			glNormal3f(Normals[1+1+ny+k*ny].x,Normals[1+1+ny+k*ny].y,Normals[1+1+ny+k*ny].z);
			glVertex3f(Points[1+1+ny+k*ny]->r.x,Points[1+1+ny+k*ny]->r.y,Points[1+1+ny+k*ny]->r.z);
		}
			glNormal3f(Normals[1].x,Normals[1].y,Normals[1].z);
			glVertex3f(Points[1]->r.x,Points[1]->r.y,Points[1]->r.z);


			glNormal3f(Normals[1+1+ny+(nx-2)*ny].x,Normals[1+1+ny+(nx-2)*ny].y,Normals[1+1+ny+(nx-2)*ny].z);	
			glVertex3f(Points[1+1+ny+(nx-2)*ny]->r.x,Points[1+1+ny+(nx-2)*ny]->r.y,Points[1+1+ny+(nx-2)*ny]->r.z);	

			glNormal3f(Normals[1+1].x,Normals[1+1].y,Normals[1+1].z);
			glVertex3f(Points[1+1]->r.x,Points[1+1]->r.y,Points[1+1]->r.z);

		
		for(k=0;k<nx-1;k++)			//up and bottom
		{	
			glNormal3f(Normals[ny*nx+1].x,Normals[ny*nx+1].y,Normals[ny*nx+1].z);
			glVertex3f(Points[ny*nx+1]->r.x,Points[ny*nx+1]->r.y,Points[ny*nx+1]->r.z);


			glNormal3f(Normals[2*ny+ny*k].x,Normals[2*ny+ny*k].y,Normals[2*ny+ny*k].z);
			glVertex3f(Points[2*ny+ny*k]->r.x,Points[2*ny+ny*k]->r.y,Points[2*ny+ny*k]->r.z);

			glNormal3f(Normals[ny+ny*k].x,Normals[ny+ny*k].y,Normals[ny+ny*k].z);
			glVertex3f(Points[ny+ny*k]->r.x,Points[ny+ny*k]->r.y,Points[ny+ny*k]->r.z);

		}

			glNormal3f(Normals[ny*nx+1].x,Normals[ny*nx+1].y,Normals[ny*nx+1].z);
			glVertex3f(Points[ny*nx+1]->r.x,Points[ny*nx+1]->r.y,Points[ny*nx+1]->r.z);

			glNormal3f(Normals[ny].x,Normals[ny].y,Normals[ny].z);
			glVertex3f(Points[ny]->r.x,Points[ny]->r.y,Points[ny]->r.z);

			glNormal3f(Normals[2*ny+ny*(nx-2)].x,Normals[2*ny+ny*(nx-2)].y,Normals[2*ny+ny*(nx-2)].z);
			glVertex3f(Points[2*ny+ny*(nx-2)]->r.x,Points[2*ny+ny*(nx-2)]->r.y,Points[2*ny+ny*(nx-2)]->r.z);

		glEnd();
	


		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T); 


		
	case 2:			// + Normal Vectors

//		glColor4f(.6,.7,.8,1);	
	//	glEnable(GL_LIGHTING);
//		glEnable(GL_BLEND);
		glBindTexture(GL_TEXTURE_2D,texture2);
		glColor4f(1,1,1,1);	
	
		step=0;
		glBegin(GL_QUADS);
		for(k=0;k<nx-1;k++)			//1 horizontal
		{	
			for(i=2;i<ny;i++)
			{
			glTexCoord2f(0,0);
				glNormal3f(Normals[i+k*ny].x,Normals[i+k*ny].y,Normals[i+k*ny].z);
				glVertex3f(Points[i+k*ny]->r.x,Points[i+k*ny]->r.y,Points[i+k*ny]->r.z);
	
			glTexCoord2f(1,0);
				glNormal3f(Normals[i+1+k*ny].x,Normals[i+1+k*ny].y,Normals[i+1+k*ny].z);
				glVertex3f(Points[i+1+k*ny]->r.x,Points[i+1+k*ny]->r.y,Points[i+1+k*ny]->r.z);
	
			glTexCoord2f(0,1);
				glNormal3f(Normals[i+1+ny+k*ny].x,Normals[i+1+ny+k*ny].y,Normals[i+1+ny+k*ny].z);
				glVertex3f(Points[i+1+ny+k*ny]->r.x,Points[i+1+ny+k*ny]->r.y,Points[i+1+ny+k*ny]->r.z);
	
			glTexCoord2f(1,1);
				glNormal3f(Normals[i+ny+k*ny].x,Normals[i+ny+k*ny].y,Normals[i+ny+k*ny].z);
				glVertex3f(Points[i+ny+k*ny]->r.x,Points[i+ny+k*ny]->r.y,Points[i+ny+k*ny]->r.z);

			//	changecol();
			}
		}

		for(i=2;i<ny;i++)
		{
			glNormal3f(Normals[i].x,Normals[i].y,Normals[i].z);
			glVertex3f(Points[i]->r.x,Points[i]->r.y,Points[i]->r.z);

			glNormal3f(Normals[i+nx*ny-ny].x,Normals[i+nx*ny-ny].y,Normals[i+nx*ny-ny].z);
			glVertex3f(Points[i+nx*ny-ny]->r.x,Points[i+nx*ny-ny]->r.y,Points[i+nx*ny-ny]->r.z);

			glNormal3f(Normals[i+1+nx*ny-ny].x,Normals[i+1+nx*ny-ny].y,Normals[i+1+nx*ny-ny].z);
			glVertex3f(Points[i+1+nx*ny-ny]->r.x,Points[i+1+nx*ny-ny]->r.y,Points[i+1+nx*ny-ny]->r.z);

			glNormal3f(Normals[i+1].x,Normals[i+1].y,Normals[i+1].z);
			glVertex3f(Points[i+1]->r.x,Points[i+1]->r.y,Points[i+1]->r.z);
		}
		glEnd();
	
		glBegin(GL_TRIANGLES);
		
		for(k=0;k<nx-1;k++)			//up and bottom
		{	
			glNormal3f(Normals[1].x,Normals[1].y,Normals[1].z);
			glVertex3f(Points[1]->r.x,Points[1]->r.y,Points[1]->r.z);

			glNormal3f(Normals[1+1+k*ny].x,Normals[1+1+k*ny].y,Normals[1+1+k*ny].z);
			glVertex3f(Points[1+1+k*ny]->r.x,Points[1+1+k*ny]->r.y,Points[1+1+k*ny]->r.z);

			glNormal3f(Normals[1+1+ny+k*ny].x,Normals[1+1+ny+k*ny].y,Normals[1+1+ny+k*ny].z);
			glVertex3f(Points[1+1+ny+k*ny]->r.x,Points[1+1+ny+k*ny]->r.y,Points[1+1+ny+k*ny]->r.z);
		}
			glNormal3f(Normals[1].x,Normals[1].y,Normals[1].z);
			glVertex3f(Points[1]->r.x,Points[1]->r.y,Points[1]->r.z);


			glNormal3f(Normals[1+1+ny+(nx-2)*ny].x,Normals[1+1+ny+(nx-2)*ny].y,Normals[1+1+ny+(nx-2)*ny].z);	
			glVertex3f(Points[1+1+ny+(nx-2)*ny]->r.x,Points[1+1+ny+(nx-2)*ny]->r.y,Points[1+1+ny+(nx-2)*ny]->r.z);	

			glNormal3f(Normals[1+1].x,Normals[1+1].y,Normals[1+1].z);
			glVertex3f(Points[1+1]->r.x,Points[1+1]->r.y,Points[1+1]->r.z);

		
		for(k=0;k<nx-1;k++)			//up and bottom
		{	
			glNormal3f(Normals[ny*nx+1].x,Normals[ny*nx+1].y,Normals[ny*nx+1].z);
			glVertex3f(Points[ny*nx+1]->r.x,Points[ny*nx+1]->r.y,Points[ny*nx+1]->r.z);


			glNormal3f(Normals[2*ny+ny*k].x,Normals[2*ny+ny*k].y,Normals[2*ny+ny*k].z);
			glVertex3f(Points[2*ny+ny*k]->r.x,Points[2*ny+ny*k]->r.y,Points[2*ny+ny*k]->r.z);

			glNormal3f(Normals[ny+ny*k].x,Normals[ny+ny*k].y,Normals[ny+ny*k].z);
			glVertex3f(Points[ny+ny*k]->r.x,Points[ny+ny*k]->r.y,Points[ny+ny*k]->r.z);

		}

			glNormal3f(Normals[ny*nx+1].x,Normals[ny*nx+1].y,Normals[ny*nx+1].z);
			glVertex3f(Points[ny*nx+1]->r.x,Points[ny*nx+1]->r.y,Points[ny*nx+1]->r.z);

			glNormal3f(Normals[ny].x,Normals[ny].y,Normals[ny].z);
			glVertex3f(Points[ny]->r.x,Points[ny]->r.y,Points[ny]->r.z);

			glNormal3f(Normals[2*ny+ny*(nx-2)].x,Normals[2*ny+ny*(nx-2)].y,Normals[2*ny+ny*(nx-2)].z);
			glVertex3f(Points[2*ny+ny*(nx-2)]->r.x,Points[2*ny+ny*(nx-2)]->r.y,Points[2*ny+ny*(nx-2)]->r.z);

		glEnd();
	
		break;

	case 3:
	
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,1,1);	
	glLineWidth(1);
	glBegin(GL_LINES);
	for(i=0;i<S1_num;i++)
	{
		glVertex3f(S1[i]->MaterialPoint1->r.x,S1[i]->MaterialPoint1->r.y,S1[i]->MaterialPoint1->r.z);
		glVertex3f(S1[i]->MaterialPoint2->r.x,S1[i]->MaterialPoint2->r.y,S1[i]->MaterialPoint2->r.z);
	}
	glEnd();


	// draw axis of rotated ellipsoid

	glLineWidth(5);
	glBegin(GL_LINES);
	
		glColor4f(1,0,0,1);	
		glVertex3f(Points[1]->r.x,Points[1]->r.y,Points[1]->r.z);
		glVertex3f(Points[nx*ny+1]->r.x,Points[nx*ny+1]->r.y,Points[nx*ny+1]->r.z);

		glColor4f(0,1,0,1);	
		glVertex3f(Points[ny/2+1]->r.x,Points[ny/2+1]->r.y,Points[ny/2+1]->r.z);
		glVertex3f(Points[ny/2+1+nx*ny/2]->r.x,Points[ny/2+1+nx*ny/2]->r.y,Points[ny/2+1+nx*ny/2]->r.z);

		glColor4f(0,0,1,1);	
		glVertex3f(Points[ny/2+1+nx*ny/4]->r.x,Points[ny/2+1+nx*ny/4]->r.y,Points[ny/2+1+nx*ny/4]->r.z);
		glVertex3f(Points[ny/2+1+nx*ny/2+nx*ny/4]->r.x,Points[ny/2+1+nx*ny/2+nx*ny/4]->r.y,Points[ny/2+1+nx*ny/2+nx*ny/4]->r.z);
		
	glEnd();



	glEnable(GL_TEXTURE_2D);
	break;

	// Enviroment mapping

	case 4:			

	//	glDisable(GL_LIGHTING);

		glColor4f(1,1,1,1);	
	//	glEnable(GL_LIGHTING);
//		glEnable(GL_BLEND);
		glBindTexture(GL_TEXTURE_2D,texture2);
	glEnable(GL_LIGHTING);
	

  /* Enable environment mapping */

		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
 
		step=0;
		glBegin(GL_QUADS);
		for(k=0;k<nx-1;k++)			//1 horizontal
		{	
			for(i=2;i<ny;i++)
			{
//			glTexCoord2f(0,0);
				glNormal3f(Normals[i+k*ny].x,Normals[i+k*ny].y,Normals[i+k*ny].z);
				glVertex3f(Points[i+k*ny]->r.x,Points[i+k*ny]->r.y,Points[i+k*ny]->r.z);
	
//			glTexCoord2f(1,0);
				glNormal3f(Normals[i+1+k*ny].x,Normals[i+1+k*ny].y,Normals[i+1+k*ny].z);
				glVertex3f(Points[i+1+k*ny]->r.x,Points[i+1+k*ny]->r.y,Points[i+1+k*ny]->r.z);
	
//			glTexCoord2f(0,1);
				glNormal3f(Normals[i+1+ny+k*ny].x,Normals[i+1+ny+k*ny].y,Normals[i+1+ny+k*ny].z);
				glVertex3f(Points[i+1+ny+k*ny]->r.x,Points[i+1+ny+k*ny]->r.y,Points[i+1+ny+k*ny]->r.z);
	
//			glTexCoord2f(1,1);
				glNormal3f(Normals[i+ny+k*ny].x,Normals[i+ny+k*ny].y,Normals[i+ny+k*ny].z);
				glVertex3f(Points[i+ny+k*ny]->r.x,Points[i+ny+k*ny]->r.y,Points[i+ny+k*ny]->r.z);

	//			changecol();
			}
	//			changecol();
		}

		for(i=2;i<ny;i++)
		{
			glNormal3f(Normals[i].x,Normals[i].y,Normals[i].z);
			glVertex3f(Points[i]->r.x,Points[i]->r.y,Points[i]->r.z);

			glNormal3f(Normals[i+nx*ny-ny].x,Normals[i+nx*ny-ny].y,Normals[i+nx*ny-ny].z);
			glVertex3f(Points[i+nx*ny-ny]->r.x,Points[i+nx*ny-ny]->r.y,Points[i+nx*ny-ny]->r.z);

			glNormal3f(Normals[i+1+nx*ny-ny].x,Normals[i+1+nx*ny-ny].y,Normals[i+1+nx*ny-ny].z);
			glVertex3f(Points[i+1+nx*ny-ny]->r.x,Points[i+1+nx*ny-ny]->r.y,Points[i+1+nx*ny-ny]->r.z);

			glNormal3f(Normals[i+1].x,Normals[i+1].y,Normals[i+1].z);
			glVertex3f(Points[i+1]->r.x,Points[i+1]->r.y,Points[i+1]->r.z);
		}
		glEnd();
	
		glBegin(GL_TRIANGLES);
		
		for(k=0;k<nx-1;k++)			//up and bottom
		{	
			glNormal3f(Normals[1].x,Normals[1].y,Normals[1].z);
			glVertex3f(Points[1]->r.x,Points[1]->r.y,Points[1]->r.z);

			glNormal3f(Normals[1+1+k*ny].x,Normals[1+1+k*ny].y,Normals[1+1+k*ny].z);
			glVertex3f(Points[1+1+k*ny]->r.x,Points[1+1+k*ny]->r.y,Points[1+1+k*ny]->r.z);

			glNormal3f(Normals[1+1+ny+k*ny].x,Normals[1+1+ny+k*ny].y,Normals[1+1+ny+k*ny].z);
			glVertex3f(Points[1+1+ny+k*ny]->r.x,Points[1+1+ny+k*ny]->r.y,Points[1+1+ny+k*ny]->r.z);
		}
			glNormal3f(Normals[1].x,Normals[1].y,Normals[1].z);
			glVertex3f(Points[1]->r.x,Points[1]->r.y,Points[1]->r.z);


			glNormal3f(Normals[1+1+ny+(nx-2)*ny].x,Normals[1+1+ny+(nx-2)*ny].y,Normals[1+1+ny+(nx-2)*ny].z);	
			glVertex3f(Points[1+1+ny+(nx-2)*ny]->r.x,Points[1+1+ny+(nx-2)*ny]->r.y,Points[1+1+ny+(nx-2)*ny]->r.z);	

			glNormal3f(Normals[1+1].x,Normals[1+1].y,Normals[1+1].z);
			glVertex3f(Points[1+1]->r.x,Points[1+1]->r.y,Points[1+1]->r.z);

		
		for(k=0;k<nx-1;k++)			//up and bottom
		{	
			glNormal3f(Normals[ny*nx+1].x,Normals[ny*nx+1].y,Normals[ny*nx+1].z);
			glVertex3f(Points[ny*nx+1]->r.x,Points[ny*nx+1]->r.y,Points[ny*nx+1]->r.z);


			glNormal3f(Normals[2*ny+ny*k].x,Normals[2*ny+ny*k].y,Normals[2*ny+ny*k].z);
			glVertex3f(Points[2*ny+ny*k]->r.x,Points[2*ny+ny*k]->r.y,Points[2*ny+ny*k]->r.z);

			glNormal3f(Normals[ny+ny*k].x,Normals[ny+ny*k].y,Normals[ny+ny*k].z);
			glVertex3f(Points[ny+ny*k]->r.x,Points[ny+ny*k]->r.y,Points[ny+ny*k]->r.z);

		}

			glNormal3f(Normals[ny*nx+1].x,Normals[ny*nx+1].y,Normals[ny*nx+1].z);
			glVertex3f(Points[ny*nx+1]->r.x,Points[ny*nx+1]->r.y,Points[ny*nx+1]->r.z);

			glNormal3f(Normals[ny].x,Normals[ny].y,Normals[ny].z);
			glVertex3f(Points[ny]->r.x,Points[ny]->r.y,Points[ny]->r.z);

			glNormal3f(Normals[2*ny+ny*(nx-2)].x,Normals[2*ny+ny*(nx-2)].y,Normals[2*ny+ny*(nx-2)].z);
			glVertex3f(Points[2*ny+ny*(nx-2)]->r.x,Points[2*ny+ny*(nx-2)]->r.y,Points[2*ny+ny*(nx-2)]->r.z);

		glEnd();
	


		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T); 

		
		break;

	default:
		break;
		
	}
	
	
	

}
