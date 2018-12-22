// Misty Room
// Copyright (c) 2002 Rob James - see license.txt for details



// Object loading and drawing functions 
// Lots of unused function in here, based on different flavours of 
// shadow volum rendering

#ifdef WIN32
#  include <windows.h>
#  include <MovieMaker.h>
#  pragma warning (disable : 4786)
#endif


#include <string.h> 
#include <stdio.h>
#include <math.h>
#include <gl/glut.h>

#include "shadobject.h"

#define FPLANE 31


void SetNormals(model_s *m)
{
unsigned int x;
float l;

  printf("SetNormals\n");
    
  for (x=0; x < m->fcount; x++)
  {
     m->verts[m->faces[x].verts[0]].nx += m->faces[x].planeeq.a;
     m->verts[m->faces[x].verts[0]].ny += m->faces[x].planeeq.b;
     m->verts[m->faces[x].verts[0]].nz += m->faces[x].planeeq.c;
     
     m->verts[m->faces[x].verts[1]].nx += m->faces[x].planeeq.a;
     m->verts[m->faces[x].verts[1]].ny += m->faces[x].planeeq.b;
     m->verts[m->faces[x].verts[1]].nz += m->faces[x].planeeq.c;

     m->verts[m->faces[x].verts[2]].nx += m->faces[x].planeeq.a;
     m->verts[m->faces[x].verts[2]].ny += m->faces[x].planeeq.b;
     m->verts[m->faces[x].verts[2]].nz += m->faces[x].planeeq.c;

     //printf("%d -> %d %d %d\n",x, m->faces[x].verts[0],m->faces[x].verts[1],m->faces[x].verts[2]);
  }      
  
  for (x=0; x <  m->vcount; x++)
  {
  //printf("%f %f %f\n", m->verts[x].nx,m->verts[x].ny,m->verts[x].nz);
    l = 1.0/sqrt(m->verts[x].nx * m->verts[x].nx + 
                 m->verts[x].ny * m->verts[x].ny + 
                 m->verts[x].nz * m->verts[x].nz );
             
     m->verts[x].nx *= l;
     m->verts[x].ny *= l;
     m->verts[x].nz *= l;
     
    // printf("%d  %f %f %f\n", x, m->verts[x].nx,m->verts[x].ny,m->verts[x].nz);
   } 
}


// recurse walk through all faces to find neighbours
void SetNeighbours(model_s *model)
{
	unsigned int p1i, p2i, p1j, p2j;
	unsigned int P1i, P2i, P1j, P2j;
	unsigned int i,j,ki,kj;

	unsigned int m;
	
	printf("Set connectivity\n");

	m = model->fcount / 10;
	
	for(i=0;i<model->fcount-1;i++)
	{
		if (i%m == 0)	printf(".");
		for(j=i+1;j<model->fcount;j++)
		{
			for(ki=0;ki<3;ki++)
			{
				if(!model->faces[i].neigh[ki])
				{
					for(kj=0;kj<3;kj++)
					{
						p1i=ki;
						p1j=kj;
						p2i=(ki+1)%3;
						p2j=(kj+1)%3;
						
						p1i=model->faces[i].verts[p1i];
						p2i=model->faces[i].verts[p2i];
						p1j=model->faces[j].verts[p1j];
						p2j=model->faces[j].verts[p2j];
						
						P1i=((p1i+p2i)-abs(p1i-p2i))*0.5;
						P2i=((p1i+p2i)+abs(p1i-p2i))*0.5;
						P1j=((p1j+p2j)-abs(p1j-p2j))*0.5;
						P2j=((p1j+p2j)+abs(p1j-p2j))*0.5;
						
						if((P1i==P1j) && (P2i==P2j))
						{  //they are neighbours
							model->faces[i].neigh[ki] = j+1;     
							model->faces[j].neigh[kj] = i+1;     
						}
					}
				}
			}
		}
	}
	printf("\n");
}



