//==============================================================
//= ROAM.cpp
//= Original coders: Trent Polack (trent@codershq.com) and
//=				     Mark Duchaineau
//==============================================================
//= The header file for the ROAM 2.0 implementation
//==============================================================
#include <SDL_opengl.h>
#include <stdio.h>
#include <math.h>

#include "ROAM.h"

//initialize the ROAM engine
void Terrain::ROAM::Init( int maxLevel, int poolSize, DemoFramework::Camera *pCamera )
{
	ROAMDiamond *pPrevDmnd, *pNextDmnd;
	ROAMDiamond *pDmnd;
	int level, i, j, c, di, dj, ix, jx;

	m_maxLevel    = maxLevel;
	m_pMDLevelSize= new float [m_maxLevel+1];

	//generate table of displacement sizes versus levels
	for( level=0; level <= m_maxLevel; level++ )
		m_pMDLevelSize[level]= 255/( ( float )sqrt( ( float )( ( ROAMint64 )1<<level ) ) );

	//create diamond store, free list
	//allocate diamond and hash table arrays
	m_poolSize = poolSize;
	m_pDmndPool= new ROAMDiamond [m_poolSize];

	//start all diamonds on free list
	for( i= 0; ( i + 1 ) < m_poolSize; i++ ) {
		pPrevDmnd= m_pDmndPool + i;
		pNextDmnd= m_pDmndPool + ( i + 1 );

		pPrevDmnd->m_pNextDmnd= pNextDmnd;
		pNextDmnd->m_pPrevDmnd= pPrevDmnd;
	}

	m_pFreeDmnd[0]= m_pDmndPool;
	m_pFreeDmnd[1]= m_pDmndPool + ( m_poolSize - 1 );

	m_pFreeDmnd[0]->m_pPrevDmnd= ( ROAMDiamond * )0;
	m_pFreeDmnd[1]->m_pNextDmnd= ( ROAMDiamond * )0;

	//set diamonds initially to be NEW and FREE
	for( i= 0; i < m_poolSize; i++ ) {
		pPrevDmnd= m_pDmndPool+i;
		
		pPrevDmnd->m_bound= -1; //indicates NEW
		pPrevDmnd->m_lockCount= 0;
	}

	//allocate all base diamonds, setting everything but the links
	for( c= 0; c < 25; c++ )
	{
		if( c < 9 ) {
			j= c/3;
			i= c%3;

			m_pLevel0Dmnd[j][i]= pDmnd= Create( );
			pDmnd->m_vert[0]= ( 2.0f*( float )( i - 1 ) );
			pDmnd->m_vert[2]= ( 2.0f*( float )( j - 1 ) );
		} else {
			j= ( c - 9 )/4;
			i= ( c - 9 )%4;

			m_pLevel1Dmnd[j][i]= pDmnd= Create( );
			pDmnd->m_vert[0]= ( 2.0f*( float )i - 3.0f );
			pDmnd->m_vert[2]= ( 2.0f*( float )j - 3.0f );
		}
		//shift the previous coords from [-3, 3] to [0, m_size-1]
		ShiftCoords( &pDmnd->m_vert[0], &pDmnd->m_vert[2] );

		//clamp the coordinates to the map size (prevent a buffer overflow)
		CLAMP( pDmnd->m_vert[0], 0, ( m_size - 1 ) );
		CLAMP( pDmnd->m_vert[2], 0, ( m_size - 1 ) );

		pDmnd->m_vert[1]= ( float )m_heightMap.GetData( ( int )( fabs( pDmnd->m_vert[0] ) ),
														  ( int )( fabs( pDmnd->m_vert[2] ) ) );

		pDmnd->m_level= ( c < 9 ? 0 : ( ( ( i^j ) & 1 ) ? -1 : -2 ) );

		pDmnd->m_bound= ( float )SQR( m_size );
		pDmnd->m_error= ( float )m_size;

		pDmnd->m_pParent[0]= pDmnd->m_pParent[1]= pDmnd->m_pParent[2]= pDmnd->m_pParent[3]= ( ROAMDiamond * )0;
		pDmnd->m_pChild[0] = pDmnd->m_pChild[1] = pDmnd->m_pChild[2] = pDmnd->m_pChild[3] = ( ROAMDiamond * )0;
	}

	//now that they all exist, set the links for level 0 diamond linkage
	for( c= 0; c < 9; c++ ) {
		j= c/3;
		i= c%3;

		pDmnd= m_pLevel0Dmnd[j][i];
		di= ( ( ( i^j ) & 1 ) ? 1 : -1 );
		dj= 1;

		ix= ( 2*i + 1 - di )>>1;
		jx= ( 2*j + 1 - dj )>>1;
		pDmnd->m_pParent[0]= m_pLevel1Dmnd[jx][ix];

		ix= ( 2*i + 1 + di )>>1;
		jx= ( 2*j + 1 + dj )>>1;
		pDmnd->m_pParent[1]= m_pLevel1Dmnd[jx][ix];

		ix= ( 2*i + 1 - dj )>>1;
		jx= ( 2*j + 1 + di )>>1;
		pDmnd->m_pParent[2]= m_pLevel1Dmnd[jx][ix];

		ix= ( 2*i + 1 + dj )>>1;
		jx= ( 2*j + 1 - di )>>1;
		pDmnd->m_pParent[3]= m_pLevel1Dmnd[jx][ix];

		ix= ( di < 0 ? 0 : 3 );
		pDmnd->m_pParent[0]->m_pChild[ix]= pDmnd;
		pDmnd->m_childIndex[0]= ix;

		ix= ( di < 0 ? 2 : 1 );
		pDmnd->m_pParent[1]->m_pChild[ix]= pDmnd;
		pDmnd->m_childIndex[1]= ix;
	}

	//calculate the level 1 base-diamond linkage
	for( c= 0; c < 16; c++ ) {
		j= c/4;
		i= c%4;

		pDmnd= m_pLevel1Dmnd[j][i];
		if( j > 0 )
			pDmnd->m_pParent[3]= m_pLevel1Dmnd[j - 1][i];
		if( j < 3 )
			pDmnd->m_pParent[2]= m_pLevel1Dmnd[j + 1][i];
		if( i > 0 )
			pDmnd->m_pParent[0]= m_pLevel1Dmnd[j][i - 1];
		if( i < 3 )
			pDmnd->m_pParent[1]= m_pLevel1Dmnd[j][i + 1];
	}
}

