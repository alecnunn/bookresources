/*********************************************************************NVMH2****
Path:  c:\Dev\devrel\Nv_sdk_4\Dx8\NVEffectsBrowser\Effects\SimpleFog
File:  SimpleObject8.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:

This code doesn't use vertex buffers for rendering.  For any sort of serious
application, this is a big mistake.  You should use vertex buffers all the time.
We have papers on our developer website about this.

Switching to vertex buffers does not change the code for vertex or pixel shaders,
and offers significant performance improvements.




******************************************************************************/



#include <assert.h>
#include "SimpleObject8.h"
#include "NV_Error.h"

#include <math.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define UL_PI      3.1415926535897932384626433832795028841971693993751
#define UL_SQRT_2  1.4142135623730950488016887242096980785696718753769
#define UL_PI2     (UL_PI/2.0)


#define BOUNDCHK(in, cmp, msg, quit) {if(in>=cmp){DoError(msg,quit);}}

//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SimpleObject8::SimpleObject8()
{
	m_pVertices = NULL;
	m_pIndices	= NULL;

	m_wNumVerts = 0;
	m_nNumInd	= 0;
	m_PrimType  = D3DPT_TRIANGLELIST;

	m_bIsValid = false;
}

SimpleObject8::~SimpleObject8()
{
	UnAllocate();
}

//////////////////////////////////////////////////////////////////////

void SimpleObject8::UnAllocate()
{
	if( m_pVertices )
		free( m_pVertices	);
	if( m_pIndices	)
		free( m_pIndices	);

	m_pVertices = NULL;
	m_pIndices = NULL;

	m_wNumVerts = 0;
	m_nNumInd	= 0;
	m_PrimType  = D3DPT_TRIANGLELIST;

	m_bIsValid = false;
}



void SimpleObject8::Allocate( WORD nvert, int nind )
{
	UnAllocate();

	m_pVertices = (SimpleObjVertex*) calloc( nvert, sizeof( SimpleObjVertex ) );
	NULLCHECK( m_pVertices, "SimpleObject8:: Can't allocate vertices!", true );
	m_wNumVerts = nvert;

	m_pIndices = (WORD*) calloc( nind, sizeof( WORD ) );
	NULLCHECK( m_pIndices, "SimpleObject8:: Can't allocate indices!", true );
	m_nNumInd = nind;

}


//-----------------------------------------------------------------------------
// Name: SimpleObject8::InitSphere( float radius, int nLat, int nLong )
// Desc: Make a 'dumb' sphere by sweeping an arc.  'dumb' cause it will have
//		a lot of points at the poles
//-----------------------------------------------------------------------------

