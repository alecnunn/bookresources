/*****************************************************************************
|
| Airplane
|
| by Graham S. Rhodes
| September 2004
|
| A simple demonstration of back of the envelope aerodynamics. This demo
| simulates the flight of a simple, longitudinal airplane. This airplane
| is a canard style airplane, with the main wing in the rear and the
| smaller, horizontal stabilizer wing in the front.
|
| Keyboard controls:
|
|  'r' - restart the simulation, and put it in pause mode
|  'q' - quit the application
|  'g' - start the simulation, or take it out of pause mode
|  'p' - pause the simulation
|  '>' - increase simulation speed
|  '<' - decrease simulation speed
|  '+' - increase the incidence angle of the horizontal stabilizer
|  '-' - decrease the incidence angle of the horizontal stabilizer
|  'l' - toggle C.G. lock. When C.G. is locked, only rotational
|        motion of airplane is simulated. Otherwise, rotation and
|        translation is simulated. Default is unlocked.
|
| TO DO:
|
|   Change the airplane geometric and mass properties to see what
|   happens.
|
|   Create a simulated flight control system that automatically adjusts
|   the horizontal stabilizer and/or wing incidence angles to balance
|   the airplane at g_fOrientation = 0.0 when the wind gusts.
|
|   Adjust the mass and inertia of the airplane to be more consistent
|   with real objects (they are just experimental numbers here) so
|   the airplane can more easily find translation equilibrium and
|   rotational equilibrium at the same time. The goal is to have the
|   airplane "trim," e.g., flying at a fixed horizontal velocity while
|   not exhibiting rotational motion.
|
|   Especially experiment with the moment of inertia, seeing what
|   much higher and much lower values do to the dynamics of the
|   airplane. Higher moments of inertia will reduce the oscillation
|   amplitude, keeping angle of attack a bit more under control during
|   a wind gust or as the horizontal stabilizer is adjusted.
|
|   Experiment with other numerical integration techniques
|
|	Try implementing a nonlinear lift model, in which the variation
|   of CL with angle of attack flattens out and eventually begins
|   to become smaller as angle-of-attack increases above 10-12
|   degrees. This will reduce the maximum and minimum orientation
|   angles the airplane reaches during pitch oscillation.
|
| This demo is a companion to the chapter titled "Back of the
| Envelope Aerodynamics for Game Physics" in the book "Game
| Programming Gems 5" published by Charles River Media
|
*******************************************************************************/

#include <gl/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include "IndexedTriMesh.h"

// Method prototypes
void QuitApplication();
void UpdatePhysics();
void PhysicsStep();
void CreateScene();
void ResetSim();
void CalcLiftDrag(const float fRootChord, const float fTipChord,
					const float fSemispan, const float fLESweep,
					const float *pfRelativeWind, float fAirplaneOrientation,
					float fWingIncidence, float *pfCL, float *pfCD);


// GLUT callback method prototypes
void DisplayFunc(void);
void KeyboardFunc(unsigned char cKey, int iX, int iY);
void ReshapeFunc(int iWidth, int iHeight);
void IdleFunc(void);

// globals

// Basic properties and conversion factors
float g_fOneOverTwoPi = 1.0f/(8.0f * atanf(1.0f));			// inverse of 2*pi.
float g_fFluidDensity = 1.225f;								// Air density around sea level, kg/meters cubed
float g_fPI = 4.0f * atanf(1.0f);							// PI
float g_fDegToRad = g_fPI / 180.0f;							// conversion factor from degrees to radians
float g_fRadToDeg = 180.0f / g_fPI;							// conversion factor from radians to degrees
float g_fGravitationalAccel = -9.8f;						// rate of gravitational acceleration, in Z direction, meters/second-squared
float g_fRandMax(float(RAND_MAX));							// maximum value returned by rand()
bool g_bCGLocked = true;									// set to true to lock C.G. in place (simulating rotation only)

// airplane mass properties
float g_fMomentOfInertia = 25000.0f;						// moment of inertia of airplane about y axis, which passes horizontally through the center of gravity and out the right wing
float g_fMass = 187.0f;										// mass of airplane, kilograms

