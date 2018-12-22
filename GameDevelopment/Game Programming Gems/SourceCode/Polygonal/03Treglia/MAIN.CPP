/* Copyright (C) Dante Treglia II, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dante Treglia II, 2000"
 */
//===========================================================================
//===========================================================================
#include "text.h"
#include "mtxlib.h"
#include "fly.h"
#include "plane.h"
#include "spline.h"
#include "light.h"
#include "auxobj.h"
#include "stars.h"
#include <gl/glaux.h>
#include <gl/glut.h>
#include <math.h>

//---------------------------------------------------------------------------
char * help[4] = {
"Static Camera State:\nWelcome to the camera technique demo. The red object is the main character \
or target. For fun, you can change the object with the 't' key. The green and blue cones represent \
a track camera and a 3rd person spring camera respectively. Notice the violet point that rigidly \
follows the target. A spring attaches that point to the blue camera. You toggle the targets \
movements with the 'm' key. Press 'c' to get to the next technique",
"Fly Camera State\nIn this stage you can use the mouse and keyboard to move about the scene. \
Moving the mouse side-to-side and up-and-down controls the yaw and pitch respectively. The \
camera movement is spring damped. Try 'w', 's', 'a, 'd, 'r', 'f' and the arrow keys. Use '?' \
to toggle this text at any time. Press 'c' to continue",
"Spline Camera State\nThis camera is attached to a spline. The light and the target are also \
attached to splines. Carefully selected semi-random control points are added to the curves \
when the sub-samples have been exhausted. You can see the splines by pressing the 'p' key, \
however, I recommend viewing them from the static camera. One more to go...",
"Third Person Spring Camera\nThis camera is implemented with a spring. You can change the \
spring constant using 'k'. Although the teapot is not lit correctly, it does demostrate the \
independent nature of the springs.\n\nWell, that concludes the demo.\n\nCheers,\n--Dante"
};

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------
void DrawInit(void);
void DrawTick(void);
void AnimTick();
void CameraInit();

void DisplayFunc(void);
void KeyboardFunc(unsigned char key, int x, int y);
void KeyboardSpecialFunc(int key, int x, int y);
void ReshapeFunc(int w, int h) {};
void IdleFunc(void);
void MouseFunc(int button, int state, int x, int y);
void MotionFunc(int x, int y);

vector3 SpringDamp(vector3 currPos, vector3 trgPos, vector3 prevTrgPos, 
		float deltaTime, float springConst, float dampConst, float springLen);

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------
matrix44 viewMtx;
vector3 camPos(100.0, 100.0, 300.0);
vector3 camUp(0.0, 1.0, 0.0);
vector3 camAt(0.0, 0.0, 0.0);
TextBox * outputP;
FlyCam * cameraP;

COLOR colors[8] = {
	{255,   0,   0, 255},
	{  0, 255,   0, 255},
	{  0,   0, 255, 255},
	{255, 255, 255, 255},
	{255,   0, 255, 255},
	{255, 255,   0, 255},
	{  0, 255, 255, 255},
	{200, 200, 200, 255},
};
#define RED colors[0]
#define GREEN colors[1]
#define BLUE colors[2]
#define WHITE colors[3]
#define VIOLET colors[4]
#define YELLOW colors[5]
#define CYAN colors[6]
#define GREY colors[7]


//---------------------------------------------------------------------------
// Objects
//---------------------------------------------------------------------------
Light light(WHITE, WHITE, GL_LIGHT0);
Spline lightSpline("data/curve.sdf");
Object * splineCameraObjP;
Cone splineCone(5.0, 15.0);
Object * springCameraObjP;
Cone springCone(5.0, 15.0);
Spline cameraSpline("data/curve.sdf");
Object * targetObjP;
Spline targetSpline("data/curve.sdf");

vector3 currTrgPos(0.0, 0.0, 0.0);

