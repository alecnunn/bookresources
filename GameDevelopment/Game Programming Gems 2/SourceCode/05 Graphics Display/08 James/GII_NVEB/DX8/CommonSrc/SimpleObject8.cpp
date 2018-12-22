/*********************************************************************NVMH2****
Path:  C:\DEV\devrel\NV_SDK_4\DX8\CommonSrc
File:  SimpleObject8.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/



#include <assert.h>
#include "SimpleObject8.h"
#include "NV_Error.h"

#include <d3d8.h>
#include <d3dx8.h>
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
	m_wNumInd	= 0;
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
	m_wNumInd	= 0;
	m_PrimType  = D3DPT_TRIANGLELIST;

	m_bIsValid = false;
}



void SimpleObject8::Allocate( WORD nvert, WORD nind )
{
	UnAllocate();

	m_pVertices = (SimpleObjVertex*) calloc( nvert, sizeof( SimpleObjVertex ) );
	NULLCHECK( m_pVertices, "SimpleObject8:: Can't allocate vertices!", true );
	m_wNumVerts = nvert;

	m_pIndices = (WORD*) calloc( nind, sizeof( WORD ) );
	NULLCHECK( m_pIndices, "SimpleObject8:: Can't allocate indices!", true );
	m_wNumInd = nind;

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
	WORD ntri  = 2 * nLong * nLat;

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
				// If at top/bottom of sphere
				u -= 0.5f / (float)nLong;
			}
			else if( j == nLat-1 )
			{
				// If at top/bottom of sphere
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
		
			BOUNDCHK( nind, m_wNumInd-2, "InitSphere:: Index too high!", true );

			m_pIndices[nind++] = i2;
			m_pIndices[nind++] = i1;
			m_pIndices[nind++] = i0;

			BOUNDCHK( nind, m_wNumInd-2, "InitSphere:: Index too high 2!", true );
			m_pIndices[nind++] = i3;
			m_pIndices[nind++] = i1;
			m_pIndices[nind++] = i2;
		}
	}

	m_bIsValid = true;
	// all done!
}


void SimpleObject8::InitSpiral( const D3DXVECTOR3 & width_direction, 
								const D3DXVECTOR3 & height_direction,
								float width, float height, int n_sub_width, 
								int n_sub_height, float n_twists )
{

	int i,j;
	int nvert, nind;

	if( n_sub_height < 0 )
		n_sub_height = 0;
	if( n_sub_width < 0 )
		n_sub_width = 0;

	D3DXVECTOR3	width_dir;
	D3DXVec3Normalize( &width_dir, &width_direction );
	D3DXVECTOR3	height_dir;
	D3DXVec3Normalize( &height_dir, &height_direction );
	D3DXVECTOR3 length_dir;
	D3DXVec3Cross( &length_dir, &width_dir, &height_dir );



	nvert = (2 + n_sub_width)*(n_sub_height + 2);
	nind = (n_sub_width+1)*(n_sub_height+1)*2 * 3;

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

	float wt, ht, lt;		// params for each position axis

	for( j=0; j < n_sub_height + 2; j++ )
	{
		for(i=0; i < n_sub_width + 2; i++)
		{
			radius = width/2.0f - dx * i;

			ind = i + j * (n_sub_width+2);

			assert( ind < nvert );

			wt = radius * (float) sin( angle ); // amount of position in width_dir direction
			lt = radius * (float) cos( angle );	// amount of position in height_dir direction
			ht = z;

			m_pVertices[ ind ].pos.x = wt * width_dir.x + ht * height_dir.x + lt * length_dir.x;
//@			radius * (float) sin( angle );
			m_pVertices[ ind ].pos.y = wt * width_dir.y + ht * height_dir.y + lt * length_dir.y;
			m_pVertices[ ind ].pos.z = wt * width_dir.z + ht * height_dir.z + lt * length_dir.z;

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

			
			// stitching pattern is:
			//						           u,v = 1,1
			//			  v2 -- v1	      v2
			//				| /		    /  |
			//				|/		   /   |
			//            v0		v0 -- v1
			// u,v = 0,0

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


//@@@@@ fix to call other initspiral

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

			
			// stitching pattern is:
			//						x,z = 1,1
			//			  v2 -- v1	      v2
			//				| /		    /  |
			//				|/		   /   |
			//            v0		v0 -- v1
			// x,z = 0,0

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



void SimpleObject8::InitTorusWrapped( float InnerRadius, float OuterRadius, UINT Sides, UINT Rings )
{
	// Creates a torus "spun" about the z axis.
	// This one has no overlapping vertices on the seams,
	//  so you must use texture coordinate wrapping when 
	//  drawing it.

	UINT i,j;
	int nvert, nind;

	if( Sides < 3 )
		Sides = 3;
	if( Rings < 3 )
		Rings = 3;
	if( InnerRadius > OuterRadius )
	{
		float tmp = InnerRadius;
		InnerRadius = OuterRadius;
		OuterRadius = InnerRadius;
	}

	nvert = Sides * Rings;
	nind = 3 * Sides * 2 * Rings;		// # of triangle vertex indices

	// allocate memory for vertices
	Allocate( nvert, nind );


	m_PrimType = D3DPT_TRIANGLELIST;	// Could optimize to create tri
										//  strips, but not today!


	// Fill in vertex data

	// phi is angle around z axis
	// theta is angle around each ring

	float d_phi = 2.0f * (float)UL_PI / ((float)Rings);
	float d_theta = 2.0f * (float)UL_PI / ((float)Sides);

	float cent_radius = (InnerRadius + OuterRadius) / 2.0f;
	float ring_radius = (OuterRadius - InnerRadius) / 2.0f;

	float angle;


	SimpleObjVertex * pRing = (SimpleObjVertex*) calloc( Sides, sizeof( SimpleObjVertex));
	NULLCHECK( pRing, "SimpleObject8:: Can't allocate a ring of vertices!", true );


	// make the first ring - centered about cent_radius;
	for( i=0; i < Sides; i++ )
	{
		angle = 2.0f * (float)UL_PI * (float)i / (float)Sides;

		pRing[i].pos.x = ring_radius * ((float)cos( angle ));
		pRing[i].pos.z = ring_radius * ((float)sin( angle ));
		pRing[i].pos.y = 0.0f;

		pRing[i].pos.x += cent_radius;

		// Set normals
		pRing[i].nrm.x = (float) cos( angle );
		pRing[i].nrm.z = (float) sin( angle );
		pRing[i].nrm.y = 0.0f;

		pRing[i].t0.x = (float)i / (float)Sides;
	}

	int vind;
	float nx, ny, nz, length;
	vind = 0;

	for( j=0; j < Rings; j++ )
	{
		// rotate each ring into position
		//  around the sweep angle
		
		angle = 2.0f * (float)UL_PI * (float)j / (float) Rings;

		for( i=0; i < Sides; i++, vind++ )
		{
			assert( vind < nvert );
			m_pVertices[ vind ].pos.x = pRing[i].pos.x * ((float)cos( angle ));
			m_pVertices[ vind ].pos.y = pRing[i].pos.x * ((float)sin( angle ));
			m_pVertices[ vind ].pos.z = pRing[i].pos.z;

			// rotate normals into position
			nx = m_pVertices[ vind ].nrm.x = pRing[i].nrm.x * ((float)cos(angle));
			ny = m_pVertices[ vind ].nrm.y = pRing[i].nrm.x * ((float)sin(angle));
			nz = m_pVertices[ vind ].nrm.z = pRing[i].nrm.z;

			// normalize normals.  Shouldn't have to in this case, but
			//  just a safety measure.
			length = (float) sqrt(nx*nx + ny*ny + nz*nz);
			assert( length > 0.0f );

			m_pVertices[ vind ].nrm.x = nx / length;
			m_pVertices[ vind ].nrm.y = ny / length;
			m_pVertices[ vind ].nrm.z = nz / length;

			// Set texture coords:
			m_pVertices[ vind ].t0.x = pRing[i].t0.x;
			m_pVertices[ vind ].t0.y = (float) j / (float) Rings;
		}
	}

	///////////////////////////////////////////
	// Now make indices to form triangles

	int ind;			// index location to fill with a vertex index
	int v1, v2, v3;		// vertex indices of a triangle
	UINT jplus, iplus;	// j+1, i+1 wrapped back to 0 if > Rings or Sides, respectively


	ind = 0;

	for( j=0; j < Rings; j ++ )
	{
		for( i=0; i < Sides; i++ )
		{
			// first triangle of each "square" patch of surface
			//  on the torus

			iplus = i + 1;
			if( iplus == Sides )
				iplus = 0;
			jplus = j + 1;
			if( jplus == Rings )
				jplus = 0;


			// CCW winding order
			v1 = j		* Sides		+ i;
			v2 = j		* Sides		+ iplus;
			v3 = jplus	* Sides		+ i;


			assert( v1 < nvert && v1 >= 0 );
			assert( v2 < nvert && v2 >= 0 );
			assert( v3 < nvert && v3 >= 0 );

			assert( ind < nind );
			m_pIndices[ ind++ ] = v1;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v2;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v3;

			// second triangle of the pair
			v1 = v3;
			v2 = v2;
			v3 = jplus * Sides	+ iplus;

			assert( v3 < nvert && v1 >= 0 );

			assert( ind < nind );
			m_pIndices[ ind++ ] = v1;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v2;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v3;

		}
	}



	//////////////////////////////////////////

	if( pRing != NULL )
	{
		free( pRing );
		pRing = NULL;
	}
}



void SimpleObject8::InitTorus( float InnerRadius, float OuterRadius, UINT Sides, UINT Rings )
{
	// Creates a torus "spun" about the z axis.
	// With this non=wrapped model, vertices overlap at the seams.  
	// These overlapped vertices have appropriate texture coords so no
	//  change in tex coord wrapping mode is necessary.


	UINT i,j;
	int nvert, nind;

	if( Sides < 3 )
		Sides = 3;
	if( Rings < 3 )
		Rings = 3;
	if( InnerRadius > OuterRadius )
	{
		float tmp = InnerRadius;
		InnerRadius = OuterRadius;
		OuterRadius = InnerRadius;
	}

	nvert = ( Sides + 1 ) * ( Rings + 1 );
	nind = 3 * Sides * 2 * Rings;		// # of triangle vertex indices

	// allocate memory for vertices
	Allocate( nvert, nind );


	m_PrimType = D3DPT_TRIANGLELIST;	// Could optimize to create tri
										//  strips, but not today!


	// Fill in vertex data

	// phi is angle around z axis
	// theta is angle around each ring

	UINT  nphi_verts;
	UINT  ntheta_verts;

	nphi_verts = Rings + 1;
	ntheta_verts = Sides + 1;

	float d_phi = 2.0f * (float)UL_PI / ((float)(nphi_verts - 1) );
	float d_theta = 2.0f * (float)UL_PI / ((float)(ntheta_verts - 1));

	float cent_radius = (InnerRadius + OuterRadius) / 2.0f;
	float ring_radius = (OuterRadius - InnerRadius) / 2.0f;

	float angle;


	SimpleObjVertex * pRing = (SimpleObjVertex*) calloc( ntheta_verts, sizeof( SimpleObjVertex));
	NULLCHECK( pRing, "SimpleObject8:: Can't allocate a ring of vertices!", true );


	// make the first ring - centered about cent_radius;
	for( i=0; i < ntheta_verts; i++ )
	{
		angle = (float) i * d_theta;

		pRing[i].pos.x = ring_radius * ((float)cos( angle ));
		pRing[i].pos.z = ring_radius * ((float)sin( angle ));
		pRing[i].pos.y = 0.0f;

		pRing[i].pos.x += cent_radius;

		// Set normals
		pRing[i].nrm.x = (float) cos( angle );
		pRing[i].nrm.z = (float) sin( angle );
		pRing[i].nrm.y = 0.0f;

		pRing[i].t0.x = (float) i / (float)(ntheta_verts - 1);
	}

	int vind;
	float nx, ny, nz, length;
	vind = 0;

	for( j=0; j < nphi_verts; j++ )
	{
		// rotate each ring into position
		//  around the sweep angle
		
		angle = (float) j * d_phi;

		for( i=0; i < ntheta_verts; i++, vind++ )
		{
			assert( vind < nvert );
			m_pVertices[ vind ].pos.x = pRing[i].pos.x * ((float)cos( angle ));
			m_pVertices[ vind ].pos.y = pRing[i].pos.x * ((float)sin( angle ));
			m_pVertices[ vind ].pos.z = pRing[i].pos.z;

			// rotate normals into position
			nx = m_pVertices[ vind ].nrm.x = pRing[i].nrm.x * ((float)cos(angle));
			ny = m_pVertices[ vind ].nrm.y = pRing[i].nrm.x * ((float)sin(angle));
			nz = m_pVertices[ vind ].nrm.z = pRing[i].nrm.z;

			// normalize normals.  Shouldn't have to in this case, but
			//  just a safety measure.
			length = (float) sqrt(nx*nx + ny*ny + nz*nz);
			assert( length > 0.0f );

			m_pVertices[ vind ].nrm.x = nx / length;
			m_pVertices[ vind ].nrm.y = ny / length;
			m_pVertices[ vind ].nrm.z = nz / length;

			// Set texture coords:
			m_pVertices[ vind ].t0.x = pRing[i].t0.x;
			m_pVertices[ vind ].t0.y = (float) j / (float)( nphi_verts - 1);
		}
	}

	///////////////////////////////////////////
	// Now make indices to form triangles

	int ind;			// index location to fill with a vertex index
	int v1, v2, v3;		// vertex indices of a triangle
	UINT jplus, iplus;	// j+1, i+1 wrapped back to 0 if > Rings or Sides, respectively


	ind = 0;

	for( j=0; j < Rings; j ++ )
	{
		for( i=0; i < Sides; i++ )
		{
			// first triangle of each "square" patch of surface
			//  on the torus

			iplus = i + 1;
			jplus = j + 1;

			// CCW winding order
			v1 = j		* ntheta_verts		+ i;
			v2 = j		* ntheta_verts		+ iplus;
			v3 = jplus	* ntheta_verts		+ i;


			assert( v1 < nvert && v1 >= 0 );
			assert( v2 < nvert && v2 >= 0 );
			assert( v3 < nvert && v3 >= 0 );

			assert( ind < nind );
			m_pIndices[ ind++ ] = v1;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v2;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v3;

			// second triangle of the pair
			v1 = v3;
			v2 = v2;
			v3 = jplus * ntheta_verts	+ iplus;

			assert( v3 < nvert && v1 >= 0 );

			assert( ind < nind );
			m_pIndices[ ind++ ] = v1;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v2;
			assert( ind < nind );
			m_pIndices[ ind++ ] = v3;
		}
	}



	//////////////////////////////////////////

	if( pRing != NULL )
	{
		free( pRing );
		pRing = NULL;
	}

}


void SimpleObject8::InitTorus( float InnerRadius, float OuterRadius, UINT Sides, UINT Rings,
								float TexCoordUMax, float TexCoordVMax )
{

	// Create a torus with user-adjustable scaling of texture coords so a 
	//  texture can repeat several times around the torus

	InitTorus( InnerRadius, OuterRadius, Sides, Rings );

	int i;

	for( i=0; i < m_wNumVerts; i ++ )
	{
		m_pVertices[i].t0.x *= TexCoordUMax;
		m_pVertices[i].t0.y *= TexCoordVMax;
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
		maxmag = 0.0f;
		minmag = FLT_MAX;

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
		assert( false );		// something's gone wrong in the calcs

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


void SimpleObject8::Translate( float x, float y, float z )
{

	int numvert = m_wNumVerts;
	int ind;

	for( ind = 0; ind < numvert; ind ++ )
	{
		m_pVertices[ind].pos.x += x;		
		m_pVertices[ind].pos.y += y;		
		m_pVertices[ind].pos.z += z;		
	}
}


void SimpleObject8::TranslateXMinToZero()
{
	int numvert = m_wNumVerts;

	float minx = FLT_MAX;
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
	while( i < m_wNumInd )
	{
		vi0 = m_pIndices[i++];		
		vi1 = m_pIndices[i++];		
		vi2 = m_pIndices[i++];		

		v0 = m_pVertices[ vi0 ].pos;
		v1 = m_pVertices[ vi1 ].pos;
		v2 = m_pVertices[ vi2 ].pos;

		d0 = v1 - v0;
		d1 = v2 - v0;

		D3DXVec3Cross( &nrm, &d1, &d0 );		// triangle normal

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
										m_wNumInd/3,	// prim count
										m_pIndices,
										D3DFMT_INDEX16,
										m_pVertices,
										sizeof( SimpleObjVertex ) );


}



/*
// Old dX7 func - no longer applicable
HRESULT SimpleObject8::Draw( LPDIRECT3DDEVICE7 pDev )
{
	HRESULT res = E_FAIL;

	if( pDev && m_bIsValid )
	{
		res = pDev->DrawIndexedPrimitive( m_PrimType, gSObjVertexFVF,
						m_pVertices, m_wNumVerts, m_pIndices, m_wNumInd, 0 );
	}

	return res;
}
*/