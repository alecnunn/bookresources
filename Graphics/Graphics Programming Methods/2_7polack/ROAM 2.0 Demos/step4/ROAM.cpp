//==============================================================
//= ROAM.cpp
//= Original coders: Trent Polack (trent@voxelsoft.com) and
//=				     Mark Duchaineau
//==============================================================
//= The header file for the ROAM implementation
//==============================================================
#include <SDL_opengl.h>
#include <cmath>

#include "..\\Framework\\Log.h"
#include "..\\Framework\\GLExtensions.h"

#include "ROAM.h"

//initialize the ROAM system
void Terrain::ROAM::Init( int maxLevel, int poolSize, DemoFramework::Camera* pCamera ) {
	Terrain::ROAMDiamond* pDmnd, *pTempDmnd0, *pTempDmnd1;
	float f;
	int iLevel, j, di, dj, ix, jx, *pInt;

	//initialize the ROAM engine
    m_qCoarse= 1990;			//1990 happened to be the "magic number" for the queue "fineness"
    m_frameCount = 0;

	m_maxLevel= maxLevel;
	m_pLevelSize= new float [m_maxLevel+1];

	//generate table of displacement sizes versus levels
	for( iLevel=0; iLevel <= m_maxLevel; iLevel++ )
		m_pLevelSize[iLevel]= 1024.0f/( ( float )sqrt( ( float )( ( __int64 )1 << iLevel ) ) );

	//create diamond store, free list
	//allocate diamond and hash table arrays
	m_poolSize = poolSize;
	m_pDmndPool= new Terrain::ROAMDiamond [m_poolSize];

	m_maxTriChunks= TRI_IMAX;
	m_PDmndIS= new int [m_maxTriChunks];

	//allocate memory for the vertex/texture coordinates
	m_buffer= new float[m_maxTriChunks*15];

	//start all diamonds on the free list
	for( int i= 0; i + 1 < m_poolSize; i++ ) {
		pTempDmnd0= m_pDmndPool + i;
		pTempDmnd1= m_pDmndPool + ( i + 1 );

		pTempDmnd0->m_pNextDmnd= pTempDmnd1;
		pTempDmnd1->m_pPrevDmnd= pTempDmnd0;
	}
	m_pFreeDmnd[0]= m_pDmndPool;
	m_pFreeDmnd[1]= m_pDmndPool + ( m_poolSize - 1 );
	m_pFreeDmnd[0]->m_pPrevDmnd= ( Terrain::ROAMDiamond* )0;
	m_pFreeDmnd[1]->m_pNextDmnd= ( Terrain::ROAMDiamond* )0;
	m_freeElements= m_poolSize;

	//initialize diamonds to be NEW and FREE
	for( i= 0; i < m_poolSize; i++ ) {
		pTempDmnd0= m_pDmndPool + i;

		pTempDmnd0->m_fBoundRad= -1;		//indicated a NEW diamond
		pTempDmnd0->m_ucLockCount= 0;
		pTempDmnd0->m_ucFlags= 0;
		pTempDmnd0->m_ucFrameCount= 255;

		pTempDmnd0->m_pParent[2]= pTempDmnd0->m_pParent[3]= ( Terrain::ROAMDiamond* )0;
		
		pTempDmnd0->m_ucCull= 0;
		pTempDmnd0->m_cChildIndex[0]= pTempDmnd0->m_cChildIndex[1]= 0;
		pTempDmnd0->m_sQueueIndex= IQMAX/2;
			
		pTempDmnd0->m_pChild[0]= pTempDmnd0->m_pChild[1]= pTempDmnd0->m_pChild[2]= pTempDmnd0->m_pChild[3]= ( Terrain::ROAMDiamond* )0;
			
		pTempDmnd0->m_cLevel= -100;
			
		pTempDmnd0->m_pParent[0]= pTempDmnd0->m_pParent[1]= ( Terrain::ROAMDiamond* )0;
			
		pTempDmnd0->m_fErrorRad= 10.0f;
	}

	//clear the split/merge priority queues
	for( i= 0; i < IQMAX; i++ )	m_pSplitQueue[i]= m_pMergeQueue[i]= ( Terrain::ROAMDiamond* )0;
	m_PQMax= -1;
	m_PQMin= IQMAX;

	//clear the triangle render-list
	for( i= 0; i < m_maxTriChunks; i++ )	m_PDmndIS[i]= -1;
	m_freeTri= 1;
	m_trisPerFrame= 0;
	m_freeTriCount= m_maxTriChunks - 1;

	//generate a LUT for float->int conversations (huge speed-saver later on)
	pInt= ( int * )( &f );
	for( i= 0; i < 256; i++ ) {
		*pInt= 0x3f800000 + ( i << 15 );
		m_log2Table[i]= ( int )floor( m_size*( log( f )/log( 2.0 ) - ( float )i/256.0 ) + 0.5f )<<12;
	}

	//allocate base diamonds, and set their information (minus link info)
	for( int k= 0; k < 32; k++ ) {
		if( k < 16 ) {
			//initialization code for dimaonds 0-15
			j= k/4;
			i= k%4;
				
			m_pLevel0Dmnd[j][i]= pDmnd= Create( );
			pDmnd->m_fVert[0]= ( 2.0f*( float )( i - 1 ) );
			pDmnd->m_fVert[2]= ( 2.0f*( float )( j - 1 ) );
		} else {
			//initialization for diamonds 15-31
			j= ( k - 16 )/4;
			i= ( k - 16 )%4;
				
			m_pLevel1Dmnd[j][i]= pDmnd= Create( );
			pDmnd->m_fVert[0]= ( 2.0f*( float )i - 3.0f );
			pDmnd->m_fVert[2]= ( 2.0f*( float )j - 3.0f );
		}

		//shift the previous coords from [-3, 3] to [0, m_size-1]
		ShiftCoords( &pDmnd->m_fVert[0], &pDmnd->m_fVert[2] );

		//clamp the coordinates to the map m_size (prevent a buffer overflow)
		CLAMP( pDmnd->m_fVert[0], 0, ( m_size-1 ) );
		CLAMP( pDmnd->m_fVert[2], 0, ( m_size-1 ) );

		pDmnd->m_fVert[1]= ( float )m_heightMap.GetData( ( int )( fabs( pDmnd->m_fVert[0] ) ),
														 ( int )( fabs( pDmnd->m_fVert[2] ) ) );
		pDmnd->m_usTriIndex[0]= pDmnd->m_usTriIndex[1]= 0;

		pDmnd->m_fBoundRad= ( float )SQR( m_size );
		pDmnd->m_fErrorRad= ( float )m_size;

		//reset all links
		pDmnd->m_pParent[0]= pDmnd->m_pParent[1]= pDmnd->m_pParent[2]= pDmnd->m_pParent[3]= ( Terrain::ROAMDiamond* )0;
		pDmnd->m_pChild[0] = pDmnd->m_pChild[1] = pDmnd->m_pChild[2] = pDmnd->m_pChild[3] = ( Terrain::ROAMDiamond* )0;

		pDmnd->m_cLevel= ( k < 16 ? 0 : ( ( ( i^j ) & 1 ) ? -1 : -2 ) );
		pDmnd->m_ucCull = 0;
		pDmnd->m_ucFlags= 0;
		pDmnd->m_ucSplitFlags= 0;
		pDmnd->m_sQueueIndex= IQMAX - 1;

		if( k < 16 && k != 5 )	pDmnd->m_ucFlags|= ROAM_CLIPPED;
		if( pDmnd->m_cLevel < 0 )	pDmnd->m_ucFlags|= ROAM_SPLIT;
	}

	//now that all of the diamonds exist, we need to set their links
	for( k= 0; k < 16; k++ ) {
		j= k/4;
		i= k%4;
		
		//links for the iLevel 0 diamonds
		pDmnd=m_pLevel0Dmnd[j][i];
		di= ( ( ( i^j ) & 1 ) ? 1 : -1 );
		dj= 1;
		ix= ( ( 2*i + 1 - di )>>1 )%4;
		jx= ( ( 2*j + 1 - dj )>>1 )%4;
		pDmnd->m_pParent[0]= m_pLevel1Dmnd[jx][ix];
			
		ix= ( ( 2*i + 1 + di )>>1 )%4;
		jx= ( ( 2*j + 1 + dj )>>1 )%4;
		pDmnd->m_pParent[1]= m_pLevel1Dmnd[jx][ix];
			
		ix= ( ( 2*i + 1 - dj )>>1 )%4;
		jx= ( ( 2*j + 1 + di )>>1 )%4;
		pDmnd->m_pParent[2]= m_pLevel1Dmnd[jx][ix];
			
		ix= ( ( 2*i + 1 + dj )>>1 )%4;
		jx= ( ( 2*j + 1 - di )>>1 )%4;
		pDmnd->m_pParent[3]= m_pLevel1Dmnd[jx][ix];
			
		ix= ( di < 0 ? 0 : 3 );
		pDmnd->m_pParent[0]->m_pChild[ix]= pDmnd;
		pDmnd->m_cChildIndex[0]= ix;
			
		ix= ( di < 0 ? 2 : 1 );
		pDmnd->m_pParent[1]->m_pChild[ix]= pDmnd;
		pDmnd->m_cChildIndex[1]= ix;
	}

	for( k= 0; k < 16; k++ ) {
		j= k/4;
		i= k%4;
		
		//links for the iLevel -1 diamonds
		pDmnd=m_pLevel1Dmnd[j][i];
		pDmnd->m_pParent[3]= m_pLevel1Dmnd[( j+3 )%4][i];
		pDmnd->m_pParent[2]= m_pLevel1Dmnd[( j+1 )%4][i];
		pDmnd->m_pParent[0]= m_pLevel1Dmnd[j][( i+3 )%4];
		pDmnd->m_pParent[1]= m_pLevel1Dmnd[j][( i+1 )%4];
	}

	//put the top-iLevel diamond on the split queue (and we can work from there
	pDmnd= m_pLevel0Dmnd[1][1];
	Enqueue( pDmnd, ROAM_SPLITQ, IQMAX-1 );

	//get the base tris
	AllocateTri( pDmnd, 0 );
	AllocateTri( pDmnd, 1 );
}

