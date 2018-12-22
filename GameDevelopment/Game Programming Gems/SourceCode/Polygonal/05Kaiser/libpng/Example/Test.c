/*
 * Test program for glpng
 * by Ben Wyatt ben@wyatt100.freeserve.co.uk
 * Featuring a shameless plug for my stunt course program
 * Available from the same site as glpng
 * http://www.wyatt100.freeserve.co.uk/download.htm
 */

#include <gl/glpng.h>
#include <gl/glut.h>
#include <stdlib.h>

int angle = 0;

/***** GLUT callback functions *****/

void KeyPress(unsigned char key, int x, int y) {
	switch (key) {
		case 27: /*ESC*/ glutDestroyWindow(glutGetWindow()); exit(0); break;
	}
}

void Update(void) {
	angle = (angle+1)%360;
	glutPostRedisplay();
}

void Display(void) {
	const float w = 2, h = 2;

	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0, 0, -10);
	glRotatef(angle, 0, 1, 0);

	glBegin(GL_QUADS);
		// Front
		glTexCoord2f(1, 1); glVertex3f( w, -h, 0);
		glTexCoord2f(1, 0); glVertex3f( w,  h, 0);
		glTexCoord2f(0, 0); glVertex3f(-w,  h, 0);
		glTexCoord2f(0, 1); glVertex3f(-w, -h, 0);

		// Back
		glTexCoord2f(1, 1); glVertex3f(-w, -h, 0);
		glTexCoord2f(1, 0); glVertex3f(-w,  h, 0);
		glTexCoord2f(0, 0); glVertex3f( w,  h, 0);
		glTexCoord2f(0, 1); glVertex3f( w, -h, 0);
	glEnd();

	glutSwapBuffers();
}

void Reshape(int w, int h) {
	glViewport(0, 0, w, h);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, (float) w/h, 1, 100);

	glMatrixMode(GL_MODELVIEW);
	Display();
}

/***** Main function *****/

void main() {
	pngInfo info;
	GLuint texture;

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(300, 300);
	glutCreateWindow("glpng test");

	#if 0 // Using pngLoad and setting texture parameters manually.
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		if (!pngLoad("Stunt.png", PNG_NOMIPMAP, PNG_SOLID, &info)) {
			puts("Can't load file");
			exit(1);
		}
	#else // Using pngLoadAndBind to set texture parameters automatically.
		texture = pngBind("Stunt.png", PNG_NOMIPMAP, PNG_SOLID, &info, GL_CLAMP, GL_NEAREST, GL_NEAREST);

		if (texture == 0) {
			puts("Can't load file");
			exit(1);
		}
	#endif

	printf("Size=%i,%i Depth=%i Alpha=%i\n", info.Width, info.Height, info.Depth, info.Alpha);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glEnable(GL_CULL_FACE);
	glColor3f(1, 1, 1);

	glutKeyboardFunc(KeyPress);
	glutIdleFunc(Update);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);

	glutMainLoop();
}
