/****************************************/
/* An easy-to-code smoothing algorithm  */
/* for 3D reconstructed surfaces        */
/*                                      */
/* Render related functions file        */
/* -----------------------------        */
/*                                      */
/* Author: Oscar Garcia-Panyella        */
/* Date: January 2003                   */
/* Contact: oscarg@salleurl.edu         */
/****************************************/

#include "render.h"

/****************************************/
/* Some colors                          */
/****************************************/
GLfloat red[]     = { 1.0, 0.0, 0.0, 1.0 };
GLfloat green1[]   = { 0.0, 7.0, 0.0, 1.0 };
GLfloat green2[]  = { 0.0, 0.8, 0.0, 1.0 };
GLfloat blue1[]    = { 0.0, 0.0, 1.0, 1.0 };
GLfloat blue2[] = { 0.0, 0.0, 0.2, 1.0 };
GLfloat yellow1[]  = { 1.0, 1.0, 0.0, 1.0 };
GLfloat yellow2[]  = { 0.8, 0.8, 0.3, 1.0 };
GLfloat magenta[] = { 1.0, 0.0, 1.0, 1.0 };
GLfloat cyan[]    = { 0.0, 1.0, 1.0, 1.0 };
GLfloat white[]   = { 1.0, 1.0, 1.0, 1.0 };
GLfloat grey1[]    = { 0.5, 0.5, 0.5, 1.0 };
GLfloat grey2[]    = { 0.4, 0.4, 0.4, 1.0 };
GLfloat grey3[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat grey4[] = { 0.6, 0.6, 0.6, 1.0 };
GLfloat black[]   = { 0.0, 0.0, 0.0, 1.0 };
GLfloat salmon1[]   = { 1.0, 0.568, 0.568, 1.0 };
GLfloat salmon2[]   = { 1.0, 0.725, 0.725, 1.0 };
GLfloat salmon3[]   = { 1.0, 0.850, 0.850, 1.0 };

/****************************************/
/* Edge rendering                       */
/****************************************/
void drawEdges(mesh *m){
    int i;
	float v1[3], v2[3];
    
	glPolygonMode(GL_FRONT,GL_LINE);
	glColor4fv(salmon3);

	glPushMatrix();
		for(i=0;i<m->numEdges;i++){
			glBegin(GL_LINES);
				v1[0]=m->vertexs[m->edges[i].vertexs[0]].position[0];
				v1[1]=m->vertexs[m->edges[i].vertexs[0]].position[1];
				v1[2]=m->vertexs[m->edges[i].vertexs[0]].position[2];
				v2[0]=m->vertexs[m->edges[i].vertexs[1]].position[0];
				v2[1]=m->vertexs[m->edges[i].vertexs[1]].position[1];
				v2[2]=m->vertexs[m->edges[i].vertexs[1]].position[2];
				glVertex3fv(v1);
				glVertex3fv(v2);
			glEnd();
		}
	glPopMatrix();
}

/****************************************/
/* Triangle rendering                   */
/****************************************/
void drawTriangles(mesh *m){
	int i;
	float v1[3], v2[3], v3[3];

	glPolygonMode(GL_FRONT,GL_FILL);
	glColor4fv(salmon1);

	for(i=0;i<m->numTriangles;i++){
		v1[0]=m->vertexs[m->triangles[i].vertexs[0]].position[0];
		v1[1]=m->vertexs[m->triangles[i].vertexs[0]].position[1];
		v1[2]=m->vertexs[m->triangles[i].vertexs[0]].position[2];
		v2[0]=m->vertexs[m->triangles[i].vertexs[1]].position[0];
		v2[1]=m->vertexs[m->triangles[i].vertexs[1]].position[1];
		v2[2]=m->vertexs[m->triangles[i].vertexs[1]].position[2];
		v3[0]=m->vertexs[m->triangles[i].vertexs[2]].position[0];
		v3[1]=m->vertexs[m->triangles[i].vertexs[2]].position[1];
		v3[2]=m->vertexs[m->triangles[i].vertexs[2]].position[2];
		glBegin(GL_TRIANGLES);
			glVertex3fv(v1);
			glVertex3fv(v2);
			glVertex3fv(v3);
		glEnd();
	}
}