//shutdown the engine
void Terrain::ROAM::Shutdown( void ) {
	delete[] m_buffer;

	delete[] m_PDmndIS;
	delete[] m_pDmndPool;
	delete[] m_pLevelSize;
}

//update the engine's mesh
void Terrain::ROAM::Update( void ) {
 	static int i0= 0;
	Terrain::ROAMDiamond* pDmnd;
	int side, overlap, overlap1, optCount, maxOptCount, i1;

	//update all active diamonds with a recursive culling update
	pDmnd= m_pLevel0Dmnd[1][1];
	UpdateChildCull( pDmnd );
	for( int i= 0; i < 4; i++ )
		if( pDmnd->m_pChild[i] )	UpdateChildCull( pDmnd->m_pChild[i] );

	//update all queued diamonds' priority
	i1= i0 + ( m_poolSize + 9 )/10;

	if( i1 >= m_poolSize )	i1= m_poolSize - 1;

	for( i= i0; i <= i1; i++ ) {
	    pDmnd= m_pDmndPool+i;
	    if( pDmnd->m_ucFlags & ROAM_ALLQ )	UpdatePriority( pDmnd );
	}

	i0= ( i1 + 1 )%m_poolSize; 

	// keep splitting/merging until either
	// (a) no split/merge priority overlap and:
	//     target tri count reached or accuracy target reached
	// (b) time is up (limit optimization-loop iterations)
	// (c) not enough free (unlocked) diamonds in cache
	maxOptCount= 2000;	//split/merge limit
	optCount   = 0;
		
	//check to see if the mesh is too coarse
	if( ( m_trisPerFrame <= m_maxTris && m_PQMax >= m_qCoarse &&
		  m_freeElements > 128		   && m_freeTriCount > 128 ) )
		side= -1;
	else
		side= 1;
		
	overlap= overlap1= m_PQMax - m_PQMin;

	//loop through performing split/merge ops
	while( ( side != 0 || overlap1 > 1 ) && optCount < maxOptCount ) {
		if( side <= 0 ) {
			if( m_PQMax > 0 ) {
				Split( m_pSplitQueue[m_PQMax] );

				//check to see if the mesh is too coarse
				if( !( m_trisPerFrame<=m_maxTris && m_PQMax>=m_qCoarse &&
					   m_freeElements>128	       && m_freeTriCount>128 ) )
					side= 1;
			}
			else	side= 0;
		} else {
			Merge( m_pMergeQueue[m_PQMin] );

			//check to see if the mesh is too coarse
			if( ( m_trisPerFrame <= m_maxTris && m_PQMax >= m_qCoarse &&
				  m_freeElements > 128		   && m_freeTriCount > 128 ) )
				side= 0;
		}

		overlap= m_PQMax - m_PQMin;
		if( overlap < overlap1 )	overlap1= overlap;

		optCount++;
	}

	m_frameCount= ( m_frameCount + 1 ) & 255;
}

