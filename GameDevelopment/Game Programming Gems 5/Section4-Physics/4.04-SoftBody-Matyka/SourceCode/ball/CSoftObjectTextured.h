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


#ifndef __I___CSOFTOBJECTTXT_H_
#define __I___CSOFTOBJECTTXT_H_



#include "CSoftObject.h"



/**
 * 
 * CSoftBody object with embeeded texture uv map.
 *
 */






class CSoftObjectTextured : public CSoftObject
{

public:

	CSoftObjectTextured
		(CVector3 translate, float rescale, float mass, float ks, float kd, char *ascobject, int id, float Pressure,
		float *uvtxt, char *texturename)
		: CSoftObject(translate,rescale,mass,ks,kd,ascobject,id,Pressure), _texturename(texturename),_uvtxt(uvtxt)
	{
	}

	/**
	 * Object GL Visualization                                                                     
	 */

		void glVisualize2(void) const;

	/**
	 * Load & Initialize texture                                                                     
	 */

		void glLoadTexture(void);

public:
	char *_texturename;
	float *_uvtxt;
	unsigned int _mytxt;			// OpenGL Texture number
};


#endif