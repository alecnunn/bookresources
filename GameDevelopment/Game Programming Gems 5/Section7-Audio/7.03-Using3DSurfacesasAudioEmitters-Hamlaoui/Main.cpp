#include "Main.h"

// Space to store the wave files.

FSOUND_SAMPLE *g_SamplePoint        = NULL;
FSOUND_SAMPLE *g_SampleSphere       = NULL;
FSOUND_SAMPLE *g_SampleBox          = NULL;
FSOUND_SAMPLE *g_SampleLineSegment  = NULL;
FSOUND_SAMPLE *g_SampleLineInfinite = NULL;

// The number of emitters to store.

#define EMITTER_MAX 5

// An array of emitters.

sEmitter g_Emitters[EMITTER_MAX];

// Camera variables.

sVector g_CameraPos   = { 0, 0, 10 };
sVector g_CameraDir   = { 0, 0, -1 };
sVector g_CameraUp    = { 0, 1,  0 };
sVector g_CameraRight = { 1, 0,  0 };
sVector g_CameraAngle = { 0, 0,  0 };

// Mouse Movement

int oldx, oldy;

// Main update function.

void Update ()
{
    // Update the position of the camera.

    FSOUND_3D_Listener_SetAttributes (&g_CameraPos.x,
                                      NULL,
                                      -g_CameraDir.x, -g_CameraDir.y, -g_CameraDir.z,
                                      g_CameraUp.x,  g_CameraUp.y,  g_CameraUp.z);

    // Cycle through each emitter and update them in turn.

    for (int i = 0; i < EMITTER_MAX; ++i)
        Emitter_Update (g_Emitters[i], g_CameraPos);

    // Make sure we update FMOD too, otherwise nothing will happen!

    FSOUND_Update ();
}

// The render function.

void Render ()
{
    // Update the world before we draw it.

    Update ();

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity ();

    glRotatef (g_CameraAngle.x, 1, 0, 0);
    glRotatef (g_CameraAngle.y, 0, 1, 0);
    glRotatef (g_CameraAngle.z, 0, 0, 1);

    glTranslatef (-g_CameraPos.x, -g_CameraPos.y, -g_CameraPos.z);

    // Draw each emitter.
    for (int i = 0; i < EMITTER_MAX; ++i)
        Emitter_Render (g_Emitters[i]);

    glutSwapBuffers ();
    glutPostRedisplay ();
}

bool Initialise ()
{
    // Initialise FMOD.
    if (!FSOUND_Init (44100, 32, 0)) return false;

    memset (&g_Emitters, 0, sizeof (g_Emitters));

    glClearColor (0.4, 0.4, 0.7, 1.0);
    glEnable (GL_DEPTH_TEST);
    glLineWidth (2);
    glCullFace (GL_NONE);
    glEnable (GL_CULL_FACE);

    // Load the samples (one for each emitter).

    g_SamplePoint        = FSOUND_Sample_Load (FSOUND_FREE, "Data/point.wav",        FSOUND_NORMAL | FSOUND_LOOP_NORMAL, 0, 0);
    g_SampleSphere       = FSOUND_Sample_Load (FSOUND_FREE, "Data/sphere.wav",       FSOUND_NORMAL | FSOUND_LOOP_NORMAL, 0, 0);
    g_SampleBox          = FSOUND_Sample_Load (FSOUND_FREE, "Data/box.wav",          FSOUND_NORMAL | FSOUND_LOOP_NORMAL, 0, 0);
    g_SampleLineSegment  = FSOUND_Sample_Load (FSOUND_FREE, "Data/linesegment.wav",  FSOUND_NORMAL | FSOUND_LOOP_NORMAL, 0, 0);
    g_SampleLineInfinite = FSOUND_Sample_Load (FSOUND_FREE, "Data/lineinfinite.wav", FSOUND_NORMAL | FSOUND_LOOP_NORMAL, 0, 0);

    // Create 5 emitters.

    Emitter_SetSphere       (g_Emitters[0], -30,  0,   -5,  2);
    Emitter_SetPoint        (g_Emitters[1], -10,  0,   -5);
    Emitter_SetLineSegment  (g_Emitters[2],  10, -1,  -10,  0,  2,  15, 10);
    Emitter_SetBox          (g_Emitters[3],  30,  0,   -5, -2, -2, -2, 2, 2, 2);
    Emitter_SetLineInfinite (g_Emitters[4],   0,  10,  -5,  1,  0,  0);

    // Start them playing.

    g_Emitters[0].AudioChannel = FSOUND_PlaySound (FSOUND_FREE, g_SampleSphere);
    g_Emitters[1].AudioChannel = FSOUND_PlaySound (FSOUND_FREE, g_SamplePoint);
    g_Emitters[2].AudioChannel = FSOUND_PlaySound (FSOUND_FREE, g_SampleLineSegment);
    g_Emitters[3].AudioChannel = FSOUND_PlaySound (FSOUND_FREE, g_SampleBox);
    g_Emitters[4].AudioChannel = FSOUND_PlaySound (FSOUND_FREE, g_SampleLineInfinite);

    return true;
}