void calcPlaneEQ(model_s *model, face_s *face)
{
planeeq_s		*p;

		p = &face->planeeq;

		float v0[3];
		v0[0] = model->verts[face->verts[1]].x - model->verts[face->verts[0]].x;
		v0[1] = model->verts[face->verts[1]].y - model->verts[face->verts[0]].y;
		v0[2] = model->verts[face->verts[1]].z - model->verts[face->verts[0]].z;

		float v1[3];
		v1[0] = model->verts[face->verts[2]].x - model->verts[face->verts[0]].x;
		v1[1] = model->verts[face->verts[2]].y - model->verts[face->verts[0]].y;
		v1[2] = model->verts[face->verts[2]].z - model->verts[face->verts[0]].z;

		p->a = v0[1] * v1[2] - v0[2] * v1[1];
		p->b = v0[2] * v1[0] - v0[0] * v1[2];
		p->c = v0[0] * v1[1] - v0[1] * v1[0];

		float l = sqrt(p->a * p->a + p->b *p->b + p->c * p->c);

		if(l == 0) // degenerate triangle
		{
			p->a = p->b = p->c = p->d = 0;
			return;
		}

		l = 1.0/l;
		p->a *=  l;
		p->b *=  l;
		p->c *=  l;

		p->d = -(p->a * model->verts[face->verts[0]].x + 
			     p->b * model->verts[face->verts[0]].y + 
				 p->c * model->verts[face->verts[0]].z);  // signed distance of a point on the plane from the origin
	
}





int LoadAscObject(char *mfile,  
                  model_s *model)
{
#define VFCOUNT 0
#define VLIST 1
#define FLIST 2
	
	FILE *fp;
	char buff[256];
	char *bp;
	int  stage = VFCOUNT;
	
	int fcount, vcount;
	int foffset, voffset;
	int vert,face, smoothing;
	float x,y,z;
	int  a,b,c;
	
	fcount = 0;
	vcount = 0;
	foffset = 0;
	voffset = 0;
	
	printf("Loading %s\n", mfile);
	fp = fopen(mfile, "r");
	while(fgets(buff,255,fp))
	{
		buff[strlen(buff)-1] = 0;
		//printf("[%s]\n", buff);    
		switch(stage)
		{
		case VFCOUNT:
			if (strstr(buff,"Vertices:") &&
				strstr(buff,"Faces:") )
			{
				bp = strstr(buff,"Vertices:");
				sscanf(bp,"Vertices: %d",&vcount);
				bp = strstr(buff,"Faces:");
				sscanf(bp,"Faces: %d", &fcount);
				printf("*** Verts %d   Faces %d\n", vcount, fcount);
				model->vcount += vcount;
				model->fcount += fcount;
				stage++;
			}
			break;
			
		case VLIST:
			if (strstr(buff,"Vertex list"))
				continue;
			
			if (strstr(buff,"Vertex "))
			{
				sscanf(buff,"Vertex %d: X:%f Y:%f Z:%f", &vert, &x, &y,&z);
				//printf("*** Vert %d [%f %f %f]\n",vert,x,y,z);
				model->verts[vert+voffset].x = x;
				model->verts[vert+voffset].y = y;
				model->verts[vert+voffset].z = z;

				model->verts[vert+voffset].nx = 0.0;
				model->verts[vert+voffset].ny = 0.0;
				model->verts[vert+voffset].nz = 0.0;

				if (vert+1 == vcount) stage++;
			}
			
			break;
			
		case FLIST:
			if (strstr(buff,"Face list"))
				continue;
			
			if (strstr(buff,"Face "))
			{
				sscanf(buff,"Face %d: A:%d B:%d C:%d", &face, &a, &b,&c);
				//printf("*** Face %d [%d %d %d]\n",face,a,b,c);
				model->faces[face+foffset].verts[0] = a;
				model->faces[face+foffset].verts[1] = b;
				model->faces[face+foffset].verts[2] = c;
			}      
			
			if (strstr(buff,"Smoothing:"))
			{
				sscanf(buff,"Smoothing: %d", &smoothing);
				//printf("*** smoothing group %d\n", smoothing);
				model->faces[face+foffset].smoothing = smoothing;
				if (face+1 == fcount)
				{
			          /* Finished this object so start again... */
			         stage = VFCOUNT;
			          foffset += fcount;
			          voffset += vcount;
			          //printf("*********************************\n");
         			}
			}      
			
			
			break;
			
		default:
			//printf("runout\n");
			;
		}
	}

	printf("v %d   f %d\n", model->vcount, model->fcount);
	
	SetNeighbours(model);             // Set Neighbour info
	
	for (unsigned int i=0;i<model->fcount;i++)  // Loop Through All faces and calc plane eq
	{
		//printf("Calc PLane Equations\n");
		calcPlaneEQ(model, &(model->faces[i]));  
	}

	SetNormals(model);
	
	return (0);
}


				  

