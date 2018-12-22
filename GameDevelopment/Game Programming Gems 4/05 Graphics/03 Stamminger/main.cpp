//
// Perspective Shadow Maps
// accompanying example code for Game Programming Gems 4
// (w)(c) 2003, Carsten Dachsbacher (dachsbacher@cs.fau.de), Marc Stamminger (stamminger@cs.fau.de)
//
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <GL/gl.h>          
#include <GL/glu.h>         
#include <GL/glut.h>
#include "oglExtension.h"
#include "p_buffer.h"
#include "vertex3dop.h"

static HDC   hDC;
static HGLRC hRC;

// choose, whatever you want to use
//#define     USE_ARB_PROGRAMS
#define     USE_P_BUFFER

#ifdef USE_ARB_PROGRAMS
#include    "vpfp.h"

GLuint      vpVertexProgramID   = 0;
GLuint      vpFragmentProgramID = 0;
#endif

VERTEX3D    cameraPosition = { 4.0f, 3.0f, 4.0f, 1.0f }, 
            cameraTarget   = { 0.0f, 0.0f, 0.0f, 1.0f };

float       zNear = 3.0f, zFar = 20.0f;

MATRIX44    matrixProj,             // camera projection matrix
            matrixModelView,        // camera modelview matrix
            matrixP,                // matrixProj * matrixModelView (object to clip space)
            matrixC,                // the pps light matrix
            matrixS;                // matrixC * matrixP

VERTEX3D    wsLight,                // world space light direction
            ppsLight;               // post perspective space light position/direction


float       maxYScene = 1.1f;       // the maximum height (y-value) of the scene
const float minZNear  = 4.0f;       // the minimum z-near distance for the light frustum

#ifdef USE_P_BUFFER
const int   SHADOWSIZE = 1024;      // size of the shadow map
#else
const int   SHADOWSIZE = 512;       // maximum size !
#endif

int         windowX, windowY;

#define     EPSILON 0.001f

#ifdef USE_P_BUFFER
CPBuffer    *shadowPBuffer;         // the p-buffer for the shadow map
#else
GLuint      shadowDepthMap;         // the shadow map texture, if we don't use p-buffers
#endif

// prototypes
void        renderScene();

#include    "commonShadowMapCode.h"

const int   nTriangles = 8700;
float       *treeVertexData;
float       *treeNormalData;

void    setupPSMContext()
{
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClearDepth( 1.0f );
    glEnable    ( GL_DEPTH_TEST );
    glDepthFunc ( GL_LESS );
    glDisable   ( GL_CULL_FACE );
    glDisable   ( GL_LIGHTING );
    glDisable   ( GL_BLEND );
    glDisable   ( GL_TEXTURE_2D );
    glDisable   ( GL_POLYGON_SMOOTH );

#ifdef USE_P_BUFFER
    glEnable    ( GL_DEPTH_CLAMP_NV );
#endif

    glPolygonOffset( 2, 2 );
}

void    setupRenderContext()
{
    glClearColor( 0.2f, 0.3f, 0.5f, 0.0f );
    glClearDepth( 1.0f );

    glShadeModel( GL_SMOOTH );
    glDisable   ( GL_POLYGON_SMOOTH );

    glEnable    ( GL_DEPTH_TEST );
    glDepthFunc ( GL_LESS );
    glEnable    ( GL_CULL_FACE );
    glFrontFace ( GL_CCW );

#ifndef USE_ARB_PROGRAMS
    // setup an OpenGL light, if we don't use the ARB programs
    //
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );

    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_ambient[]  = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv( GL_LIGHT0, GL_AMBIENT,  light_ambient );
    glLightfv( GL_LIGHT0, GL_DIFFUSE,  light_diffuse );
    glLightfv( GL_LIGHT0, GL_SPECULAR, light_specular );
    glLightf ( GL_LIGHT0, GL_SPOT_EXPONENT, 20 );

    glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );

    glEnable( GL_NORMALIZE );
#endif

    // setup some material properties
    //
    const float material_ambient[]  = { 0.1f, 0.1f, 0.1f, 1.0f };
    const float material_diffuse[]  = { 0.7f, 0.7f, 0.7f, 1.0f };
    const float material_specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    const float material_shininess  = 40.0f;
    const float material_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT,   material_ambient );
    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,   material_diffuse );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,  material_specular );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, &material_shininess );
    glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION,  material_emission );
}