//render the engine
void Terrain::ROAM::Render( void ) {
	float *buffer;

	buffer= m_buffer + 15;

	//bind the primary color texture to the first texture unit
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, m_textureMap.GetID( ) );
	glTexCoordPointer( 2, GL_FLOAT, 20, buffer );

	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 3, GL_FLOAT, 20, buffer + 2 );

	if( GLExtensions::canCVA )	GLExtensions::glLockArraysEXT( 0, m_maxTriChunks*18 );
	
	glDrawArrays( GL_TRIANGLES, 0, 3*( m_freeTri - 1 ) );

	if( GLExtensions::canCVA )	GLExtensions::glUnlockArraysEXT(  );

	glDisableClientState( GL_VERTEX_ARRAY );

	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisable( GL_TEXTURE_2D );
}

//allocate room in the vertex array for a new triangle
void Terrain::ROAM::AllocateTri( ROAMDiamond* pDmnd, int j )
{
    int iFlags;

	//clipped diamonds never have triangles
	if( pDmnd->m_ucFlags & ROAM_CLIPPED )
		return;

	iFlags= pDmnd->m_pParent[j]->m_ucFlags;

	//CLIPPED parent j means no triangle on side j
	if( iFlags & ROAM_CLIPPED )	return;

	//indicate that the triangle on side j is active
	pDmnd->m_ucFlags|= ROAM_TRI0<<j;

	//if not IN, take the triangle off of the OUT list
	if( !( pDmnd->m_ucCull & CULL_OUT ) )	AddTri( pDmnd, j );
}

