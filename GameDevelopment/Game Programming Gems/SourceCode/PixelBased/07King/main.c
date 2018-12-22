/* Copyright (C) Yossarian King, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Yossarian King, 2000"
 */
/************************************************************

    Game Programming Gems
    Ground Plane Shadows Demo

    Y. King / May 2000
    Electronic Arts Canada, Inc.

 ************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <GL/glut.h>

// Uncomment one or more of these.
#define DOYZPLANE      1
#define DOXZPLANE      1
#define DOXYPLANE      1

#define LIGHT_DISTANCE          1000
#define GLUT_NO_BUTTON          -1
#define PI                      3.14159265
#define DEG2RAD(D)              ((D) * 2.0 * PI / 360.0)

enum
{
    MENU_CAMERA,
    MENU_LIGHT,
    MENU_ZOOM,
    MENU_XZPLANE,
    MENU_YZPLANE,
    MENU_XYPLANE,
    MENU_DEPTHCOMPARE,
    MENU_BACKFACECULL,
    MENU_TEAPOT
};

GLfloat light_diffuse[4] = {1.0, 0.0, 0.0, 1.0};  /* Red diffuse light. */
GLfloat light_position[4] = {1.0, 1.0, 1.0, 0.0};  /* Infinite light location. */

// Data for cube rendering.
GLfloat nBox[6][3] = {  /* Normals for the 6 faces of a cube. */
  {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0},
  {0.0, -1.0, 0.0}, {0.0, 0.0, -1.0}, {0.0, 0.0, 1.0} };
