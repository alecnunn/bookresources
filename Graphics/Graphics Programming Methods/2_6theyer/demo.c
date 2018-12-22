/*****************************************************************
 * Copyright (c) 2002 TheyerGFX Pty Ltd (www.theyergfx.com)
 *****************************************************************
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.
 *****************************************************************
 * Project		: Game Programming Methods
 *****************************************************************
 * File			: demo.c
 * Language		: ANSI C
 * Author		: Mark Theyer
 * Created		: 19 Dec 2002
 *****************************************************************
 * Description	: Demo' for Curved PN Triangles
 *****************************************************************/

/*
 * includes
 */

#include <pntri.h>
#include <GL/glut.h>
#include <math.h>

/*
 * macros
 */

/*
 * typedefs
 */

/*
 * prototypes
 */

/*
 * globals
 */

// GL lighting
GLfloat light_ambient[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat light_diffuse[]  = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };

// points and normals
Point_f3d points[4]  = { {-1.2f, 1.0f, 0.0f}, {1.2f, 1.0f, 0.0f}, {-0.6f, -1.0f, 0.0f}, {0.6f, -0.5f, 0.0f} };
Point_f3d normals[4] = { {0.707f, -0.707f, 1.0f}, {0.707f, 0.707f, 1.0f}, {-0.707f, -0.707f, 1.0f}, {0.707f, -0.707f, 1.0f} };

// flags
int     mode = 1;
int		lod  = 2;
int		wireframe = 0;

/*
 * functions
 */


/******************************************************************************
 * Function:
 * flattriangles -- draw a pair of flat triangles
 * 
 * Description:
 *
 * Returns:
 * 
 */

void flattriangles( void ) 
{
	/* draw flat triangles */
	glBegin(GL_TRIANGLES); {
		/* first triangle */
		glNormal3fv( (GLfloat *)&normals[0] );
		glVertex3fv( (GLfloat *)&points[0]  );
		glNormal3fv( (GLfloat *)&normals[1] );
		glVertex3fv( (GLfloat *)&points[1]  );
		glNormal3fv( (GLfloat *)&normals[2] );
		glVertex3fv( (GLfloat *)&points[2]  );
		/* second triangle */
		glNormal3fv( (GLfloat *)&normals[1] );
		glVertex3fv( (GLfloat *)&points[1]  );
		glNormal3fv( (GLfloat *)&normals[2] );
		glVertex3fv( (GLfloat *)&points[2]  );
		glNormal3fv( (GLfloat *)&normals[3] );
		glVertex3fv( (GLfloat *)&points[3]  );
	} glEnd();
}


/******************************************************************************
 * Function:
 * pnfilter -- use PN filter functions to draw a pair of PN triangles
 * 
 * Description:
 *
 * Returns:
 * 
 */

void pnfilter( void ) 
{
	int			 i, ntri;
	Point_f3d	 p[32];
	Point_f3d	 n[32];
	Point_f3d	 net[32];
	short		 tripts[72];
	short		*t;

	/* first triangle */
	pnControlNet( points, normals, net );
	pnBezierPatch( net, lod, p, n );
	ntri = pnTessellate( p, lod, tripts );
	/* draw PN triangles */
	t = tripts;
	ntri *= 3;
	glBegin(GL_TRIANGLES); {
		for ( i=0; i<ntri; i++ ) {
			glNormal3fv( (GLfloat *)&n[*t] );
			glVertex3fv( (GLfloat *)&p[*t] );
			t++;
		}
	} glEnd();

	/* second triangle */
	pnControlNet( &points[1], &normals[1], net );
	pnBezierPatch( net, lod, p, n );
	ntri = pnTessellate( p, lod, tripts );
	/* draw PN triangles */
	t = tripts;
	ntri *= 3;
	glBegin(GL_TRIANGLES); {
		for ( i=0; i<ntri; i++ ) {
			glNormal3fv( (GLfloat *)&n[*t] );
			glVertex3fv( (GLfloat *)&p[*t] );
			t++;
		}
	} glEnd();
}


/******************************************************************************
 * Function:
 * pntriangles -- use PN primitive functions to draw variable edge PN triangles
 * 
 * Description:
 *
 * Returns:
 * 
 */