void    initPSM()
{
    hDC = wglGetCurrentDC();
    hRC = wglGetCurrentContext();

    getOpenGLExtensions();

#ifdef USE_P_BUFFER
    if ( !supportWGL_ARB_pbuffer || 
         !supportWGL_ARB_pixel_format || 
         !supportWGL_ARB_render_texture )
    {
        MessageBox( NULL, "p-buffers are not supported !", "PSM", MB_OK );
        exit( 1 );
    }
#endif

#ifdef USE_ARB_PROGRAMS
    if ( !supportARB_vertex_program || 
         !supportARB_fragment_program )
    {
        MessageBox( NULL, "ARB vertex/fragment programs are not supported !", "PSM", MB_OK );
        exit( 1 );
    }
#endif

#ifdef USE_P_BUFFER
    // create p-buffer
    shadowPBuffer = new CPBuffer( SHADOWSIZE, SHADOWSIZE, hDC );
    
    if ( !shadowPBuffer->exist() ) exit( 1 );

    wglShareLists( hRC, shadowPBuffer->getHRC() );

    // setup of shadow p-buffer context
    //
    shadowPBuffer->makeCurrent();
    setupPSMContext();

    // setup of visible rendering context
    //
    wglMakeCurrent( hDC, hRC );
    setupRenderContext();
#else
    wglMakeCurrent( hDC, hRC );
    createShadowTextureMap();
#endif

#ifdef USE_ARB_PROGRAMS
    // load the vertex and fragment program for the perspective shadow maps
    //
    setupProgram( "./programs/psm.vp", GL_VERTEX_PROGRAM_ARB, &vpVertexProgramID );
    setupProgram( "./programs/psm.fp", GL_FRAGMENT_PROGRAM_ARB, &vpFragmentProgramID );
#endif

    // read vertex and normal data for our tree model
    //
    treeVertexData = new float[ nTriangles * 3 ];
    treeNormalData = new float[ nTriangles * 3 ];

    FILE *f = fopen( "./data/tree.vtx", "rb" );
    
    float maxy = -112312341.0f;
    for ( int i = 0; i < nTriangles; i++ )
    {
        fread( &treeVertexData[ i * 3 + 0 ], sizeof( float ), 3, f );
        fread( &treeNormalData[ i * 3 + 0 ], sizeof( float ), 3, f );
    maxy = max( maxy,  treeVertexData[ i * 3 + 1  ] );
    }

maxy=maxy;
    fclose( f );
}

