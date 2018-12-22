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
    Lens Flare Demo

    Y. King / May 2000
    Electronic Arts Canada, Inc.

 ************************************************************/

#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <string.h>

#include "flare.h"

/************************************************************
    Defines.
 ************************************************************/

// Get height from width, assuming certain aspect ratio.
#define HEIGHTFROMWIDTH(w)  ((320*(w)*SCREENheight)/(240*SCREENwidth))

#define isqrt(x)        (int)((double)(x))

/************************************************************
    Texture management.
 ************************************************************/

typedef struct TEXTURE_DEF
{
    char    *filename;
    int     width;
    int     height;
    void    *pixels;
    unsigned char   *memory;        // file buffer; free this when done with texture
}
    TEXTURE_DEF;

// Texture enumeration -- matches the data that gets loaded.
enum
{
    TEX_CRCL,
    TEX_HXGN,
    TEX_RING,
    TEX_FLAR,

    NPIECETEXTURES,

    TEX_SUN = NPIECETEXTURES,
    TEX_BACK,

    NTEXTURES
};

// Source files for textures.
TEXTURE_DEF gTextures[ NTEXTURES ] =
            {
                { "crcl.raw" },
                { "hxgn.raw" },
                { "ring.raw" },
                { "flar.raw" },
                { "sun.raw"  },
                { "back.raw" }
            };

TEXTURE_DEF *TM_getNthTexture( int n );
TEXTURE_DEF *TM_getNamedTexture( char *name );
void        TM_loadTextures( void );
void        TM_purgeTextures( void );
void        TM_setTexture( TEXTURE_DEF *tex );

/************************************************************
    Internal function prototypes.
 ************************************************************/

void    drawQuad( int x, int y, int width, int height, TEXTURE_DEF *tex, unsigned int colour );

/************************************************************
    Global variables.
 ************************************************************/

int     SCREENwidth = 640;
int     SCREENheight = 480;

/************************************************************

    Public:
        FLARE_randomize

    Description:
        Generate a random lens flare.

 ************************************************************/

void    FLARE_randomize(FLARE_DEF *flare,
                        int nTextures,
                        int nPieces,
                        float fMaxSize,
                        unsigned int minColour,
                        unsigned int maxColour)
{
    int             i;
    float           fFracDist;
    float           fEnvelopeSize;
    FLARE_ELEMENT_DEF *element;

    // Initialize flare with requested number of elements, with
    // random placement, colour, and size of elements.
    flare->nPieces = nPieces;
    flare->fScale = fMaxSize;
    flare->fMaxSize = fMaxSize;
    fFracDist = 1.0f/(float)(flare->nPieces - 1);
    
    for (i = 0; i < flare->nPieces; ++i)
    {
        element = &flare->element[i];
        element->fDistance = (fFracDist*i) + FLARE_FRANGE(0,fFracDist);

        // Envelope size is maximum at ends of line, minimum in the middle (i.e. two
        // cones, touching at the tips).
        fEnvelopeSize = (float)fabs(1.0f - 2*element->fDistance);

        element->fSize = FLARE_FRANGE(0.6f, 1.0f) * fEnvelopeSize;
        element->argb = FLARE_RANGE(minColour & 0xff000000, maxColour & 0xff000000) |
                      FLARE_RANGE(minColour & 0x00ff0000, maxColour & 0x00ff0000) |
                      FLARE_RANGE(minColour & 0x0000ff00, maxColour & 0x0000ff00) |
                      FLARE_RANGE(minColour & 0x000000ff, maxColour & 0x000000ff);
        element->texture = TM_getNthTexture( FLARE_RANGE(0, nTextures - 1) );
    }
}

/************************************************************

    Public:
        FLARE_render

    Description:
        Draw lens flare with specified (lx,ly) light position
        in screen coordinates, for given (cx,cy) position of
        center of screen.

 ************************************************************/

