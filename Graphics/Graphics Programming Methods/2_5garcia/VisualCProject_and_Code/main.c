/****************************************/
/* An easy-to-code smoothing algorithm  */
/* for 3D reconstructed surfaces        */
/*                                      */
/* The main application file            */
/* -------------------------            */
/*                                      */
/* Author: Oscar Garcia-Panyella        */
/* Date: January 2003                   */
/* Contact: oscarg@salleurl.edu         */
/****************************************/

#include "main.h"

/****************************************/
/* Some needed variables                */
/****************************************/
mesh myTestMesh, backupMesh;
float tol, alfa, sceneXRotation, sceneYRotation, sceneLastX,
sceneLastY;

/****************************************/
/* Mouse callback (GLUT)                */
/****************************************/
void mouse(int button, int button_state, int x, int y){
	// Virtual trackball effect
	if (button==GLUT_LEFT_BUTTON && button_state==GLUT_DOWN){
		sceneLastX=x;
		sceneLastY=y;
	}
}

/****************************************/
/* Mouse movement callback (GLUT)       */
/****************************************/
void mouseMovement(int x, int y){
	// Virtual trackball effect
	sceneXRotation+=(float)(y-sceneLastY);
	sceneYRotation+=(float)(x-sceneLastX);
	sceneLastX=x;
	sceneLastY=y;
	glutPostRedisplay();
}

/****************************************/
/* Display callback (GLUT)              */
/****************************************/
void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Two rotations before the render, for the virtual trackball effect
	glPushMatrix();
		glRotatef(sceneYRotation, 0.0, 1.0, 0.0);
		glRotatef(sceneXRotation, 1.0, 0.0, 0.0);
		drawTriangles(&myTestMesh);
		drawEdges(&myTestMesh);
	glPopMatrix();

	glutSwapBuffers();
}

/****************************************/
/* Keyboard callback (GLUT)             */
/****************************************/
void keys(unsigned char key, int x, int y){
	// Press 's or S' repeatedly for the algorithm
	// Press 'a or A' for loading the original mesh again
	// Press 'e or E' for exitting the demo
	if ((key == 's')|(key == 'S')) smoothingAlgorithm(&myTestMesh, tol, alfa);
	if ((key == 'a')|(key == 'A')) copyMesh(&backupMesh, &myTestMesh);
	if ((key == 'e')|(key == 'E')){
		deallocateMesh(&myTestMesh);
		deallocateMesh(&backupMesh);
		exit(-1);
	}
	glutPostRedisplay();
}

/****************************************/
/* Rendering context inicialization     */
/****************************************/
void initRender(void){
	float frustrumDimensions[6];

	// The bounding box of the mesh is calculated
	// The frustrum dimensions are set from the bigger dimension
	// of the bounding box
	boundingBoxMesh(&myTestMesh, frustrumDimensions);
	sort(frustrumDimensions, 6);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(frustrumDimensions[0]*-1.5,
		frustrumDimensions[0]*1.5,
		frustrumDimensions[0]*-1.5,
		frustrumDimensions[0]*1.5,
		frustrumDimensions[0]*1.5,
		frustrumDimensions[0]*-1.5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Enabling the Culling and the Z-Buffer
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
}

/****************************************/
/* The main function                    */
/****************************************/
int main(int argc, char** argv){
	// Command line correctness control
	if ((argc!=2)&(argc!=3)&(argc!=4)){
		printf("USAGE: smooth <input_mesh_file> <OPTIONAL: tolerance> <OPTIONAL: alfa>");
		exit(-1);
	}
	else{
		if (argc==2){
			tol=DEFAULT_TOL;
			alfa=DEFAULT_ALFA;
		}
		else if (argc==3){
			tol=(float)atof(argv[2]);
			alfa=DEFAULT_ALFA;
		}
		else{
			tol=(float)atof(argv[2]);
			alfa=(float)atof(argv[3]);
		}

		// Window creation
		glutInit(&argc, argv);
		glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
		glutInitWindowSize (600,600); 
		glutInitWindowPosition (0,0);
		glutCreateWindow("The Smoothing Algorithm");
  
		// Callback definition
		glutDisplayFunc(display);
		glutKeyboardFunc(keys);
		glutMouseFunc(mouse);
		glutMotionFunc(mouseMovement);

		// The input mesh is loaded. Edges are arranged. Backup of the mesh.
		loadMesh(&myTestMesh, argv[1]);
		arrangeEdges(&myTestMesh);
		copyMesh(&myTestMesh, &backupMesh);

		// Rendering context / Viewport inicialization
		initRender();

		glutMainLoop();
	}
	return 0;   
}