// Render Callback
//
void    display()
{
    //
    // step #1: create the perspective shadow map
    // 

    // switch to p-buffer or PSM-render context
    //
#ifdef USE_P_BUFFER
    shadowPBuffer->makeCurrent();
#else
    setupPSMContext();
#endif

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    extern int windowX, windowY;
    gluPerspective( 45.0f, (float)windowX / (float)max( 1, windowY ), zNear, zFar );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    gluLookAt( cameraPosition.x, cameraPosition.y, cameraPosition.z,
               0.0f, 0.0f, 0.0f,
               0.0f, 1.0f, 0.0f );

    glGetFloatv( GL_PROJECTION_MATRIX, (float*)&matrixProj );
    glGetFloatv( GL_MODELVIEW_MATRIX,  (float*)&matrixModelView );


    // world space light direction
    wsLight.x = 1.0f;
    wsLight.y = 2.0f;
    wsLight.z = 0.0f;
    wsLight.w = 0.0f;

    ~wsLight;

    // adjust frustum
    //

    // criteria 1: near plane minimum distance
    //
    float d = 0.0f;

    if ( zNear < minZNear )
        d = minZNear - zNear;

    // criteria 2: relevant shadowing objects behind the viewer ?
    //
    VERTEX3D    lookAtDir = { matrixModelView[ 2 ], matrixModelView[ 6 ], matrixModelView[ 10 ], 0.0f };
    float       dot       = wsLight * lookAtDir;

    if ( dot < 0.0f && fabs( wsLight.y ) > EPSILON )
    {
        // light comes from behind of the camera
        float t = -( maxYScene - cameraPosition.y ) / wsLight.y;

        d = max( d, t * sqrtf( wsLight.x * wsLight.x + wsLight.z * wsLight.z ) );
    }

    // adjustment of the frustum required ?
    //
    if ( d > 0.0f )
    {
        glMatrixMode( GL_MODELVIEW );
        glPushMatrix();
        glLoadIdentity();

        // change translation along z-axis
        matrixModelView[ 3 * 4 + 2 ] -= d;

        // adjust near and far plane of the frustum
        extern int windowX, windowY;
        gluPerspective( 45.0f, (float)windowX / (float)max( 1, windowY ), zNear + d, zFar + d );
        glGetFloatv( GL_MODELVIEW_MATRIX, (float*)&matrixProj );

        glPopMatrix();
    }

    // matrix: object to clip space
    multMatrix( matrixP, matrixProj, matrixModelView );

    // light frustum
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();

    // post perspective light
    //
    ppsLight = matrixP * wsLight;

    // case 1: p_w == 0
    //
    if ( fabsf( ppsLight.w ) < EPSILON )
    {
        // the post perspective light is a parallel light source, too
        const float sqrt2 = 1.4142135623730950488016887242097f;

        // this frustum could be even smaller (up-vector)
        glOrtho( -sqrt2, sqrt2, -sqrt2, sqrt2, -sqrt2, +sqrt2 );

        gluLookAt(  0.0f, 0.0f, 0.0f,
                    -ppsLight.x,                                    
                    -ppsLight.y,                                    
                    -ppsLight.z,                                    
                    0.0f, 0.0f, 1.0f );
    } else
    {
        // the post perspective light is a point light source

        // black hole ?
        if ( ppsLight.w < 0.0f )
            glScalef( 1.0f, 1.0f, -1.0f );

        ppsLight   *= 1.0f / ppsLight.w;
        ppsLight.w  = 0.0f;

        // adjust light frustum that it contains the bounding sphere of the clipspace [-1;1]^3
        float   radius      = 1.7320508075688772935274463415059f;
        float   dist        = sqrtf( ppsLight * ppsLight );
        float   fov         = 2.0f * atanf( radius / dist ) * 180.0f / 3.141592f;
        float   shadowZNear = max( dist - radius, 0.001f );
        float   shadowZFar  = dist + radius;

        gluPerspective( fov, 1.0f, shadowZNear, shadowZFar );

        gluLookAt( ppsLight.x, 
                   ppsLight.y, 
                   ppsLight.z,                                  
                   0.0f, 0.0f, 0.0f, 
                   0.0f, 0.0f, 1.0f );                              
    }


    glGetFloatv( GL_MODELVIEW_MATRIX, (float*)&matrixC );
    glPopMatrix();

    // 
    // the final shadow map matrix
    //
    multMatrix( matrixS, matrixC, matrixP );


    // movement of lightsource
    float t = GetTickCount()*0.001f;

    // setup viewport and matrices and render the PSM
    renderShadowMap();

    //
    // step #2: render with the shadow map
    //
#ifdef USE_P_BUFFER
    wglMakeCurrent( hDC, hRC );
#else
    setupRenderContext();
#endif

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    extern int windowX, windowY;
    gluPerspective( 45.0f, (float)windowX / (float)max( 1, windowY ), zNear, zFar );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    gluLookAt( cameraPosition.x, cameraPosition.y, cameraPosition.z,
               0.0f, 0.0f, 0.0f,
               0.0f, 1.0f, 0.0f );

    // setup texture coordinate generation
    //
    setupShadowTexCoordGeneration();

#ifdef USE_P_BUFFER
    glBindTexture( GL_TEXTURE_2D, shadowPBuffer->getTexID() );
    shadowPBuffer->bind( WGL_DEPTH_COMPONENT_NV );
#else
    glBindTexture( GL_TEXTURE_2D, shadowDepthMap );
#endif

    //
    // build matrices for the vertex program
    //
#ifdef USE_ARB_PROGRAMS

    MATRIX44 matTexture,        // texture matrix, containing bias/scale and matrixS
             matModelView,      // current modelview matrix for texgen
             matInvMV,          // inverse of the upper matrix
             matTexInvMV;       // matTexInvMV = matTexture * matInvMV

    glGetFloatv( GL_TEXTURE_MATRIX,   (float*)&matTexture );
    glGetFloatv( GL_MODELVIEW_MATRIX, (float*)&matModelView );

    InverseMatrixAnglePreserving( matInvMV, matModelView );

    multMatrix( matTexInvMV, matTexture, matInvMV );

    glBindProgramARB( GL_VERTEX_PROGRAM_ARB, vpVertexProgramID );
    glBindProgramARB( GL_FRAGMENT_PROGRAM_ARB, vpFragmentProgramID );

    // texture matrix * inverse modelview for eye-linear texgen mode
    glProgramEnvParameter4fARB( GL_VERTEX_PROGRAM_ARB, 16, matTexInvMV[  0 ], matTexInvMV[  4 ], matTexInvMV[  8 ], matTexInvMV[ 12 ] );
    glProgramEnvParameter4fARB( GL_VERTEX_PROGRAM_ARB, 17, matTexInvMV[  1 ], matTexInvMV[  5 ], matTexInvMV[  9 ], matTexInvMV[ 13 ] );
    glProgramEnvParameter4fARB( GL_VERTEX_PROGRAM_ARB, 18, matTexInvMV[  2 ], matTexInvMV[  6 ], matTexInvMV[ 10 ], matTexInvMV[ 14 ] );
    glProgramEnvParameter4fARB( GL_VERTEX_PROGRAM_ARB, 19, matTexInvMV[  3 ], matTexInvMV[  7 ], matTexInvMV[ 11 ], matTexInvMV[ 15 ] );

    // lighting parameters

    // light direction
    glProgramEnvParameter4fARB( GL_VERTEX_PROGRAM_ARB, 0, wsLight.x, wsLight.y, wsLight.z, 0.0f );

    // camera position
    glProgramEnvParameter4fARB( GL_VERTEX_PROGRAM_ARB, 1, cameraPosition.x, cameraPosition.y, cameraPosition.z, 0.0f );

    glEnable( GL_VERTEX_PROGRAM_ARB );
    glEnable( GL_FRAGMENT_PROGRAM_ARB );

    renderScene();

    glDisable( GL_VERTEX_PROGRAM_ARB );
    glDisable( GL_FRAGMENT_PROGRAM_ARB );

#else
    glLightfv( GL_LIGHT0, GL_POSITION, (float*)&wsLight );

    renderScene();
#endif

    // disable texture coordinate generation
    //
    clearShadowTexCoordGeneration();

    //
    // display the post-perspective space as seen by the light source
    //
#ifdef USE_P_BUFFER
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glColor4ub( 255, 255, 255, 255 );

    glDisable( GL_LIGHTING );
    glDepthFunc( GL_ALWAYS );

    glBindTexture( GL_TEXTURE_2D, shadowPBuffer->getTexID() );
    shadowPBuffer->bind( WGL_FRONT_LEFT_ARB );
    
    const float qsize = 0.5f;

    glBegin( GL_TRIANGLE_STRIP );
        
        glTexCoord2i( 0,  1 );
        glVertex2f  ( -1.0f, -1.0f + qsize );

        glTexCoord2i( 0,  0 );
        glVertex2f  ( -1.0f, -1.0f );

        glTexCoord2i( 1,  1 );
        glVertex2f  ( -1.0f + qsize, -1.0f + qsize );

        glTexCoord2i( 1,  0 );
        glVertex2f  ( -1.0f + qsize, -1.0f );

    glEnd();

    shadowPBuffer->release();

    glEnable( GL_LIGHTING );
    glDepthFunc( GL_LEQUAL );
#endif

    glutSwapBuffers();
}