void SimpleObject8::InitSphere( float radius, int nLat, int nLong )
{
	int i,j;

	if( nLong < 1 )	nLong = 1;
	if( nLat  < 3 ) nLat  = 3;
	if( radius < 0 ) radius = -radius;

	nLong++;		// repeat edge for texture coord happy
	nLat+=2;		// two rings on the ends

	WORD nvert = nLong * nLat;	
	int ntri  = 2 * nLong * nLat;

	m_PrimType  = D3DPT_TRIANGLELIST;

	// Allocate memory:
	Allocate( nvert, ntri * 3 );

	//m_csDesc = "I'm a sphere!";
	//m_csName = "Sphere01";

	// make vertex positions
	int index;
	float ringz;			// z cord of a latitude ring
	float ring_r;			// radius of x,y ring
	float theta, rho;		// theta = x,y plane angle, rho = z
	float x,y,u,v;				// x,y coord of pt.
	float mag;
	

	for( j=0; j < nLat; j++ )
	{
		rho = (float)( UL_PI * j / (float)(nLat-1) - UL_PI/2.0f );
		ringz  = (float) ( radius * sin(rho) );
		ring_r = (float) sqrt( radius * radius - ringz*ringz );

		for( i=0; i < nLong; i++)
		{
			theta = (float)( 2* UL_PI * i / (float)( nLong - 1 ) );
			x = (float) ( ring_r * cos(theta) );
			y = (float) ( ring_r * sin(theta) );

			index = i + j*nLong;
			if( index > m_wNumVerts )
			{
				FDebug("InitSphere Error! %n index of only %n allocated!\n", index, m_wNumVerts );
				index = m_wNumVerts -1;
			}

			m_pVertices[ index ].pos.x = x;
			m_pVertices[ index ].pos.y = y;
			m_pVertices[ index ].pos.z = ringz;
			
			u = (float)( (float)i/(float)(nLong-1) );
			v = (float)( (float)j/(float)(nLat-1)  );
			// Correct for sphere pinch & "degenerate" triangle at caps
			if( j==0 )
			{
				u -= 0.5f / (float)nLong;
			}
			else if( j == nLat-1 )
			{
				u += 0.5f / (float)nLong;
			}
			m_pVertices[ index ].t0.x = u;
			m_pVertices[ index ].t0.y = v;

			//FDebug("U:  %f    V:  %f   ind: %d\n", u, v, index );

			mag = (float)sqrt( x*x + y*y + ringz*ringz );
			m_pVertices[ index ].nrm.x = x/mag;
			m_pVertices[ index ].nrm.y = y/mag;
			m_pVertices[ index ].nrm.z = ringz/mag;
			
			// m_pVertices[ index ].diffuse = D3DRGB( u, v, 1-u );
			m_pVertices[ index ].diffuse = D3DCOLOR_ARGB( 255, 255, 255, 255 );

		}
	}


	// stitch the tris
	// Culling is CCW

	WORD  nind = 0;
	WORD  i0, i1, i2, i3;

	for( j=0; j < nLat-1; j++ )
	{
		for( i=0; i < nLong; i++)
		{
			i0 = i   + j*nLong;
			i1 = i   + (j+1)*nLong;
			i2 = (i+1)%nLong + j*nLong;		// % to wrap back to zero
			i3 = (i+1)%nLong + (j+1)*nLong;
		
			BOUNDCHK( nind, m_nNumInd-2, "InitSphere:: Index too high!", true );

			m_pIndices[nind++] = i2;
			m_pIndices[nind++] = i1;
			m_pIndices[nind++] = i0;

			BOUNDCHK( nind, m_nNumInd-2, "InitSphere:: Index too high 2!", true );
			m_pIndices[nind++] = i3;
			m_pIndices[nind++] = i1;
			m_pIndices[nind++] = i2;
		}
	}

	m_bIsValid = true;
	// all done!
}

void SimpleObject8::InitSpiral( float width, float height, int n_sub_width,
					int n_sub_height, float n_twists )
{

	int i,j;
	int nvert, nind;

	if( n_sub_height < 0 )
		n_sub_height = 0;
	if( n_sub_width < 0 )
		n_sub_width = 0;


	nvert = (2 + n_sub_width)*(n_sub_height + 2);
	nind = (n_sub_width+1)*(n_sub_height+1)*2 * 3;


	if( nvert > MAX_VERT_IND )
	{
		FDebug("Can't create object - too many verts!\n");
		assert( false );
		return;
	}


	Allocate( nvert, nind );

	m_PrimType = D3DPT_TRIANGLELIST;

	float z;
	float dz, dx;
	float angle;
	float da;
	float radius;

	z = -height / 2.0f;
	angle = 0;
	
	dz = height / (n_sub_height+1.0f);
	da = (float)( n_twists * UL_PI * 2.0f ) / (n_sub_height + 1.0f);

	dx =  width / ( (float)n_sub_width + 1.0f );


	int ind;

	for( j=0; j < n_sub_height + 2; j++ )
	{
		for(i=0; i < n_sub_width + 2; i++)
		{
			radius = width/2.0f - dx * i;

			ind = i + j * (n_sub_width+2);

			assert( ind < nvert );

			m_pVertices[ ind ].pos.x = radius * (float) sin( angle );
			m_pVertices[ ind ].pos.y = radius * (float) cos( angle );
			m_pVertices[ ind ].pos.z = z;

			m_pVertices[ ind ].t0.x = (float)i/(n_sub_width  +1.0f);
			m_pVertices[ ind ].t0.y = (float)j/(n_sub_height +1.0f);

		}

		z += dz;
		angle += da;
	}

	// Set the indices
	int triind;				// triind = index of triangle -> 3 for each
							//   so 0,1,2 = 1st tri, 3,4,5 = 2nd tri
	triind = 0;




	for( j=0; j < n_sub_height + 1; j++ )
	{
		for( i=0; i < n_sub_width + 1; i++ )
		{

			if( i + j*(n_sub_width + 1) >= nvert )
				assert( false );
			if( i + 1 + j * ( n_sub_width + 2 ) >= nvert )
				assert( false );

			if( triind >= nind )
			{
				FDebug( "bad index calc: %d of %d", triind, nind );
				assert( false );
			}

			assert( triind < nind );
			m_pIndices[ triind++ ] = i + j*(n_sub_width+2);
			assert( triind < nind );
			m_pIndices[ triind++ ] = i + 1 + (j+1)*(n_sub_width+2);
			assert( triind < nind );
			m_pIndices[ triind++ ] = i + 1 + j*(n_sub_width+2);
			assert( triind < nind );

			m_pIndices[ triind++ ] = i + j*(n_sub_width+2);
			assert( triind < nind );
			m_pIndices[ triind++ ] = i + (j+1)*(n_sub_width+2);
			assert( triind < nind );
			m_pIndices[ triind++ ] = i + 1 + (j+1)*(n_sub_width+2);		
		}
	}
}


