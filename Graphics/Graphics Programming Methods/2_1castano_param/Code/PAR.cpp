/*===========================================================================
	Title: PAR.cpp
	Module: GPM/PAR
	Author: Ignacio Castaño
	Description: OpenGL mesh viewer.
	Changes:
		11/01/2003 - Ignacio Castaño
			File added.

===========================================================================*/

/*---------------------------------------------------------------------------
	Doc:
---------------------------------------------------------------------------*/

/** @file PAR.cpp
 * Main header file.
**/


/*---------------------------------------------------------------------------
	Includes:
---------------------------------------------------------------------------*/

#include "PAR.h"

// MeshLib
#include "ImportMesh.h"
#include "ImportPly.h"
#include "TriMesh.h"

// Raster
#include "Raster.h"

#include "RenderMan.h"


#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif



/*----------------------------------------------------------------------------
	Definitions
----------------------------------------------------------------------------*/

#define GPM_PM_POINTS		1
#define GPM_PM_LINES		2
#define GPM_PM_TRIANGLES	3

#define GPM_LM_NONE			4
#define GPM_LM_FLAT			5
#define GPM_LM_GOURAUD		6

#define GPM_TX_NONE			7
#define GPM_TX_DEFAULT		8
#define GPM_TX_LIGHTMAP		9
#define GPM_TX_WOOD			10
#define GPM_TX_MARBLE		11
#define GPM_TX_NORMAL		12

#define GPM_QUIT			13


#define GL_TEXTURE_CUBE_MAP               0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP       0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X    0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y    0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y    0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z    0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z    0x851A

#define GL_CLAMP_TO_EDGE                  0x812F



/*---------------------------------------------------------------------------
	Globals:
---------------------------------------------------------------------------*/

extern Vec3	Vec3Origin( 0, 0, 0 );

int GWidth, GHeight;

PiTriMesh * venus;
int dlist = 0;
int dlist_smooth, dlist_flat, dlist_sphere;
uint cube=0;

float LightDiffuse[] = { 1, 1, 1, 0 };
float LightPosition[] = { 1, -1, -1, 0 };

int mouse_start_x;
int mouse_start_y;
int mouse_button = 0;

float obj_pos_x = 0;
float obj_pos_y = 0;
float obj_pos_z = 0;
float obj_angle_x = 0;
float obj_angle_y = 0;



/*---------------------------------------------------------------------------
	Functions:
---------------------------------------------------------------------------*/

static void GenTexture( int mode );

/** Report OpenGL errors */
static void ReportGLErrors( void ) {
	int error = glGetError();

	if( error!=GL_NO_ERROR ){
		switch( error ){
			case GL_INVALID_ENUM:
				piDebug( "*** ReportGLErrors: Invalid enum.\n" );
			break;
			case GL_INVALID_VALUE:
				piDebug( "*** ReportGLErrors: Invalid value.\n" );
			break;
			case GL_INVALID_OPERATION:
				piDebug( "*** ReportGLErrors: Invalid operation.\n" );
			break;
			case GL_STACK_OVERFLOW:
				piDebug( "*** ReportGLErrors: Stack overflow.\n" );
			break;
			case GL_STACK_UNDERFLOW:
				piDebug( "*** ReportGLErrors: Stack underflow.\n" );
			break;
			case GL_OUT_OF_MEMORY:
				piDebug( "*** ReportGLErrors: Out of memory.\n" );
			break;
			default:
				piDebug( "*** ReportGLErrors: Unknown error!\n" );
		}
	}
}


/** Display callback. */
static void Display( void ) {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	if( dlist ) {
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		glTranslatef( -obj_pos_x, -obj_pos_y, -512-obj_pos_z );
		glRotatef( -90+obj_angle_y, 1, 0, 0 );
		glRotatef( -obj_angle_x, 0, 0, 1 );

		glCallList( dlist );
	}
	
	glutSwapBuffers();
}


/** Reshape callback. */
static void Reshape( GLint width, GLint height ) {
	GWidth = width;
	GHeight = height;

	glViewport( 0, 0, width, height );

	glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
	gluPerspective( 45.0, float(width) / float(height), 2.0f, 2000.0f );

	ReportGLErrors();
}