void    quitPSM()
{
#ifdef USE_P_BUFFER
    delete shadowPBuffer;
#endif

    wglMakeCurrent( hDC, hRC );
}

// renders our small test scene: a floor and some trees
//
void renderScene()
{
    glPushMatrix();

    glRotatef( (float)GetTickCount() * 0.005f, 0, 1, 0 );

    glVertexPointer( 3, GL_FLOAT, 0, treeVertexData );
    glNormalPointer( GL_FLOAT, 0, treeNormalData );
    
    glEnable( GL_VERTEX_ARRAY );
    glEnable( GL_NORMAL_ARRAY );

    glDisable( GL_CULL_FACE );      

    const float diffuseBrown[] = { 0.7f, 0.6f, 0.3f, 1.0f };
    const float diffuseGreen[] = { 0.4f, 0.9f, 0.4f, 1.0f };
    const float diffuseFloor[] = { 0.4f, 0.4f, 0.3f, 1.0f };

    for ( int j = 0; j < 5; j++ )
        for ( int i = 0; i < 5; i++ )
        {
            glPushMatrix();
            glTranslatef( 3*i-6.0f, 0, 3*j-6.0f );

            // branches
            glColor3ub( 180, 150, 60 );
            glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseBrown );

            glDrawArrays( GL_TRIANGLES, 0, 7500 );
            
            // leaves
            glColor3ub( 100, 230, 100 );
            glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseGreen );
            
            glDrawArrays( GL_TRIANGLES, 7500, nTriangles-7500 );

            glPopMatrix();
        }
        
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_NORMAL_ARRAY );

    // floor
    glColor3ub( 100, 100, 75 );
    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseFloor );
    
    glBegin( GL_QUADS );
    const float height = -0.5f;
    glNormal3f( 0.0f, 1.0f, 0.0f );
    glVertex3f( -10.0f, height, -10.0f );
    glVertex3f( -10.0f, height,  10.0f );
    glVertex3f(  10.0f, height,  10.0f );
    glVertex3f(  10.0f, height, -10.0f );
    glEnd();

    glPopMatrix();
}


void reshape( int w, int h )
{
    glViewport( 0, 0, w, h );

    windowX = w;
    windowY = max( 1, h );
}

void idleFunc()
{
    glutPostRedisplay(); 
}


int main( int argc, char **argv )
{
    glutInit( &argc, argv );

    glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE );
    glutInitWindowSize( 512, 512 );
    glutCreateWindow( "perspective shadow maps" );

    initPSM();

    glutIdleFunc   ( idleFunc );
    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutMainLoop();

    quitPSM();

    return 0;
}