// center of gravity location and overall airplane orientation
float g_pfLocation[] = {0.0f, 0.0f, 5000.0f};				// current location of center of gravity of airplane, meters
float g_pfVelocity[] = {-100.0f, 0.0f, 0.0f};				// current velocity of airplane (meters/sec)
float g_pfAbsoluteWind[] = {0.0f, 0.0f, 0.0f};				// absolute velocity of wind, relative to world (meters/sec)
float g_fOrientation = 0.0f;								// orientation of airplane x axis relative to the world, radians
float g_fPreviousOrientation = 0.0f;						// orientation at previous time step
float g_fPitchSpeed = 0.0f;									// Rotational velocity about pitch (y) axis.

// geometry of the horizontal stabilizer (front wing). The
// following diagram illustrates the parameters from the top
// down. These are the parameters used to define the geometry
// of the main wing as well.
//
//                       |------------------
//                     / | \   L
//                   /   |   \
//                 /     |     \
//               /       |       \
//             /         |         \
//           /   q       *      q    \            
//         /             |             \          ^
//       /              C|r              \        |
//      |                |                |       |
//     C|t       ********|********        |       |
//      |********                 ********|       |
//                                                D
//                       <-------b/2------>       |
//                                                |
//                                                |
//                      C.G.------> Y             V
//                       |
//                       |
//                       |
//                       |
//                       V
//
//                       X
//
//   C.G. - center of gravity of the airplane
//     Ct - Tip chord length (at y = semispan)
//     Cr - Root chord length (at symmetric wing centerline, y = 0)
//      L - leading edge sweep angle
//    b/2 - semispan length
//      q - location of quarter-chord line for each
//          wing half, at the y location of the
//          effective spanwise center-of-lift for
//          the wing half. This is described in the
//          chapter text.
//      * - center of lift for the entire wing. It
//          is the average of q from both wing halves.
//          For a symmetric wing, it has the same X
//          location as the q's, but Y = 0.
//      D - X direction offset from airplane center-of-
//          gravity to center of lift for the wing.
//          
float g_fDXStabilizerWingCenterOfLift = -5.0f;				// offset in x direction from C.G. of horizontal stabilizer (front wing) center-of-lift, meters
float g_fStabilizerRootChord = 2.0f;						// root chord length of horizontal stabilizer, meters
float g_fStabilizerTipChord = 1.5f;							// wing tip chord length of horizontal stabilizer, meters
float g_fStabilizerAvgChord = 1.0f;							// average chord length of horizontal stabilizer, meters
float g_fStabilizerLeadingEdgeSweep = 5.0f * g_fDegToRad;	// leading edge sweep of horizontal stab, radians
float g_fStabilizerSemiSpan = 7.0f;						// semispan of horizontal stab, meters
float g_fStabilizerIncidenceAngle = 4.0f;					// angle of horizontal stab relative to airplane x axis, degrees

// geometry of the main wing (the rear wing)
float g_fDXMainWingCenterOfLift = 1.5f;						// offset in x direction from C.G. of main wing (the rear wing) quarter-chord point
float g_fMainWingRootChord = 4.0f;							// root chord length of horizontal stabilizer, meters
float g_fMainWingTipChord = 2.0f;							// wing tip chord length of horizontal stabilizer, meters
float g_fMainWingAvgChord = 3.0f;							// average chord length of main wing, meters
float g_fMainWingSemiSpan = 14.0f;							// semispan of main wing, meters
float g_fMainWingLeadingEdgeSweep = 20.0f * g_fDegToRad;	// leading edge sweep of main wing, radians
float g_fMainWingIncidenceAngle = 4.0f;						// angle of main wing relative to airplane x axis, degrees

// application settings
unsigned int g_uiNumMeshes = 0;								// Number of meshes in the scene
IndexedTriMesh *g_pMeshes = 0;								// Array of meshes
float g_fPhysicsTimeStep = 0.02f;							// lock physics to 50 updates/second
float g_fTimeScaleFactor = 1.0f;							// Factor to slow down/speed up time
float g_fAccumulatedTimeStep = 0.0f;						// accumulated time since last physics update
float g_fSimTime = 0.0f;									// current simulation time
LARGE_INTEGER g_liTimerFrequency;							// Timer frequency
LARGE_INTEGER g_liPreviousCounter;							// Value of performance counter at last update.
bool g_bPaused = true;										// true if sim is paused, false otherwise

//-------------------------------------------------------------------------