void    FLARE_render(FLARE_DEF *flare, int lx, int ly, int cx, int cy)
{
    int     dx, dy;          // Screen coordinates of "destination"
    int     px, py;          // Screen coordinates of flare element
    int     maxflaredist, flaredist, flaremaxsize, flarescale;
    int     width, height, alpha;    // Piece parameters;
    int     i;
    FLARE_ELEMENT_DEF    *element;

    // Compute how far off-center the flare source is.
    maxflaredist = isqrt(cx*cx + cy*cy);
    flaredist = isqrt((lx - cx)*(lx - cx)+
                      (ly - cy)*(ly - cy));
    flaredist = (maxflaredist - flaredist);
    flaremaxsize = (int)(SCREENwidth * flare->fMaxSize);
    flarescale = (int)(SCREENwidth * flare->fScale);

    // Destination is opposite side of centre from source
    dx = cx + (cx - lx);
    dy = cy + (cy - ly);

    // Render each element.
    for (i = 0; i < flare->nPieces; ++i)
    {
        element = &flare->element[i];

        // Position is interpolated along line between start and destination.
        px = (int)((1.0f - element->fDistance)*lx + element->fDistance*dx);
        py = (int)((1.0f - element->fDistance)*ly + element->fDistance*dy);

        // Piece size are 0 to 1; flare size is proportion of
        // screen width; scale by flaredist/maxflaredist.
        width = (int)((flaredist*flarescale*element->fSize)/maxflaredist);

        // Width gets clamped, to allows the off-axis flares
        // to keep a good size without letting the elements get
        // too big when centered.
        if (width > flaremaxsize)
        {
            width = flaremaxsize;
        }

        // Flare elements are square (round) so height is just
        // width scaled by aspect ratio.
        height = HEIGHTFROMWIDTH(width);
        alpha = (flaredist*(element->argb >> 24))/maxflaredist;

        if ( width > 1 )
        {
            unsigned int    argb = (alpha << 24 ) | (element->argb & 0x00ffffff);

            drawQuad( px - width/2, py - height/2, width, height, element->texture, argb );
        }
    }
}

/*
    drawQuad -- used to draw individual elements of the lens flare
    in 2D. This is the main thing that needs to be changed to render
    with a different engine (e.g. D3D instead of OpenGL).
*/

void    drawQuad( int x, int y, int width, int height, TEXTURE_DEF *tex, unsigned int colour )
{
    TM_setTexture( tex );

    glBegin(GL_QUADS);

    glColor4ub( (GLbyte)(colour >> 16 & 0xff),
                (GLbyte)(colour >>  8 & 0xff),
                (GLbyte)(colour >>  0 & 0xff),
                (GLbyte)(colour >> 24 & 0xff) );

    glTexCoord2f( 0.0f, 0.0f );
    glVertex3f( x, y, 1.0f );
    glTexCoord2f( 1.0f, 0.0f );
    glVertex3f( x + width, y, 1.0f );
    glTexCoord2f( 1.0f, 1.0f );
    glVertex3f( x + width, y + height, 1.0f );
    glTexCoord2f( 0.0f, 1.0f );
    glVertex3f( x, y + height, 1.0f );

    glEnd();
}


// -------------- texture manager ---------------

TEXTURE_DEF *TM_getNthTexture( int n )
{
    if ( ( n < 0 ) || ( n >= NTEXTURES ) )
        return NULL;

    return &gTextures[n];
}

TEXTURE_DEF *TM_getNamedTexture( char *name )
{
    int     i;

    for ( i = 0; i < NTEXTURES; ++i )
    {
        if ( strncmp( name, gTextures[i].filename, strlen( name ) ) == 0 )
            return &gTextures[i];
    }

    return NULL;
}

void    TM_loadTextures( void )
{
    int             i;

    TEXTURE_DEF *tex = &gTextures[0];

    for ( i = 0; i < NTEXTURES; ++i, ++tex )
    {
        FILE    *f = fopen( tex->filename, "rb" );
        int     nBytes;

        tex->height = 0;
        tex->width = 0;
        tex->pixels = NULL;
        tex->memory = NULL;

        if ( f )
        {
            int             j;
            unsigned char   *ppix;

            // Find file length by seeking to end of file.
            fseek( f, 0, SEEK_END );
            nBytes = ftell( f );
            fseek( f, 0, SEEK_SET );

            tex->memory = malloc( nBytes );
            fread( tex->memory, 1, nBytes, f );
            fclose( f );

            // Read little-endian texture sizes in machine-independent way.
            tex->width  = ( tex->memory[1] << 8 ) | ( tex->memory[0] );
            tex->height = ( tex->memory[3] << 8 ) | ( tex->memory[2] );
            tex->pixels = tex->memory + 4;

            // Fix ARGB --> RGBA pixel byte order to suit OpenGL pixel format.
            j = tex->width * tex->height;
            ppix = tex->pixels;
            while ( j-- )
            {
                unsigned char b = ppix[0];
                unsigned char g = ppix[1];
                unsigned char r = ppix[2];
                unsigned char a = ppix[3];

                ppix[0] = r;
                ppix[1] = g;
                ppix[2] = b;
                ppix[3] = a;

                ppix += 4;
            }
        }
    }
}

