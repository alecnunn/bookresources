/*****************************************************************************
|
| WindStorm
|
| by Graham S. Rhodes
| September 2004
|
| A simple demonstration of back of the envelope aerodynamics. This demo
| simulates a simple tornado, with low mass, spherical particles moved by
| an aerodynamic drag force.
|
| FOR FUN:
|
|  - Try placing multiple tornadoes in the world, and see how particles
|    behave
|
|  - Try making the core of the storm something other than a vertical
|    line, e.g., make a tornado whose centerline is curved, more like
|    real life storms.
|
|  - Change the variation of storm strength with altitude to get a shape
|    that is more realistic.
|
|  - Pretend the particles are spinning at different rates, and add in
|    a lift force calculation based on the particle spin. (This can
|    cause particles to gain altitude, perhaps rising above the storm,
|    then shooting out like a normal projectile only to reenter the
|    storm again once their altitudes decrease. Could look pretty cool.
|
|  - Try the above, but animate the shape of the vortex core line.
|
|  - Try a weaker storm and scatter particles much further away from the
|    core. Make a hurricane instead of a tornado!
|
|  - Kill particles once they reach some maximum distance from the core,
|    based on altitude, then respawn new particles in the center of the
|    storm so you always have particles in the center. (Depends on what
|    you want the thing to look like, and whether or not you'll be able
|    to see the core within your game.)
|
| FOR EFFICIENCY AND STABILITY:
|
|  Use fewer particles, fancier rendering, a true particle system that
|  deals with the life/evolution/death of particles, etc.
|
|  (A BIG DEAL) Upgrade to use Verlet or Runge-Kutta integration. This will
|  help stability. The system, as is, is conditionally stable meaning its
|  stable for certain combinations of storm strength, particle mass, particle
|  radius, and physics time step. The stability bounds can be violated and if
|  this happens particles will behave strangely, overflow and other bad things
|  may happen, etc.
|
|  Render particles as a batch?
|
| This demo is a companion to the chapter titled "Back of the
| Envelope Aerodynamics for Game Physics" in the book "Game
| Programming Gems 5" published by Charles River Media
|
*******************************************************************************/

#pragma warning(disable : 4505)
#include <gl/glut.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

/******************************************************************************
| Defines that affect the way the simulation runs
*******************************************************************************/

// uncomment the following if you wish particles to be affected by gravity
// #define INCLUDE_GRAVITY

// uncomment the following if you wish to do a test where particles simply
// follow the velocity field of the storm.
// #define MASSLESS_PARTICLES

// uncomment the following if you wish the storm velocities to be evaluated
// precisely at the current particle location. If you do NOT define the
// following, then the velocity will be calculated in such a way that
// massless particles remain on a perfect concentric circle. Normally,
// you want to just use the true wind velocity at a particle point. The
// other option is just an experiment.
#define TRUE_WIND_VELOCITY

/******************************************************************************
| Method prototypes
*******************************************************************************/

void InitParticles();
void QuitApplication();
float WindStormStrength(float fAltitude);
void WindVelocity(float *pfLocation, float *pfVelocity);
void UpdatePhysics();
void PhysicsStep();

/******************************************************************************
| GLUT callback prototypes
*******************************************************************************/

void DisplayFunc(void);
void KeyboardFunc(unsigned char cKey, int iX, int iY);
void ReshapeFunc(int iWidth, int iHeight);
void IdleFunc(void);

/******************************************************************************
| The particle class stores the current state of each particle.
*******************************************************************************/

class Particle
{
	public:
		Particle();
		
	public:
		float m_pfPosition[3];	///<< current position of particle, in world coordinates
		float m_pfVelocity[3];	///<< current velocity of particle, in world coordinates
		float m_fMass;			///<< mass of particle, kilograms
		float m_fHalfRhoSref;	///<< 0.5 * fluid density (rho) * reference area (Sref)
};

/******************************************************************************
| Global variables
*******************************************************************************/