/** Redraw. */
static void Idle( void ) {
	glutPostRedisplay();
}


/** Keyboard callback. */
static void Keyboard( unsigned char key, int x, int y ) {
}


/** Mouse buttons callback. */
static void MouseButton( int button, int state, int x, int y ) {
	if( state==GLUT_DOWN ) {
		mouse_start_x = x;
		mouse_start_y = y;
		mouse_button = button;
	}
	else {
		mouse_button = 0;
	}
}


/** Mouse motion callback. */
static void MouseMotion( int x, int y ) {

	if( mouse_button==GLUT_LEFT_BUTTON ) {
		obj_angle_x += 0.4f * (mouse_start_x - x);
		obj_angle_y -= 0.4f * (mouse_start_y - y);
		mouse_start_x = x;
		mouse_start_y = y;
	}
	if( mouse_button==GLUT_MIDDLE_BUTTON ) {
		obj_pos_z -= (mouse_start_y - y);
		mouse_start_x = x;
		mouse_start_y = y;
	}
	if( mouse_button==GLUT_RIGHT_BUTTON ) {
		obj_pos_x += 0.4f * (mouse_start_x - x);
		obj_pos_y -= 0.4f * (mouse_start_y - y);
		mouse_start_x = x;
		mouse_start_y = y;
	}

	glutPostRedisplay();
}



/** Menu callback. */
static void Menu( int value ) {
	switch( value ) {
		case GPM_PM_POINTS:
			glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );
			piDebug( "--- Polygon Mode: points.\n" );
		break;
		case GPM_PM_LINES:
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			piDebug( "--- Polygon Mode: lines.\n" );
		break;
		case GPM_PM_TRIANGLES:
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			piDebug( "--- Polygon Mode: fill.\n" );
		break;
		case GPM_LM_NONE:
			glDisable( GL_LIGHTING );
			dlist = dlist_smooth;
			piDebug( "--- Lighting Mode: disable.\n" );
		break;
		case GPM_LM_FLAT:
			glEnable( GL_LIGHTING );
			glShadeModel( GL_FLAT );
			dlist = dlist_flat;
			piDebug( "--- Lighting Mode: flat.\n" );
		break;
		case GPM_LM_GOURAUD:
			glEnable( GL_LIGHTING );
			glShadeModel( GL_SMOOTH );
			dlist = dlist_smooth;
			piDebug( "--- Lighting Mode: gouraud.\n" );
		break;
		case GPM_TX_NONE:
			glDisable( GL_TEXTURE_CUBE_MAP );
		break;
		case GPM_TX_DEFAULT:
			GenTexture( GPM_TX_DEFAULT );
		break;
		case GPM_TX_LIGHTMAP:
			GenTexture( GPM_TX_LIGHTMAP );
			glDisable( GL_LIGHTING );
			dlist = dlist_smooth;
			piDebug( "--- Lighting Mode: disable.\n" );
		break;
		case GPM_TX_WOOD:
			GenTexture( GPM_TX_WOOD );
			glDisable( GL_LIGHTING );
			dlist = dlist_smooth;
			piDebug( "--- Lighting Mode: disable.\n" );
		break;
		case GPM_TX_MARBLE:
			GenTexture( GPM_TX_MARBLE );
			glDisable( GL_LIGHTING );
			dlist = dlist_smooth;
			piDebug( "--- Lighting Mode: disable.\n" );
		break;
		case GPM_TX_NORMAL:
			GenTexture( GPM_TX_NORMAL );
			glDisable( GL_LIGHTING );
			dlist = dlist_smooth;
			piDebug( "--- Lighting Mode: disable.\n" );
		break;
		case GPM_QUIT:
			exit(0);
		break;
	};
	glutPostRedisplay();
}