//free room in the vertex array for a new triangle
void Terrain::ROAM::FreeTri( ROAMDiamond* pDmnd, int j )
{
	int iFlags;

	//CLIPPED diamonds never have triangles
	if( pDmnd->m_ucFlags & ROAM_CLIPPED )
		return;

	iFlags= pDmnd->m_pParent[j]->m_ucFlags;

	//CLIPPED parent j means no triangle on side j
	if( iFlags & ROAM_CLIPPED )
		return;

	//indicate that the triangle on side j is not active
	pDmnd->m_ucFlags&= ~( ROAM_TRI0<<j );

	//if not OUT, take the triangle off of the IN list
	if( !( pDmnd->m_ucCull & CULL_OUT ) )	RemoveTri( pDmnd, j );
}

//add a triangle to the vertex array
void Terrain::ROAM::AddTri( ROAMDiamond* pDmnd, int j )
{
	Terrain::ROAMDiamond *pDmndTable[3];
	float *buffer;
	int i;

	/* grab free tri and fill in */
	//get a free triangle and "fill" it in 
	i= m_freeTri++;
	if( i >= m_maxTriChunks ) {
		g_log.Write( DemoFramework::LOG_FAILURE,"ROAM ERROR: out of triangle memory" );
		return;
	}
	m_freeTriCount--;
	pDmnd->m_usTriIndex[j]= i;
	m_PDmndIS[i]= ( ( pDmnd-m_pDmndPool ) << 1 ) | j;

	//fill in the information for the triangle
	pDmndTable[1]= pDmnd->m_pParent[j];
	if( j ) {
		pDmndTable[0]= pDmnd->m_pParent[3];
		pDmndTable[2]= pDmnd->m_pParent[2];
	} else {
		pDmndTable[0]= pDmnd->m_pParent[2];
		pDmndTable[2]= pDmnd->m_pParent[3];
	}

	//fill the vertex buffer with the information
	buffer= ( float * )m_buffer + 15*i;
	for( int vi= 0; vi < 3; vi++, buffer+= 5 ) {
		buffer[2]= pDmndTable[vi]->m_fVert[0];
		buffer[3]= pDmndTable[vi]->m_fVert[1];
		buffer[4]= pDmndTable[vi]->m_fVert[2];

		buffer[0]= buffer[2]/m_size;
		buffer[1]= buffer[4]/m_size;
	}

	m_vertsPerFrame+= 3;
	m_trisPerFrame++;
}

//remove a triangle from the vertex array
void Terrain::ROAM::RemoveTri( ROAMDiamond* pDmnd, int j ) {
	Terrain::ROAMDiamond *pDmndX;
	float *buffer, *fpVBIndex;
	int iDmndIS, ix, jx, i;

	i= pDmnd->m_usTriIndex[j];

	//put the triangle back on the "free" list for use
	pDmnd->m_usTriIndex[j]=0;
	m_freeTri--;
	m_freeTriCount++;

	//copy the last triangle on the list (non-free) to the freed triangle
	ix  = m_freeTri;
	iDmndIS= m_PDmndIS[ix];
	jx  = iDmndIS & 1;
	pDmndX = m_pDmndPool+( iDmndIS>>1 );
		
	pDmndX->m_usTriIndex[jx]= i;
	m_PDmndIS[i]= iDmndIS;
		
	buffer= ( float * )m_buffer;
	fpVBIndex= buffer + 15*ix;
	buffer+= 15*i;
	memcpy( ( void* )buffer, ( void* )fpVBIndex, 15*sizeof( float ) );

	m_vertsPerFrame-= 3;
	m_trisPerFrame--;
}

//create a new ROAM diamond
Terrain::ROAMDiamond *Terrain::ROAM::Create( void ) {
	Terrain::ROAMDiamond *pDmnd;

	//recycle the least recently used diamond
	pDmnd= m_pFreeDmnd[0];
	if( !pDmnd ) {
		g_log.Write( DemoFramework::LOG_FAILURE, "Out of ROAM diamond storage" );
		return NULL;
	}

	//if the diamond is not NEW, reset its links
	if( pDmnd->m_fBoundRad >= 0.0f ) {
		pDmnd->m_pParent[0]->m_pChild[pDmnd->m_cChildIndex[0]]= ( Terrain::ROAMDiamond * )0;
		Unlock( pDmnd->m_pParent[0] );
		
		pDmnd->m_pParent[1]->m_pChild[pDmnd->m_cChildIndex[1]]= ( Terrain::ROAMDiamond * )0;
		Unlock( pDmnd->m_pParent[1] );
		pDmnd->m_sQueueIndex= IQMAX >> 1;
	}
	else	pDmnd->m_fBoundRad= 0.0f;	//set diamond to NEW

	//make sure that the frame count is old (so we can perform updates and such)
	pDmnd->m_ucFrameCount= ( m_frameCount - 1 ) & 255;

	//lock the gathered diamond and return it for use
	Lock( pDmnd );
	return pDmnd;
}

