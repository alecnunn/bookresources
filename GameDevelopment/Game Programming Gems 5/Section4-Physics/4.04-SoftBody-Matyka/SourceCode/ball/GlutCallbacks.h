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


#ifndef _I_GlutCallbacks_H_
#define _I_GlutCallbacks_H_

/**
 * Namespace GLUT is used to keep all procedures (mostly callbacks) of GLUT Api.
 *
 */

namespace GLUT
{


	/**
	 * A 'must be' include.	
	 */

#include <GL/glut.h> 
#include <windows.h>

	/**
	 * GLUT standard callbacks.	
	 */

	void Motion (int x, int y);
	void Mouse (int button, int state, int x, int y);
	void Key(unsigned char key, int x, int y);
	void Init(void);
	void Reshape(int width, int height);
	void Draw(void);
	void Idle(void);

	/**
	 * 'Global' in 'GLUT' namespace variables
	 */

	extern int width;
	extern int height;

}



#endif