/** Initialize OpenGL. */
static void InitializeGlut( int *argc, char *argv[] ) {
	
	glutInitWindowSize( 512, 512 );
	glutInit( argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutCreateWindow( argv[0] );
	glutDisplayFunc( Display );
	glutReshapeFunc( Reshape );
	glutIdleFunc( Idle );

//	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	

//	glLineWidth( 2 );
//	glEnable( GL_LINE_SMOOTH );
//	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//	glEnable( GL_BLEND );
//	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );


	// Enable a single OpenGL light.
	glLightfv( GL_LIGHT0, GL_DIFFUSE, LightDiffuse );
	glLightfv( GL_LIGHT0, GL_POSITION, LightPosition );
	glEnable( GL_LIGHT0 );
//	glDisable( GL_LIGHTING );
	glEnable( GL_LIGHTING );

	glColor3f( 1, 1, 1 );

	// Use depth buffering for hidden surface elimination.
	glEnable( GL_DEPTH_TEST );

	glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );	// bug workaround?
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );


	// Setup the view of the cube.
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 45.0, 1.0, 2.0, 2000.0 );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef( 0, 0, -512 );
	glRotatef( -90, 1, 0, 0 );

	ReportGLErrors();



	int pm, lm, tx, ab;

	pm = glutCreateMenu( Menu );
	glutAddMenuEntry( "Points", GPM_PM_POINTS );
	glutAddMenuEntry( "Lines", GPM_PM_LINES );
	glutAddMenuEntry( "Triangles", GPM_PM_TRIANGLES );

	lm = glutCreateMenu( Menu );
	glutAddMenuEntry( "None", GPM_LM_NONE );
	glutAddMenuEntry( "Flat", GPM_LM_FLAT );
	glutAddMenuEntry( "Gouraud", GPM_LM_GOURAUD );

	tx = glutCreateMenu( Menu );
	glutAddMenuEntry( "None", GPM_TX_NONE );  
	glutAddMenuEntry( "Default", GPM_TX_DEFAULT );  
	glutAddMenuEntry( "Lightmap", GPM_TX_LIGHTMAP );
	glutAddMenuEntry( "Wood", GPM_TX_WOOD );  
	glutAddMenuEntry( "Marble", GPM_TX_MARBLE );
	glutAddMenuEntry( "Normapmap", GPM_TX_NORMAL );


	ab = glutCreateMenu( Menu );
	glutAddMenuEntry( "This program has been created by:", 0 );  
	glutAddMenuEntry( " Ignacio Castaño / castanyo@yahoo.es", 0 );
	glutAddMenuEntry( " for the GPM book.", 0 );

	glutCreateMenu( Menu );
	glutAddSubMenu( "Polygon Mode", pm );
	glutAddSubMenu( "Lighting Mode", lm );
	glutAddSubMenu( "Texture Mode", tx );
	glutAddSubMenu( "About", ab );
	glutAddMenuEntry( "Quit", GPM_QUIT );
	glutAttachMenu( GLUT_RIGHT_BUTTON );

	ReportGLErrors();
}