//get a new child and set it's information
Terrain::ROAMDiamond* Terrain::ROAM::GetChild( ROAMDiamond *pDmnd, int i ) {
 	Terrain::ROAMDiamond *k, *pParentX, *pChildX;
	float *pDmndCenter, *pParentVert0, *pParentVert1;
	float fSqrBound, fSqrBoundTemp, *fTempVert;
	int ix;

	//if the diamond is already alive, return it
	if( ( k= pDmnd->m_pChild[i] ) ) {
		Lock( k );
		return k;
	}

	//lock the center diamond to prevent early recycling
	Lock( pDmnd );

	//recursively create other parent for the child (i)
	if( i < 2 ) {
		pParentX= pDmnd->m_pParent[0];
		ix= ( pDmnd->m_cChildIndex[0] + ( i==0 ? 1 : -1 ) ) & 3;
	} else {
		pParentX= pDmnd->m_pParent[1];
		ix= ( pDmnd->m_cChildIndex[1] + ( i==2 ? 1 : -1 ) ) & 3;
	}

	pChildX= GetChild( pParentX, ix ); //lock other child parent

	//create a new child, and lock it
	k= Create( );

	//set all of the child's links
	pDmnd->m_pChild[i]= k;
	ix= ( i & 1 )^1;
	if( pChildX->m_pParent[1] == pParentX )	ix|= 2;

	pChildX->m_pChild[ix]= k;
	if( i & 1 ) {
		k->m_pParent[0]    = pChildX;
		k->m_cChildIndex[0]= ix;
		k->m_pParent[1]    = pDmnd;
		k->m_cChildIndex[1]= i;
	} else {
		k->m_pParent[0]    = pDmnd;
		k->m_cChildIndex[0]= i;
		k->m_pParent[1]    = pChildX;
		k->m_cChildIndex[1]= ix;
	}
	k->m_pParent[2]= pDmnd->m_pParent[i >> 1];
	k->m_pParent[3]= pDmnd->m_pParent[( ( ( i + 1 ) & 2 ) >> 1 ) + 2];
	k->m_pChild[0]= k->m_pChild[1]= k->m_pChild[2]= k->m_pChild[3]= ( Terrain::ROAMDiamond * )0;

	//set child information (iLevel, vertex info, flags, etc.)
	k->m_ucCull= 0;
	k->m_ucFlags= 0;
	k->m_ucSplitFlags= 0;
	if( ( k->m_pParent[2]->m_ucFlags & ROAM_CLIPPED ) ||
		( ( pDmnd->m_ucFlags & ROAM_CLIPPED ) && ( pChildX->m_ucFlags & ROAM_CLIPPED ) ) )
		k->m_ucFlags|= ROAM_CLIPPED;
	k->m_usTriIndex[0]= k->m_usTriIndex[1]= 0;
	k->m_sQueueIndex= -10;
	k->m_cLevel= pDmnd->m_cLevel + 1;

	pDmndCenter= k->m_fVert;

	pParentVert0= k->m_pParent[2]->m_fVert;
	pParentVert1= k->m_pParent[3]->m_fVert;
	k->m_fVert[0]= ( float )fabs( ( pParentVert0[0] + pParentVert1[0] )/2.0f )*m_scale[0];
	k->m_fVert[2]= ( float )fabs( ( pParentVert0[2] + pParentVert1[2] )/2.0f )*m_scale[2];
	k->m_fVert[1]= ( float )m_heightMap.GetData( ( int )k->m_fVert[0],
												 ( int )k->m_fVert[2] )*m_scale[1];

    //compute radius of diamond bounding sphere (squared)
	//calculate the bounding sphere for the current triangle
	//calculation 1
	fTempVert= k->m_pParent[0]->m_fVert;
	fSqrBound= SQR( ( fTempVert[0] - pDmndCenter[0] ) ) +
	 		   SQR( ( fTempVert[1] - pDmndCenter[1] ) ) +
			   SQR( ( fTempVert[2] - pDmndCenter[2] ) );

	fTempVert= k->m_pParent[1]->m_fVert;
	//calculation 2
	fSqrBoundTemp= SQR( ( fTempVert[0] - pDmndCenter[0] ) ) +
				   SQR( ( fTempVert[1] - pDmndCenter[1] ) ) +
				   SQR( ( fTempVert[2] - pDmndCenter[2] ) );
	
	//check to see if this is the largest distance we've calculated so far
	if( fSqrBoundTemp>fSqrBound )
		fSqrBound= fSqrBoundTemp;

	//calculation 3
	fTempVert= k->m_pParent[2]->m_fVert;
	fSqrBoundTemp= SQR( ( fTempVert[0] - pDmndCenter[0] ) ) +
				   SQR( ( fTempVert[1] - pDmndCenter[1] ) ) +
				   SQR( ( fTempVert[2] - pDmndCenter[2] ) );
	
	//check to see if this is the largest distance we've calculated so far
	if( fSqrBoundTemp>fSqrBound )
		fSqrBound= fSqrBoundTemp;

	//calculation 4
	fTempVert= k->m_pParent[3]->m_fVert;
	fSqrBoundTemp= SQR( ( fTempVert[0] - pDmndCenter[0] ) ) +
				   SQR( ( fTempVert[1] - pDmndCenter[1] ) ) +
				   SQR( ( fTempVert[2] - pDmndCenter[2] ) );
	
	//check to see if this is the largest distance we've calculated so far
	if( fSqrBoundTemp>fSqrBound )
		fSqrBound= fSqrBoundTemp;

	k->m_fBoundRad= fSqrBound;
	k->m_fErrorRad= SQR( m_pLevelSize[k->m_cLevel] );

    return k;
}