int main(int iArgC, char *ppcArgV[])
{
	glutInit(&iArgC, ppcArgV);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutCreateWindow("Longitudinal Airplane Demo");

	glutDisplayFunc(DisplayFunc);
	glutKeyboardFunc(KeyboardFunc);
	glutReshapeFunc(ReshapeFunc);
	glutIdleFunc(IdleFunc);

	// The demo currently supports Win32 systems with high resolution
	// performance counter support. If that support is not present,
	// the demo will not run.
	if (FALSE == QueryPerformanceFrequency(&g_liTimerFrequency))
		return(1);

	CreateScene();
	ResetSim();

	if (FALSE == QueryPerformanceCounter(&g_liPreviousCounter))
		QuitApplication();

	glutMainLoop();

	return(0);
}

void DisplayFunc(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (unsigned int i = 0; i < g_uiNumMeshes; i++)
	{
		glLoadIdentity();
// top view
//		gluLookAt(2.0f, 0.0f, 100.0f, 2.0f, 0.0f, 3.0f, 1.0f, 0.0f, 0.0f);
// side view
		gluLookAt(g_pfLocation[0], 50.0f, 5010.0f,
					g_pfLocation[0], 0.0f, 5000.0f, 0.0f, 0.0f, 1.0f);

		glTranslatef(g_pfLocation[0], g_pfLocation[1], g_pfLocation[2]);
		glRotatef(g_fOrientation, 0.0f, 1.0f, 0.0f);
		
		g_pMeshes[i].Draw();
	}

	glLoadIdentity();
	gluLookAt(g_pfLocation[0], 50.0f, 5010.0f,
				0.0f, 0.0f, 5000.0f, 0.0f, 0.0f, 1.0f);
	glTranslatef(g_pfLocation[0], g_pfLocation[1], g_pfLocation[2]);
	glRotatef(g_fOrientation, 0.0f, 1.0f, 0.0f);
	glDisable(GL_LIGHTING);
	glColor3f(1.f, 1.f, 1.f);
	glBegin(GL_POINTS);
		glVertex3fv(g_pfLocation);
	glEnd();
	glEnable(GL_LIGHTING);

	glutSwapBuffers();
}

void KeyboardFunc(unsigned char cKey, int iX, int iY)
{
	switch(cKey)
	{
		case 'q':
		case 'Q':
			QuitApplication();
			break;

		case 'r':
		case 'R':
			g_bPaused = true;
			ResetSim();
			break;

		case 'g':
		case 'G':
			g_bPaused = false;
			break;

		case 'p':
		case 'P':
			g_bPaused = true;
			break;

		case '+':
			g_fStabilizerIncidenceAngle += 0.25f;
			g_pMeshes[0].m_fYRotation += 0.25f; // for visualization
			break;

		case '-':
			g_fStabilizerIncidenceAngle -= 0.25f;
			g_pMeshes[0].m_fYRotation -= 0.25f; // for visualization
			break;

		case 'l':
		case 'L':
			g_bCGLocked = !g_bCGLocked;
			break;

		// increase simulation time scale factor by 0.1f;
		case '>':
			g_fTimeScaleFactor += 0.1f;
			if (g_fTimeScaleFactor > 2.0f)
				g_fTimeScaleFactor = 2.0f;
			break;
			
		// decrease simulation time scale factor by 0.1f;
		case '<':
			g_fTimeScaleFactor -= 0.1f;
			if (g_fTimeScaleFactor < 0.1f)
				g_fTimeScaleFactor = 0.1f;
			break;
	}
}

void ReshapeFunc(int iWidth, int iHeight)
{
	glViewport(0, 0, iWidth, iHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, float(iWidth)/float(iHeight), 1.0f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glPointSize(3.0f);
	float fWhite[] = {1.0f, 1.0f, 1.0f, 1.0f};
	float fDirectionalLight0Position[] = {0.0f, 0.0f, 1.0f, 0.0f};
	float fDirectionalLight1Position[] = {-1.0f, 0.0f, 0.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, fWhite);
	glLightfv(GL_LIGHT0, GL_POSITION, fDirectionalLight0Position);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, fWhite);
	glLightfv(GL_LIGHT1, GL_POSITION, fDirectionalLight1Position);
}

void IdleFunc(void)
{
	UpdatePhysics();
	glutPostRedisplay();
}

void QuitApplication()
{
	g_uiNumMeshes = 0;
	delete [] g_pMeshes;
	g_pMeshes = 0;
	exit(0);
}