Stars stars(5000, 1000.0);
Plane plane(2, 2, 100, 100, 1, 1, GREY);
Sphere sphere(6.0);
Box box(8.0, 8.0, 8.0);
Torus torus(4.0, 8.0);
Cylinder cylinder(8.0, 10.0);
Cone cone(8.0, 10.0);
Icosahedron icosahedron(8.0);
Tetrahedron tetrahedron(8.0);
Octahedron octahedron(8.0);
Docecahedron docecahedron(8.0);
Teapot teapot(5.0);

Object * objP[10] = {
	&sphere,
	&box,
	&torus,
	&cylinder,
	&cone,
	&icosahedron,
	&tetrahedron,
	&octahedron,
	&docecahedron,
	&teapot,
};
int objectIndex = 2;

int cameraMode = 0;
int movementMode = 1;
int zoomMode = 0;
int helpMode = 1;
int springMode = 1;
int splineMode = 0;

//---------------------------------------------------------------------------
// Main
//---------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
					 LPSTR lpCmdLine, int nCmdShow) {

	// Glut Init
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(200, 0);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Camera Techniques");

	glutDisplayFunc(DisplayFunc);
	glutKeyboardFunc(KeyboardFunc);
	glutSpecialFunc(KeyboardSpecialFunc);
	glutReshapeFunc(ReshapeFunc);
	glutIdleFunc(IdleFunc);
	glutMouseFunc(MouseFunc);
	glutPassiveMotionFunc(MotionFunc);
	
	// Program Inits
	DrawInit();
	CameraInit();

	// Go!
	glutSwapBuffers();
	glutMainLoop();

	return 0;
}

//---------------------------------------------------------------------------
// Camera
//---------------------------------------------------------------------------
static void CameraInit () {
	glViewport(0, 0, 640, 480);

	glMatrixMode(GL_PROJECTION);
	gluPerspective(33, 640.0F/480.0F, 1, 3000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	cameraP = new FlyCam(camPos, DegToRad(-45), DegToRad(-40), 0);
	viewMtx = cameraP->GetViewMtx();
}

//---------------------------------------------------------------------------
// Draw Init
//---------------------------------------------------------------------------
static void DrawInit() {
	int i;

	// OpenGL
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0F, 0.0F, 0.0F, 0.0F);

	// Light
	light.SetPosition(vector3(10.0, 20.0, 30.0));

	// Plane
	plane.AddLight((void *)&light);
	plane.lightsOn = 1;
	plane.SetMaterial(0.25F, GREY);

	// Add Lights
	for (i = 0; i < 10; i++) {
		objP[i]->AddLight((void *)&light);
		objP[i]->lightsOn = 1;
		objP[i]->SetMaterial(0.15F, RED);
	}

	// Spline Cone
	splineCone.AddLight((void *)&light);
	splineCone.lightsOn = 1;
	splineCone.SetMaterial(0.25F, GREEN);

	// Spring Cone
	springCone.AddLight((void *)&light);
	springCone.lightsOn = 1;
	springCone.SetMaterial(0.25F, BLUE);

	// Set Objects
	splineCameraObjP = &splineCone;
	springCameraObjP = &springCone;
	targetObjP = &icosahedron;

	outputP = new TextBox(0, 640, 400, 480);
	outputP->SetMode(TEXT_DRAW_BACKGROUND | TEXT_WRAP_SPACES | TEXT_DRAW_BOX);
	outputP->Printf("%s", help[cameraMode]);
}

//---------------------------------------------------------------------------
// Draw a Frame
//---------------------------------------------------------------------------
void DrawTick() {
	stars.Draw(viewMtx);

	plane.Draw(viewMtx);

	light.Draw(viewMtx);

	lightSpline.Draw(viewMtx);
	cameraSpline.Draw(viewMtx);
	targetSpline.Draw(viewMtx);

	splineCameraObjP->Draw(viewMtx);
	springCameraObjP->Draw(viewMtx);

	glLoadMatrixf((float *)&viewMtx);
	glPointSize(2.0);
	glBegin(GL_POINTS);
	glColor3f(1.0, 0.0, 1.0);
	glVertex3f(currTrgPos.x, currTrgPos.y, currTrgPos.z);
	glEnd();

	targetObjP->Draw(viewMtx);

	if (helpMode) outputP->Draw();
}

