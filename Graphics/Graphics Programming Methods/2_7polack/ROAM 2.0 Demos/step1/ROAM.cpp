//==============================================================
//= ROAM.cpp
//= Original coders: Trent Polack (trent@codershq.com) and
//=				     Mark Duchaineau
//==============================================================
//= The header file for the ROAM 2.0 implementation
//==============================================================
#include <cstdio>
#include <cmath>

#include <SDL_opengl.h>

#include "..\\Framework\\Texture.h"
#include "..\\Framework\\Log.h"

#include "ROAM.h"
#include "randtab.h"

//initialize the ROAM engine
void Terrain::ROAM::Init( int maxLevel, DemoFramework::Camera *pCamera ) {
	char gridData[128*128*4];	//texture for showing mesh
	float a0, a1, a2;
	float f;
	int k;

	//generate the midpoint-displacement table
	m_maxLevel= maxLevel;
	m_pMDLevelSize= new float [m_maxLevel + 1];

	//generate table of displacement sizes versus levels
	for( int level= 0; level <= m_maxLevel; level++ )
		m_pMDLevelSize[level]= 0.3f/( ( float )sqrt( ( float )( ( __int64 )( 1<<level ) ) ) );

    //generate grid texture
	for( int y= 0; y < 128; y++ ) {
		for( int x= 0; x < 128; x++ ) {
			//create bump-shaped function f that is zero on each edge
			a0= ( float )y/127.0f;
			a1= ( float )( 127 - x )/127.0f;
			a2= ( float )( x - y )/127.0f;

			f= a0*a1*a2;
			f= ( float )sqrt( f );

			//quantize the function value and make into color
			//store in rgb components of texture entry
			k= ( int )floor( 1400.0f*f );

			CLAMP( k, 0, 255 );

			gridData[4*( y*128 + x ) + 0]= 30+( SQR( k ) )/290;
			gridData[4*( y*128 + x ) + 1]= k;
			gridData[4*( y*128 + x ) + 2]= ( k<128 ? 0 : ( k-128 )*2 );
			gridData[4*( y*128 + x ) + 3]= 0;
		}
	}

	//make texture coordinates for three triangle vertices
	//all triangles use same tex coords when showing grid
	//suck in by half a texel to be correct for bilinear textures
	a0= 0.5f/128.0f;
	a1= 1.0f - a0;

	//set the texture coordinates for three vertices
	m_gridTexCoords[0][0]= a0;	m_gridTexCoords[0][1]= a0;
	m_gridTexCoords[1][0]= a1;	m_gridTexCoords[1][1]= a0;
	m_gridTexCoords[2][0]= a1;	m_gridTexCoords[2][1]= a1;

	//set up the gridview texture
	glGenTextures( 1,&m_gridID );
	glBindTexture( GL_TEXTURE_2D, m_gridID );

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGBA,
				  GL_UNSIGNED_BYTE, 0 );

	glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 128, 128,
					 GL_RGBA, GL_UNSIGNED_BYTE, gridData );

	//set the class's camera pointer
	m_pCamera= pCamera;
}

//shutdown the ROAM engine
void Terrain::ROAM::Shutdown( void ) {
	delete[] m_pMDLevelSize;
}

//the high-level rendering function to being ROAM rendering
void Terrain::ROAM::Render( void ) {
	float verts[4][3];
	int i;    

	//reset the debug counters
	m_vertsPerFrame= 0;
	m_trisPerFrame = 0;

	//turn on textured drawing (with the grid texture for now)
	glBindTexture( GL_TEXTURE_2D, m_gridID );
	glEnable( GL_TEXTURE_2D );

	//render the roam mesh
	//compute four corners of the base square 
	for( i= 0; i < 4; i++ ) {
		verts[i][0]= ( ( i & 1 ) ? 1.0f : -1.0f );
		verts[i][1]= 0.0f;
		verts[i][2]= ( ( i & 2 ) ? 1.0f : -1.0f );
	}

	glColor3f( 1.0f, 1.0f, 1.0f );

	//recurse on the two base triangles
	glBegin( GL_TRIANGLES );
		RenderChild( 0, verts[0], verts[1], verts[3] );
		RenderChild( 0, verts[3], verts[2], verts[0] );
	glEnd( );

	//end texturing
	glDisable( GL_TEXTURE_2D );
}

//recursive rendering function
void Terrain::ROAM::RenderChild( int level, float *pV1, float *pV2, float *pV3 ) {
	unsigned char *pC;
	unsigned int s;
    float newVert[3];	// new (split) vertex
	float md, dist;
	float randHash;
	int *pInt;
	int  i;

	//max midpoint-displacement size
	md= m_pMDLevelSize[level];

    //compute split point of base edge (Y/Z coordinates are reversed for now)
    newVert[0]= ( pV1[0] + pV3[0] )/2.0f;
    newVert[1]= ( pV1[2] + pV3[2] )/2.0f;
	newVert[2]= 0.0f;

	//determine random perturbation of center z using hash of x,y
	//random number lookup per byte of (x, z) data, all added
	pC= ( unsigned char* )newVert;
	for( i=0, s=0; i < 8; i++ )
		s+= randtab[( i<<8 ) | pC[i]];

	//stuff random hash value bits from s into float (float viewed
	//as an int, IEEE float tricks here...)
	pInt= ( int* )( &randHash );

	*pInt	 = 0x40000000+( s & 0x007fffff );
	randHash-= 3.0f;

	//the random height value for the vertex
	newVert[2]= newVert[1];
	newVert[1]= ( ( pV1[1] + pV3[1] )/2.0f )+randHash*md;

	//distance calculation
	dist= SQR( ( newVert[0] - m_pCamera->m_eyePos[0] ) )+
		  SQR( ( newVert[1] - m_pCamera->m_eyePos[1] ) )+
		  SQR( ( newVert[2] - m_pCamera->m_eyePos[2] ) );

    if( level < m_maxLevel && SQR( md ) > dist*0.00001f ) {
		//render the children
        RenderChild( level + 1, pV1, newVert, pV2 );
		RenderChild( level + 1, pV2, newVert, pV3 );
        
		//the current node doesn't need to be rendered,
		//since both of its children are
		return;
    }

	//send the vertices to the rendering API
	glTexCoord2fv( m_gridTexCoords[0] ); glVertex3fv( pV1 );
	glTexCoord2fv( m_gridTexCoords[1] ); glVertex3fv( pV2 );
	glTexCoord2fv( m_gridTexCoords[2] ); glVertex3fv( pV3 );

	m_vertsPerFrame+= 3;
	m_trisPerFrame++;
}