void        TM_purgeTextures( void )
{
    int         i;
    TEXTURE_DEF *tex = &gTextures[0];

    for ( i = 0; i < NTEXTURES; ++i, ++tex )
    {
        if ( tex->memory )
            free( tex->memory );
    }
}

void        TM_setTexture( TEXTURE_DEF *tex )
{
    if ( tex )
    {
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glEnable( GL_TEXTURE_2D );
        glTexImage2D(   GL_TEXTURE_2D,
                        0,                  // level
                        4,                  // components
                        tex->width,
                        tex->height,
                        0,                  // border
                        GL_RGBA,            // format
                        GL_UNSIGNED_BYTE,   //type
                        tex->pixels );
    }
    else
    {
        glDisable( GL_TEXTURE_2D );
    }
}

// -------------- glut-based demo code ---------------

#define GLUT_NO_BUTTON          -1

enum
{
    MENU_BACKGROUND,
    MENU_SUN,
    MENU_RANDOMFLARE,
    MENU_FILEFLARE
};

#define SUNWIDTH        50
#define SUNHEIGHT       HEIGHTFROMWIDTH(SUNWIDTH)

// Background enumeration -- what to show behind the flare.
enum
{
    BACK_TEXTURED,
    BACK_COLOUR0,
    BACK_COLOUR1,

    NBACK
};

#define FLARE_MINCOLOUR        MAKEID(140, 100, 50, 100)
#define FLARE_MAXCOLOUR        MAKEID(255, 255, 200, 255)
#define FLARE_MINELEMENTSPERFLARE         8
#define FLARE_MAXSIZE                   0.3f

/*
    Global variables
*/

int     xFlare = 10;
int     yFlare = 10;

FLARE_DEF       renderFlare;    // Flare to render.

TEXTURE_DEF *texBack = NULL;
TEXTURE_DEF *texSun = NULL;
int     nBackground = BACK_TEXTURED;
int     bShowSun = 0;

void    loadFlareFile( FLARE_DEF *flare, char *filename )
{
    int     n = 0;
    FILE    *f;
    char    buf[256];

    memset( flare, 0, sizeof( FLARE_DEF ) );

    f = fopen( filename, "r" );
    if ( f )
    {
        fgets( buf, sizeof( buf ), f );
        sscanf( buf, "%f %f", &flare->fScale, &flare->fMaxSize );

        while ( !feof(f) )
        {
            char            name[8] = { '\0', };
            double          dDist = 0.0, dSize = 0.0;
            unsigned int    a = 0, r = 0, g = 0, b = 0;

            fgets( buf, sizeof( buf ), f );
            if ( sscanf( buf, "%s %lf %lf ( %d %d %d %d )",
                    name, &dDist, &dSize, &a, &r, &g, &b ) )
            {
                flare->element[ n ].texture = TM_getNamedTexture( name );
                flare->element[ n ].fDistance = (float)dDist;
                flare->element[ n ].fSize = (float)dSize;
                flare->element[ n ].argb = ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | ( b << 0 );

                ++n;
            }
        }

        flare->nPieces = n;

        fclose( f );
    }
}

void    newFlare( int bFromFile )
{
    if ( bFromFile )
        loadFlareFile( &renderFlare, "flare.txt" );
    else
        FLARE_randomize(&renderFlare, NPIECETEXTURES, FLARE_RANGE(FLARE_MINELEMENTSPERFLARE, FLARE_MAXELEMENTSPERFLARE), FLARE_MAXSIZE, FLARE_MINCOLOUR, FLARE_MAXCOLOUR);
}