//---------------------------------------------------------------------------
// Helper Functions
//---------------------------------------------------------------------------
void RAND_FUNC_LIGHT (vector3 &input) {
	static float rot = 0;

	rot += 0.6F;
	input.x = cosf(rot) * 50;
	input.y = ((float)(rand()%2048))/2048.0F * 40.0F;
	input.z = sinf(rot) * 50;
}

//---------------------------------------------------------------------------
void RAND_FUNC_CAMERA (vector3 &input) {
	static float rot = 0;

	rot -= 0.5F;
	input.x = cosf(rot) * 250;
	input.y = ((float)(rand()%2048))/2048.0F * 50.0F + 50;
	input.z = sinf(rot) * 250;
}

//---------------------------------------------------------------------------
void RAND_FUNC_TARGET (vector3 &input) {
	static float rot1 = 0;
	static float rot2 = 0;
	static int dir = 0;
	static int tick = 0;

	if (tick-- <= 0) {
		tick = rand() % 10;
		dir ^= 1;
	}

	if (dir) {
		rot1 += 0.9F;
		rot2 += 0.5F;
	}
	else {
		rot1 -= 0.9F;
		rot2 += 0.5F;
	}
	input.x = cosf(rot1) * 120.0F * cosf(rot2);
	input.y = ((float)(rand()%2048))/2048.0F * 20.0F + 20;
	input.z = sinf(rot1) * 120.0F * cosf(rot2);
}

//---------------------------------------------------------------------------
// Helper Functs
//---------------------------------------------------------------------------
int GET_SPLINE_INDEX (Spline * sP, float * distance, void (funcP(vector3 &))) {
	int index;

	// Get index at distance
	index = sP->GetIndexAtDistance(*distance);

	// Check to see if we have gone beyond the spline (-1)
	if (index  == -1) {
		// Move Last 3 Control Points to First 3
		for (int i = 0; i < 3; i++) {
			sP->controlData[i].pos = sP->controlData[i+1].pos;
		}
		// ADD New Point
		(*funcP)(sP->controlData[3].pos);
		// Rebuild Spline
		sP->Build(sP->curveData[sP->curveCnt - 1].nrm);
		// Reset Distance
		*distance = 0.0;
		// Get new index
		index = sP->GetIndexAtDistance(*distance);
		assert(index != -1 && "Bad Index");
	}

	return index;
}

//---------------------------------------------------------------------------
#define GET_LIGHT_INDEX(dist) GET_SPLINE_INDEX(&lightSpline, dist, &RAND_FUNC_LIGHT)
#define GET_CAMERA_INDEX(dist) GET_SPLINE_INDEX(&cameraSpline, dist, &RAND_FUNC_CAMERA)
#define GET_TARGET_INDEX(dist) GET_SPLINE_INDEX(&targetSpline, dist, &RAND_FUNC_TARGET)

