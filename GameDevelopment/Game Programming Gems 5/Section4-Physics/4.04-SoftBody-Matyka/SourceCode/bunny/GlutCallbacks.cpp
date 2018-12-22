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





/**
 * Global players...
 *
 */

#include "CSoftObject.h"
#include "CSoftObjectTextured.h"
#include "CMaterialPoint.h"

extern CSoftObjectTextured mySoftBody;
extern CVector3 Gravity;


/**
 *	Mouse state variables
 */

int mousedown=0;
float xMouse=0;
float yMouse=0;

/*
 *	Those two will be used for user / mouse interaction with soft body
 */

CMaterialPoint *p1;
CVector3 MousePosition;

/**
 * Start GLUT Namespace.
 *
 */

#include "GlutCallbacks.h"


/**
 * Namespace GLUT is used to keep all procedures (mostly callbacks) of GLUT Api.
 *
 */


namespace GLUT
{

	/**
	 * Window size                                                                     
	 */

	int width;
	int height;

	/*
	 *	time
	 */

	float time;
	float dt = 0.001; 
	float dtmax = 0.001; 
	int ActualTime(0);
	int EndTime(0);
	int DeltaTime(0);

	/**
	 * Idle function, will be called all the time
	 *
	 */

	void Idle(void)
	{

			ActualTime = GetTickCount();
			//DeltaTime = 20;

			for(int i = 0 ; i<=(float)(DeltaTime/50) ; i++)
			{
					/*
					 *	Update time
					 */

					time = time + dt;

					/*
					 *	Physics (forces + integration of equations of motion)
					 */

					mySoftBody.doCalculateForces();
					mySoftBody.doIntegration(1, dt);			// 1 - Heun, 0 - Euler

					/*
					 *	Change the pressure
					 */

					//mySoftBody.setPressure(644550*(1+sin(time*1220)));
			}


			/*
			 *	debug info (pressure)
			 */

			//cout << mySoftBody.getPressure() << endl;
			//cout << DeltaTime << endl;

			/*
			 *	Calculate Delta Time (between two frames)
			 */

			glutPostRedisplay(); 

			//EndTime = GetTickCount();
			//DeltaTime = EndTime - ActualTime;
			//cout << DeltaTime << endl;

	}



	/**
	 * Init function, open gl and not only opengl initialization procedures
	 *
	 */
	
	void Init(void)
	{
		    glClearColor(0.0, 0.0, 0.0, 1.0);

 	}
	


	/**
	 * Draw function, will be all the time to update screen
	 *
	 */
	
	void Draw(void)
	{
		    glClearColor(0.0, 0.0, 0.0, 1.0);
	//    glClearColor(0.5, 0.6, 0.7, 1.0);
	  //  glClearColor(0.7, 0.8, 0.9, 1.0);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);



		glEnable(GL_DEPTH_TEST);

		glShadeModel(GL_SMOOTH);

		float light_pos0[]={2,1,1};
		float light_pos1[]={2,1,-1};
		GLfloat mat_specular[] = { 0.7, 0.74, 0.77, 1 };
		GLfloat mat_shininess[] = { 25.9 };
		GLfloat mat_ambient[] = { 0.95,0.96,0.98,1 };
		GLfloat mat_diffuse[] = { 0.74,0.87,0.98,1 };

		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);



		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(50.,float(width)/float(height), 1., 100.);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
		glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);
		glEnable(GL_BLEND);
		glEnable(GL_LIGHTING);

		gluLookAt(5.,0.,15.,0.,0.,0.,0.,1.,0.);

	

		glColor3f(0,0,0);
		glPointSize(4);
		glLineWidth(3);
		
		glPushMatrix();


		/*
		 *	Draw the soft body in the window
		 */

		glRotatef(180,0,1,0);

		glTranslatef(0,1.67,0);
		glScalef(1.4,1.4,1.4);

			::mySoftBody.glVisualize2();
//			::mySoftBody.glVisualize();
		
		glPopMatrix();
		
		glutSwapBuffers(); 
	}


	/**
	 * Mouse handling procedures                                                                     
	 */

	void Mouse (int button, int state, int x, int y)
	{
		
		if (button == GLUT_LEFT_BUTTON)
		{
			if (state == GLUT_DOWN)
			{
				mousedown = 1;

//				xMouse = 2.0 * ((float)x/(float)width - 0.5);
//				yMouse = -2.0 * ((float)y/(float)height - 0.5);
				xMouse = 8.0 * ((float)x/(float)width - 0.5);
				yMouse = -8.0 * ((float)y/(float)height - 0.5);

				cout << xMouse << endl;
				cout << yMouse << endl;


				mySoftBody._vPoints.push_back(CMaterialPoint(0,CVector3(xMouse,yMouse,0),CVector3(0,0,0)));

				mySoftBody._vSprings.push_back(
					CHookeSpring(
					&mySoftBody._vPoints[mySoftBody._vPoints.size()-1],
					&mySoftBody._vPoints[1],20000,80,0,100)
					);

			}
			else if (state == GLUT_UP)
			{

			/*
			 *	If last spring is a mouse - object spring (actually we should not
			 *  recognize the spring by checking ks factor, however, who cares, I am
			 *  free man :)
			 */

//			if((mySoftBody._vSprings[mySoftBody._vSprings.size()-1])._ks == 1)
//			{
//
//				
//			}

			mySoftBody._vPoints.pop_back();
			mySoftBody._vSprings.pop_back();

	       		mousedown = 0;
	  		}
		 }
	}

	/**
	 * Mouse motion                                                                     
	 */

	void Motion (int x, int y)
	{
		if (mousedown)
		{
			xMouse = -8.0 * ((float)x/(float)width - 0.5);
			yMouse = -8.0 * ((float)y/(float)height - 0.5);

			mySoftBody._vPoints[mySoftBody._vPoints.size()-1]._r = CVector3(xMouse,yMouse,0);


			glutPostRedisplay();
		} else
		{
		}

	}


	/**
	 * Key map function, will be called when user hit a key on keyboard
	 *
	 */

	void Key(unsigned char key, int x, int y)
	{
		
	    switch (key) 
		{
			case 27:
				exit(0);
				break;

			default:
				return;
        }


		glutPostRedisplay(); 
	}

	/**
	 * Reshape function, standard function called when window is reshaped by user
	 *
	 */
	
	void Reshape(int w, int h)
	{

		/*
		 *	Set Actual Sizes of the Window (in namespace GLUT::myWindowWidth/Height)
		 */

		width = (GLint)w;
		height = (GLint)h;

		/*
		 *	Define GL View Port rectangle
		 */

		glViewport(0, 0, width, height);

		/*
		 *	World Transformation Matrix
		 */

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);

		glutPostRedisplay(); 
	}


}