void pntriangles( int nsides ) 
{
	int			 i, ntri;
	Point_f3d	 p[32];
	Point_f3d	 n[32];
	Point_f3d	 net[32];
	short		 tripts[72];
	short		*t;

	/* first triangle */
	n[0] = normals[0];	// first 3 normals used for lighting
	n[1] = normals[1];
	n[2] = normals[2];
	n[3] = normals[0];  // unused
	n[4] = normals[0];	// last 6 used for curve creation
	n[5] = normals[1];
	n[6] = normals[1];
	n[7] = normals[2];
	n[8] = normals[2];
	n[9] = normals[0];
	pnControlNet3( points, n, net );
	pnBezierPatch3( net, nsides, lod, p, n );
	ntri = pnTessellate3( p, nsides, lod, tripts );
	/* draw PN triangles */
	t = tripts;
	ntri *= 3;
	glColor3f( 1.0f, 1.0f, 1.0f );
	glBegin(GL_TRIANGLES); {
		for ( i=0; i<ntri; i++ ) {
			glNormal3fv( (GLfloat *)&n[*t] );
			glVertex3fv( (GLfloat *)&p[*t] );
			t++;
		}
	} glEnd();

	/* second triangle */
	n[0] = normals[1];
	n[1] = normals[2];
	n[2] = normals[3];
	n[3] = normals[0];  // unused
	n[4] = normals[1];
	n[5] = normals[2];
	n[6] = normals[2];
	n[7] = normals[3];
	n[8] = normals[3];
	n[9] = normals[1];
	pnControlNet3( &points[1], n, net );
	pnBezierPatch3( net, nsides, lod, p, n );
	ntri = pnTessellate3( p, nsides, lod, tripts );
	/* draw PN triangles */
	t = tripts;
	ntri *= 3;
	glColor3f( 1.0f, 0.0f, 0.0f );
	glBegin(GL_TRIANGLES); {
		for ( i=0; i<ntri; i++ ) {
			glNormal3fv( (GLfloat *)&n[*t] );
			glVertex3fv( (GLfloat *)&p[*t] );
			t++;
		}
	} glEnd();
}


/******************************************************************************
 * Function:
 * pnquad -- use PN quad primitive functions to draw a PN quad
 * 
 * Description:
 *
 * Returns:
 * 
 */

void pnquad( void ) 
{
	int			 i, ntri;
	Point_f3d	 p[64];
	Point_f3d	 n[64];
	Point_f3d	 net[32];
	short		 tripts[148];
	short		*t;

	/* quad */
	n[0] = normals[0];	// first 4 normals used for lighting
	n[1] = normals[1];
	n[2] = normals[2];
	n[3] = normals[3];
	n[4] = normals[0];	// next 8 used for curve creation
	n[5] = normals[1];
	n[6] = normals[1];
	n[7] = normals[3];
	n[8] = normals[3];
	n[9] = normals[2];
	n[10] = normals[2];
	n[11] = normals[0];
	/* first triangle */
	pnControlNet4( points, n, net );
	pnBezierPatch4( net, 4, lod, p, n );
	ntri = pnTessellate4( p, 4, lod, tripts );
	/* draw PN triangles */
	t = tripts;
	ntri *= 3;
	glBegin(GL_TRIANGLES); {
		for ( i=0; i<ntri; i++ ) {
			glNormal3fv( (GLfloat *)&n[*t] );
			glVertex3fv( (GLfloat *)&p[*t] );
			t++;
		}
	} glEnd();
}


/******************************************************************************
 * Function:
 * draw -- draw the display
 * 
 * Description:
 *
 * Returns:
 * 
 */

void draw( void )
{
	GLfloat		 vn[3];
	int			 i;

	/* wireframe display? */
	if ( wireframe )
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	else
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	/* draw flat triangles, PN triangles or as a PN quad */
	glEnable(GL_LIGHTING);
	glColor3f( 1.0f, 1.0f, 1.0f );
	switch(mode) {
	case 1:
		flattriangles();
		break;
	case 2:
		pnfilter();
		break;
	case 3:
		pntriangles(1);
		break;
	case 4:
		pntriangles(2);
		break;
	case 5:
		pntriangles(3);
		break;
	case 6:
		pnquad();
		break;
	}

	/* draw normals */
	glDisable(GL_LIGHTING);
	glColor3f( 1.0f, 1.0f, 0.0f );
	glBegin(GL_LINES); {
		for ( i=0; i<4; i++ ) {
			glVertex3fv( (GLfloat *)&points[i] );
			vn[0] = points[i].x + normals[i].x;
			vn[1] = points[i].y + normals[i].y;
			vn[2] = points[i].z + normals[i].z;
			glVertex3fv(vn);
		}
	} glEnd();
}