// procedure for drawing the object 
void drawModel(model_s *model)
{
	unsigned int i, j;
	
	//glDisable(GL_TEXTURE_2D);
	//glDisable(GL_LIGHTING);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	
		
	glBegin(GL_TRIANGLES);
	for (i=0; i<model->fcount; i++)
	{
		for (j=0; j<3; j++)
		{
			/*glColor4f(	model->faces[i].planeeq.a,
						model->faces[i].planeeq.b,
						model->faces[i].planeeq.c,
						model->faces[i].planeeq.d);// just for testing*/

			glNormal3f(model->verts[model->faces[i].verts[j]].nx,
					   model->verts[model->faces[i].verts[j]].ny,
					   model->verts[model->faces[i].verts[j]].nz);
			
			glVertex3f(model->verts[model->faces[i].verts[j]].x,
					   model->verts[model->faces[i].verts[j]].y,
					   model->verts[model->faces[i].verts[j]].z);
		}
	}
	glEnd();
}

void  drawModelShadow(model_s *model, float *lp, bool showshadowvol)
{
	unsigned int   i, j, k, jj;
	unsigned int   p1, p2;
	vert_s         v1, v2;
	float         side;
	
	//set visual parameter
	for (i=0;i<model->fcount;i++)
	{
		// chech to see if light is in front or behind the plane (face plane)
		side =	model->faces[i].planeeq.a*lp[0]+
				model->faces[i].planeeq.b*lp[1]+
				model->faces[i].planeeq.c*lp[2]+
				model->faces[i].planeeq.d*lp[3];
		
		if (side >0) 
			model->faces[i].visible = TRUE;
		else 
			model->faces[i].visible = FALSE;
	}
	
    
	//glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	
	glEnable(GL_STENCIL_TEST);
	
	if (showshadowvol)
		glColorMask(1, 0, 0, 0);
	else
		glColorMask(0, 0, 0, 0);
	
	glStencilFunc(GL_ALWAYS, 1, 0xffffffff);
	
	// first pass, stencil operation decreases stencil value
	glFrontFace(GL_CCW);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	for (i=0; i<model->fcount;i++)
	{
		if (model->faces[i].visible)
			for (j=0;j<3;j++)
			{
				k = model->faces[i].neigh[j];
				if ((!k) || (!model->faces[k-1].visible))
				{
					// here we have an edge, we must draw a polygon
					p1 = model->faces[i].verts[j];
					jj = (j+1)%3;
					p2 = model->faces[i].verts[jj];
					
					//calculate the length of the vector
					v1.x = (model->verts[p1].x - lp[0])*FPLANE;
					v1.y = (model->verts[p1].y - lp[1])*FPLANE;
					v1.z = (model->verts[p1].z - lp[2])*FPLANE;
					
					v2.x = (model->verts[p2].x - lp[0])*FPLANE;
					v2.y = (model->verts[p2].y - lp[1])*FPLANE;
					v2.z = (model->verts[p2].z - lp[2])*FPLANE;
					
					//draw the polygon
					glBegin(GL_TRIANGLE_STRIP);
					glVertex3f(model->verts[p1].x,
						model->verts[p1].y,
						model->verts[p1].z);
					glVertex3f(model->verts[p1].x + v1.x,
						model->verts[p1].y + v1.y,
						model->verts[p1].z + v1.z);
					
					glVertex3f(model->verts[p2].x,
						model->verts[p2].y,
						model->verts[p2].z);
					glVertex3f(model->verts[p2].x + v2.x,
						model->verts[p2].y + v2.y,
						model->verts[p2].z + v2.z);
					glEnd();
				}
			}
	}
	
	// second pass, stencil operation increases stencil value
	glFrontFace(GL_CW);
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	for (i=0; i<model->fcount;i++)
	{
		if (model->faces[i].visible)
			for (j=0;j<3;j++)
			{
				k = model->faces[i].neigh[j];
				if ((!k) || (!model->faces[k-1].visible))
				{
					// here we have an edge, we must draw a polygon
					p1 = model->faces[i].verts[j];
					jj = (j+1)%3;
					p2 = model->faces[i].verts[jj];
					
					//calculate the length of the vector
					v1.x = (model->verts[p1].x - lp[0])*FPLANE;
					v1.y = (model->verts[p1].y - lp[1])*FPLANE;
					v1.z = (model->verts[p1].z - lp[2])*FPLANE;
					
					v2.x = (model->verts[p2].x - lp[0])*FPLANE;
					v2.y = (model->verts[p2].y - lp[1])*FPLANE;
					v2.z = (model->verts[p2].z - lp[2])*FPLANE;
					
					//draw the polygon
					glBegin(GL_TRIANGLE_STRIP);
					glVertex3f(model->verts[p1].x,
						model->verts[p1].y,
						model->verts[p1].z);
					glVertex3f(model->verts[p1].x + v1.x,
						model->verts[p1].y + v1.y,
						model->verts[p1].z + v1.z);
					
					glVertex3f(model->verts[p2].x,
						model->verts[p2].y,
						model->verts[p2].z);
					glVertex3f(model->verts[p2].x + v2.x,
						model->verts[p2].y + v2.y,
						model->verts[p2].z + v2.z);
					glEnd();
				}
			}
	}
	
	glFrontFace(GL_CCW);
	glColorMask(1, 1, 1, 1);
	
	//draw a shadowing rectangle covering the entire screen
	glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glStencilFunc(GL_NOTEQUAL, 0, 0xffffffff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(-0.1f, 0.1f,-0.11f);
	glVertex3f(-0.1f,-0.1f,-0.11f);
	glVertex3f( 0.1f, 0.1f,-0.11f);
	glVertex3f( 0.1f,-0.1f,-0.11f);
	glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);
	
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	//glEnable(GL_LIGHTING);
	glDisable(GL_STENCIL_TEST);
	glShadeModel(GL_SMOOTH);
}



