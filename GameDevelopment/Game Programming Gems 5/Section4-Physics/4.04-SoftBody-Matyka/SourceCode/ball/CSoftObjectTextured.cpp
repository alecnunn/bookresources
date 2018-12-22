/************************************************************************/
/*                                                                      */
/*                       Soft Body 3.0                                  */
/*                 by Maciej Matyka, 21.IX.2004                         */
/*                                                                      */
/*  http://panoramix.ift.uni.wroc.pl/~maq                               */
/*                                                                      */
/*  Made for purposes of Grame Programming Gems 5 article.              */
/*                                                                      */
/*  note: this code has been attached to article  only as               */
/*  an example of described method. No any guarrantee of anything is    */
/*  is given. Use at your own risk.                                     */
/*                                                                      */
/*  if you want to use any part of that code in your project,           */
/*  email Author at:                                                    */
/*                                                                      */
/*  maq@panoramix.ift.uni.wroc.pl                                       */
/*  http://panoramix.ift.uni.wroc.pl/~maq                               */
/************************************************************************/




#include <GL/glut.h> 
#include "CSoftObject.h"
#include "CSoftObjectTextured.h"



#include "Texture.h"
#include "Bitmap.h"

void CSoftObjectTextured::glVisualize2(void) const
{
	int i;

	/**
	 * Draw my Box                                                                     
	 */

	const float xmin = -4.5;
	const float xmax = 4.5;
	const float ymin = -4.5;
	const float ymax = 4.5;
	const float zmin = -4.5;
	const float zmax = 4.5;

	glDisable(GL_LIGHTING);
	glBegin(GL_QUADS);
		glColor3f(0.44,0.53,0.62);

    	glVertex3f(xmin,ymax,zmin);
		glVertex3f(xmax,ymax,zmin);
		glVertex3f(xmax,ymax,zmax);
		glVertex3f(xmin,ymax,zmax);

		glColor3f(0.14,0.23,0.48);

		glVertex3f(xmin,ymin,zmin);
		glVertex3f(xmax,ymin,zmin);
		glVertex3f(xmax,ymin,zmax);
		glVertex3f(xmin,ymin,zmax);

		glColor3f(0.44,0.43,0.62);

		glVertex3f(xmin,ymin,zmin);
		glVertex3f(xmin,ymax,zmin);
		glVertex3f(xmin,ymax,zmax);
		glVertex3f(xmin,ymin,zmax);

		glColor3f(0.44,0.43,0.52);

		glVertex3f(xmax,ymin,zmin);
		glVertex3f(xmax,ymax,zmin);
		glVertex3f(xmax,ymax,zmax);
		glVertex3f(xmax,ymin,zmax);


	glEnd();





	// init texture

	glBindTexture(GL_TEXTURE_2D,_mytxt);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);	
	
	// disable auto uv generation

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T); 

	// set color

	glColor4f(1,1,1,1);
	
	/**
	 *	Draw Faces
	 */

	glBegin(GL_TRIANGLES);
		
//		glColor3f(0,0,1);

		for(i=0; i<_vFaces.size() ; i++)
		{
			glNormal3f(_vFaces[i]._n.x,_vFaces[i]._n.y,_vFaces[i]._n.z);
			glTexCoord2f(_uvtxt[_vFaces[i]._p1->_i*2 + 0] , 1-_uvtxt[_vFaces[i]._p1->_i*2 + 1]);
			glVertex3f(_vFaces[i]._p1->_r.x,_vFaces[i]._p1->_r.y,_vFaces[i]._p1->_r.z);

			glTexCoord2f(_uvtxt[_vFaces[i]._p2->_i*2 + 0] ,  1-_uvtxt[_vFaces[i]._p2->_i*2 + 1]);
			glVertex3f(_vFaces[i]._p2->_r.x,_vFaces[i]._p2->_r.y,_vFaces[i]._p2->_r.z);
			
			glTexCoord2f(_uvtxt[_vFaces[i]._p3->_i*2 + 0] , 1-_uvtxt[_vFaces[i]._p3->_i*2 + 1]);
			glVertex3f(_vFaces[i]._p3->_r.x,_vFaces[i]._p3->_r.y,_vFaces[i]._p3->_r.z);
		}

	glEnd();

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T); 
	glDisable(GL_TEXTURE_2D);	

	/**
	 *	Draw Springs
	 */

	glDisable(GL_LIGHTING);
	glColor3f(1,1,1);
	glBegin(GL_LINES);
		
		/*
		 *	Only last spring
		 */
	
	    for(i=_vSprings.size()-1; i<_vSprings.size() ; i++)
		if(_vSprings[i]._ks==600000)							// mouse spring?
		{
			glVertex3f(_vSprings[i]._p1->_r.x,_vSprings[i]._p1->_r.y,_vSprings[i]._p1->_r.z);
			glVertex3f(_vSprings[i]._p2->_r.x,_vSprings[i]._p2->_r.y,_vSprings[i]._p2->_r.z);
		}

	glEnd();

	glEnable(GL_LIGHTING);

	
	/**
	 *	Draw Points
	 */

//	glColor3f(1,0,0);
//	glBegin(GL_POINTS);
//		
//	    for(i=0; i<_vPoints.size() ; i++)
//		{
//			glVertex3f(_vPoints[i]._r.x,_vPoints[i]._r.y,_vPoints[i]._r.z);
//		}
//
//	glEnd();


}







void CSoftObjectTextured::glLoadTexture(void)
{
	glGenTextures(1,&_mytxt);
	glBindTexture(GL_TEXTURE_2D,_mytxt);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	TextureLoadMipmap(_texturename);
}