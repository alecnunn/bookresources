#include "GL/glut.h"
#include <math.h>
#include <stdlib.h>
#include "../COMMON/common.h"
#include <sys/time.h>
#include <unistd.h>
#include "../COMMON/audioenv.h"

// Globals
AVEnvironment g_Env;

AudioEnvPtr Object::audioObject = NULL; // static pointer to audio object used in Object::Move 

// Constants
const float STRIDE = 0.9f;
const float PI = 3.1415926535f;

float TimeDiff()
{
   static double oldTime = 0;
   double newTime;
   timeval newTimeVal;
   float returnTimeDiff;
   bool returnZero = false;
   
   if (oldTime == 0) { returnZero = true; } else { returnZero = false; }
   
   gettimeofday(&newTimeVal, NULL);
   newTime = (double) newTimeVal.tv_sec + ((double) newTimeVal.tv_usec / 1000000);
   
   returnTimeDiff =  (float) (newTime - oldTime);
   oldTime = newTime;

   if (returnZero == false) {
      return returnTimeDiff;
   } else {
      return 0.0f;
   }
}

void display(void)
{
    float td = TimeDiff();
    g_Env.DrawBuffer(td);
    if (td != 0) { g_Env.SetFPS(1/td); }
    g_Env.PlaceCamera();
    glutSwapBuffers();
}

void myinit (void)
{
	g_Env.Init();
}

void parsekey(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 27:  	exit(0); break;
	        case 'f':       g_Env.ToggleFPS(); break;
		case 'z': 	g_Env.Step((-PI/2), STRIDE); break;
		case 'x': 	g_Env.Step((PI/2), STRIDE); break;
	}
}

void parsekey_special(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_UP:			g_Env.Step(0, STRIDE); break;
		case GLUT_KEY_DOWN:			g_Env.Step(3.14f, STRIDE); break;
		case GLUT_KEY_RIGHT:		g_Env.Turn(0.15f * STRIDE); break;
		case GLUT_KEY_LEFT:			g_Env.Turn(-0.15f * STRIDE); break;
	}
}

void SetCamera()
{
    g_Env.PlaceCamera();
}

void Animate(void)
{
    SetCamera();
    glutPostRedisplay();
}

void myReshape(int w, int h)
{
    SetCamera();
    g_Env.ChangeView(w, h);
}

int main(int argc, char *argv[])
{
	int sz;
	
	if (argc > 1)
		sz = atoi(argv[1]);
	else
		sz = 200;

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(200, 100);
	glutInitWindowSize((int) (sz*1.33*3), sz*3);
	glutCreateWindow("XL Demo");
	glutDisplayFunc(display);
	glutKeyboardFunc(parsekey);
	glutSpecialFunc(parsekey_special);
	glutReshapeFunc(myReshape);
	glutIdleFunc(Animate);
	
	myinit();
	glutSwapBuffers();

        glutMainLoop();
}