void drawfrontcap (model_s *model, int i)
{
	int j;
	
	glBegin(GL_TRIANGLES);
	/*
	glColor4f(	model->faces[i].planeeq.a,
				model->faces[i].planeeq.b,
				model->faces[i].planeeq.c,
				model->faces[i].planeeq.d);*/

	for (j=2; j>=0; j--)
	{
		glNormal3f(model->faces[i].norms[j].x,
				   model->faces[i].norms[j].y,
				   model->faces[i].norms[j].z);
		
		glVertex3f(model->verts[model->faces[i].verts[j]].x,
				   model->verts[model->faces[i].verts[j]].y,
				   model->verts[model->faces[i].verts[j]].z);
	}
	glEnd();
}


void drawbackcap (model_s *model, int i, float *lp, float pscale)
{
	unsigned int   p1, p2, p3;
	vert_s         v1, v2, v3;
	
	//project back facing poly's out far far away
	// reverse winding order
	p1 = model->faces[i].verts[0];
	p2 = model->faces[i].verts[1];
	p3 = model->faces[i].verts[2];
	
	//calculate the length of the vector
	v1.x = (model->verts[p1].x - lp[0])*FPLANE*pscale;
	v1.y = (model->verts[p1].y - lp[1])*FPLANE*pscale;
	v1.z = (model->verts[p1].z - lp[2])*FPLANE*pscale;
	
	v2.x = (model->verts[p2].x - lp[0])*FPLANE*pscale;
	v2.y = (model->verts[p2].y - lp[1])*FPLANE*pscale;
	v2.z = (model->verts[p2].z - lp[2])*FPLANE*pscale;
	
	
	v3.x = (model->verts[p3].x - lp[0])*FPLANE*pscale;
	v3.y = (model->verts[p3].y - lp[1])*FPLANE*pscale;
	v3.z = (model->verts[p3].z - lp[2])*FPLANE*pscale;
	
	//draw the polygon
	glBegin(GL_TRIANGLES);

	/*
	glColor4f(	model->faces[i].planeeq.a,
				model->faces[i].planeeq.b,
				model->faces[i].planeeq.c,
				model->faces[i].planeeq.d);*/

	
	glVertex3f(model->verts[p1].x + v1.x,
			   model->verts[p1].y + v1.y,
			   model->verts[p1].z + v1.z);
	
	glVertex3f(model->verts[p2].x + v2.x,
			   model->verts[p2].y + v2.y,
			   model->verts[p2].z + v2.z);
	
	glVertex3f(model->verts[p3].x + v3.x,
			   model->verts[p3].y + v3.y,
			   model->verts[p3].z + v3.z);
	
	glEnd();
}