/** Load the meshes, build parameterization and create display lists. */
static void LoadMeshes( void ) {


	//IImportMesh * venus_ply = PiImportObj::Create( "venus.obj" );
	IImportMesh * venus_ply = PiImportPly::Create( "venus-25.ply" );
	//IImportMesh * venus_ply = PiImportPly::Create( "test.ply" );
	//IImportMesh * venus_ply = PiImportPly::Create( "venus-50.ply" );
	//IImportMesh * venus_ply = PiImportObj::Create( "asteroid/geographos.obj" );
	//IImportMesh * venus_ply = PiImportObj::Create( "asteroid/ky26.obj" );

	if( !venus_ply->Import() )
		return;

	venus = new PiTriMesh();
	venus->Open( venus_ply );
	delete venus_ply;


//	venus->InterchangeYZ();
	venus->FlipFaces();
	venus->ComputeNormals();
	venus->FitCube( 128 );

	piDebug( "--- Building mesh adjacency.\n" );
	venus->BuildAdjacency();

	if( !venus->IsClosed() ) {
		piDebug( "--- The mesh is NOT closed.\n" );
	}
	else {
		piDebug( "--- Building conformal map.\n" );
		int v0 = venus->GetLowestVertex( 2 );
		//int v1 = venus->GetMediumVertex( 2 );
		int v1 = venus->GetLowestVertex( 0 );
		int v2 = venus->GetHighestVertex( 2 );
		venus->ConformalSphericalMap( v0, v1, v2 );
	}

	piDebug( "--- Building display lists.\n" );
	int f;
	dlist_smooth = glGenLists(1);
	glNewList( dlist_smooth, GL_COMPILE );
		glBegin( GL_TRIANGLES );
		for( f=0; f<venus->GetFaceNum(); f++ ) {
			glTexCoord3fv( venus->GetVertex(venus->GetFace(f).v0).tex );
			glNormal3fv( venus->GetVertex(venus->GetFace(f).v0).nor );
			glVertex3fv( venus->GetVertex(venus->GetFace(f).v0).pos );
			glTexCoord3fv( venus->GetVertex(venus->GetFace(f).v1).tex );
			glNormal3fv( venus->GetVertex(venus->GetFace(f).v1).nor );
			glVertex3fv( venus->GetVertex(venus->GetFace(f).v1).pos );
			glTexCoord3fv( venus->GetVertex(venus->GetFace(f).v2).tex );
			glNormal3fv( venus->GetVertex(venus->GetFace(f).v2).nor );
			glVertex3fv( venus->GetVertex(venus->GetFace(f).v2).pos );
		}
		glEnd();
	glEndList();

	dlist_flat = glGenLists(1);
	glNewList( dlist_flat, GL_COMPILE );
		glBegin( GL_TRIANGLES );
		for( f=0; f<venus->GetFaceNum(); f++ ) {

			Vec3 e0, e1, n;
			e0.Sub( venus->GetVertex(venus->GetFace(f).v1).pos, venus->GetVertex(venus->GetFace(f).v0).pos );
			e1.Sub( venus->GetVertex(venus->GetFace(f).v2).pos, venus->GetVertex(venus->GetFace(f).v0).pos );
			n.Cross( e0, e1 );
			n.Normalize();

			glTexCoord3fv( venus->GetVertex(venus->GetFace(f).v0).tex );
			glNormal3fv( n );
			glVertex3fv( venus->GetVertex(venus->GetFace(f).v0).pos );
			glTexCoord3fv( venus->GetVertex(venus->GetFace(f).v1).tex );
			glNormal3fv( n );
			glVertex3fv( venus->GetVertex(venus->GetFace(f).v1).pos );
			glTexCoord3fv( venus->GetVertex(venus->GetFace(f).v2).tex );
			glNormal3fv( n );
			glVertex3fv( venus->GetVertex(venus->GetFace(f).v2).pos );
		}
		glEnd();
	glEndList();

	dlist_sphere = glGenLists(1);
	glNewList( dlist_sphere, GL_COMPILE );
		glBegin( GL_TRIANGLES );
		for( f=0; f<venus->GetFaceNum(); f++ ) {
			Vec3 p = venus->GetVertex(venus->GetFace(f).v0).tex; p *= 128;
			glTexCoord3fv( venus->GetVertex(venus->GetFace(f).v0).tex );
			glNormal3fv( venus->GetVertex(venus->GetFace(f).v0).nor );
			glVertex3fv( p );

			p = venus->GetVertex(venus->GetFace(f).v0).tex; p *= 128;
			glTexCoord3fv( venus->GetVertex(venus->GetFace(f).v1).tex );
			glNormal3fv( venus->GetVertex(venus->GetFace(f).v1).nor );
			glVertex3fv( p );

			p = venus->GetVertex(venus->GetFace(f).v0).tex; p *= 128;
			glTexCoord3fv( venus->GetVertex(venus->GetFace(f).v2).tex );
			glNormal3fv( venus->GetVertex(venus->GetFace(f).v2).nor );
			glVertex3fv( p );
		}
		glEnd();
	glEndList();

	dlist = dlist_smooth;

	ReportGLErrors();
}


/** Unload the meshes. */
static void UnloadMeshes( void ) {
	delete venus; venus = NULL;
}