void UpdatePhysics()
{
	LARGE_INTEGER PC;

	QueryPerformanceCounter(&PC);

	LONGLONG DeltaCounter = PC.QuadPart - g_liPreviousCounter.QuadPart;

	g_liPreviousCounter = PC;

	float fA = float(DeltaCounter);
	float fB = float(g_liTimerFrequency.QuadPart);
	float fDeltaTime = float(DeltaCounter) / float(g_liTimerFrequency.QuadPart);

	if (g_bPaused)
		fDeltaTime = 0.0f;

	g_fAccumulatedTimeStep += fDeltaTime;

	while (g_fAccumulatedTimeStep > g_fPhysicsTimeStep)
	{
		PhysicsStep();
		g_fAccumulatedTimeStep -= g_fPhysicsTimeStep;
	}
}

void PhysicsStep()
{
	// calculate the relative wind
	float pfRelativeWind[] = {	g_pfAbsoluteWind[0] - g_pfVelocity[0],
								g_pfAbsoluteWind[1] - g_pfVelocity[1],
								g_pfAbsoluteWind[2] - g_pfVelocity[2]};

	// calculate dynamic pressure, as experienced by the wing
	float fRelWindSpeed = sqrtf(pfRelativeWind[0] * pfRelativeWind[0] +
								pfRelativeWind[1] * pfRelativeWind[1] +
								pfRelativeWind[2] * pfRelativeWind[2]);
	float fDynamicPressure = 0.5f * g_fFluidDensity * fRelWindSpeed;

	// calc lift/drag on the horizontal stabilizer
	float fCL_hs = 0.0f, fCD_hs = 0.0f;

	// calculate the relative wind at the horizontal stabilizer center
	// of lift. It is different from the center-of-gravity relative
	// wind due to airplane rotation
	float pfHorizontalStabRelativeWind[] = {pfRelativeWind[0],
											pfRelativeWind[1],
											pfRelativeWind[2]};
	pfHorizontalStabRelativeWind[0] += g_fDXStabilizerWingCenterOfLift * g_fPitchSpeed * sin(g_fOrientation*g_fDegToRad);
	pfHorizontalStabRelativeWind[2] += g_fDXStabilizerWingCenterOfLift * g_fPitchSpeed * cos(g_fOrientation*g_fDegToRad);

	float fX = g_fDXStabilizerWingCenterOfLift * g_fPitchSpeed * sin(g_fOrientation*g_fDegToRad);
	float fY = g_fDXStabilizerWingCenterOfLift * g_fPitchSpeed * cos(g_fOrientation*g_fDegToRad);

	CalcLiftDrag(g_fStabilizerRootChord, g_fStabilizerTipChord,
					g_fStabilizerSemiSpan, g_fStabilizerLeadingEdgeSweep,
					pfHorizontalStabRelativeWind, g_fOrientation,
					g_fStabilizerIncidenceAngle, &fCL_hs, &fCD_hs);

//	fCL_hs *= 0.95f;

	// this SRef is for the _full_ stabilizer---both halves.
	float fSRef_hs = g_fStabilizerSemiSpan * (g_fStabilizerTipChord + g_fStabilizerRootChord);

	float fLiftWindAxes_hs = fDynamicPressure * fCL_hs * fSRef_hs;
	float fDragWindAxes_hs = fDynamicPressure * fCD_hs * fSRef_hs;

	// calc lift/drag on the main wing
	float fCL_w = 0.0f, fCD_w = 0.0f;

	// calculate the relative wind at the main wing center
	// of lift. It is different from the center-of-gravity relative
	// wind due to airplane rotation
	float pfMainWingRelativeWind[] = {pfRelativeWind[0],
											pfRelativeWind[1],
											pfRelativeWind[2]};
	pfMainWingRelativeWind[0] += g_fDXMainWingCenterOfLift * g_fPitchSpeed * sin(g_fOrientation*g_fDegToRad);
	pfMainWingRelativeWind[2] += g_fDXMainWingCenterOfLift * g_fPitchSpeed * cos(g_fOrientation*g_fDegToRad);

	fX = g_fDXMainWingCenterOfLift * g_fPitchSpeed * sin(g_fOrientation*g_fDegToRad);
	fY = g_fDXMainWingCenterOfLift * g_fPitchSpeed * cos(g_fOrientation*g_fDegToRad);

	CalcLiftDrag(g_fMainWingRootChord, g_fMainWingTipChord,
					g_fMainWingSemiSpan, g_fMainWingLeadingEdgeSweep,
					pfRelativeWind, g_fOrientation, g_fMainWingIncidenceAngle,
					&fCL_w, &fCD_w);

	// this SRef is for the _full_ wing---both halves.
	float fSRef_w = g_fMainWingSemiSpan * (g_fMainWingTipChord + g_fMainWingRootChord);

	float fLiftWindAxes_w = fDynamicPressure * fCL_w * fSRef_w;
	float fDragWindAxes_w = fDynamicPressure * fCD_w * fSRef_w;

	// Note that the two lift forces are perpendicular to the relative wind,
	// two drag forces are in the direction of the relative wind.
	
	// Calculate the net force and pitching moment. The lift and drag
	// magnitudes were computed in wind axes. The following increments
	// transform them into world axes, so that the net force is in
	// world axes.
	float pfNetForceWorldAxes[] = {0.0f, g_fGravitationalAccel * g_fMass};
	float fNetPitchingMoment = 0.0f;

	// add lift for horizontal stab, in wind axes
	pfNetForceWorldAxes[0] += -pfRelativeWind[1] * fLiftWindAxes_hs / fRelWindSpeed;
	pfNetForceWorldAxes[1] +=  pfRelativeWind[0] * fLiftWindAxes_hs / fRelWindSpeed;

	// add drag for horizontal stab, in wind axes
	pfNetForceWorldAxes[0] += pfRelativeWind[0] * fDragWindAxes_hs / fRelWindSpeed;
	pfNetForceWorldAxes[1] += pfRelativeWind[1] * fDragWindAxes_hs / fRelWindSpeed;

	// add lift for main wing, in wind axes
	pfNetForceWorldAxes[0] += -pfRelativeWind[1] * fLiftWindAxes_w / fRelWindSpeed;
	pfNetForceWorldAxes[1] +=  pfRelativeWind[0] * fLiftWindAxes_w / fRelWindSpeed;

	// add drag for main wing, in wind axes
	pfNetForceWorldAxes[0] += pfRelativeWind[0] * fDragWindAxes_w / fRelWindSpeed;
	pfNetForceWorldAxes[1] += pfRelativeWind[1] * fDragWindAxes_w / fRelWindSpeed;

	// To keep the airplane moving horizontally at a constant speed...add in
	// a thrust force equal to the horizontal force opposing velocity. Basically,
	// this means zeroing out the horizontal component of force! In a real
	// game or simulation, the thrust would be simulated somehow as well.
	pfNetForceWorldAxes[0] = 0.0f;

	// Calculate the pitching moment
	// this simple is approximate, but accurate when the airplane X axis is at a small angle to relative wind

	// add approximate pitching moment due to horizontal stabilizer
	fNetPitchingMoment -= fLiftWindAxes_hs * g_fDXStabilizerWingCenterOfLift;

	// add approximate pitching moment due to main wing
	fNetPitchingMoment -= fLiftWindAxes_w * g_fDXMainWingCenterOfLift;

	// update the center-of-gravity location. If Center of gravity is locked,
	// don't update position.
	if (!g_bCGLocked)
	{
		// increment the translational velocity based on acceleration due to applied
		// forces. This is a simple Euler integration, which is simple,
		// but not at all the best method to use in general.
		//
		// note that since we are tracking airplane velocity and location for a full
		// 3D world coordinate system, we apply pfNetForceWorldAxes[0] to world
		// velocity/position[0], and pfNetForceWorldAxes[1] to world velocity/position[2].
		g_pfVelocity[0] += g_fPhysicsTimeStep * g_fTimeScaleFactor * pfNetForceWorldAxes[0] / g_fMass;
		g_pfVelocity[2] += g_fPhysicsTimeStep * g_fTimeScaleFactor * pfNetForceWorldAxes[1] / g_fMass;

		// update the position. Not simple Euler since it uses the updated (t + dt)
		// velocity.
		g_pfLocation[0] += g_pfVelocity[0] * g_fPhysicsTimeStep * g_fTimeScaleFactor;
		g_pfLocation[1] += g_pfVelocity[1] * g_fPhysicsTimeStep * g_fTimeScaleFactor;
		g_pfLocation[2] += g_pfVelocity[2] * g_fPhysicsTimeStep * g_fTimeScaleFactor;
	}

	// update rotational velocity based on pitching moment. Pitch speed is
	// radians per second. This is simple Euler integration. We only need
	// pitch speed to compute local object velocity at center of lift for
	// the two wings.
	g_fPitchSpeed += g_fPhysicsTimeStep * g_fTimeScaleFactor * fNetPitchingMoment / g_fMomentOfInertia;

	// finally, update airplane orientation. Must convert increment from radians
	// to degrees, since orientation is stored as degrees. This is Verlet integration,
	// which is more stable than simple Euler.
	float fTemp = g_fOrientation;
	g_fOrientation = (2.0f * g_fOrientation) - g_fPreviousOrientation + fNetPitchingMoment * (g_fRadToDeg * g_fPhysicsTimeStep * g_fTimeScaleFactor * g_fPhysicsTimeStep * g_fTimeScaleFactor) / g_fMomentOfInertia;
	g_fPreviousOrientation = fTemp;

}