//---------------------------------------------------------------------------
// Animate a Frame
//---------------------------------------------------------------------------
void AnimTick() {
	static int first_time = 1;
	// Distance & Speed
	static float lightDistance = 0.0;
	static float cameraDistance = 0.0;
	static float targetDistance = 0.0;
	static float lightSpeed = 20.0;
	static float cameraSpeed = 40.0;
	static float targetSpeed = 40.0;
	// Time
	static float prevSecs;
	float currSecs;
	float deltaSecs;
	// Temp
	int index;
	static float tempAng = 0;
	// Spring 
	static vector3 prevTrgPos(0.0, 0.0, 0.0);
	matrix44 splineViewMtx;
	matrix44 springViewMtx;
	float springConstants[3] = {0.5, 2.5, 4.5};

	// Zoom
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (zoomMode) gluPerspective(cos(tempAng += 0.03F)*10 + 33, 640.0F/480.0F, 1, 3000);
	else gluPerspective(33, 640.0F/480.0F, 1, 3000);
	glMatrixMode(GL_MODELVIEW);

	if (first_time) {
		// Ranomly Initialize Splines
		for(int i = 0; i < 4; i++) {
			RAND_FUNC_LIGHT(lightSpline.controlData[i].pos);
			RAND_FUNC_CAMERA(cameraSpline.controlData[i].pos);
			RAND_FUNC_TARGET(targetSpline.controlData[i].pos);
		}
		lightSpline.Build();
		cameraSpline.Build();
		targetSpline.Build();

		// Set Timer
		prevSecs = ((float)GetTickCount()) / 1000.0F;
	}

	// Update Time
	currSecs = (((float)GetTickCount()) / 1000.0F);
	deltaSecs = currSecs - prevSecs;
	prevSecs = currSecs;

	// Update Distances
	lightDistance += (deltaSecs) * lightSpeed;
	cameraDistance += (deltaSecs) * cameraSpeed;
	if (movementMode) targetDistance += (deltaSecs) * targetSpeed;

	// Update Light
	index = GET_LIGHT_INDEX(&lightDistance);
	light.SetPosition(lightSpline.curveData[index].pos);
	
	// Set Target Object
	targetObjP = objP[objectIndex];

	// Udate Target
	index = GET_TARGET_INDEX(&targetDistance);
	targetObjP->position = targetSpline.curveData[index].pos;
	targetObjP->orientation = OrthoNormalMatrix44(
		targetSpline.curveData[index].sid,
		targetSpline.curveData[index].nrm,
		targetSpline.curveData[index].tan
	);
	targetObjP->normalMtx = InvertMatrix44(targetObjP->orientation);

	// *** Update Spring Cmamera
	prevTrgPos = currTrgPos;
	currTrgPos = targetObjP->position + (-40.0F * targetSpline.curveData[index].tan);
	springCameraObjP->position = 
		SpringDamp(springCameraObjP->position, currTrgPos, prevTrgPos, 
			deltaSecs, springConstants[springMode], 0.25F, 1.0F);
	// Initialization
	if (first_time) springCameraObjP->position = currTrgPos;
	springViewMtx = LookAtMatrix44(springCameraObjP->position,
		targetObjP->position + (-10.0F * targetSpline.curveData[index].tan),
		vector3(0.0, 1.0, 0.0));
	springCameraObjP->orientation = LookAtMatrix44(vector3(0.0, 0.0, 0.0),
		(targetObjP->position + (-10.0F * targetSpline.curveData[index].tan)
			- springCameraObjP->position).normalize(),
		vector3(0.0, 1.0, 0.0)).invert();
	springCameraObjP->normalMtx = InvertMatrix44(springCameraObjP->orientation);

	// *** Update Spline Camera
	index = GET_CAMERA_INDEX(&cameraDistance);
	splineCameraObjP->position = cameraSpline.curveData[index].pos;
	splineViewMtx = LookAtMatrix44(splineCameraObjP->position,
		//targetObjP->position, 
		vector3(0.0, 20.0, 0.0),
		vector3(0.0, 1.0, 0.0));
	splineCameraObjP->orientation = LookAtMatrix44(vector3(0.0, 0.0, 0.0),
		//(targetObjP->position - splineCameraObjP->position).normalize(),
		(vector3(0.0, 2.0, 0.0) - splineCameraObjP->position).normalize(),
		vector3(0.0, 1.0, 0.0)).invert();
	splineCameraObjP->normalMtx = InvertMatrix44(splineCameraObjP->orientation);


	// *** Set Proper View Matrix
	switch (cameraMode) {
		case 0:
			viewMtx = LookAtMatrix44(camPos, camAt, camUp);
		break;
		case 1:
			cameraP->Tick(deltaSecs);
			viewMtx = cameraP->GetViewMtx();
		break;
		case 2:
			viewMtx = splineViewMtx;
		break;
		case 3:
			viewMtx = springViewMtx;
		break;
	}


	// No more
	first_time = 0;
}


