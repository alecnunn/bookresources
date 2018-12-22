
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




#include <iostream>
using namespace std;


#include "CVector3.h"

#include "CMaterialPoint.h"
#include "CHookeSpring.h"
#include "CSoftObject.h"
#include "CSoftObjectTextured.h"

#include "GlutCallbacks.h"

#include "iTexturesUV.h"


/*
 *	Create GLOBAL soft body object
 */



// remember to change also
//
// 1. extern definition in GlutCallbacks file from
// CSoftObjectTextured to -> CSoftObject
//
// 2. Gravity (below) to 0
//
// 3. GLUT:: Visualise2 -> Visualise

//CSoftObject mySoftBody(
//							CVector3(0,0,0),				// initial translation in space 
//							0.9,							// rescaling loaded object
//							1.0f,							// mass of single mass point
//							121100,							// ks spring coefficient
//							110.0f,							// kd damping factor
//							"Objects/ball.asc",			// object file (.asc 3d studio file)
//							1,								// id of the body (for collisions between bodies)
//							181120							// initial pressure of the body
//						);


// remember to change 
//
// 1. extern definition in GlutCallbacks file from
// CSoftObject to -> CSoftObjectTextured
//
// 2. Gravity (below) to 0
//
// 3. GLUT:: Visualise -> Visualise2

CSoftObjectTextured
				 mySoftBody(
							CVector3(0,0,0),				// initial translation in space 
							0.3,							// rescaling loaded object
							1.0f,							// mass of single mass point
							121100,							// ks spring coefficient
							110.0f,							// kd damping factor
							"Objects/bunny.asc",			// object file (.asc 3d studio file)
							1,								// id of the body (for collisions between bodies)
							2311120,						// initial pressure of the body
							linux_uv,						// uv table 
							"Textures/bunny.bmp"		// texture name			
						);

/*
 *	Main function
 */

int main(void)
{

	mySoftBody.setGravity(CVector3(0,0,0));

	/*
	 * Yep, I know, that "using namespace" exist
	 */ 

	GLUT::width = 600;
	GLUT::height = 600;


	/*
	 *	Print Debug info
	 */

	cout << "Window Width: " << GLUT::width << endl;
	cout << "Window Height: " << GLUT::height << endl << endl;
	cout << "Loaded: " << mySoftBody.iNumf() << " faces..." << endl;
	cout << "Loaded: " << mySoftBody.iNump() << " points..." << endl;
	cout << "Created: " << mySoftBody.iNums() << " spring connections..." << endl;


	GLUT::glutInitWindowPosition(200, 200); 
	GLUT::glutInitWindowSize( GLUT::width, GLUT::height);
	GLUT::glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);

	if (GLUT::glutCreateWindow("Soft Body 3.0 by Maciej Matyka, http://panoramix.ift.uni.wroc.pl/~maq") == GL_FALSE) 
	{
		::exit(0);
	}


	// load textures
	mySoftBody.glLoadTexture();

	GLUT::glutMouseFunc(GLUT::Mouse);
	GLUT::glutMotionFunc(GLUT::Motion);
	GLUT::glutKeyboardFunc(GLUT::Key);
	GLUT::glutReshapeFunc(GLUT::Reshape);
	GLUT::glutDisplayFunc(GLUT::Draw);
	GLUT::glutIdleFunc(GLUT::Idle);

	GLUT::glutMainLoop(); 


	
	return (2==3);
}






















