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

//initialize the ROAM engine
void Terrain::ROAM::Init( int maxLevel, DemoFramework::Camera *pCamera ) {
	int level;

	m_maxLevel= maxLevel;
	m_pMDLevelSize= new float [m_maxLevel+1];

	//generate table of displacement sizes versus levels
	for( level=0; level <= m_maxLevel; level++ )
		m_pMDLevelSize[level]= 255/( ( float )sqrt( ( float )( 1<<level ) ) );

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
	glBindTexture( GL_TEXTURE_2D, m_texture.GetID( ) );
	glEnable( GL_TEXTURE_2D );

	//render the roam mesh
	//compute four corners of the base square 
	for( i=0; i < 4; i++ ) {
		verts[i][0]= ( ( i & 1 ) ? ( m_size - 1 ) : 0.0f );
		verts[i][2]= ( ( i & 2 ) ? ( m_size - 1 ) : 0.0f );

		verts[i][1]= m_heightMap.GetData( ( int )verts[i][0], ( int )verts[i][2] );
	}

	glColor3f( 1.0f, 1.0f, 1.0f );

	//recurse on the two base triangles
	glBegin( GL_TRIANGLES );
		RenderChild( 0, verts[0], verts[1], verts[3], 0 );
		RenderChild( 0, verts[3], verts[2], verts[0], 0 );
	glEnd( );

	//end texturing
	glDisable( GL_TEXTURE_2D );
}

//recursive rendering function
void Terrain::ROAM::RenderChild( int level, float *pV1, float *pV2, float *pV3, int cull ) {
	float newVert[3], dist, sqrBound, sqrBoundTemp;

	//compute split point of base edge
	newVert[0]= ( pV1[0] + pV3[0] )/2.0f;
	newVert[2]= ( pV1[2] + pV3[2] )/2.0f;
	newVert[1]= m_heightMap.GetData( ( int )newVert[0], ( int )newVert[2] );

	//calculate the bounding sphere for the current triangle
	//calculation 1
	sqrBound= SQR( ( pV1[0]-newVert[0] ) )+
			  SQR( ( pV1[1]-newVert[1] ) )+
			  SQR( ( pV1[2]-newVert[2] ) );

	//calculation 2
	sqrBoundTemp= SQR( ( pV2[0]-newVert[0] ) )+
				  SQR( ( pV2[1]-newVert[1] ) )+
				  SQR( ( pV2[2]-newVert[2] ) );
	
	//check to see if this is the largest distance we've calculated so far
	if( sqrBoundTemp > sqrBound )
		sqrBound= sqrBoundTemp;

	//calculation 3
	sqrBoundTemp= SQR( ( pV3[0]-newVert[0] ) )+
				  SQR( ( pV3[1]-newVert[1] ) )+
				  SQR( ( pV3[2]-newVert[2] ) );
	
	//check to see if this is the largest distance we've calculated so far
	if( sqrBoundTemp > sqrBound )
		sqrBound= sqrBoundTemp;

	//check the triangle's bounding sphere against the view frustum
	if( cull != CULL_ALLIN ) {
		float r;
		int j, m;
 
		//perform culling against the view frustum
		for( j= 0, m= 1; j < 6; j++, m<<=1 ) {
			if( !( cull & m ) ) {
				r= m_frustum[j][0]*newVert[0]+
				   m_frustum[j][1]*newVert[1]+
				   m_frustum[j][2]*newVert[2]+
				   m_frustum[j][3];

				//check for frustum inclusion
				if( r*r > sqrBound ) {

					//check to see if the triangle is actually within the viewing frustum
					if( r < 0.0f )
						return;
					
					//triangle is within view
					cull|= m;
				}
			}
		} 
	}

	//distance calculation
	dist= SQR( ( newVert[0] - m_pCamera->m_eyePos[0] ) )+
		  SQR( ( newVert[1] - m_pCamera->m_eyePos[1] ) )+
		  SQR( ( newVert[2] - m_pCamera->m_eyePos[2] ) );

	if( level < m_maxLevel && SQR( m_pMDLevelSize[level] ) > dist*0.000001f ) {
		//render the children
		RenderChild( level+1, pV1, newVert, pV2, cull );
		RenderChild( level+1, pV2, newVert, pV3, cull );
        
		//the current node doesn't need to be rendered,
		//since both of its children are
		return;
	}

	//send the vertices to the rendering API
	glTexCoord2f( pV1[0]/m_size, pV1[2]/m_size ); glVertex3fv( pV1 );
	glTexCoord2f( pV2[0]/m_size, pV2[2]/m_size ); glVertex3fv( pV2 );
	glTexCoord2f( pV3[0]/m_size, pV3[2]/m_size ); glVertex3fv( pV3 );

	m_vertsPerFrame+= 3;
	m_trisPerFrame++;
}