static Vec3 Marble( const Vec3 &P, const Vec3 &N, REAL iter=10, REAL point_scale=1,
			const Vec3 &color0=Vec3(1, 1, 1), const Vec3 &color1=Vec3(0.2, 0.2, 0.7) ) {

    REAL sum = 0.0;
    REAL size = 1/iter;
	
	Vec3 PP = P;
	PP *= point_scale;
	PP.x += noise3( PP.x/2, PP.y/2, PP.z/2 ) * 2;
	PP.y += noise3( PP.y/2, PP.z/2, PP.x/2 ) * 2;
	PP.z += noise3( PP.z/2, PP.x/2, PP.y/2 ) * 2;


    for( REAL scale=1; scale >= size ; scale /= 2) {
        sum += scale * noise3( PP.x/scale, PP.y/scale, PP.z/scale );
	}

	sum = (sum+1)*0.5;

//	REAL value = clamp(sum*sum, 0.0f, 1.0f);
//	REAL value = clamp(4*sum-3, 0.0f, 1.0f);
	REAL value = sin( PP.y * 2 + sum * 4 );

	Vec3 C;
	C.Mix( color0, color1, value );
	return C;
}


/** Wood surface shader. */
static Vec3 Wood( const Vec3 &P, const Vec3 &N, REAL ring_scale=10, REAL point_scale=1, REAL turbulence=1, 
		  const Vec3 &color0=Vec3(0.3, 0.12, 0.03), const Vec3 &color1=Vec3(0.05, 0.01, 0.005) ) {

	Vec3 PP = P;
	PP *= point_scale;
	PP.x += noise3( PP.x, PP.y, PP.z ) * turbulence * 0.3;
	PP.y += noise3( PP.y, PP.z, PP.x ) * turbulence * 0.3;
	PP.z += noise3( PP.z, PP.x, PP.y ) * turbulence * 0.3;

	float r = sqrt( PP.y*PP.y + PP.z*PP.z );

	r *= ring_scale;
	r += fabs( 0.5 * (1+noise1(r)) * turbulence );
	r -= floor(r);  

	/* Use r to select wood color */
	r = smoothstep( 0, 0.8, r ) - smoothstep( 0.83, 1.0, r );
	Vec3 C;
	C.Blend( color0, color1, r );

	return C;
}

static int SampleDefault( void * param, const RA_Vertex * vertex, const RA_Vertex * dx, const RA_Vertex * dy, RA_Feature * hit ) {

	float color0_x, color0_y, color0_z;
	float color1_x, color1_y, color1_z;

	int s, t;
	if( fabs(vertex->tx1.x) > fabs(vertex->tx1.y) && fabs(vertex->tx1.x) > fabs(vertex->tx1.z) ) {
		color0_x = 1; color0_y = .8; color0_z = .8;
		color1_x = .68; color1_y = .55; color1_z = .55;
	}
	else if( fabs(vertex->tx1.y) > fabs(vertex->tx1.z) ) {
		color0_x = .8; color0_y = 1; color0_z = .8;
		color1_x = .55; color1_y = .68; color1_z = .55;
	}
	else {
		color0_x = .8; color0_y = .8; color0_z = 1;
		color1_x = .55; color1_y = .55; color1_z = .68;
	}
	s = vertex->tex.x;
	t = vertex->tex.y;

	s >>= 4;
	t >>= 4;
	if( (s&1) ^ (t&1) ) {
		hit->attrib[0] = color0_x * 65535;
		hit->attrib[1] = color0_y * 65535;
		hit->attrib[2] = color0_z * 65535;
		hit->attrib[4] = 65535;
	}
	else {
		hit->attrib[0] = color1_x * 65535;
		hit->attrib[1] = color1_y * 65535;
		hit->attrib[2] = color1_z * 65535;
		hit->attrib[4] = 65535;
	}

	hit->samples = 1;
	return 1;
}

static int SampleLightmap( void * param, const RA_Vertex * vertex, const RA_Vertex * dx, const RA_Vertex * dy, RA_Feature * hit ) {

	Vec3 n = vertex->nor;
	n.Normalize();

	// ambient
	float f = 0.1;

	// light 0
	Vec3 L( 1, 1, -1 ); 
	L.Normalize();
	f += clamp( Vec3DotProduct( n, L ), 0.0f, 1.0f ) * 0.8;

	// light 1
	L.Set( -1, -1, 0.75 ); 
	L.Normalize();
	f += clamp( Vec3DotProduct( n, L ), 0.0f, 1.0f ) * 0.3;

	f = clamp( f, 0.0f, 1.0f );
	hit->attrib[0] = f * 65535.0f;
	hit->attrib[1] = f * 65535.0f;
	hit->attrib[2] = f * 65535.0f;
	hit->attrib[3] = 0;

	hit->samples = 1;
	return 1;
}