float g_fPhysicsTimeStep = 0.02f;					// lock physics to 50 updates/second
float g_fAccumulatedTimeStep = 0.0f;				// accumulated time since last physics update
float g_fSimTime = 0.0f;							// current simulation time
float g_OneOverTwoPi = 1.0f/(8.0f * atanf(1.0f));	// inverse of 2*pi.
float g_pfStormCenter[] = {0.0f, 0.0f};				// 2D center of storm at ground level.
float g_fFluidDensity = 1.225;						// Air density around sea level, kg/meters cubed
float g_fPI = 4.0f * atanf(1.0f);					// PI
float g_fGravitationalAccel = -9.8f;				// rate of gravitational acceleration, in Z direction, meters/second-squared
float g_fRandMax(float(RAND_MAX));					// maximum value returned by rand()
float g_fS0 = 0.5f;									// particle strength at altitude = 0 meters
float g_fS500 = 250.0f;								// particle strength at altitude = 500 meters
LARGE_INTEGER g_liTimerFrequency;					// Timer frequency
LARGE_INTEGER g_liPreviousCounter;					// Value of performance counter at last update.

const unsigned int g_uiGoalNumParticles = 20000;	// desired number of particles
unsigned int g_uiNumParticles = 0;					// Number of particles being simulated
Particle *g_pParticles = 0;							// Array of particles for a *very* simple particle system

/******************************************************************************
| All the code!
*******************************************************************************/

int main(int iArgC, char *ppcArgV[])
{
	glutInit(&iArgC, ppcArgV);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutCreateWindow("WindStorm Demo");

	glutDisplayFunc(DisplayFunc);
	glutKeyboardFunc(KeyboardFunc);
	glutReshapeFunc(ReshapeFunc);
	glutIdleFunc(IdleFunc);

	// The demo currently supports Win32 systems with high resolution
	// performance counter support. If that support is not present,
	// the demo will not run.
	if (FALSE == QueryPerformanceFrequency(&g_liTimerFrequency))
		return(1);

	InitParticles();

	if (FALSE == QueryPerformanceCounter(&g_liPreviousCounter))
		QuitApplication();

	glutMainLoop();

	return(0);
}

void DisplayFunc(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

// a semi-interesting viewpoint, from slightly above, looking slightly down
// on the storm.
//	gluLookAt(1000.0f, 1000.0f, 0.0f, 0.0f, 0.0f, 250.0f, 0.0f, 0.0f, 1.0f);

// The particular choice of numbers here makes the camera angle approximately
// 45 degrees from horizontal, looking down on the storm.
	gluLookAt(707.0f, 0.0f, 957.0f, 0.0f, 0.0f, 250.0f, -707.0f, 0.0f, 707.0f);

// a top view
//	gluLookAt(0.0f, 0.0f, 1000.0f, 0.0f, 0.0f, 250.0f, 1.0f, 0.0f, 0.0f);

	// render the particles
	glBegin(GL_POINTS);
	for (unsigned int i = 0; i < g_uiNumParticles; i++)
	{
		glNormal3f(g_pParticles[i].m_pfPosition[0], g_pParticles[i].m_pfPosition[1], 0.0f);
		glVertex3fv(g_pParticles[i].m_pfPosition);
	}
	glEnd();

	glutSwapBuffers();
}

void KeyboardFunc(unsigned char cKey, int iX, int iY)
{
	iX;
	iY;
	switch(cKey)
	{
		case 'q':
			QuitApplication();
			break;
	}
}

void ReshapeFunc(int iWidth, int iHeight)
{
	glViewport(0, 0, iWidth, iHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, float(iWidth)/float(iHeight), 1.0f, 20000.0f);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	float fColor[] = {0.8f, 0.8f, 1.0f, 1.0f};
	float fAmbientColor[] = {0.25f, 0.25f, 0.45f, 1.0f};
	float fLightPosition[] = {1.0f, 0.0f, 0.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, fColor);
	glLightfv(GL_LIGHT0, GL_AMBIENT, fAmbientColor);
	glLightfv(GL_LIGHT0, GL_POSITION, fLightPosition);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, fColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, fAmbientColor);
	glEnable(GL_NORMALIZE);
	glPointSize(1.0f);
}

void IdleFunc(void)
{
	UpdatePhysics();
	glutPostRedisplay();
}

Particle::Particle() : m_pfPosition(), m_pfVelocity(),
	m_fMass(1.0f), m_fHalfRhoSref(1.0f)
{
	m_pfPosition[0] = 0.0f;
	m_pfPosition[1] = 0.0f;
	m_pfPosition[2] = 0.0f;

	m_pfVelocity[0] = 0.0f;
	m_pfVelocity[1] = 0.0f;
	m_pfVelocity[2] = 0.0f;
}