GLint faces[6][4] = {  /* Vertex indices for the 6 faces of a cube. */
  {0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
  {4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} };
GLfloat vBox[8][3];  /* Will be filled in with X,Y,Z vertexes. */

// Ground plane (gets rotated for other planes).
GLfloat vPlane[4][3] =
        {
            {   -20.0, 0.0, -20.0   },
            {    20.0, 0.0, -20.0   },
            {    20.0, 0.0,  20.0   },
            {   -20.0, 0.0,  20.0   }
        };

// Pitch and heading of light position.
float   fLightHeading = 0.0f;
float   fLightPitch = 50.0f;

// Camera parameters.
float   fHeading = 75.0f;
float   fPitch = 30.0f;
float   fZoom = 40.0f;

// Offset of shadow from plane.
float   fPlaneOffset[3] = { -10.0f, -2.95f, -10.0f };

// Mouse handling.
int     nMouseMode = MENU_LIGHT;
int     nButton = GLUT_NO_BUTTON;
int     xMouse = 0, yMouse = 0;

// Rendering options.
int bDepthCompare = 1;
int bBackFaceCull = 0;
int bTeapot = 1;

/***********************************************************************
    calcShadowMatrix

    Here's the guts of the demo -- this routine calculates a projection
    matrix that projects points onto a plane suitable for shadow
    rendering, given the object and light position. Routine is
    generalized to allow projection onto vertical axis-aligned planes
    as well as the ground plane. Could be generalized further to
    project onto an arbitrary plane, but the math gets a bit trickier.

    Note: Real-Time Rendering by Moller and Haines shows how to do this,
    but unfortunately presumes that fourth column of matrix is used and
    that homogenous transformed coordinate is renormalized -- i.e. that
    there's a divide. Which OpenGL doesn't do.
 ***********************************************************************/
float   *calcShadowMatrix( float light_pos[3], float object_pos[3], int iPlane )
{
    static float    matrix[4*4];
    static float    identity[4*4] =
                    {
                        1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f
                    };

    float   dx, dy, dz;

    dx = light_pos[0] - object_pos[0];
    dy = light_pos[1] - object_pos[1];
    dz = light_pos[2] - object_pos[2];

    memcpy( matrix, identity, sizeof( matrix ) );

    switch ( iPlane )
    {
        case MENU_XZPLANE:
            matrix[1*4 + 0] = -dx/dy;
            matrix[1*4 + 1] = 0.0f;
            matrix[1*4 + 2] = -dz/dy;

            matrix[3*4 + 0] = fPlaneOffset[1]*dx/dy;
            matrix[3*4 + 1] = fPlaneOffset[1] + 0.1f;  // additional bias for z-buffer benefit
            matrix[3*4 + 2] = fPlaneOffset[1]*dz/dy;
        break;
        case MENU_YZPLANE:
            matrix[0*4 + 0] = 0.0f;
            matrix[0*4 + 1] = -dy/dx;
            matrix[0*4 + 2] = -dz/dx;

            matrix[3*4 + 0] = fPlaneOffset[0] + 0.1f;  // additional bias for z-buffer benefit
            matrix[3*4 + 1] = fPlaneOffset[0]*dy/dx;
            matrix[3*4 + 2] = fPlaneOffset[0]*dz/dx;
        break;
        case MENU_XYPLANE:
            matrix[2*4 + 0] = -dx/dz;
            matrix[2*4 + 1] = -dy/dz;
            matrix[2*4 + 2] = 0.0f;

            matrix[3*4 + 0] = fPlaneOffset[2]*dx/dz;
            matrix[3*4 + 1] = fPlaneOffset[2]*dy/dz;
            matrix[3*4 + 2] = fPlaneOffset[2] + 0.1f;  // additional bias for z-buffer benefit
        break;
    }

    return matrix;
}

/*
    Set light position based on light pitch and heading.
*/
void    updateLightPosition( void )
{
    light_position[0] = LIGHT_DISTANCE * cos( DEG2RAD( fLightHeading ) ) * cos( DEG2RAD( fLightPitch ) );
    light_position[1] = LIGHT_DISTANCE * sin( DEG2RAD( fLightPitch ) );
    light_position[2] = LIGHT_DISTANCE * sin( DEG2RAD( fLightHeading ) ) * cos( DEG2RAD( fLightPitch ) );
}


/*
    Draw polys for a plane.
 */
void drawPlanePolys(void)
{
    glBegin(GL_QUADS);

    glColor3ub(255, 0, 0);
    glVertex3fv(&vPlane[0][0]);

    glColor3ub(0, 255, 0);
    glVertex3fv(&vPlane[1][0]);

    glColor3ub(0, 0, 255);
    glVertex3fv(&vPlane[2][0]);

    glColor3ub(255, 255, 255);
    glVertex3fv(&vPlane[3][0]);

    glEnd();
}

/*
    draw planes
*/

void drawPlane(void)
{
    glDisable(GL_LIGHTING);
    glDisable( GL_CULL_FACE );

    #if DOYZPLANE
    glPushMatrix();
    glRotatef( -90.0f, 0.0f, 0.0f, 1.0f );
    glTranslatef( 0.0f, fPlaneOffset[0], 0.0f );
    drawPlanePolys();
    glPopMatrix();
    #endif

    #if DOXZPLANE
    glPushMatrix();
    glTranslatef( 0.0f, fPlaneOffset[1], 0.0f );
    drawPlanePolys();
    glPopMatrix();
    #endif

    #if DOXYPLANE
    glPushMatrix();
    glRotatef( 90.0f, 1.0f, 0.0f, 0.0f );
    glTranslatef( 0.0f, fPlaneOffset[2], 0.0f );
    drawPlanePolys();
    glPopMatrix();
    #endif

    glEnable( GL_CULL_FACE );
}

/*
    draw an object -- teapot or cube.
*/

void    drawObjectPolys( void )
{
    if ( bTeapot )
    {
        if ( !bBackFaceCull )
            glDisable( GL_CULL_FACE );
        glutSolidTeapot( 4.0f );
        glEnable( GL_CULL_FACE );
    }
    else
    {
        int     i;

        for (i = 0; i < 6; i++)
        {
            glBegin(GL_QUADS);

            glNormal3fv(&nBox[i][0]);
            glVertex3fv(&vBox[faces[i][0]][0]);
            glVertex3fv(&vBox[faces[i][1]][0]);
            glVertex3fv(&vBox[faces[i][2]][0]);
            glVertex3fv(&vBox[faces[i][3]][0]);

            glEnd();
        }
    }
}

void drawObject( void )
{
    /* Enable a single light. Note that the light position must be set
       each frame (or at least each time the modelview matrix changes). */
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    drawObjectPolys();
}

/*
    draw a shadow -- use shadow projection matrix
    and re-render the object.
*/

void drawObjectShadow( void )
{
    float   object_pos[3] = { 0.0f, 0.0f, 0.0f };
    float   *pfMatrix;

    // Draw the (projected) box.
    if ( !bDepthCompare )
        glDisable(GL_DEPTH_TEST);
    glDepthFunc( GL_LESS );
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4ub(0, 0, 0, 100);       // translucent black

    #if DOYZPLANE
    if ( light_position[0] > fPlaneOffset[0] )
    {
        glPushMatrix();
        pfMatrix = calcShadowMatrix( light_position, object_pos, MENU_YZPLANE );
        glMultMatrixf( pfMatrix );
        drawObjectPolys();
        glPopMatrix();
    }
    #endif

    #if DOXZPLANE
    if ( light_position[1] > fPlaneOffset[1] )
    {
        glPushMatrix();
        pfMatrix = calcShadowMatrix( light_position, object_pos, MENU_XZPLANE );
        glMultMatrixf( pfMatrix );
        drawObjectPolys();
        glPopMatrix();
    }
    #endif

    #if DOXYPLANE
    if ( light_position[2] > fPlaneOffset[2] )
    {
        glPushMatrix();
        pfMatrix = calcShadowMatrix( light_position, object_pos, MENU_XYPLANE );
        glMultMatrixf( pfMatrix );
        drawObjectPolys();
        glPopMatrix();
    }
    #endif

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void mousebutton( int button, int state, int x, int y )
{
    nButton = GLUT_NO_BUTTON;
    if ( state == GLUT_DOWN )
    {
        nButton = button;
        xMouse = x;
        yMouse = y;
    }
}

void    mousemove( int x, int y )
{
    if ( nButton == GLUT_LEFT_BUTTON )
    {
        switch ( nMouseMode )
        {
            case MENU_CAMERA:
            {
                fHeading += (float)(x - xMouse) * 0.5f;
                fPitch += (float)(y - yMouse) * 0.5f;
                glutPostRedisplay();
            }
            break;

            case MENU_ZOOM:
            {
                fZoom += (float)(y - yMouse) * 0.5f;
                glutPostRedisplay();
            }
            break;

            case MENU_LIGHT:
            {
                fLightHeading += (float)(x - xMouse) * 0.5f;
                fLightPitch -= (float)(y - yMouse) * 0.5f;
                updateLightPosition();
                glutPostRedisplay();
            }
            break;

            case MENU_YZPLANE:
            {
                fPlaneOffset[0] -= (float)(y - yMouse) * 0.5f;
                glutPostRedisplay();
            }
            break;

            case MENU_XZPLANE:
            {
                fPlaneOffset[1] -= (float)(y - yMouse) * 0.5f;
                glutPostRedisplay();
            }
            break;

            case MENU_XYPLANE:
            {
                fPlaneOffset[2] -= (float)(y - yMouse) * 0.5f;
                glutPostRedisplay();
            }
            break;
        }
    }

    // Remember last mouse position.
    xMouse = x;
    yMouse = y;
}

void    render(void)
{
    float   xEye, yEye, zEye;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Camera transformation.
    xEye = fZoom * cos( DEG2RAD(fHeading) ) * cos( DEG2RAD( fPitch ) );
    yEye = fZoom * sin( DEG2RAD( fPitch ) );
    zEye = fZoom * sin( DEG2RAD(fHeading) ) * cos( DEG2RAD( fPitch ) );
    gluLookAt(  xEye, yEye, zEye,
                0.0, 0.0, 0.0,      /* center is at (0,0,0) */
                0.0, 1.0, 0.);      /* up is in positive Y direction */

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawPlane();
    drawObjectShadow();
    drawObject();

    glutSwapBuffers();
}

void    menufunc( int value )
{
    switch ( value )
    {
        case MENU_DEPTHCOMPARE: bDepthCompare = !bDepthCompare;     break;
        case MENU_BACKFACECULL: bBackFaceCull = !bBackFaceCull;     break;
        case MENU_TEAPOT:       bTeapot = !bTeapot;                 break;

        default:
            nMouseMode = value;
            break;
    }

    glutPostRedisplay();
}

void init(void)
{
    int nMenu;

    /* Create menu. */
    nMenu = glutCreateMenu( menufunc );
    glutAddMenuEntry( "camera", MENU_CAMERA );
    glutAddMenuEntry( "light", MENU_LIGHT );
    glutAddMenuEntry( "zoom", MENU_ZOOM );
    #if DOYZPLANE
    glutAddMenuEntry( "x offset", MENU_YZPLANE );
    #endif
    #if DOXZPLANE
    glutAddMenuEntry( "y offset", MENU_XZPLANE );
    #endif
    #if DOXYPLANE
    glutAddMenuEntry( "z offset", MENU_XYPLANE );
    #endif
    glutAddMenuEntry( "toggle depth compare", MENU_DEPTHCOMPARE );
    glutAddMenuEntry( "toggle back face cull", MENU_BACKFACECULL );
    glutAddMenuEntry( "toggle teapot", MENU_TEAPOT );
    glutAttachMenu( GLUT_RIGHT_BUTTON );

    /* Setup cube vertex data. */
    vBox[0][0] = vBox[1][0] = vBox[2][0] = vBox[3][0] = 0;
    vBox[4][0] = vBox[5][0] = vBox[6][0] = vBox[7][0] = 4;
    vBox[0][1] = vBox[1][1] = vBox[4][1] = vBox[5][1] = 0;
    vBox[2][1] = vBox[3][1] = vBox[6][1] = vBox[7][1] = 4;
    vBox[0][2] = vBox[3][2] = vBox[4][2] = vBox[7][2] = 4;
    vBox[1][2] = vBox[2][2] = vBox[5][2] = vBox[6][2] = 0;

    /* Use depth buffering for hidden surface elimination. */
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glFrontFace( GL_CW );

    /* Setup the view of the cube. */
    glMatrixMode(GL_PROJECTION);
    gluPerspective( 40.0,       /* field of view in degree */
                    1.0,        /* aspect ratio */
                    1.0,        /* Z near */
                    400.0);     /* Z far */

    updateLightPosition();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("testing");

    glutMouseFunc(mousebutton);
    glutMotionFunc(mousemove);
    glutDisplayFunc(render);

    init();
    glutMainLoop();

    return 0;             /* ANSI C requires main to return int. */
}

#if _WIN32
// WinMain is windows only -- just calls main.
#include <windows.h>

int WINAPI WinMain(  HINSTANCE hInstance,   // handle to current instance
  HINSTANCE hPrevInstance,                  // handle to previous instance
  LPSTR lpCmdLine,                          // pointer to command line
  int nCmdShow                              // show state of window
  )
{
    return main( __argc, __argv );
}
#endif