static int SampleWood( void * param, const RA_Vertex * vertex, const RA_Vertex * dx, const RA_Vertex * dy, RA_Feature * hit ) {
	SampleLightmap( param, vertex, dx, dy, hit );
	Vec3 c0( 0.68, 0.4, 0.05 );
	Vec3 c1( 0.3, 0.12, 0.03 );
	Vec3 color = Wood( vertex->pos, vertex->nor, 0.8, 0.1, 1, c0, c1 );
	hit->attrib[0] = color.x * float(hit->attrib[0]);
	hit->attrib[1] = color.y * float(hit->attrib[1]);
	hit->attrib[2] = color.z * float(hit->attrib[2]);
	return 1;
}

static int SampleMarble( void * param, const RA_Vertex * vertex, const RA_Vertex * dx, const RA_Vertex * dy, RA_Feature * hit ) {
	SampleLightmap( param, vertex, dx, dy, hit );
	Vec3 color = Marble( vertex->pos, vertex->nor, 10, 0.05 );
	hit->attrib[0] = color.x * float(hit->attrib[0]);
	hit->attrib[1] = color.y * float(hit->attrib[1]);
	hit->attrib[2] = color.z * float(hit->attrib[2]);
	return 1;
}

static int SampleNormal( void * param, const RA_Vertex * vertex, const RA_Vertex * dx, const RA_Vertex * dy, RA_Feature * hit ) {
	Vec3 n = vertex->nor;
	n.Normalize();

	hit->attrib[0] = PackFloatInWord( n.x );
	hit->attrib[1] = PackFloatInWord( n.y );
	hit->attrib[2] = PackFloatInWord( n.z );
	hit->attrib[3] = 0;

	hit->samples = 1;
	return 1;
}