void InitParticles()
{
	g_pParticles = new Particle[g_uiGoalNumParticles];
	float fRadius, Sref;
	if (0 != g_pParticles)
	{
		g_uiNumParticles = g_uiGoalNumParticles;
		for (unsigned int i = 0; i < g_uiGoalNumParticles; i++)
		{
			g_pParticles[i].m_pfPosition[0] = -2.5f + 5.0f * float(rand()) / g_fRandMax;
			g_pParticles[i].m_pfPosition[1] = -2.5f + 5.0f * float(rand()) / g_fRandMax;
			g_pParticles[i].m_pfPosition[2] = 500.0f * float(rand()) / g_fRandMax;

			g_pParticles[i].m_fMass = 0.05f + .05f * float(rand()) / g_fRandMax;
			fRadius = 0.2f + 0.2f*float(rand()) / g_fRandMax;
			Sref = g_fPI * fRadius * fRadius;
			g_pParticles[i].m_fHalfRhoSref = 0.5 * Sref * g_fFluidDensity;
		}
	}
}

void QuitApplication()
{
	delete [] g_pParticles;
	exit(0);
}

// calculate the storm strength at the sent altitude
float WindStormStrength(float fAltitude)
{
	return(g_fS0 + ((g_fS500 - g_fS0) * fAltitude * fAltitude / 250000.0f));
}

// calculate the wind velocity due to the storm, at the sent
// location
void WindVelocity(float *pfLocation, float *pfVelocity)
{
	if (0 == pfLocation || 0 == pfVelocity)
		return;

	float fDX = pfLocation[0] - g_pfStormCenter[0];
	float fDY = pfLocation[1] - g_pfStormCenter[1];

	float fR = sqrtf(fDX * fDX + fDY * fDY);
	if (fR < 1.e-4)
		fR = 1.0f;

// for a true potential vortex, the velocity approaches
// infinity at the core. We need to limit this to avoid
// undesirable effects. We limit it here by artificially
// forcing particles inside a certain radius to behave
// as though they were at that limit radius. Another
// approach would be to have the vortex strength fade
// towards zero at the core.
	float fOneOverR = __min(1.0f, 1.0f / fR);

// TRUE_WIND_VELOCITY calculates the actual velocity at a point due to
// a potential vortex. If this is NOT defined, a numerical approximation
// to the velocity is calculated instead, based on the physics time step,
// that is designed to keep a particle with zero mass on an exact circular
// path (to floating point precision). The latter case can produce better
// results, since it prevents particle drift due to anything other than
// inertia. The former case, using the true potential vortex velocity,
// will result in particles that drift away from a circular path just
// due to truncation error in the time stepped solution.
//#define TRUE_WIND_VELOCITY
#ifdef TRUE_WIND_VELOCITY
	float fCommonFactor = WindStormStrength(pfLocation[2]) * fOneOverR * g_OneOverTwoPi;

	pfVelocity[0] =  pfLocation[1] * fCommonFactor;
	pfVelocity[1] = -pfLocation[0] * fCommonFactor;
#else
	float fTangentialVelocity = WindStormStrength(pfLocation[2]) * fOneOverR * g_OneOverTwoPi;
	float fDeltaAngle = fTangentialVelocity * g_fPhysicsTimeStep;
	float fAngle = atan2(fDY, fDX) + fDeltaAngle;
	float fNewX = fR * cos(fAngle);
	float fNewY = fR * sin(fAngle);
	pfVelocity[0] = (fNewX - pfLocation[0]) / g_fPhysicsTimeStep;
	pfVelocity[1] = (fNewY - pfLocation[1]) / g_fPhysicsTimeStep;
#endif // TRUE_WIND_VELOCITY

// there is no vertical wind velocity due to the storm in this example. You
// could add it to get interesting effects, though.
	pfVelocity[2] = 0.0f;
}

// Update the physics as necessary. This uses a simple
// frame-rate independent technique!
void UpdatePhysics()
{
	LARGE_INTEGER PC;

	if (FALSE == QueryPerformanceCounter(&PC))
		return;

	LONGLONG DeltaCounter = PC.QuadPart - g_liPreviousCounter.QuadPart;

	g_liPreviousCounter = PC;

	float fDeltaTime = float(DeltaCounter) / float(g_liTimerFrequency.QuadPart);

	g_fAccumulatedTimeStep += fDeltaTime;

	while (g_fAccumulatedTimeStep > g_fPhysicsTimeStep)
	{
		PhysicsStep();
		g_fAccumulatedTimeStep -= g_fPhysicsTimeStep;
	}
}