//lock the child to prevent "diamond discharge"
void Terrain::ROAM::Lock( ROAMDiamond *pDmnd ) {
    Terrain::ROAMDiamond *pPrevDmnd, *pNextDmnd;

    //remove from free list if first reference
	if( pDmnd->m_ucLockCount == 0 ) {
		pPrevDmnd= pDmnd->m_pPrevDmnd;
		pNextDmnd= pDmnd->m_pNextDmnd;

		if( pPrevDmnd )	pPrevDmnd->m_pNextDmnd= pNextDmnd;
		else	m_pFreeDmnd[0]= pNextDmnd;

		if( pNextDmnd )	pNextDmnd->m_pPrevDmnd= pPrevDmnd;
		else	m_pFreeDmnd[1]= pPrevDmnd;

		m_freeElements--;
	}
	
	pDmnd->m_ucLockCount++;
}

//unlock a child so it may be used by others
void Terrain::ROAM::Unlock( ROAMDiamond *pDmnd ) {
	Terrain::ROAMDiamond* pPrevDmnd;

	pDmnd->m_ucLockCount--;

	//add to free list if no references left
	if( pDmnd->m_ucLockCount == 0 ) {
		pPrevDmnd= m_pFreeDmnd[1];

		pDmnd->m_pPrevDmnd= pPrevDmnd;
		pDmnd->m_pNextDmnd= ( Terrain::ROAMDiamond * )0;

		if( pPrevDmnd )
			pPrevDmnd->m_pNextDmnd= pDmnd;
		else
			m_pFreeDmnd[0]= pDmnd;

		m_pFreeDmnd[1]= pDmnd;

		m_freeElements++;
	}
}

//update the child culling flags
void Terrain::ROAM::UpdateChildCull( ROAMDiamond *pDmnd ) {
    Terrain::ROAMDiamond* pChild;
    int cull, i;

	//CLIPPED diamonds have no interest here, back out
	if( pDmnd->m_ucFlags & ROAM_CLIPPED )
		return;

	cull= pDmnd->m_ucCull; //save old culling flag for comparison

	//update the diamond's culling flags
	UpdateCull( pDmnd );

	//skip subtree if nothing has really changed
	if( cull==pDmnd->m_ucCull && ( cull==CULL_OUT || cull==CULL_ALLIN ) )	return;

	//update diamond priority if culling OUT state has changed
	if( ( cull^pDmnd->m_ucCull ) & CULL_OUT )	UpdatePriority( pDmnd );

	//if diamond is split, recurse down to it's four children if they exist
	if( pDmnd->m_ucFlags & ROAM_SPLIT ) {
		for( i= 0; i < 4; i+= 2 ) {
			if( pChild= pDmnd->m_pChild[i] ) {
				if( pChild->m_pParent[0] == pDmnd ) {
					if( pChild->m_pChild[0] )	UpdateChildCull( pChild->m_pChild[0] );
					if( pChild->m_pChild[1] )	UpdateChildCull( pChild->m_pChild[1] );
				} else {
					if( pChild->m_pChild[2] )	UpdateChildCull( pChild->m_pChild[2] );
					if( pChild->m_pChild[3] )	UpdateChildCull( pChild->m_pChild[3] );
				}
			}
		}
	}
}

//update a diamond's culling flag
void Terrain::ROAM::UpdateCull( ROAMDiamond *pDmnd ) {
    float r;
    int cull;

	//get the diamond's parent's culling flag
	cull= pDmnd->m_pParent[2]->m_ucCull;

	//if needed, update for all non-IN halfspaces
	if( cull != CULL_ALLIN && cull != CULL_OUT ) {
		for( int j= 0, m= 1; j < 6; j++, m <<= 1 ) {
			if( !( cull & m ) ) {
				r= m_frustum[j][0]*pDmnd->m_fVert[0] +
				   m_frustum[j][1]*pDmnd->m_fVert[1] +
				   m_frustum[j][2]*pDmnd->m_fVert[2] +
				   m_frustum[j][3];

				//cull the diamond
				if( SQR( r ) > pDmnd->m_fBoundRad ) {
					if( r < 0.0f )	cull= CULL_OUT;
					else	cull|= m; //IN
				} //else still overlaps this frustum plane
			}
		} 
	}

    //if OUT state changes, update in/out listing on any draw tris
	if( ( pDmnd->m_ucCull^cull ) & CULL_OUT ) {
		for( int j= 0; j < 2; j++ ) {
			if( pDmnd->m_ucFlags & ( ROAM_TRI0<<j ) ) {
				if( cull & CULL_OUT )	RemoveTri( pDmnd, j );
				else	AddTri( pDmnd, j );
			}
		}
	}

	//store the updated cull flags
	pDmnd->m_ucCull= cull;
}

