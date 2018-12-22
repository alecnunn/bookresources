/*****************************************************************************
|
| CurveBall
|
| by Graham S. Rhodes
| September 2004
|
| A simple demonstration of back of the envelope aerodynamics. This demo
| simulates the flight of a baseball, with spin that results in a lifting
| force that causes the ball to curve to the left or right.
|
| Keyboard controls:
|
|  'r' - restart the simulation, and put it in pause mode
|  'q' - quit the application
|  'a' - decrease the speed of the throw
|  's' - increase the speed of the throw
|  '+' - increase the rate of spin
|  '-' - decreate the rate of spin
|  'g' - start the simulation, or take it out of pause mode
|  'p' - pause the simulation
|  '>' - increase simulation speed
|  '<' - decrease simulation speed
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

// supporting classes

class IndexedTriMesh
{
	public:
		IndexedTriMesh();
		~IndexedTriMesh();

		void Draw();

		bool Load(FILE *pIn);

	protected:
		void Reset();

	public:
		unsigned int m_uiNumVerts;					// Number of vertices
		unsigned int m_uiNumIndices;				// Number of indices
		float *m_pfVertCoords;						// array of vertex coordinates, 3 entries for each vertex
		float *m_pfVertNormals;						// array of vertex normals, 3 entries for each vertex
		float *m_pfVertTexCoords;					// array of texture coordinates, 2 entries for each vertex
		unsigned int *m_puiIndices;					// array of indices defining the mesh, 3 entries for each triangle
		float m_pfColor[4];							// Color for the mesh.
		float m_pfLocation[3];						// Location in world space
};

// Method prototypes
void QuitApplication();
void UpdatePhysics();
void PhysicsStep();
void CreateScene();
void ResetSim();

// GLUT callback method prototypes
void DisplayFunc(void);
void KeyboardFunc(unsigned char cKey, int iX, int iY);
void ReshapeFunc(int iWidth, int iHeight);
void IdleFunc(void);

// globals
float g_fPhysicsTimeStep = 0.02f;					// lock physics to 50 updates/second
float g_fTimeScaleFactor = 1.0f;					// Factor to slow down/speed up time
float g_fAccumulatedTimeStep = 0.0f;				// accumulated time since last physics update
float g_fSimTime = 0.0f;							// current simulation time
float g_OneOverTwoPi = 1.0f/(8.0f * atanf(1.0f));	// inverse of 2*pi.
float g_fFluidDensity = 1.225f;						// Air density around sea level, kg/meters cubed
float g_fPI = 4.0f * atanf(1.0f);					// PI
float g_fGravitationalAccel = -9.8f;				// rate of gravitational acceleration, in Z direction, meters/second-squared
float g_fRandMax(float(RAND_MAX));					// maximum value returned by rand()
float g_fRadiusOfBall = 0.03632f;					// radius of ball, in meters (equivalent to 1.43 inches).
float g_fRotationalVelocity = (100.0f / 60.0f) * (2.0f * 3.1415f);	// rotational velocity of ball, radians per second (initially corresponds to 100 revolutions per minute)
float g_fThrowSpeed = 0.447f * 85.0f;					// velocity of ball as thrown, meters per second. (Initially corresponds to around 85 miles per hour)
float g_pfRotationDirection[] = {0.0f, 0.0f, 1.0f};	// Direction of ball spin. Vertical up or down produces curve ball. Horizontal towards the left of the pitcher produces drop ball.
float g_pfCurrentVelocity[] = {0.0f, 0.0f, 0.0f};	// Current velocity of the ball, meters/second
float g_fBallMass = 0.142f;							// Mass of ball, kilograms (equivalent to 5 ounces)
float g_fHalfRhoSref = 0.5f * g_fFluidDensity * g_fPI * g_fRadiusOfBall * g_fRadiusOfBall;	// 1/2 * density * reference area
unsigned int g_uiNumMeshes = 0;						// Number of meshes in the scene
IndexedTriMesh *g_pMeshes = 0;						// Array of meshes
LARGE_INTEGER g_liTimerFrequency;					// Timer frequency
LARGE_INTEGER g_liPreviousCounter;					// Value of performance counter at last update.
bool g_bPaused = true;								// true if sim is paused, false otherwise

int main(int iArgC, char *ppcArgV[])
{
	glutInit(&iArgC, ppcArgV);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutCreateWindow("CurveBall Demo");

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

	// render the meshes
	for (unsigned int i = 0; i < g_uiNumMeshes; i++)
	{
		glLoadIdentity();
		// top view
		 gluLookAt(0.0f, 0.0f, 150.0f, 0.0f, 0.0f, 3.0f, 1.0f, 0.0f, 0.0f);
		// side view
		//gluLookAt(100.0f, 100.0f, 10.0f, 0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 1.0f);
		g_pMeshes[i].Draw();
	}

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

		// increase throw speed by 5 miles per hour
		case 's':
		case 'S':
			g_fThrowSpeed += (0.447f * 5.0f);
			if (g_fThrowSpeed > (0.447f * 90.0f))
				g_fThrowSpeed = 0.447f * 90.0f;
			break;

		// decrease throw speed by 5 miles per hour
		case 'a':
		case 'A':
			g_fThrowSpeed -= (0.447f * 5.0f);
			if (g_fThrowSpeed < (0.447f * 70.0f))
				g_fThrowSpeed = 0.447f * 70.0f;
			break;

		// increment rotation velocity by 10 rev/minute
		case '+':
			g_fRotationalVelocity += (10.0f / 60.0f) * (2.0f * 3.1415f);
			if (g_fRotationalVelocity > (100.0f / 60.0f) * (2.0f * 3.1415f))
				g_fRotationalVelocity = (100.0f / 60.0f) * (2.0f * 3.1415f);
			break;

		// decrement rotation velocity by 10 rev/minute
		case '-':
			g_fRotationalVelocity -= (10.0f / 60.0f) * (2.0f * 3.1415f);
			if (g_fRotationalVelocity < (-100.0f / 60.0f) * (2.0f * 3.1415f))
				g_fRotationalVelocity = (-100.0f / 60.0f) * (2.0f * 3.1415f);
			break;

		case 'g':
		case 'G':
			g_bPaused = false;
			break;

		case 'p':
		case 'P':
			g_bPaused = true;
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
	float fVwind[3], fForce[3], fVrelwind_squared;

	// Since there is no wind, the relative wind is just the opposite
	// of the current velocity of the baseball. For clarity, we copy
	// that into the following variable:
	fVwind[0] = -g_pfCurrentVelocity[0];
	fVwind[1] = -g_pfCurrentVelocity[1];
	fVwind[2] = -g_pfCurrentVelocity[2];

	fVrelwind_squared = fVwind[0] * fVwind[0] + fVwind[1] * fVwind[1] +
						fVwind[2] * fVwind[2];

	// we need this to normalize the drag direction
	float fOneOverVrelwind = 1.0f / sqrtf(fVrelwind_squared);

	// calculate the drag force. Assume Reynold's Number
	// is in the turbulent drag range due to the roughness
	// on the ball surface caused by the lace. Let the
	// drag coefficient be a constant at 0.2.
	const float fCD = 0.2f;
	float fDragMagnitude = g_fHalfRhoSref * fVrelwind_squared * fCD;

	// accumulate drag into the total force
	fForce[0] = fDragMagnitude * fVwind[0] * fOneOverVrelwind;
	fForce[1] = fDragMagnitude * fVwind[1] * fOneOverVrelwind;
	fForce[2] = fDragMagnitude * fVwind[2] * fOneOverVrelwind;

	// calculate the lift force.
	float fLift[3];

	// the following multiplier is simply the magnitude of all the constants
	// from Equation (9) in the text. Some of the multiplies could be precomputed,
	// but are expanded here to clarity when comparing to the text. The "length"
	// of the bounding cylinder is 2 * radius, which produces an extra factor
	// of 2 and an extra radius in place of the variable l.
	float fLiftMultiplier = 0.785f * 4.0f * g_fFluidDensity * g_fRadiusOfBall *
							g_fRadiusOfBall * g_fRadiusOfBall * g_fPI *
							g_fRotationalVelocity;

	fLift[0] = fLiftMultiplier * (fVwind[1] * g_pfRotationDirection[2] - fVwind[2] * g_pfRotationDirection[1]);
	fLift[1] = fLiftMultiplier * (fVwind[2] * g_pfRotationDirection[0] - fVwind[0] * g_pfRotationDirection[2]);
	fLift[2] = fLiftMultiplier * (fVwind[0] * g_pfRotationDirection[1] - fVwind[1] * g_pfRotationDirection[0]);

	// accumulate lift into the total force
	fForce[0] += fLift[0];
	fForce[1] += fLift[1];
	fForce[2] += fLift[2];

	// finally, accumulate the ball's weight into the total force, to simulate
	// the effect of gravity
	fForce[2] += g_fBallMass * g_fGravitationalAccel;

	// increment the velocity based on acceleration due to applied
	// forces. This is a simple Euler integration, which is simple,
	// but not at all the best method to use.
	g_pfCurrentVelocity[0] += g_fPhysicsTimeStep * g_fTimeScaleFactor * fForce[0] / g_fBallMass;
	g_pfCurrentVelocity[1] += g_fPhysicsTimeStep * g_fTimeScaleFactor * fForce[1] / g_fBallMass;
	g_pfCurrentVelocity[2] += g_fPhysicsTimeStep * g_fTimeScaleFactor * fForce[2] / g_fBallMass;

	// update the location. Not simple Euler since it uses the updated (t + dt)
	// velocity.
	g_pMeshes[1].m_pfLocation[0] += g_pfCurrentVelocity[0] * g_fPhysicsTimeStep * g_fTimeScaleFactor;
	g_pMeshes[1].m_pfLocation[1] += g_pfCurrentVelocity[1] * g_fPhysicsTimeStep * g_fTimeScaleFactor;
	g_pMeshes[1].m_pfLocation[2] += g_pfCurrentVelocity[2] * g_fPhysicsTimeStep * g_fTimeScaleFactor;
}

void CreateScene()
{
	g_pMeshes = new IndexedTriMesh[2];
	if (0 != g_pMeshes)
		g_uiNumMeshes = 2;

	FILE *pIn = fopen("baseball_diamond.gsrm", "rt");
	if (0 != pIn)
	{
		g_pMeshes[0].Load(pIn);
		g_pMeshes[0].m_pfColor[0] = 0.0f;
		g_pMeshes[0].m_pfColor[1] = 0.75f;
		g_pMeshes[0].m_pfColor[2] = 0.0f;
		g_pMeshes[0].m_pfColor[3] = 1.0f;
		fclose(pIn);
	}

	pIn = fopen("baseball.gsrm", "rt");
	if (0 != pIn)
	{
		g_pMeshes[1].Load(pIn);
		fclose(pIn);
		g_pMeshes[1].m_pfColor[0] = 0.25f;
		g_pMeshes[1].m_pfColor[1] = 0.25f;
		g_pMeshes[1].m_pfColor[2] = 0.0f;
		g_pMeshes[1].m_pfColor[3] = 1.0f;
	}
}

void ResetSim()
{
	g_pfCurrentVelocity[0] = -g_fThrowSpeed;
	g_pfCurrentVelocity[1] = 0.0f;
	g_pfCurrentVelocity[2] = g_fThrowSpeed * 0.2f;
	g_pMeshes[1].m_pfLocation[0] = 0.0f;
	g_pMeshes[1].m_pfLocation[1] = 0.0f;
	g_pMeshes[1].m_pfLocation[2] = 5.0f;
}

IndexedTriMesh::IndexedTriMesh():
	m_uiNumVerts(0), m_uiNumIndices(0), m_pfVertCoords(0),
	m_pfVertNormals(0), m_pfVertTexCoords(0), m_puiIndices(0),
	m_pfColor(), m_pfLocation()
{
	m_pfColor[0] = 1.0f;
	m_pfColor[1] = 1.0f;
	m_pfColor[2] = 1.0f;
	m_pfColor[3] = 1.0f;
	m_pfLocation[0] = 0.0f;
	m_pfLocation[1] = 0.0f;
	m_pfLocation[2] = 0.0f;
}

IndexedTriMesh::~IndexedTriMesh()
{
	Reset();
}

void IndexedTriMesh::Reset()
{
	delete [] m_pfVertCoords;
	delete [] m_pfVertNormals;
	delete [] m_pfVertTexCoords;
	delete [] m_puiIndices;
	m_uiNumVerts = 0;
	m_uiNumIndices = 0;
	m_pfVertCoords = 0;
	m_pfVertNormals = 0;
	m_pfVertTexCoords = 0;
	m_puiIndices = 0;
	m_pfColor[0] = 1.0f;
	m_pfColor[1] = 1.0f;
	m_pfColor[2] = 1.0f;
	m_pfColor[3] = 1.0f;
	m_pfLocation[0] = 0.0f;
	m_pfLocation[1] = 0.0f;
	m_pfLocation[2] = 0.0f;
}

void IndexedTriMesh::Draw()
{
//	glColor4fv(m_pfColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_pfColor);
	glTranslatef(m_pfLocation[0], m_pfLocation[1], m_pfLocation[2]);
	glBegin(GL_TRIANGLES);
		for (unsigned int i = 0; i < m_uiNumIndices; i += 3)
		{
			glVertex3fv(&m_pfVertCoords[3 * m_puiIndices[i + 0]]);
			glNormal3fv(&m_pfVertNormals[3 * m_puiIndices[i + 0]]);

			glVertex3fv(&m_pfVertCoords[3 * m_puiIndices[i + 1]]);
			glNormal3fv(&m_pfVertNormals[3 * m_puiIndices[i + 1]]);

			glVertex3fv(&m_pfVertCoords[3 * m_puiIndices[i + 2]]);
			glNormal3fv(&m_pfVertNormals[3 * m_puiIndices[i + 2]]);
		}
	glEnd();
}

bool IndexedTriMesh::Load(FILE *pIn)
{
	if (0 == pIn)
		return(false);

	Reset();

	char szLine[256];

	// read and skip first line
	fgets(szLine, 255, pIn);

	// read number of vertices and triangles
	unsigned int uiNumTris = 0;
	fscanf(pIn, "%i %i\n", &m_uiNumVerts, &uiNumTris);

	m_uiNumIndices = uiNumTris * 3;

	m_pfVertCoords = new float[m_uiNumVerts * 3];
	m_pfVertNormals = new float[m_uiNumVerts * 3];
	m_pfVertTexCoords = new float[m_uiNumVerts * 2];
	m_puiIndices = new unsigned int[m_uiNumIndices];
	if (0 == m_pfVertCoords || 0 == m_pfVertNormals || 0 == m_pfVertTexCoords ||
		0 == m_puiIndices)
	{
		Reset();
		return(false);
	}

	unsigned int i = 0, uiCoordIndex = 0, uiTexCoordIndex = 0;
	for (i = 0; i < m_uiNumVerts; i++,
			uiCoordIndex +=3, uiTexCoordIndex += 2)
	{
		fscanf(pIn, "%f %f %f %f %f %f %f %f\n",
				&m_pfVertCoords[uiCoordIndex + 0],
				&m_pfVertCoords[uiCoordIndex + 1],
				&m_pfVertCoords[uiCoordIndex + 2],
				&m_pfVertNormals[uiCoordIndex + 0],
				&m_pfVertNormals[uiCoordIndex + 1],
				&m_pfVertNormals[uiCoordIndex + 2],
				&m_pfVertTexCoords[uiTexCoordIndex + 0],
				&m_pfVertTexCoords[uiTexCoordIndex + 1]);
	}

	unsigned int uiIndexIndex = 0;
	for (i = 0; i < uiNumTris; i++, uiIndexIndex += 3)
	{
		fscanf(pIn, "%i %i %i\n", &m_puiIndices[uiIndexIndex + 0],
				&m_puiIndices[uiIndexIndex + 1],
				&m_puiIndices[uiIndexIndex + 2]);

		// subtract 1 so that we have zero-based indices
		m_puiIndices[uiIndexIndex + 0]--;
		m_puiIndices[uiIndexIndex + 1]--;
		m_puiIndices[uiIndexIndex + 2]--;
	}

	return(true);
}