//---------------------------------------------------------------------------
// Display
//---------------------------------------------------------------------------
void DisplayFunc () {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawTick();
	glutSwapBuffers();
}

//---------------------------------------------------------------------------
// Idle
//---------------------------------------------------------------------------
void IdleFunc () {
	AnimTick();
	glutPostRedisplay();
}

//---------------------------------------------------------------------------
// Keyboard
//---------------------------------------------------------------------------
void KeyboardFunc(unsigned char key, int x, int y) {
	switch(key) {
		case 'c':
		case 'C':
			cameraMode = (cameraMode + 1) % 4;
			outputP->Clear();
			outputP->Printf("%s", help[cameraMode]);
		break;
		case 't':
		case 'T':
			objectIndex = (objectIndex + 1) % 10;
		break;
		case 'm':
		case 'M':
			movementMode ^= 1;
		break;
		case 'Z':
		case 'z':
			zoomMode ^= 1;
		break;
		case '?':
			helpMode ^= 1;
		break;
		case 'k':
			springMode = (springMode + 1) % 3;
		break;
		case 'p':
			splineMode = (splineMode + 1) % 4;
			switch(splineMode) {
				case 0:
					lightSpline.displayControlMode = 0;
					lightSpline.displayCurveMode = 0;
					lightSpline.displayOrientMode = 0;
					targetSpline.displayControlMode = 0;
					targetSpline.displayCurveMode = 0;
					targetSpline.displayOrientMode = 0;
					cameraSpline.displayControlMode = 0;
					cameraSpline.displayCurveMode = 0;
					cameraSpline.displayOrientMode = 0;
				break;
				case 1:
					lightSpline.displayControlMode = 0;
					lightSpline.displayCurveMode = 1;
					lightSpline.displayOrientMode = 0;
					targetSpline.displayControlMode = 0;
					targetSpline.displayCurveMode = 1;
					targetSpline.displayOrientMode = 0;
					cameraSpline.displayControlMode = 0;
					cameraSpline.displayCurveMode = 1;
					cameraSpline.displayOrientMode = 0;
				break;
				case 2:
					lightSpline.displayControlMode = 2;
					lightSpline.displayCurveMode = 1;
					lightSpline.displayOrientMode = 0;
					targetSpline.displayControlMode = 2;
					targetSpline.displayCurveMode = 1;
					targetSpline.displayOrientMode = 0;
					cameraSpline.displayControlMode = 2;
					cameraSpline.displayCurveMode = 1;
					cameraSpline.displayOrientMode = 0;
				break;
				case 3:
					lightSpline.displayControlMode = 2;
					lightSpline.displayCurveMode = 1;
					lightSpline.displayOrientMode = 3;
					targetSpline.displayControlMode = 2;
					targetSpline.displayCurveMode = 1;
					targetSpline.displayOrientMode = 3;
					cameraSpline.displayControlMode = 2;
					cameraSpline.displayCurveMode = 1;
					cameraSpline.displayOrientMode = 3;
				break;
			}
		break;

		case 27: 
			delete outputP;
			delete cameraP;
			exit(0);
	}
}

//---------------------------------------------------------------------------
// Special Key
//---------------------------------------------------------------------------
void KeyboardSpecialFunc(int key, int x, int y) {
}

//---------------------------------------------------------------------------
// Mouse
//---------------------------------------------------------------------------
void MouseFunc(int button, int state, int x, int y) {
}

//---------------------------------------------------------------------------
// Motion (Mouse)
//---------------------------------------------------------------------------
void MotionFunc(int x, int y) {
	cameraP->Mouse((float) x/640.0F, (float) y/480.0F);
}