void  drawModelShadowCR(model_s *model, float *lp, bool showshadowvol,float pscale, bool final)
{
	// Carmacks Reverse
	unsigned int   i, j, k, jj;
	unsigned int   p1, p2;
	vert_s         v1, v2;
	float         side;
	
	//set visual parameter
	for (i=0;i<model->fcount;i++)
	{
		// chech to see if light is in front or behind the plane (face plane)
		side =	model->faces[i].planeeq.a*lp[0]+
				model->faces[i].planeeq.b*lp[1]+
				model->faces[i].planeeq.c*lp[2]+
				model->faces[i].planeeq.d*lp[3];
		
		if (side >= 0.0) 
		{
			//printf("face %d is visble\n",i);
			model->faces[i].visible = TRUE;
		}
		else 
		{
			//printf("face %d is not visble\n",i);
			model->faces[i].visible = FALSE;
		}
	}
    
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LESS);
	
	glEnable(GL_STENCIL_TEST);
	
	if (showshadowvol)
		glColorMask(1, 0, 0, 0);
	else
		glColorMask(0, 0, 0, 0);
	
	glStencilFunc(GL_ALWAYS, 1, 0xffffffff);
	
	// first pass, stencil operation inc stencil value
	glFrontFace(GL_CW);
	glStencilOp(GL_KEEP, GL_INCR, GL_KEEP );
	for (i=0; i<model->fcount;i++)
	{
		if (model->faces[i].visible)
		{
			for (j=0;j<3;j++)
			{
				k = model->faces[i].neigh[j];
				if ((!k) || (!model->faces[k-1].visible))
				{
					//printf("here we have an edge, we must draw a polygon\n");
					p1 = model->faces[i].verts[j];
					jj = (j+1)%3;
					p2 = model->faces[i].verts[jj];
					
					//calculate the length of the vector
					v1.x = (model->verts[p1].x - lp[0])*FPLANE*pscale;
					v1.y = (model->verts[p1].y - lp[1])*FPLANE*pscale;
					v1.z = (model->verts[p1].z - lp[2])*FPLANE*pscale;
					
					v2.x = (model->verts[p2].x - lp[0])*FPLANE*pscale;
					v2.y = (model->verts[p2].y - lp[1])*FPLANE*pscale;
					v2.z = (model->verts[p2].z - lp[2])*FPLANE*pscale;
					
					//draw the polygon
					glBegin(GL_TRIANGLE_STRIP);
					glVertex3f(model->verts[p1].x,
						model->verts[p1].y,
						model->verts[p1].z);
					glVertex3f(model->verts[p1].x + v1.x,
						model->verts[p1].y + v1.y,
						model->verts[p1].z + v1.z);
					
					glVertex3f(model->verts[p2].x,
						model->verts[p2].y,
						model->verts[p2].z);
					glVertex3f(model->verts[p2].x + v2.x,
						model->verts[p2].y + v2.y,
						model->verts[p2].z + v2.z);
					glEnd();
				}
			}
		}
	}


	
	for (i=0; i<model->fcount;i++)
	{
		if (model->faces[i].visible)
		{
			//drawfrontcap(model,i);	
		}
		else
		{
			drawfrontcap(model,i);	
			drawbackcap (model,  i, lp, pscale);
		}
	}
	
	// second pass, stencil operation increases stencil value
	glFrontFace(GL_CCW);
	glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
	for (i=0; i<model->fcount;i++)
	{
		if (model->faces[i].visible)
		{
			//drawfrontcap(model,i);
			//drawbackcap (model,  i, lp);
			
			for (j=0;j<3;j++)
			{
				k = model->faces[i].neigh[j];
				if ((!k) || (!model->faces[k-1].visible))
				{
					// here we have an edge, we must draw a polygon
					p1 = model->faces[i].verts[j];
					jj = (j+1)%3;
					p2 = model->faces[i].verts[jj];
					
					//calculate the length of the vector
					v1.x = (model->verts[p1].x - lp[0])*FPLANE*pscale;
					v1.y = (model->verts[p1].y - lp[1])*FPLANE*pscale;
					v1.z = (model->verts[p1].z - lp[2])*FPLANE*pscale;
					
					v2.x = (model->verts[p2].x - lp[0])*FPLANE*pscale;
					v2.y = (model->verts[p2].y - lp[1])*FPLANE*pscale;
					v2.z = (model->verts[p2].z - lp[2])*FPLANE*pscale;
					
					//draw the polygon
					glBegin(GL_TRIANGLE_STRIP);
					glVertex3f(model->verts[p1].x,
						model->verts[p1].y,
						model->verts[p1].z);
					glVertex3f(model->verts[p1].x + v1.x,
						model->verts[p1].y + v1.y,
						model->verts[p1].z + v1.z);
					
					glVertex3f(model->verts[p2].x,
						model->verts[p2].y,
						model->verts[p2].z);
					glVertex3f(model->verts[p2].x + v2.x,
						model->verts[p2].y + v2.y,
						model->verts[p2].z + v2.z);
					glEnd();
				}
			}
		}
		else
		{
		  //faces away from light
		   //drawbackcap (model,  i, lp);
		}
	}


	for (i=0; i<model->fcount;i++)
	{
		if (model->faces[i].visible)
		{
			//drawfrontcap(model,i);	
		}
		else
		{
			drawfrontcap(model,i);	
			drawbackcap (model,  i, lp, pscale);
		}
	}
	
	glFrontFace(GL_CCW);
	glColorMask(1, 1, 1, 1);	
	
	//glDisable(GL_STENCIL_TEST);
	//draw a shadowing rectangle covering the entire screen
	if (final)
	{
		glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glStencilFunc(GL_NOTEQUAL, 0, 0xffffffff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glPushMatrix();
		glLoadIdentity();
		glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(-0.1f, 0.1f,-0.11f);
		glVertex3f(-0.1f,-0.1f,-0.11f);
		glVertex3f( 0.1f, 0.1f,-0.11f);
		glVertex3f( 0.1f,-0.1f,-0.11f);
		glEnd();
		glPopMatrix();
		glDisable(GL_BLEND);
	}

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	
	glDisable(GL_STENCIL_TEST);
	glShadeModel(GL_SMOOTH);
	
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}



