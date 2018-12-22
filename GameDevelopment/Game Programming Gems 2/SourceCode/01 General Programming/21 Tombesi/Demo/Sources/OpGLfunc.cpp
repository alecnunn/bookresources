/**********************************************************************
 	MODULE: SkelDemo
	
	DESCRIPTION: This module is a demo of how to pratically use MRC data
				 to finally render the animation in an OpenGl window.				 	
	
	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2001
 **********************************************************************/

#include "OpGL.h"

// ============================================================================
// display a bidimensional grid with 20 divisions on XZ plane
void DisplayGrid(float dim, const color col)
{
	float x0 = - dim*0.5f;
	float z0 = - dim*0.5f;
	int delta = (int) (dim / 20);
	
	glDisable(GL_LIGHTING);
	glLineWidth(1.0);
	glColor4fv(col);

	int tmp = 0;
	for(int i = 0; i < 21; i++, tmp += delta)
	{
		if(i == 10)
			continue;
		glBegin(GL_LINES);
			glVertex3f(x0+tmp, 0.f, z0);
			glVertex3f(x0+tmp, 0.f, z0+dim);
		glEnd();
	}

	tmp = 0;
	for(i = 0; i < 21; i++, tmp += delta)
	{
		if(i == 10)
			continue;
		glBegin(GL_LINES);
			glVertex3f(x0, 0.f, z0+tmp);
			glVertex3f(x0+dim, 0.f, z0+tmp);
		glEnd();
	}

	glLineWidth(2.0);
	glColor4fv(white);
	glBegin(GL_LINES);
		glVertex3f(x0, 0.f, 0.f);
		glVertex3f(x0+dim, 0.f, 0.f);
		glVertex3f(0.f, 0.f, z0);
		glVertex3f(0.f, 0.f, z0+dim);
	glEnd();

	glLineWidth(1.0);
	glEnable(GL_LIGHTING);
}

// ============================================================================
// display each vertex normal with f as magnitude
void DisplayNormals(_Skin *pSkin, const float s)
{
	vect *Vert = pSkin->TempVert;

	faceIdx *face = pSkin->Face;
	vect* Norm = pSkin->Norm;

	glDisable(GL_LIGHTING);
	glColor4fv(yellow);
			
	glBegin(GL_LINES);
	for(int i = 0; i < pSkin->FaceCnt; ++i)
	{
		vect dst;

		glVertex3fv(Vert[face[i][0]]);
		VectMul(dst, Norm[face[i][0]], s);
		VectAdd(dst, Vert[face[i][0]], dst);
		glVertex3fv(dst);
			
		glVertex3fv(Vert[face[i][1]]);
		VectMul(dst, Norm[face[i][1]], s);
		VectAdd(dst, Vert[face[i][1]], dst);
		glVertex3fv(dst);
			
		glVertex3fv(Vert[face[i][2]]);
		VectMul(dst, Norm[face[i][2]], s);
		VectAdd(dst, Vert[face[i][2]], dst);
		glVertex3fv(dst);
	}
	glEnd();
	
	glEnable(GL_LIGHTING);
}

// ============================================================================
// Display the mesh
void DisplayMesh(_Skin *pSkin)
{
	vect *Vert = pSkin->TempVert;

	faceIdx *face = pSkin->Face;
	
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, pSkin->Color);
	vect* Norm = pSkin->Norm;
	glBegin(GL_TRIANGLES);
		for(int i = 0; i < pSkin->FaceCnt; ++i)
		{
			glNormal3fv(Norm[face[i][0]]);
			glVertex3fv(Vert[face[i][0]]);
			glNormal3fv(Norm[face[i][1]]);
			glVertex3fv(Vert[face[i][1]]);
			glNormal3fv(Norm[face[i][2]]);
			glVertex3fv(Vert[face[i][2]]);
		}
	glEnd();
}

// ============================================================================
// display skeletal animated structure
void DisplaySkel(const _Bone &Bone)
{
	vect posS;
	GetTranslation(Bone.BoneAnim_TM, posS);
	// draw the joint
	glPointSize(6.0);
	glBegin(GL_POINTS);             
		glColor4fv(red);
		glVertex3fv(posS);
	glEnd();
	glPointSize(1.0);
	
	glColor4fv(blue);
	glLineWidth(3.0);
	for(int i = 0; i < Bone.BoneChildsCount; i++)
	{
		vect posE;
		GetTranslation(Bone.BoneChilds[i]->BoneAnim_TM, posE);
		// draw line from bone to its child
		glBegin(GL_LINES);
			glVertex3fv(posS);
			glVertex3fv(posE);
		glEnd();
		
		//recurse into the child
		DisplaySkel(*Bone.BoneChilds[i]);
	}
	glLineWidth(1.0);
}