/******************************************************************************
 * Function:
 * display -- refresh the display
 * 
 * Description:
 *
 * Returns:
 * 
 */

void display( void )
{
	/* draw the display */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	draw();
	glutSwapBuffers();
}


/******************************************************************************
 * Function:
 * normalise -- normalise a vector
 * 
 * Description:
 *
 * Returns:
 * 
 */

void normalise ( 
	Point_f3d  *n 
	)
{
	float		nlen;

	nlen = 1.0f/(float)sqrt((double) ((n->x * n->x) + (n->y * n->y) + (n->z * n->z)) );
	n->x *= nlen;
	n->y *= nlen;
	n->z *= nlen;
}


/******************************************************************************
 * Function:
 * spin -- spin the model by holding the mouse
 * 
 * Description:
 *
 * Returns:
 * 
 */

void spin( int x, int y )
{
	float		mat[16];
	int			mx, my;

	static int	lastx, lasty = 0;

	/* rotate the model */
	if ( lastx != 0 ) {
		mx = x - lastx;
		my = y - lasty;
		glMatrixMode( GL_MODELVIEW );
		glGetFloatv( GL_MODELVIEW_MATRIX, mat );
		glLoadIdentity();
		if ( mx != 0 ) glRotatef( (float)mx, 0.0f, 1.0f, 0.0f );
		if ( my != 0 ) glRotatef( (float)my, 1.0f, 0.0f, 0.0f );
		glMultMatrixf( mat );
	}

	/* save */
	lastx = x;
	lasty = y;

	/* redraw */
	display();
}


/******************************************************************************
 * Function:
 * keyboard -- process keyboard events
 * 
 * Description:
 *
 * Returns:
 * 
 */

void keyboard( unsigned char key, int x, int y )
{
	switch (key) {
	case '0':
		lod = 0;
		printf( "level of detail = 0\n" );
		break;
	case '1':
		lod = 1;
		printf( "level of detail = 1\n" );
		break;
	case '2':
		lod = 2;
		printf( "level of detail = 2\n" );
		break;
	case '3':
		lod = 3;
		printf( "level of detail = 3\n" );
		break;
	case '4':
		lod = 4;
		printf( "level of detail = 4\n" );
		break;
	case '5':
		lod = 5;
		printf( "level of detail = 5\n" );
		break;
	case 27:
		exit(1);
		break;
	case ' ':
		mode++;
	   /* limit mode */
	   if ( mode > 6 ) 
		   mode = 1;
		switch (mode) {
		case 1:
			printf( "display = flat triangles\n" );
			break;
		case 2:
			printf( "display = PN filter\n" );
			break;
		case 3:
			printf( "display = PN triangles, 1 sided curves\n" );
			break;
		case 4:
			printf( "display = PN triangles, 2 sided curves\n" );
			break;
		case 5:
			printf( "display = PN triangles, 3 sided curves\n" );
			break;
		case 6:
			printf( "display = PN quad\n" );
			break;
		}
		break;
	case 'w':
		wireframe = !wireframe;
		break;
	}


   /* redraw */
	display();
}


/******************************************************************************
 * Function:
 * init -- initialise the demo'
 * 
 * Description:
 *
 * Returns:
 * 
 */

void init( void )
{
	/* normalise my normals */
	normalise( &normals[0] );
	normalise( &normals[1] );
	normalise( &normals[2] );
	normalise( &normals[3] );
  
	/* init OpenGL */
	glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE,  light_diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	gluPerspective( 40.0f, 1.0f, 1.0f, 10.0f );
	gluLookAt( 0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f );
	glMatrixMode(GL_MODELVIEW);
}


/******************************************************************************
 * Function:
 * main -- run the demo' using GLUT and OpenGL
 * 
 * Description:
 *
 * Returns:
 * 
 */

int main( int argc, char **argv )
{
	/* init */
	glutInit( &argc, argv );
	glutInitDisplayMode( (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH) );
	glutCreateWindow( "PN Triangles Demo" );
	glutMotionFunc( spin );
	glutDisplayFunc( display );
	glutKeyboardFunc( keyboard );
	init();

	/* instructions */
	printf( "\n \
PN Triangles Demo \n \
----------------- \n \
 \n \
Keys: \n \
	left mouse = rotate model \n \
	esc key = exit \n \
	space bar = primitives toggle \n \
	w key = wireframe toggle \n \
	0,1,2,3,4 or 5 key = level of detail \n \
 \n \
display = flat triangles\n" );

	/* main loop */
	glutMainLoop();
	return(0);
}