void  drawModelShadowCRxx(model_s *model, float *lp, bool showshadowvol,float pscale, bool final)
{
	// Carmacks Reverse
	unsigned int   i, j, k, jj;
	unsigned int   p1, p2;
	vert_s         v1, v2;
	float         side;
	
	//set visual parameter
	for (i=0;i<model->fcount;i++)
	{
		// chech to see if light is in front or behind the plane (face plane)
		side =	model->faces[i].planeeq.a*lp[0]+
				model->faces[i].planeeq.b*lp[1]+
				model->faces[i].planeeq.c*lp[2]+
				model->faces[i].planeeq.d*lp[3];
		
		if (side >= 0.0) 
		{
			//printf("face %d is visble\n",i);
			model->faces[i].visible = TRUE;
		}
		else 
		{
			//printf("face %d is not visble\n",i);
			model->faces[i].visible = FALSE;
		}
	}
    
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LESS);
	
	glEnable(GL_STENCIL_TEST);
	
	if (showshadowvol)
		glColorMask(1, 0, 0, 0);
	else
		glColorMask(0, 0, 0, 0);
	
	glStencilFunc(GL_ALWAYS, 1, 0xffffffff);
	
	// first pass, stencil operation inc stencil value
	glFrontFace(GL_CW);
	glStencilOp(GL_KEEP, GL_INCR, GL_KEEP );
	for (i=0; i<model->fcount;i++)
	{
		if (model->faces[i].visible)
		{
			for (j=0;j<3;j++)
			{
				k = model->faces[i].neigh[j];
				if ((!k) || (!model->faces[k-1].visible))
				{
					//printf("here we have an edge, we must draw a polygon\n");
					p1 = model->faces[i].verts[j];
					jj = (j+1)%3;
					p2 = model->faces[i].verts[jj];
					
					//calculate the length of the vector
					v1.x = (model->verts[p1].x - lp[0])*FPLANE*pscale;
					v1.y = (model->verts[p1].y - lp[1])*FPLANE*pscale;
					v1.z = (model->verts[p1].z - lp[2])*FPLANE*pscale;
					
					v2.x = (model->verts[p2].x - lp[0])*FPLANE*pscale;
					v2.y = (model->verts[p2].y - lp[1])*FPLANE*pscale;
					v2.z = (model->verts[p2].z - lp[2])*FPLANE*pscale;
					
					//draw the polygon
					glBegin(GL_TRIANGLE_STRIP);
					glVertex3f(model->verts[p1].x,
						model->verts[p1].y,
						model->verts[p1].z);
					glVertex3f(model->verts[p1].x + v1.x,
						model->verts[p1].y + v1.y,
						model->verts[p1].z + v1.z);
					
					glVertex3f(model->verts[p2].x,
						model->verts[p2].y,
						model->verts[p2].z);
					glVertex3f(model->verts[p2].x + v2.x,
						model->verts[p2].y + v2.y,
						model->verts[p2].z + v2.z);
					glEnd();
				}
			}
		}
	}


	for (i=0; i<model->fcount;i++)
	{
		if (model->faces[i].visible)
			drawfrontcap(model,i);	
		else
			drawbackcap (model,  i, lp, pscale);
	}
	
	// second pass, stencil operation increases stencil value
	glFrontFace(GL_CCW);
	glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
	for (i=0; i<model->fcount;i++)
	{
		if (model->faces[i].visible)
		{
			//drawfrontcap(model,i);
			//drawbackcap (model,  i, lp);
			
			for (j=0;j<3;j++)
			{
				k = model->faces[i].neigh[j];
				if ((!k) || (!model->faces[k-1].visible))
				{
					// here we have an edge, we must draw a polygon
					p1 = model->faces[i].verts[j];
					jj = (j+1)%3;
					p2 = model->faces[i].verts[jj];
					
					//calculate the length of the vector
					v1.x = (model->verts[p1].x - lp[0])*FPLANE*pscale;
					v1.y = (model->verts[p1].y - lp[1])*FPLANE*pscale;
					v1.z = (model->verts[p1].z - lp[2])*FPLANE*pscale;
					
					v2.x = (model->verts[p2].x - lp[0])*FPLANE*pscale;
					v2.y = (model->verts[p2].y - lp[1])*FPLANE*pscale;
					v2.z = (model->verts[p2].z - lp[2])*FPLANE*pscale;
					
					//draw the polygon
					glBegin(GL_TRIANGLE_STRIP);
					glVertex3f(model->verts[p1].x,
						model->verts[p1].y,
						model->verts[p1].z);
					glVertex3f(model->verts[p1].x + v1.x,
						model->verts[p1].y + v1.y,
						model->verts[p1].z + v1.z);
					
					glVertex3f(model->verts[p2].x,
						model->verts[p2].y,
						model->verts[p2].z);
					glVertex3f(model->verts[p2].x + v2.x,
						model->verts[p2].y + v2.y,
						model->verts[p2].z + v2.z);
					glEnd();
				}
			}
		}
		else
		{
		  //faces away from light
		   //drawbackcap (model,  i, lp);
		}
	}
