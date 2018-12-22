/*
 * Real-Time Halftoning: Fast and Simple Stylized Shading
 * Bert Freudenberg, Maic Masuch, and Thomas Strothotte
 * University of Magdeburg
 * Published in Game Programming Gems 4, Charles River Media 2004
 *
 * Contact: bert@isg.cs.uni-magdeburg.de
 *
 * Windows: create a project linking to opengl32.dll, glu32.dll, and glut32.dll
 * MacOS X: gcc -o halftone -framework GLUT -framework OpenGL -framework Cocoa halftone.c
 * Linux:   gcc -o halftone -lglut -lGL -lGLU halftone.c
 */

#ifdef __APPLE__
#  include <GLUT/glut.h>
#  include <OpenGL/glext.h>
#else
#  include <GL/glut.h>
#  include <GL/glext.h>
#endif

GLubyte tex[] = { 255, 255, 0, 0};     // 2x2 stripe texture

GLdouble rotx = 0.0;                   // camera rotation
GLdouble roty = 0.0;

int mx, my;                            // mouse pointer

void initgl(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// stage 0: smooth halftoning
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 2, 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_SUBTRACT_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_ONE_MINUS_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 4);
	glEnable(GL_TEXTURE_2D);
	// stage 1: invert
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 2, 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, tex); // unused
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);                // unused
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,  GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,  GL_PREVIOUS_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_ONE_MINUS_SRC_COLOR);
	glEnable(GL_TEXTURE_2D);
	// tile texture
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glMatrixMode(GL_TEXTURE);
	glScalef(8.0, 8.0, 8.0);
	glMatrixMode(GL_MODELVIEW);
}

void idle(void)
{
	const GLfloat light_dir[] = { 0.0, 1.0, 1.0, 0.0 };
	static int angle = 0;
	// rotate light
	glPushMatrix();
	glRotatef(++angle, 0.0, 1.0, 0.0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_dir);
	glPopMatrix();
	// switch to checkerboard
	if (angle == 360) {
		angle = 0;
		tex[1] ^= 255;
		tex[3] ^= 255;
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 2, 2, GL_LUMINANCE, GL_UNSIGNED_BYTE, tex);
	}
	glutPostRedisplay();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glRotated(rotx, 1.0, 0.0, 0.0);
	glRotated(roty, 0.0, 1.0, 0.0);
	glutSolidTeapot(0.6);
	glPopMatrix();
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void mouse(int button, int state, int x, int y)
{
	mx = x;
	my = y;
}

void motion(int x, int y)
{
	roty += (mx - x); mx = x;
    rotx += (my - y); my = y;
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Freudenberg, Masuch, Strothotte: Realtime Halftoning");
	glutReshapeFunc (reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	initgl();
	glutMainLoop();
	return 0;
}