//update a diamond's priority in the split/merge queue
void Terrain::ROAM::UpdatePriority( ROAMDiamond *pDmnd ) {
	float d;
	int j, k, *pInt;

	//skip the update procedure if it has already been done
	if( m_frameCount == pDmnd->m_ucFrameCount )	return;
	pDmnd->m_ucFrameCount= m_frameCount;

	if( ( pDmnd->m_ucFlags & ROAM_CLIPPED ) || pDmnd->m_cLevel >= m_maxLevel )	k= 0;
	else {
		pInt= ( int * )( &d );		//set the local integer pointer (for the IEEE floating-point tricks)

		d= pDmnd->m_fErrorRad;

		//compute the fixed-point log_2 value (based on the error metric)
		k= *pInt;
		k+= m_log2Table[( k >> 15 ) & 0xff];
    
		//distance calculation
		d= SQR( ( pDmnd->m_fVert[0] - m_camX ) ) +
		   SQR( ( pDmnd->m_fVert[1] - m_camY ) ) +
		   SQR( ( pDmnd->m_fVert[2] - m_camZ ) );

		//compute the fixed-point log_2 value (based on the distance to the camera)
		j= *pInt;
		j+= m_log2Table[( j >> 15 ) & 0xff];

		//compute the fixed-point log_2(error/distance)
		k= ( k - j ) + 0x10000000;

		//scale and clamp the priority index to [1, IQMAX-1]
		if( k<0 )	k= 0;
			
		k= ( k >> 16 ) + 1;
		if( k >= IQMAX )	k= IQMAX - 1;

		//for OUT diamonds, reduce priority (but leave them ordered)
		if( pDmnd->m_ucCull & CULL_OUT ) {
			if( k>m_size )	k-= ( m_size >> 1 );
			else	k= ( k + 1 ) >> 1;
		}
	}

	//update the queue index
	Enqueue( pDmnd, pDmnd->m_ucFlags & ROAM_ALLQ, k );
}

//"re-queue" a diamond in the appropriate queue
void Terrain::ROAM::Enqueue( ROAMDiamond *pDmnd, int qFlags, int newPQIndex ) {
	Terrain::ROAMDiamond** pQueue, *pDmndX;
  	int i, dmndLock;

	//return early if the diamond is already in the queue
	if( ( pDmnd->m_ucFlags & ROAM_ALLQ ) == qFlags && pDmnd->m_sQueueIndex == newPQIndex )	return;

	//determine the overall change in the diamond lock count
	dmndLock= 0;
	if( pDmnd->m_ucFlags & ROAM_ALLQ )	dmndLock--;
	if( qFlags & ROAM_ALLQ )	dmndLock++;

	//remove the diamond from the old queue (if needed)
	if( pDmnd->m_ucFlags & ROAM_ALLQ ) {
		pQueue= ( ( pDmnd->m_ucFlags & ROAM_SPLITQ ) ? m_pSplitQueue : m_pMergeQueue );
		if( pDmnd->m_pPrevDmnd )	pDmnd->m_pPrevDmnd->m_pNextDmnd= pDmnd->m_pNextDmnd;
		else {
			pQueue[pDmnd->m_sQueueIndex]= pDmnd->m_pNextDmnd;
			if( !pDmnd->m_pNextDmnd ) {
				if( pDmnd->m_ucFlags & ROAM_SPLITQ ) {
					if( pDmnd->m_sQueueIndex == m_PQMax ) {
						pDmndX= pQueue[0];
						pQueue[0]= ( Terrain::ROAMDiamond * )1;

						//loop until we get 'i' where we want it
						for( i= pDmnd->m_sQueueIndex; !pQueue[i]; i-- )
							;
						if( !( pQueue[0]= pDmndX ) && i == 0 )	i--;
						m_PQMax= i;
					}
				} else {
					if( pDmnd->m_sQueueIndex == m_PQMin ) {
						pDmndX= pQueue[IQMAX - 1];
						pQueue[IQMAX - 1]= ( Terrain::ROAMDiamond * )1;

						//loop until we get 'i' where we want it
						for( i= pDmnd->m_sQueueIndex; !pQueue[i]; i++ )
							;
						if( !( pQueue[IQMAX - 1]= pDmndX ) && i == IQMAX - 1 )	i++;
						m_PQMin= i;
					}
				}
			}
		}
		if( pDmnd->m_pNextDmnd )	pDmnd->m_pNextDmnd->m_pPrevDmnd= pDmnd->m_pPrevDmnd;
		pDmnd->m_ucFlags&= ~ROAM_ALLQ;
	}

	//update the diamond's priority
	pDmnd->m_sQueueIndex= newPQIndex;

	//insert the diamond into the new queue if needed
	if( qFlags & ROAM_ALLQ ) {
		//insert into the necessary queue bucket (split or merge)
		pQueue= ( ( qFlags & ROAM_SPLITQ ) ? m_pSplitQueue : m_pMergeQueue );
		pDmnd->m_pPrevDmnd= ( Terrain::ROAMDiamond* )0;
		pDmnd->m_pNextDmnd= pQueue[pDmnd->m_sQueueIndex];

		pQueue[pDmnd->m_sQueueIndex]= pDmnd;
		if( pDmnd->m_pNextDmnd )	pDmnd->m_pNextDmnd->m_pPrevDmnd= pDmnd;
		else {
			if( qFlags & ROAM_SPLITQ ) {
				if( pDmnd->m_sQueueIndex>m_PQMax )	m_PQMax= pDmnd->m_sQueueIndex;
			}	else
				if( pDmnd->m_sQueueIndex<m_PQMin )	m_PQMin= pDmnd->m_sQueueIndex;
		}

		//specify which queue the current diamond is in
		pDmnd->m_ucFlags|= qFlags;
	}

	//perform any required locking/unlocking
	if( dmndLock != 0 ) {
		if( dmndLock < 0 ) Unlock( pDmnd );
		else	Lock( pDmnd );
	}
}