/*
	for (i=0; i<model->fcount;i++)
	{
		if (model->faces[i].visible)
			drawfrontcap(model,i);	
		else
			drawbackcap (model,  i, lp, pscale);
	}
*/
	
	for (i=0; i<model->fcount;i++)
	{
		if (model->faces[i].visible)
		{
			//drawfrontcap(model,i);	
		}
		else
		{
			drawfrontcap(model,i);	
			drawbackcap (model,  i, lp, pscale);
		}
	}
	glFrontFace(GL_CCW);
	glColorMask(1, 1, 1, 1);	
	
	//glDisable(GL_STENCIL_TEST);
	//draw a shadowing rectangle covering the entire screen
	if (final)
	{
		glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glStencilFunc(GL_NOTEQUAL, 0, 0xffffffff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glPushMatrix();
		glLoadIdentity();
		glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(-0.1f, 0.1f,-0.11f);
		glVertex3f(-0.1f,-0.1f,-0.11f);
		glVertex3f( 0.1f, 0.1f,-0.11f);
		glVertex3f( 0.1f,-0.1f,-0.11f);
		glEnd();
		glPopMatrix();
		glDisable(GL_BLEND);
	}

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	
	glDisable(GL_STENCIL_TEST);
	glShadeModel(GL_SMOOTH);
	
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void drawShadowQuad()
{

	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LESS);
	
	glEnable(GL_STENCIL_TEST);
	glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glStencilFunc(GL_NOTEQUAL, 0, 0xffffffff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(-0.1f, 0.1f,-0.11f);
	glVertex3f(-0.1f,-0.1f,-0.11f);
	glVertex3f( 0.1f, 0.1f,-0.11f);
	glVertex3f( 0.1f,-0.1f,-0.11f);
	glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	
	glDisable(GL_STENCIL_TEST);
	glShadeModel(GL_SMOOTH);
	
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

}


void  drawModelShadowVolume(model_s *model, float *lp, float pscale)
{
// Carmacks Reverse
	unsigned int   i, j, k, jj;
	unsigned int   p1, p2;
	vert_s         v1, v2;
	float         side;
	
	//set visual parameter
	for (i=0;i<model->fcount;i++)
	{
		// chech to see if light is in front or behind the plane (face plane)
		side =	model->faces[i].planeeq.a*lp[0]+
				model->faces[i].planeeq.b*lp[1]+
				model->faces[i].planeeq.c*lp[2]+
				model->faces[i].planeeq.d*lp[3];
		
		if (side >= 0.0) 
		{
			//printf("face %d is visble\n",i);
			model->faces[i].visible = TRUE;
		}
		else 
		{
			//printf("face %d is not visble\n",i);
			model->faces[i].visible = FALSE;
		}
	}
    
	for (i=0; i<model->fcount;i++)
	{
		if (model->faces[i].visible)
		{
			for (j=0;j<3;j++)
			{
				k = model->faces[i].neigh[j];
				if ((!k) || (!model->faces[k-1].visible))
				{
					//printf("here we have an edge, we must draw a polygon\n");
					p1 = model->faces[i].verts[j];
					jj = (j+1)%3;
					p2 = model->faces[i].verts[jj];
					
					//calculate the length of the vector
					v1.x = (model->verts[p1].x - lp[0])*FPLANE*pscale;
					v1.y = (model->verts[p1].y - lp[1])*FPLANE*pscale;
					v1.z = (model->verts[p1].z - lp[2])*FPLANE*pscale;
					
					v2.x = (model->verts[p2].x - lp[0])*FPLANE*pscale;
					v2.y = (model->verts[p2].y - lp[1])*FPLANE*pscale;
					v2.z = (model->verts[p2].z - lp[2])*FPLANE*pscale;
					
					//draw the polygon
					glBegin(GL_TRIANGLE_STRIP);

					glVertex3f(model->verts[p1].x,
						model->verts[p1].y,
						model->verts[p1].z);
					glVertex3f(model->verts[p1].x + v1.x,
						model->verts[p1].y + v1.y,
						model->verts[p1].z + v1.z);
					
					glVertex3f(model->verts[p2].x,
						model->verts[p2].y,
						model->verts[p2].z);
					glVertex3f(model->verts[p2].x + v2.x,
						model->verts[p2].y + v2.y,
						model->verts[p2].z + v2.z);
					glEnd();
				}
			}
		}
	}


	for (i=0; i<model->fcount;i++)
	{
		if (model->faces[i].visible)
		{
			//drawfrontcap(model,i);	
		}
		else
		{
			drawfrontcap(model,i);	
			drawbackcap (model,  i, lp, pscale);
		}
	}
}