void Shutdown ()
{
    // Don't forget to do this!
    FSOUND_Close ();
}

//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------

void Keyboard (unsigned char Key, int x, int y)
{
    float Forward = 0;
    float Right   = 0;
    float Up      = 0;

    switch (Key)
    {
        case 'W': case 'w':
            Forward += 0.3;
            break;

        case 'S': case 's':
            Forward -= 0.3;
            break;

        case 'A': case 'a':
            Right -= 0.3;
            break;

        case 'D': case 'd':
            Right += 0.3;
            break;

        case ' ':
            Up += 0.3;
            break;

        case 'C': case 'c':
            Up -= 0.3;
            break;
    }

    g_CameraPos.x += g_CameraDir.x * Forward;
    g_CameraPos.y += g_CameraDir.y * Forward;
    g_CameraPos.z += g_CameraDir.z * Forward;

    g_CameraPos.x += g_CameraUp.x * Up;
    g_CameraPos.y += g_CameraUp.y * Up;
    g_CameraPos.z += g_CameraUp.z * Up;

    g_CameraPos.x += g_CameraRight.x * Right;
    g_CameraPos.y += g_CameraRight.y * Right;
    g_CameraPos.z += g_CameraRight.z * Right;

    glutPostRedisplay ();
}

// This will rotate the view Angle degrees along the axis xyz.
void RotateView (float Angle, float x, float y, float z)
{
    sVector NewDir;

    float CosAngle = cosf (radf (Angle));
    float SinAngle = sinf (radf (Angle));

    NewDir.x  = (CosAngle + (1 - CosAngle) * x * x)        * g_CameraDir.x;
    NewDir.x += ((1 - CosAngle) * x * y - z * SinAngle)    * g_CameraDir.y;
    NewDir.x += ((1 - CosAngle) * x * z + y * SinAngle)    * g_CameraDir.z;

    NewDir.y  = ((1 - CosAngle) * x * y + z * SinAngle)    * g_CameraDir.x;
    NewDir.y += (CosAngle + (1 - CosAngle) * y * y)        * g_CameraDir.y;
    NewDir.y += ((1 - CosAngle) * y * z - x * SinAngle)    * g_CameraDir.z;

    NewDir.z  = ((1 - CosAngle) * x * z - y * SinAngle)    * g_CameraDir.x;
    NewDir.z += ((1 - CosAngle) * y * z + x * SinAngle)    * g_CameraDir.y;
    NewDir.z += (CosAngle + (1 - CosAngle) * z * z)        * g_CameraDir.z;

    NewDir.Normalize ();

    g_CameraDir.Set (NewDir);
}


void Motion (int x, int y)
{
    float AngleX, AngleY;

    AngleY = (oldx - x) / 3.0f;
    AngleX = (oldy - y) / 3.0f;

    g_CameraAngle.x -= AngleX;
    g_CameraAngle.y -= AngleY;

    sVector Cross;

    Cross.CrossProduct (g_CameraDir, g_CameraUp);
    Cross.Normalize ();

    RotateView (AngleX, Cross.x, Cross.y, Cross.z);
    RotateView (AngleY, g_CameraUp.x, g_CameraUp.y, g_CameraUp.z);

    g_CameraRight.CrossProduct (g_CameraDir, g_CameraUp);
    g_CameraRight.Normalize ();

    oldx = x;
    oldy = y;

    glutPostRedisplay ();
}

void Mouse (int button, int state, int x, int y)
{
    oldx = x;
    oldy = y;
}

void Reshape (int Width, int Height)
{
    glViewport (0, 0, Width, Height);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective (50.0f, (float) Width / (float) Height, 0.05, 1000);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();

    glutPostRedisplay ();
}

int main (int argc, char *argv[])
{
    glutInit (&argc, argv);

    glutInitDisplayMode (GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE);
    glutCreateWindow ("GPG5 - \"Using 3D Surfaces as Audio Emitters\" by Sami Hamlaoui");
    glutDisplayFunc (Render);
    glutInitWindowPosition (0, 0);
    glutInitWindowSize (500, 500);
    glutKeyboardFunc (Keyboard);
    glutMotionFunc (Motion);
    glutReshapeFunc (Reshape);
    glutMouseFunc (Mouse);

    if (!Initialise ())
        return 0;

// For some reason glut won't call anything past glutMainLoop. If you can
// make it do so then change #if 1 to #if 0.
#if 1
    atexit (Shutdown);
    glutSwapBuffers ();
    glutMainLoop ();
#else
    glutSwapBuffers ();
    glutMainLoop ();
    Shutdown ();
#endif

    return 0;
}