//split a diamond (to achieve a finer level of detail)
void Terrain::ROAM::Split( ROAMDiamond *pDmnd ) {
	Terrain::ROAMDiamond *pChild, *pParent;
	int i, s;

	//if the diamond has already been split, then skip it! And skip it good!
	if( pDmnd->m_ucFlags & ROAM_SPLIT )	return;

	//split parents recursively (as needed)
	for( i= 0; i < 2; i++ ) {
		pParent= pDmnd->m_pParent[i];
		Split( pParent );

		//if the diamond is pParent's first split child, take pParent off of the merge queue
		if( !( pParent->m_ucSplitFlags & SPLIT_K ) )	Enqueue( pParent, ROAM_UNQ, pParent->m_sQueueIndex );
		pParent->m_ucSplitFlags|= SPLIT_K0<<pDmnd->m_cChildIndex[i];
	}

	//get the children, update flags, and put on the split queue
	for( i= 0; i < 4; i++ ) {
		pChild= GetChild( pDmnd, i );
		UpdateCull( pChild );
		UpdatePriority( pChild );

		//children of the newly split diamond now go on the split queue
		Enqueue( pChild, ROAM_SPLITQ, pChild->m_sQueueIndex );
		s= ( pChild->m_pParent[1] == pDmnd ) ? 1 : 0;
		pChild->m_ucSplitFlags|= SPLIT_P0 << s;
		Unlock( pChild );

		//put the child triangles on the render list
		AllocateTri( pChild, s );
	}

	//diamond is split, update it's queue, and add to "check list"
	pDmnd->m_ucFlags|= ROAM_SPLIT;
	Enqueue( pDmnd, ROAM_MERGEQ, pDmnd->m_sQueueIndex );	//newly split diamond goes on merge queue

	//put parent tris back on the free list
	FreeTri( pDmnd, 0 );
	FreeTri( pDmnd, 1 );
}

//merge a diamond (to achieve a coarser level of detail)
void Terrain::ROAM::Merge( ROAMDiamond *pDmnd ) {
	Terrain::ROAMDiamond *k, *pParent;
	int i, s;

	//if this diamond has already been merged, then skip
	if( !( pDmnd->m_ucFlags & ROAM_SPLIT ) )	return;

	//children off split queue if their other parent is not split
	for( i= 0; i < 4; i++ ) {
		k= pDmnd->m_pChild[i];
		s= ( k->m_pParent[1] == pDmnd ) ? 1 : 0;

		k->m_ucSplitFlags&= ~( SPLIT_P0 << s );
		if( !( k->m_ucSplitFlags & SPLIT_P ) )	Enqueue( k, ROAM_UNQ, k->m_sQueueIndex );

		//put the tris back on the free list
		FreeTri( k, s );
	}

	//diamond is not split, update it's queue, and add to "check list"
	pDmnd->m_ucFlags&= ~ROAM_SPLIT;
	Enqueue( pDmnd, ROAM_SPLITQ, pDmnd->m_sQueueIndex );

	//update the diamond's parents, only if it is needed
	for( i= 0; i < 2; i++ ) {
		pParent= pDmnd->m_pParent[i];

		pParent->m_ucSplitFlags&= ~( SPLIT_K0<<pDmnd->m_cChildIndex[i] );
		if( !( pParent->m_ucSplitFlags & SPLIT_K ) ) {
			UpdatePriority( pParent );
			Enqueue( pParent, ROAM_MERGEQ, pParent->m_sQueueIndex );
		}
	}

	//put the parent tris on the triangle render list
	AllocateTri( pDmnd, 0 );
	AllocateTri( pDmnd, 1 );
}