//shutdown the ROAM engine
void Terrain::ROAM::Shutdown( void ) {
	delete[] m_pDmndPool;
	delete[] m_pMDLevelSize;
}

//the high-level rendering function to being ROAM rendering
void Terrain::ROAM::Render( void ) {
	//reset the debug counters
	m_vertsPerFrame= 0;
	m_trisPerFrame = 0;

	//turn on textured drawing (with the grid texture for now)
	glBindTexture( GL_TEXTURE_2D, m_texture.GetID( ) );
	glEnable( GL_TEXTURE_2D );

	glColor3f( 1.0f, 1.0f, 1.0f );

	//recurse on the two base triangles
	glBegin( GL_TRIANGLES );
		RenderChild( m_pLevel1Dmnd[2][1], 2, 0 );
		RenderChild( m_pLevel1Dmnd[1][2], 0, 0 );
	glEnd( );

	//end texturing
	glDisable( GL_TEXTURE_2D );
}

//recursive rendering function
void Terrain::ROAM::RenderChild( ROAMDiamond *pDmnd, int i, int cull ) {
    ROAMDiamond *centerDmnd;	//center, split-edge diamond
    float *dmndCenter;			//center vertex
	float dist;

	//get selected child create if not there, indicate recent use
	centerDmnd= GetChild( pDmnd, i );
	dmndCenter= centerDmnd->m_vert;

	//check the triangle's bounding sphere against the view frustum
	if( cull != CULL_ALLIN ) {
		float r, rad;
		int j, m;
 
		rad= centerDmnd->m_bound;

		//perform culling against the view frustum
		for( j= 0, m= 1; j < 6; j++, m<<= 1 ) {
			if( !( cull & m ) ) {
				r= m_frustum[j][0]*dmndCenter[0] +
				   m_frustum[j][1]*dmndCenter[1] +
				   m_frustum[j][2]*dmndCenter[2] +
				   m_frustum[j][3];

				//check for frustum inclusion
				if( r*r > rad ) {
					//check to see if the triangle is actually within the viewing frustum
					if( r < 0.0f ) {
						Unlock( centerDmnd );
						return;
					}
					
					//triangle is within view
					cull|= m;
				}
			}
		} 
	}

    //distance calculation
    dist= SQR( ( dmndCenter[0] - m_camX ) ) +
		  SQR( ( dmndCenter[1] - m_camY ) ) +
		  SQR( ( dmndCenter[2] - m_camZ ) );

	//recurse down to the next level if possible
	if( centerDmnd->m_level < m_maxLevel && centerDmnd->m_error > dist*0.000001f ) {
		if( centerDmnd->m_pParent[0] == pDmnd ) {
			RenderChild( centerDmnd, 0, cull );
			RenderChild( centerDmnd, 1, cull );
		} else {
			RenderChild( centerDmnd, 2, cull );
			RenderChild( centerDmnd, 3, cull );
		}
	} else {
		ROAMDiamond *pPrevDmnd, *pNextDmnd; //diamonds of other two triangle vertices

		//determine all tri's verts
		if( centerDmnd->m_pParent[0] == pDmnd ) {
			pPrevDmnd= centerDmnd->m_pParent[2];
			pNextDmnd= centerDmnd->m_pParent[3];
		} else {
			pPrevDmnd= centerDmnd->m_pParent[3];
			pNextDmnd= centerDmnd->m_pParent[2];
		}

		//draw the tri with simple per-tri texture the shows the grid
		glTexCoord2f( pPrevDmnd->m_vert[0]/m_size, pPrevDmnd->m_vert[2]/m_size ); glVertex3fv( pPrevDmnd->m_vert );
		glTexCoord2f( pDmnd->m_vert[0]/m_size,	   pDmnd->m_vert[2]/m_size );	  glVertex3fv( pDmnd->m_vert );
		glTexCoord2f( pNextDmnd->m_vert[0]/m_size, pNextDmnd->m_vert[2]/m_size ); glVertex3fv( pNextDmnd->m_vert );

		m_vertsPerFrame+= 3;
		m_trisPerFrame++;
	}

	//undo locking from GetChild()
	Unlock( centerDmnd );
}