void SimpleObject8::AddPositionNoise( D3DXVECTOR3 vNoiseDir, float noise_magnitude )
{
	
	int numvert = m_wNumVerts;

	D3DXVECTOR3 noise;

	for( int ind = 0; ind < numvert; ind++ )
	{
		noise = vNoiseDir * noise_magnitude * ( ((float)rand())/(float)RAND_MAX );
		
		m_pVertices[ ind ].pos.x += noise.x;
		m_pVertices[ ind ].pos.y += noise.y;
		m_pVertices[ ind ].pos.z += noise.z;		
	}
}


void SimpleObject8::AddPositionNoiseGrid( D3DXVECTOR3 vNoiseDir, float noise_magnitude,
										  int grid_nx, int nsmooth )
{
	// Add noise to the positions with possibility for averaging the noise
	//   values with their nearest neighbors as though they lie on a grid 
	//   of x dimension grid_nx.
	// Function does a crude compensation if the number of vertices doesn't fill
	//   out the last line of the imposed grid.
	// noise_magnitude is max magnitude that will be added - the noise field
	//   is scaled so the max value is that.

	int numvert = m_wNumVerts;

	int gridx = grid_nx;
	int gridy = m_wNumVerts/gridx;

	if( gridy * gridx != m_wNumVerts )
	{
		FDebug("SimpleObject8 -- AddPositionNoiseGrid Vertices not a rectangular grid!\n");
		FDebug("    Compensating for remaining verts...\n");

		gridy++;
	}

	D3DXVECTOR3 noise;

	float * noisemag = (float*) calloc( numvert, sizeof( float ));
	assert( noisemag != NULL );

	int ind;

	for( ind = 0; ind < numvert; ind++ )
	{
		noisemag[ind] = ( ((float)rand())/(float)RAND_MAX );
	}

	// now smooth the noise according to the grid
	int i,j, n;
	int ind0, ind1;

	float avg;
	float maxmag;
	float minmag;

	for( n=0; n < nsmooth; n++ )
	{
		maxmag = 0;
		minmag = 9e9f;

		for( j=0; j < gridy; j++ )
			for( i=0; i < gridx; i++ )
			{	
				ind0 = j*gridx + i;
				
				if( ind0 > numvert-1 )
				{
					FDebug("broke on:  %d    %d\n", i,j );
					break;
				}

				avg = noisemag[ind0];


				ind1 = ind0 + 1;

				if( ind1 > numvert-1 )
					ind1 = numvert-1;
				avg += noisemag[ind1];


				ind1 = ind0 + gridx;

				if( ind1 > numvert-1 )
				{
					ind1 = i;
				}
				avg += noisemag[ind1];


				ind1 = ind0 + gridx + 1;

				if( ind1 > numvert-1 )
				{
					ind1 = i + 1;
				}
				avg += noisemag[ind1];

				noisemag[ ind0 ] = avg / 4.0f;

				if( noisemag[ ind0 ] > maxmag )
				{
					maxmag = noisemag[ ind0 ];
				}
				if( noisemag[ ind0 ] < minmag )
				{
					minmag = noisemag[ ind0 ];
				}
			}
	}

	// scale the noise values so their max is noise_magnitude
	float scale;
	maxmag = maxmag - minmag;

	if( maxmag > 0.0f )
		scale = noise_magnitude / maxmag;
	else
	{
		scale = 0.0f;
		FDebug("AddPositionNoiseGrid: Using scale of 0.0f!!\n");
	}


	for( i=0; i < numvert; i++ )
	{
		noisemag[i] = ( noisemag[i] - minmag ) * scale;
	}


	for( ind = 0; ind < numvert; ind++ )
	{
		noise = vNoiseDir * noisemag[ind];
		
		m_pVertices[ ind ].pos.x += noise.x;
		m_pVertices[ ind ].pos.y += noise.y;
		m_pVertices[ ind ].pos.z += noise.z;		
	}


	free( noisemag );
}