// Take one physics step. This is where the actual physics integration
// step is performed. The time step is always fixed at g_fPhysicsTimeStep
void PhysicsStep()
{
	float fVwind[3], fForce[3], fDragMagnitude, fVrelwind_squared, fSpeed;
	for (unsigned int i = 0; i < g_uiNumParticles; i++)
	{
		WindVelocity(g_pParticles[i].m_pfPosition, fVwind);

// DO_TEST treats the particles as though they were massless, in which
// case they simply take on the velocity of the windfield instead of
// reacting physically to applied forces. This is useful to check and
// ensure that the simulation does not exhibit significant numerical
// drift (error).
#ifndef MASSLESS_PARTICLES
		// turn fVwind into relative wind and compute its
		// magnitude squared
		fVwind[0] -= g_pParticles[i].m_pfVelocity[0];
		fVwind[1] -= g_pParticles[i].m_pfVelocity[1];
		fVwind[2] -= g_pParticles[i].m_pfVelocity[2];
		fVrelwind_squared = fVwind[0] * fVwind[0] + fVwind[1] * fVwind[1] + fVwind[2] * fVwind[2];
		fSpeed = sqrtf(fVrelwind_squared);

		// calculate the drag force. Assume Reynold's Number
		// is between 2000 and 200,000, and, therefore,
		// drag coefficient is roughly a constant at 0.4.
		const float fCD = 0.4;

		fDragMagnitude = g_pParticles[i].m_fHalfRhoSref * fVrelwind_squared * fCD;

		// fVwind[*] / fSpeed gives the direction of the drag force
		fForce[0] = fDragMagnitude * fVwind[0] / fSpeed;
		fForce[1] = fDragMagnitude * fVwind[1] / fSpeed;
		fForce[2] = fDragMagnitude * fVwind[2] / fSpeed;

		// increment the force to add in the weight of the particle.
#ifdef INCLUDE_GRAVITY
		fForce[2] += g_fGravitationalAccel * g_pParticles[i].m_fMass;
#endif // INCLUDE_GRAVITY

		// increment the velocity based on acceleration due to applied
		// forces. This is a simple Euler integration, which is easy
		// but certainly NOT the best choice for a robust game physics
		// implementation!
		g_pParticles[i].m_pfVelocity[0] += g_fPhysicsTimeStep * fForce[0] / g_pParticles[i].m_fMass;
		g_pParticles[i].m_pfVelocity[1] += g_fPhysicsTimeStep * fForce[1] / g_pParticles[i].m_fMass;
		g_pParticles[i].m_pfVelocity[2] += g_fPhysicsTimeStep * fForce[2] / g_pParticles[i].m_fMass;


#else // massless particle option
		g_pParticles[i].m_pfVelocity[0] = fVwind[0];
		g_pParticles[i].m_pfVelocity[1] = fVwind[1];
		g_pParticles[i].m_pfVelocity[2] = fVwind[2];
#endif // DO_TEST

		// Final step is to update position based on current velocity.
		// Since we are using the updated velocity, this is not simple
		// Euler integration, since it uses the updated (t + dt) velocity.\
		// But, still conditionally stable.
		g_pParticles[i].m_pfPosition[0] += g_fPhysicsTimeStep * g_pParticles[i].m_pfVelocity[0];
		g_pParticles[i].m_pfPosition[1] += g_fPhysicsTimeStep * g_pParticles[i].m_pfVelocity[1];
		g_pParticles[i].m_pfPosition[2] += g_fPhysicsTimeStep * g_pParticles[i].m_pfVelocity[2];

		// respawn the particle if it falls to the ground (due to gravity)
		if (g_pParticles[i].m_pfPosition[2] <= 0.0f)
		{
			g_pParticles[i].m_pfPosition[0] = -2.5f + 5.0f * float(rand()) / g_fRandMax;
			g_pParticles[i].m_pfPosition[1] = -2.5f + 5.0f * float(rand()) / g_fRandMax;
			g_pParticles[i].m_pfPosition[2] = 500.0f * float(rand()) / g_fRandMax;
		}
	}
}