//create room for a new diamond
Terrain::ROAMDiamond *Terrain::ROAM::Create( void ) {
	Terrain::ROAMDiamond *pDmnd;

	//use the most recent "freed" diamond
	pDmnd= m_pFreeDmnd[0];
	if( !pDmnd ) {
		printf( "Out of memory for ROAM child\n" );

		return ( ROAMDiamond * )0;
	}

	//if the diamond has been used before, delete it's previous parent links
	if( pDmnd->m_bound >= 0.0f ) {
		pDmnd->m_pParent[0]->m_pChild[pDmnd->m_childIndex[0]]= ( ROAMDiamond *)0;
		Unlock( pDmnd->m_pParent[0] );
		
		pDmnd->m_pParent[1]->m_pChild[pDmnd->m_childIndex[1]]= ( ROAMDiamond *)0;
		Unlock( pDmnd->m_pParent[1] );
	}

	//lock the new diamond
	Lock( pDmnd );
    return pDmnd;
}

//get and update a child diamond
Terrain::ROAMDiamond *Terrain::ROAM::GetChild( ROAMDiamond *pDmnd, int i ) {
	Terrain::ROAMDiamond *pChild, *px, *cx;
	float *pDmndCenter, *pParentVert0, *pParentVert1;
	float sqrBound, sqrBoundTemp, *tempVert;
	int ix;

	//we don't need to create the child if it already exists
	if( ( pDmnd->m_pChild[i] ) ) {
		Lock( pDmnd->m_pChild[i] );
		return pDmnd->m_pChild[i];
	}

	//lock center diamond early to avoid an automatic "diamond discharge"
	Lock( pDmnd );

	//allocate new kid
	pChild= Create( );

	//recursively create other parent to the current child
	if( i < 2 ) {
		px= pDmnd->m_pParent[0];
		
		ix= ( pDmnd->m_childIndex[0] + ( i == 0 ? 1 : -1 ) ) & 3;
	} else {
		px= pDmnd->m_pParent[1];

		ix= ( pDmnd->m_childIndex[1] + ( i == 2 ? 1 : -1 ) ) & 3;
	}

	//get the other child
	cx= GetChild( px, ix );

	//set all the links
	pDmnd->m_pChild[i]= pChild;
	ix= ( i & 1 )^1;
	if( cx->m_pParent[1] == px )
		ix|= 2;
		
	cx->m_pChild[ix]= pChild;
	if( i & 1 ) {
		pChild->m_pParent[0]   = cx;
		pChild->m_childIndex[0]= ix;

		pChild->m_pParent[1]   = pDmnd;
		pChild->m_childIndex[1]= i;
	} else {
		pChild->m_pParent[0]   = pDmnd;
		pChild->m_childIndex[0]= i;

		pChild->m_pParent[1]   = cx;
		pChild->m_childIndex[1]= ix;
	}

	pChild->m_pParent[2]= pDmnd->m_pParent[i>>1];
	pChild->m_pParent[3]= pDmnd->m_pParent[( ( ( i + 1 ) & 2 )>>1 ) + 2];

	pChild->m_pChild[0]= pChild->m_pChild[1]= pChild->m_pChild[2]= pChild->m_pChild[3]= ( ROAMDiamond* )0;

	//calculate the child's level and vertex information
	pChild->m_level= pDmnd->m_level + 1;

	pParentVert0= pChild->m_pParent[2]->m_vert;
	pParentVert1= pChild->m_pParent[3]->m_vert;
	pChild->m_vert[0]= ( float )fabs( ( pParentVert0[0] + pParentVert1[0] )/2.0f );
	pChild->m_vert[2]= ( float )fabs( ( pParentVert0[2] + pParentVert1[2] )/2.0f );

	CLAMP( 	pChild->m_vert[0], 0, ( m_size - 1 ) );
	CLAMP( 	pChild->m_vert[2], 0, ( m_size - 1 ) );

	pChild->m_vert[1]= m_heightMap.GetData( ( int )pChild->m_vert[0],
											  ( int )pChild->m_vert[2] );

	pDmndCenter= pChild->m_vert;

    //compute radius of diamond bounding sphere (squared)
	//calculate the bounding sphere for the current triangle
	//calculation 1
	tempVert= pChild->m_pParent[0]->m_vert;
	sqrBound= SQR( ( tempVert[0] - pDmndCenter[0] ) ) +
			  SQR( ( tempVert[1] - pDmndCenter[1] ) ) +
			  SQR( ( tempVert[2] - pDmndCenter[2] ) );

	tempVert= pChild->m_pParent[1]->m_vert;
	//calculation 2
	sqrBoundTemp= SQR( ( tempVert[0] - pDmndCenter[0] ) ) +
				  SQR( ( tempVert[1] - pDmndCenter[1] ) ) +
				  SQR( ( tempVert[2] - pDmndCenter[2] ) );
	
	//check to see if this is the largest distance we've calculated so far
	if( sqrBoundTemp > sqrBound )
		sqrBound= sqrBoundTemp;

	//calculation 3
	tempVert= pChild->m_pParent[2]->m_vert;
	sqrBoundTemp= SQR( ( tempVert[0] - pDmndCenter[0] ) ) +
				  SQR( ( tempVert[1] - pDmndCenter[1] ) ) +
				  SQR( ( tempVert[2] - pDmndCenter[2] ) );
	
	//check to see if this is the largest distance we've calculated so far
	if( sqrBoundTemp > sqrBound )
		sqrBound= sqrBoundTemp;

	//calculation 4
	tempVert= pChild->m_pParent[3]->m_vert;
	sqrBoundTemp= SQR( ( tempVert[0] - pDmndCenter[0] ) ) +
				  SQR( ( tempVert[1] - pDmndCenter[1] ) ) +
				  SQR( ( tempVert[2] - pDmndCenter[2] ) );
	
	//check to see if this is the largest distance we've calculated so far
	if( sqrBoundTemp > sqrBound )
		sqrBound= sqrBoundTemp;

	pChild->m_bound= sqrBound;
	pChild->m_error= SQR( m_pMDLevelSize[pChild->m_level] );

    return pChild;
}