void SimpleObject8::TranslateXMinToZero()
{
	int numvert = m_wNumVerts;

	float minx = 9e9f;
	int ind;

	for( ind = 0; ind < numvert; ind++ )
	{
		if( m_pVertices[ ind ].pos.x < minx )
			minx = m_pVertices[ ind ].pos.x;
	}

	for( ind = 0; ind < numvert; ind++ )
	{
		m_pVertices[ ind ].pos.x -= minx;
	}
}


void SimpleObject8::GenerateNormals()
{
	assert( m_pIndices != NULL );
	assert( m_pVertices != NULL );

	if( m_PrimType != D3DPT_TRIANGLELIST )
	{
		FDebug("Error: primitive type must be D3DPT_TRIANGLELIST\n");
		assert( false );
	}


	int i;
	int vi0, vi1, vi2;	    // vertex indices of triangle

	// zero normals for accumulation
	for( i=0; i < m_wNumVerts; i++ )
	{
		m_pVertices[i].nrm.x = 0.0f;
		m_pVertices[i].nrm.y = 0.0f;
		m_pVertices[i].nrm.z = 0.0f;
	}

	// find each triangle's normal and add into the 
	//   vertex normal

	D3DXVECTOR3 v0, v1, v2;
	D3DXVECTOR3 d0, d1, nrm;

	i=0;
	while( i < m_nNumInd )
	{
		vi0 = m_pIndices[i++];		
		vi1 = m_pIndices[i++];		
		vi2 = m_pIndices[i++];		

		v0 = m_pVertices[ vi0 ].pos;
		v1 = m_pVertices[ vi1 ].pos;
		v2 = m_pVertices[ vi2 ].pos;

		d0 = v1 - v0;
		d1 = v2 - v0;

		D3DXVec3Cross( &nrm, &d0, &d1 );		// triangle normal

		//  accumulate into vertex normals:

		m_pVertices[ vi0 ].nrm += nrm;
		m_pVertices[ vi1 ].nrm += nrm;
		m_pVertices[ vi2 ].nrm += nrm;
	}

	// normalize the normals
	for( i=0; i < m_wNumVerts; i++ )
	{
		D3DXVec3Normalize( &( m_pVertices[i].nrm ), &( m_pVertices[i].nrm ) );
	}
}







void SimpleObject8::Draw( LPDIRECT3DDEVICE8  pD3DDev )
{
	assert( pD3DDev != NULL );

	pD3DDev->DrawIndexedPrimitiveUP(	m_PrimType,
										0,				// min index
										m_wNumVerts,
										m_nNumInd/3,	// prim count
										m_pIndices,
										D3DFMT_INDEX16,
										m_pVertices,
										sizeof( SimpleObjVertex ) );


}