void CreateScene()
{
// The first mesh is the horizontal stabilizer (the forward wing)
	g_pMeshes = new IndexedTriMesh[2];
	g_pMeshes[0].m_pfVertCoords = new float[6 * 3];		// 6 verts: two for each wingtip (port and starboard) + 2 for centerline
	g_pMeshes[0].m_pfVertNormals = new float[6 * 3];

	// x position of horizontal stab wingtip relative to horizontal stab wing root at leading edge
	float fDXTip = g_fStabilizerSemiSpan * tan(g_fStabilizerLeadingEdgeSweep);

	// x position of horizontal stab average quarter-chord, relative to leading edge root
	float fTipDXQuarterChord = (0.5f * fDXTip) + 0.25f * g_fStabilizerAvgChord;

	// first two vertices are left horizontal stab tip, leading edge
	// first. Last two vertices are right horizontal stab tip, leading
	// edge first
	g_pMeshes[0].m_pfVertCoords[0] = fDXTip - fTipDXQuarterChord;
	g_pMeshes[0].m_pfVertCoords[1] = -g_fStabilizerSemiSpan;
	g_pMeshes[0].m_pfVertCoords[2] = 0.0f;
	g_pMeshes[0].m_pfVertCoords[12] = g_pMeshes[0].m_pfVertCoords[0];
	g_pMeshes[0].m_pfVertCoords[13] = g_fStabilizerSemiSpan;
	g_pMeshes[0].m_pfVertCoords[14] = 0.0f;

	g_pMeshes[0].m_pfVertCoords[3] = g_pMeshes[0].m_pfVertCoords[0] + g_fStabilizerTipChord;
	g_pMeshes[0].m_pfVertCoords[4] = -g_fStabilizerSemiSpan;
	g_pMeshes[0].m_pfVertCoords[5] = 0.0f;
	g_pMeshes[0].m_pfVertCoords[15] = g_pMeshes[0].m_pfVertCoords[3];
	g_pMeshes[0].m_pfVertCoords[16] = g_fStabilizerSemiSpan;
	g_pMeshes[0].m_pfVertCoords[17] = 0.0f;

	// middle two vertices are centerline
	g_pMeshes[0].m_pfVertCoords[6] = - fTipDXQuarterChord;
	g_pMeshes[0].m_pfVertCoords[7] = 0.0f;
	g_pMeshes[0].m_pfVertCoords[8] = 0.0f;

	g_pMeshes[0].m_pfVertCoords[9] = g_pMeshes[0].m_pfVertCoords[6] + g_fStabilizerRootChord;
	g_pMeshes[0].m_pfVertCoords[10] = 0.0f;
	g_pMeshes[0].m_pfVertCoords[11] = 0.0f;

	for (unsigned int i = 0; i < 6; i++)
	{
		g_pMeshes[0].m_pfVertNormals[i * 3 + 0] = 0.0f;
		g_pMeshes[0].m_pfVertNormals[i * 3 + 1] = 0.0f;
		g_pMeshes[0].m_pfVertNormals[i * 3 + 2] = 1.0f;
	}

	g_pMeshes[0].m_uiNumVerts = 6;
	g_pMeshes[0].m_uiNumIndices = 12;					// four triangles: two for left wing halfk, two for right wing half
	g_pMeshes[0].m_puiIndices = new unsigned int[12];

	// left side horizontal stab
	g_pMeshes[0].m_puiIndices[0] = 0;
	g_pMeshes[0].m_puiIndices[1] = 1;
	g_pMeshes[0].m_puiIndices[2] = 3;
	g_pMeshes[0].m_puiIndices[3] = 0;
	g_pMeshes[0].m_puiIndices[4] = 3;
	g_pMeshes[0].m_puiIndices[5] = 2;

	// right side horizontal stab
	g_pMeshes[0].m_puiIndices[6] = 2;
	g_pMeshes[0].m_puiIndices[7] = 3;
	g_pMeshes[0].m_puiIndices[8] = 5;
	g_pMeshes[0].m_puiIndices[9] = 2;
	g_pMeshes[0].m_puiIndices[10] = 5;
	g_pMeshes[0].m_puiIndices[11] = 4;

	g_pMeshes[0].m_pfLocation[0] = g_fDXStabilizerWingCenterOfLift;
	g_pMeshes[0].m_pfLocation[1] = 0.0f;
	g_pMeshes[0].m_pfLocation[2] = 0.0f;

	g_pMeshes[0].m_fYRotation = g_fStabilizerIncidenceAngle;

// second wing is the larger, rear wing
	g_pMeshes[1].m_pfVertCoords = new float[6 * 3];		// 6 verts: two for each wingtip (port and starboard) + 2 for centerline
	g_pMeshes[1].m_pfVertNormals = new float[6 * 3];

	// x position of main wingtip relative to main wing root
	fDXTip = g_fMainWingSemiSpan * tan(g_fMainWingLeadingEdgeSweep);

	// x position of main average quarter-chord, relative to main
	// wing root
	fTipDXQuarterChord = (0.5f * fDXTip) + 0.25f * g_fMainWingAvgChord;

	// first two vertices are left main wing tip, leading edge
	// first. Last two vertices are right main wing tip, leading
	// edge first
	g_pMeshes[1].m_pfVertCoords[0] = fDXTip - fTipDXQuarterChord;
	g_pMeshes[1].m_pfVertCoords[1] = -g_fMainWingSemiSpan;
	g_pMeshes[1].m_pfVertCoords[2] = 0.0f;
	g_pMeshes[1].m_pfVertCoords[12] = g_pMeshes[1].m_pfVertCoords[0];
	g_pMeshes[1].m_pfVertCoords[13] = g_fMainWingSemiSpan;
	g_pMeshes[1].m_pfVertCoords[14] = 0.0f;

	g_pMeshes[1].m_pfVertCoords[3] = g_pMeshes[1].m_pfVertCoords[0] + g_fMainWingTipChord;
	g_pMeshes[1].m_pfVertCoords[4] = -g_fMainWingSemiSpan;
	g_pMeshes[1].m_pfVertCoords[5] = 0.0f;
	g_pMeshes[1].m_pfVertCoords[15] = g_pMeshes[1].m_pfVertCoords[3];
	g_pMeshes[1].m_pfVertCoords[16] = g_fMainWingSemiSpan;
	g_pMeshes[1].m_pfVertCoords[17] = 0.0f;

	// middle two vertices are centerline
	g_pMeshes[1].m_pfVertCoords[6] = - fTipDXQuarterChord;
	g_pMeshes[1].m_pfVertCoords[7] = 0.0f;
	g_pMeshes[1].m_pfVertCoords[8] = 0.0f;

	g_pMeshes[1].m_pfVertCoords[9] = g_pMeshes[1].m_pfVertCoords[6] + g_fMainWingRootChord;
	g_pMeshes[1].m_pfVertCoords[10] = 0.0f;
	g_pMeshes[1].m_pfVertCoords[11] = 0.0f;

	for (i = 0; i < 6; i++)
	{
		g_pMeshes[1].m_pfVertNormals[i * 3 + 0] = 0.0f;
		g_pMeshes[1].m_pfVertNormals[i * 3 + 1] = 0.0f;
		g_pMeshes[1].m_pfVertNormals[i * 3 + 2] = 1.0f;
	}

	g_pMeshes[1].m_uiNumVerts = 6;
	g_pMeshes[1].m_uiNumIndices = 12;					// four triangles: two for left wing halfk, two for right wing half
	g_pMeshes[1].m_puiIndices = new unsigned int[12];

	// left side main wing
	g_pMeshes[1].m_puiIndices[0] = 0;
	g_pMeshes[1].m_puiIndices[1] = 1;
	g_pMeshes[1].m_puiIndices[2] = 3;
	g_pMeshes[1].m_puiIndices[3] = 0;
	g_pMeshes[1].m_puiIndices[4] = 3;
	g_pMeshes[1].m_puiIndices[5] = 2;

	// right side main wing
	g_pMeshes[1].m_puiIndices[6] = 2;
	g_pMeshes[1].m_puiIndices[7] = 3;
	g_pMeshes[1].m_puiIndices[8] = 5;
	g_pMeshes[1].m_puiIndices[9] = 2;
	g_pMeshes[1].m_puiIndices[10] = 5;
	g_pMeshes[1].m_puiIndices[11] = 4;

	g_pMeshes[1].m_pfLocation[0] = g_fDXMainWingCenterOfLift;
	g_pMeshes[1].m_pfLocation[1] = 0.0f;
	g_pMeshes[1].m_pfLocation[2] = 0.0f;

	g_pMeshes[1].m_fYRotation = g_fMainWingIncidenceAngle;

	g_uiNumMeshes = 2;
}