//lock the child to prevent a "diamond discharge"
void Terrain::ROAM::Lock( ROAMDiamond *pDmnd ) {
    Terrain::ROAMDiamond *pPrevDmnd, *pNextDmnd;

    //remove from free list if first reference
    if( pDmnd->m_lockCount == 0 ) {
		pPrevDmnd= pDmnd->m_pPrevDmnd;
		pNextDmnd= pDmnd->m_pNextDmnd;

		if( pPrevDmnd )
			pPrevDmnd->m_pNextDmnd= pNextDmnd;
		else
			m_pFreeDmnd[0]= pNextDmnd;

		if( pNextDmnd )
			pNextDmnd->m_pPrevDmnd= pPrevDmnd;
		else
			m_pFreeDmnd[1]= pPrevDmnd;
	}
	
	pDmnd->m_lockCount++;
}

//unlock the child so it may be used by others
void Terrain::ROAM::Unlock( ROAMDiamond *pDmnd ) {
	Terrain::ROAMDiamond *pPrevDmnd;

	pDmnd->m_lockCount--;

	//add to free list if no references left
	if( pDmnd->m_lockCount == 0 ) {
		pPrevDmnd= m_pFreeDmnd[1];

		pDmnd->m_pPrevDmnd= pPrevDmnd;
		pDmnd->m_pNextDmnd= ( Terrain::ROAMDiamond * )0;

		if( pPrevDmnd )
			pPrevDmnd->m_pNextDmnd= pDmnd;
		else
			m_pFreeDmnd[0]= pDmnd;

		m_pFreeDmnd[1]= pDmnd;
	}
}