// Mouse handling.
int     nButton = GLUT_NO_BUTTON;
int     xMouse = 0, yMouse = 0;

void mousebutton( int button, int state, int x, int y )
{
    nButton = GLUT_NO_BUTTON;
    if ( state == GLUT_DOWN )
    {
        nButton = button;
        xMouse = x;
        yMouse = y;

        xFlare = x*SCREENwidth/glutGet(GLUT_WINDOW_WIDTH);
        yFlare = y*SCREENheight/glutGet(GLUT_WINDOW_HEIGHT);
    }
}

void    mousemove( int x, int y )
{
    if ( nButton == GLUT_LEFT_BUTTON )
    {
        // Scale mouse coordinates to compensate for window size.
        xFlare += (x - xMouse)*SCREENwidth/glutGet(GLUT_WINDOW_WIDTH);
        yFlare += (y - yMouse)*SCREENheight/glutGet(GLUT_WINDOW_HEIGHT);

        // Clamping -- wouldn't be needed in fullscreen mode.
        if ( xFlare >= SCREENwidth )
            xFlare = SCREENwidth - 1;
        if ( xFlare < 0 )
            xFlare = 0;
        if ( yFlare >= SCREENheight )
            yFlare = SCREENheight - 1;
        if ( yFlare < 0 )
            yFlare = 0;

        glutPostRedisplay();
    }

    // Remember last mouse position.
    xMouse = x;
    yMouse = y;
}

void    menufunc( int value )
{
    switch ( value )
    {
        case MENU_BACKGROUND:
            ++nBackground;
            if ( nBackground >= NBACK )
                nBackground = 0;
            break;

        case MENU_SUN:
            bShowSun = !bShowSun;
            break;

        case MENU_RANDOMFLARE:
            newFlare( 0 );
            break;

        case MENU_FILEFLARE:
            newFlare( 1 );
            break;

        default:
            break;
    }

    glutPostRedisplay();
}

void    render(void)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    switch ( nBackground )
    {
        case BACK_TEXTURED:
            drawQuad( 0, 0, SCREENwidth, SCREENheight, texBack, 0xffffffff );
            break;
        case BACK_COLOUR0:
            drawQuad( 0, 0, SCREENwidth, SCREENheight, NULL, 0xff6060ff );
            break;
        case BACK_COLOUR1:
            drawQuad( 0, 0, SCREENwidth, SCREENheight, NULL, 0xff000000 );
            break;
    }
    if ( bShowSun )
        drawQuad( xFlare - SUNWIDTH/2, yFlare - SUNHEIGHT/2, SUNWIDTH, SUNHEIGHT, texSun, 0xffffffe0 );

    FLARE_render(&renderFlare, xFlare, yFlare, SCREENwidth/2, SCREENheight/2);

    glutSwapBuffers();
}

void    initglut(int argc, char *argv[])
{
    int     nMenu;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("flare");

    SCREENwidth = glutGet( GLUT_WINDOW_WIDTH );
    SCREENheight = glutGet( GLUT_WINDOW_HEIGHT );

    glutMouseFunc(mousebutton);
    glutMotionFunc(mousemove);
    glutDisplayFunc(render);

    /* Create menu. */
    nMenu = glutCreateMenu( menufunc );
    glutAddMenuEntry( "cycle background", MENU_BACKGROUND );
    glutAddMenuEntry( "toggle sun", MENU_SUN );
    glutAddMenuEntry( "random flare", MENU_RANDOMFLARE );
    glutAddMenuEntry( "flare from file", MENU_FILEFLARE );
    glutAttachMenu( GLUT_RIGHT_BUTTON );

    /* Set for orthographic projection. */
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, SCREENwidth, SCREENheight, 0);
}

int main(int argc, char *argv[])
{
    initglut( argc, argv );

    TM_loadTextures();
    texSun = TM_getNamedTexture( "sun" );
    if ( texSun )
        bShowSun = 1;
    texBack = TM_getNamedTexture( "back" );
    if ( texBack )
        nBackground = BACK_TEXTURED;
    else
        nBackground = BACK_COLOUR0;

    newFlare( 0 );
    glutMainLoop();

    TM_purgeTextures();

    return 0;
}

#if _WIN32
// WinMain is windows only -- just call main.
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