/** */
static void GenTexture( int mode ) {

	RA_SAMPLING_CALLBACK Sampler = SampleDefault;

	switch( mode ) {
		case GPM_TX_DEFAULT:
			piDebug( "--- Building default texture.\n" );
			Sampler = SampleDefault;
		break;
		case GPM_TX_LIGHTMAP:
			piDebug( "--- Building mesh lightmap.\n" );
			Sampler = SampleLightmap;
		break;
		case GPM_TX_WOOD:
			piDebug( "--- Building mesh wood texture.\n" );
			Sampler = SampleWood;
		break;
		case GPM_TX_MARBLE:
			piDebug( "--- Building mesh marble texture.\n" );
			Sampler = SampleMarble;
		break;
		case GPM_TX_NORMAL:
			piDebug( "--- Building mesh normal map.\n" );
			Sampler = SampleNormal;
		break;
	}


	int size = 256;

	// Gen textures:
	RA_Image * rt = RA_Image_New( size, size );
	RA_Image * lf = RA_Image_New( size, size );
	RA_Image * ft = RA_Image_New( size, size );
	RA_Image * bk = RA_Image_New( size, size );
	RA_Image * up = RA_Image_New( size, size );
	RA_Image * dn = RA_Image_New( size, size );


	int f, f_num = venus->GetFaceNum();
	for( f=0; f<f_num; f++ ) {
		int v0 = venus->GetFace(f).v0;
		int v1 = venus->GetFace(f).v1;
		int v2 = venus->GetFace(f).v2;

		RA_Triangle t;
		t[0].pos = venus->GetVertex(v0).pos;
		t[0].nor = venus->GetVertex(v0).nor;
		t[0].tx1 = venus->GetVertex(v0).tex;
		t[1].pos = venus->GetVertex(v1).pos;
		t[1].nor = venus->GetVertex(v1).nor;
		t[1].tx1 = venus->GetVertex(v1).tex;
		t[2].pos = venus->GetVertex(v2).pos;
		t[2].nor = venus->GetVertex(v2).nor;
		t[2].tx1 = venus->GetVertex(v2).tex;

		// Render face x+
		if( t[0].tx1.x > 0 && t[1].tx1.x > 0 && t[2].tx1.x > 0 ) {

			t[0].tex.Set( size*( -t[0].tx1.z / t[0].tx1.x * 0.5 + 0.5), size*( -t[0].tx1.y / t[0].tx1.x * 0.5 + 0.5), 0 );
			t[1].tex.Set( size*( -t[1].tx1.z / t[1].tx1.x * 0.5 + 0.5), size*( -t[1].tx1.y / t[1].tx1.x * 0.5 + 0.5), 0 );
			t[2].tex.Set( size*( -t[2].tx1.z / t[2].tx1.x * 0.5 + 0.5), size*( -t[2].tx1.y / t[2].tx1.x * 0.5 + 0.5), 0 );

			RA_Process( rt, t, 0, Sampler, NULL );
		}
		// Render face x-
		else if( t[0].tx1.x < 0 && t[1].tx1.x < 0 && t[2].tx1.x < 0 ) {

			t[0].tex.Set( size*( t[0].tx1.z / -t[0].tx1.x * 0.5 + 0.5), size*( -t[0].tx1.y / -t[0].tx1.x * 0.5 + 0.5), 0 );
			t[1].tex.Set( size*( t[1].tx1.z / -t[1].tx1.x * 0.5 + 0.5), size*( -t[1].tx1.y / -t[1].tx1.x * 0.5 + 0.5), 0 );
			t[2].tex.Set( size*( t[2].tx1.z / -t[2].tx1.x * 0.5 + 0.5), size*( -t[2].tx1.y / -t[2].tx1.x * 0.5 + 0.5), 0 );

			RA_Process( lf, t, 0, Sampler, NULL );
		}
		// render face y+
		if( t[0].tx1.y > 0 && t[1].tx1.y > 0 && t[2].tx1.y > 0 ) {

			t[0].tex.Set( size*( t[0].tx1.x / t[0].tx1.y * 0.5 + 0.5), size*( t[0].tx1.z / t[0].tx1.y * 0.5 + 0.5), 0 );
			t[1].tex.Set( size*( t[1].tx1.x / t[1].tx1.y * 0.5 + 0.5), size*( t[1].tx1.z / t[1].tx1.y * 0.5 + 0.5), 0 );
			t[2].tex.Set( size*( t[2].tx1.x / t[2].tx1.y * 0.5 + 0.5), size*( t[2].tx1.z / t[2].tx1.y * 0.5 + 0.5), 0 );

			RA_Process( ft, t, 0, Sampler, NULL );
		}
		// render face y-
		else if( t[0].tx1.y < 0 && t[1].tx1.y < 0 && t[2].tx1.y < 0 ) {

			t[0].tex.Set( size*( t[0].tx1.x / -t[0].tx1.y * 0.5 + 0.5), size*( -t[0].tx1.z / -t[0].tx1.y * 0.5 + 0.5), 0 );
			t[1].tex.Set( size*( t[1].tx1.x / -t[1].tx1.y * 0.5 + 0.5), size*( -t[1].tx1.z / -t[1].tx1.y * 0.5 + 0.5), 0 );
			t[2].tex.Set( size*( t[2].tx1.x / -t[2].tx1.y * 0.5 + 0.5), size*( -t[2].tx1.z / -t[2].tx1.y * 0.5 + 0.5), 0 );

			RA_Process( bk, t, 0, Sampler, NULL );
		}
		// render face z+
		if( t[0].tx1.z > 0 && t[1].tx1.z > 0 && t[2].tx1.z > 0 ) {

			t[0].tex.Set( size*( t[0].tx1.x / t[0].tx1.z * 0.5 + 0.5), size*( -t[0].tx1.y / t[0].tx1.z * 0.5 + 0.5), 0 );
			t[1].tex.Set( size*( t[1].tx1.x / t[1].tx1.z * 0.5 + 0.5), size*( -t[1].tx1.y / t[1].tx1.z * 0.5 + 0.5), 0 );
			t[2].tex.Set( size*( t[2].tx1.x / t[2].tx1.z * 0.5 + 0.5), size*( -t[2].tx1.y / t[2].tx1.z * 0.5 + 0.5), 0 );

			RA_Process( up, t, 0, Sampler, NULL );
		}
		// render face z-
		else if( t[0].tx1.z < 0 && t[1].tx1.z < 0 && t[2].tx1.z < 0 ) {

			t[0].tex.Set( size*( -t[0].tx1.x / -t[0].tx1.z * 0.5 + 0.5), size*( -t[0].tx1.y / -t[0].tx1.z * 0.5 + 0.5), 0 );
			t[1].tex.Set( size*( -t[1].tx1.x / -t[1].tx1.z * 0.5 + 0.5), size*( -t[1].tx1.y / -t[1].tx1.z * 0.5 + 0.5), 0 );
			t[2].tex.Set( size*( -t[2].tx1.x / -t[2].tx1.z * 0.5 + 0.5), size*( -t[2].tx1.y / -t[2].tx1.z * 0.5 + 0.5), 0 );

			RA_Process( dn, t, 0, Sampler, NULL );
		}
	}

	int samples = 2;
	RA_Filter_Downsample( rt, samples );
	RA_Filter_Downsample( lf, samples );
	RA_Filter_Downsample( ft, samples );
	RA_Filter_Downsample( bk, samples );
	RA_Filter_Downsample( up, samples );
	RA_Filter_Downsample( dn, samples );
	size /= samples;


	glEnable( GL_TEXTURE_CUBE_MAP );

	if( cube!=0 ) glDeleteTextures( 1, &cube );
	glGenTextures( 1, &cube );

	glBindTexture( GL_TEXTURE_CUBE_MAP, cube );

	int i;
	uint8 * buffer = new uint8[size*size*3];
	for( i=0; i<size*size; i++ ) {
		buffer[3*i+0] = (uint8)(rt->features[i].attrib[0]>>8);
		buffer[3*i+1] = (uint8)(rt->features[i].attrib[1]>>8);
		buffer[3*i+2] = (uint8)(rt->features[i].attrib[2]>>8);
	}
	glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer );

	for( i=0; i<size*size; i++ ) {
		buffer[3*i+0] = (uint8)(lf->features[i].attrib[0]>>8);
		buffer[3*i+1] = (uint8)(lf->features[i].attrib[1]>>8);
		buffer[3*i+2] = (uint8)(lf->features[i].attrib[2]>>8);
	}
	glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer );

	for( i=0; i<size*size; i++ ) {
		buffer[3*i+0] = (uint8)(ft->features[i].attrib[0]>>8);
		buffer[3*i+1] = (uint8)(ft->features[i].attrib[1]>>8);
		buffer[3*i+2] = (uint8)(ft->features[i].attrib[2]>>8);
	}
	glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer );

	for( i=0; i<size*size; i++ ) {
		buffer[3*i+0] = (uint8)(bk->features[i].attrib[0]>>8);
		buffer[3*i+1] = (uint8)(bk->features[i].attrib[1]>>8);
		buffer[3*i+2] = (uint8)(bk->features[i].attrib[2]>>8);
	}
	glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer );

	for( i=0; i<size*size; i++ ) {
		buffer[3*i+0] = (uint8)(up->features[i].attrib[0]>>8);
		buffer[3*i+1] = (uint8)(up->features[i].attrib[1]>>8);
		buffer[3*i+2] = (uint8)(up->features[i].attrib[2]>>8);
	}
	glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer );

	for( i=0; i<size*size; i++ ) {
		buffer[3*i+0] = (uint8)(dn->features[i].attrib[0]>>8);
		buffer[3*i+1] = (uint8)(dn->features[i].attrib[1]>>8);
		buffer[3*i+2] = (uint8)(dn->features[i].attrib[2]>>8);
	}
	glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer );

	delete [] buffer;

	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );



	ReportGLErrors();

	RA_Image_Delete( rt );
	RA_Image_Delete( lf );
	RA_Image_Delete( ft );
	RA_Image_Delete( bk );
	RA_Image_Delete( up );
	RA_Image_Delete( dn );
}



/** Main. */
int main( int argc, char *argv[] ) {

	InitializeGlut( &argc, argv );

//	glClearColor( 1, 1, 1, 1);

	LoadMeshes();

	GenTexture( GPM_TX_DEFAULT );

	glutMainLoop();

	glDeleteTextures( 1, &cube );

	UnloadMeshes();

	return 0;
}