void ResetSim()
{
	g_pfLocation[0] = 0.0f;
	g_pfLocation[1] = 0.0f;
	g_pfLocation[2] = 5000.0f;

	g_pfVelocity[0] = -100.0f;
	g_pfVelocity[1] = 0.0f;
	g_pfVelocity[2] = 0.0f;

	g_pfAbsoluteWind[0] = 0.0f;
	g_pfAbsoluteWind[1] = 0.0f;
	g_pfAbsoluteWind[2] = 0.0f;

	g_fStabilizerIncidenceAngle = 4.0f;
	g_fMainWingIncidenceAngle = 4.0f;

	g_fOrientation = 0.0f;
	g_fPreviousOrientation = 0.0f;
	g_fPitchSpeed = 0.0f;
}

// calculate the lift and drag coefficient of a wing
// this function is NOT written for efficiency or speed! It
// is written for clarity. It is based on Equations 10 and
// 11 from the text.
void CalcLiftDrag(const float fRootChord, const float fTipChord,
	const float fSemispan, const float fLESweep,
	const float *pfRelativeWind, float fAirplaneOrientation,
	float fWingIncidence, float *pfCL, float *pfCD)
{
	if (0 == pfRelativeWind || 0 == pfCL || 0 == pfCD)
		return;

	float fAspectRatio = 2.0f * fSemispan / (fRootChord + fTipChord);

	// calculate angle of attack. This is simply the angle between
	// the relative wind and the wing plane (for untwisted wings that have
	// curvature that is symmetric top-to-bottom)

	// wing absolute incidence is angle of the wing, measured relative to
	// world xy plane, positive when the wing leading edge is above the
	// trailing edge.
	float fWingAbsoluteIncidence = g_fDegToRad * (fAirplaneOrientation + fWingIncidence);

	// fWingCenterline is a vector representing the direction of the wing centerline,
	// measured in world coordinates. Since wing is symmetric about Y, we don't
	// need a Y coordinate (it is 0). This is a vector in the xz plane.
	float fWingCenterline[] = {cos(fWingAbsoluteIncidence),
								-sin(fWingAbsoluteIncidence)};

	// Dot wing centerline with relative wind direction to get the cosine of
	// angle of attack, fCosAlpha. Ignore y component since we are
	// just dealing with longitudinal geometry.
	float fRelWindSpeed = sqrtf(pfRelativeWind[0] * pfRelativeWind[0] +
								pfRelativeWind[2] * pfRelativeWind[2]);

	// The second line of this equation is correct, since fWingCenterline
	// written as a 2D (xz) vector. We're just implicitly projecting
	// pfRelative wind into the xz plane.
	float fCosAlpha =	(pfRelativeWind[0] * fWingCenterline[0] +
						 pfRelativeWind[2] * fWingCenterline[1]) / fRelWindSpeed;

	// find magnitude of angle of attack, fAlpha
	float fAlpha = acosf(fCosAlpha);

	// determine sign of alpha by observing the sign of the z coordinate of
	// pfRelativeWind and fWingCenterline. For the true 3D case, you will
	// have to observe the 3D cross product relative to the orientation of
	// a plane parallel to the wing's local xz plane.
	float fCrossProductZ = pfRelativeWind[0] * fWingCenterline[1] - pfRelativeWind[2] * fWingCenterline[0];
	if (fCrossProductZ > 0.0f)
		fAlpha = -fAlpha;

	// calculate lift coefficient
	static float fCL0 = 0.0f;
	float fRatio = fAspectRatio / (2.0f * cosf(fLESweep));
	float fDenominator = 1.0f + sqrtf(1.0f + fRatio * fRatio);
	*pfCL = fCL0 + (g_fPI * fAspectRatio * fAlpha / fDenominator);

	// calculate drag coefficient
	static float fCD0 = 0.045f;
	static float fK = 1.0f/(g_fPI * fAspectRatio * 0.8f);
	*pfCD = fCD0 + fK * (*pfCL) * (*pfCL);
}
