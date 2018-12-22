/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: bezier.cpp
 *    Desc: Bezier patch and object classes
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"

#include <string>
using namespace std;

#include "bezier.h"



matrix4 cBezierPatch::m_basisMatrix = matrix4(
    -1.0f,	 3.0f,	-3.0f,	 1.0f,
	 3.0f,	-6.0f,	 3.0f,	 0.0f,
	-3.0f,	 3.0f,	 0.0f,	 0.0f,
	 1.0f,	 0.0f,	 0.0f,	 0.0f
);

cBezierPatch::cBezierPatch()
{
	m_vertList = NULL;
	m_triList = NULL;
	m_uTangList = NULL;
	m_vTangList = NULL;
}


cBezierPatch::~cBezierPatch()
{
	// delete our vertex list
	if( m_vertList )
		delete [] m_vertList;

	delete [] m_triList;
	delete [] m_uTangList;
	delete [] m_vTangList;
}


void cBezierPatch::Init( int size )
{
	delete [] m_vertList;
	delete [] m_triList;
	delete [] m_uTangList;
	delete [] m_vTangList;

	m_size = size;

	// allocate our lists
	m_vertList = new sVertex[ size * size ];
	m_triList = new sTri[ (size-1) * (size-1) * 2 ];

	m_uTangList = new point3[ size * size ];
	m_vTangList = new point3[ size * size ];

	Tesselate();
}


/**
 * Fill in the grid of values (all the dynamic arrays
 * have been initialized already).  The grid is of
 * size mxn where m = n = m_size
 */
void cBezierPatch::Tesselate()
{
	int u, v; // patch-space coordinates.
	point3 p1,p2,p3,p4;

	/**
	 * These are the four curves that will define the control points for the
	 * rest of the curves
	 */
	cFwdDiffIterator mainCurve1;
	cFwdDiffIterator mainCurve2;
	cFwdDiffIterator mainCurve3;
	cFwdDiffIterator mainCurve4;

	int nSteps = m_size;
	mainCurve1 = cFwdDiffIterator( nSteps, m_ctrlPoints[0], m_ctrlPoints[4], m_ctrlPoints[8], m_ctrlPoints[12] );
	mainCurve2 = cFwdDiffIterator( nSteps, m_ctrlPoints[1], m_ctrlPoints[5], m_ctrlPoints[9], m_ctrlPoints[13] );
	mainCurve3 = cFwdDiffIterator( nSteps, m_ctrlPoints[2], m_ctrlPoints[6], m_ctrlPoints[10], m_ctrlPoints[14] );
	mainCurve4 = cFwdDiffIterator( nSteps, m_ctrlPoints[3], m_ctrlPoints[7], m_ctrlPoints[11], m_ctrlPoints[15] );

	mainCurve1.Start();
	mainCurve2.Start();
	mainCurve3.Start();
	mainCurve4.Start();
	
	for(v=0;v<m_size;v++)
	{
		/**
		 * Generate our four control points for this curve
		 */
		p1 = mainCurve1.GetCurr();
		p2 = mainCurve2.GetCurr();
		p3 = mainCurve3.GetCurr();
		p4 = mainCurve4.GetCurr();

		/**
		 * Now step along the curve filling in the data
		 */
		cTangentIterator tanIter( nSteps, p1, p2, p3, p4 );
		tanIter.Start();
		cFwdDiffIterator iter( nSteps, p1, p2, p3, p4 );
		u = 0;
		for( 
			iter.Start(); !iter.Done(); iter.CalcNext(), u++ )
		{
			m_vertList[m_size*v+u].loc = iter.GetCurr();

			// We're piggybacking our u-direction tangent vector calculation here.
			m_uTangList[m_size*v+u] = tanIter.GetCurr();
			tanIter.CalcNext();
		}

		mainCurve1.CalcNext();
		mainCurve2.CalcNext();
		mainCurve3.CalcNext();
		mainCurve4.CalcNext();
	}

	/**
	 * Since we can't generate the v-tangents in the same run as the u-tangents (we need to go
	 * in the opposite direction), we have to go through the process again, but this time in the
	 * perpendicular direction we went the first time
	 */
	mainCurve1 = cFwdDiffIterator( nSteps, m_ctrlPoints[0], m_ctrlPoints[1], m_ctrlPoints[2], m_ctrlPoints[3] );
	mainCurve2 = cFwdDiffIterator( nSteps, m_ctrlPoints[4], m_ctrlPoints[5], m_ctrlPoints[6], m_ctrlPoints[7] );
	mainCurve3 = cFwdDiffIterator( nSteps, m_ctrlPoints[8], m_ctrlPoints[9], m_ctrlPoints[10], m_ctrlPoints[11] );
	mainCurve4 = cFwdDiffIterator( nSteps, m_ctrlPoints[12], m_ctrlPoints[13], m_ctrlPoints[14], m_ctrlPoints[15] );

	mainCurve1.Start();
	mainCurve2.Start();
	mainCurve3.Start();
	mainCurve4.Start();
	
	for(v=0;v<m_size;v++)
	{
		// create a horizontal bezier curve by calc'ing points along the 4 vertical ones

		p1 = mainCurve1.GetCurr();
		p2 = mainCurve2.GetCurr();
		p3 = mainCurve3.GetCurr();
		p4 = mainCurve4.GetCurr();

		cTangentIterator iter( nSteps, p1, p2, p3, p4 );
		u = 0;
		for( iter.Start(); !iter.Done(); iter.CalcNext(), u++ )
		{
			// We don't get the location because all we want here is the v-tangents
			m_vTangList[m_size*u+v] = iter.GetCurr();
		}

		mainCurve1.CalcNext();
		mainCurve2.CalcNext();
		mainCurve3.CalcNext();
		mainCurve4.CalcNext();
	}

	int offset;
	for(v=0;v<m_size;v++)
	{
		// tesselate across the horizontal bezier
		for(u=0;u<m_size;u++)
		{
			offset = m_size*v+u;

			point3 norm;
			norm = m_vTangList[offset] ^ m_uTangList[offset];
			norm.Normalize();

			m_vertList[offset].norm = norm;
			m_vertList[offset].u = 0;
			m_vertList[offset].v = 0;
		}
	}

	// use an incremented pointer to the triangle list
	sTri* pCurrTri = m_triList;


	// build the tri list
	for( v=0; v< (m_size-1); v++ )
	{
		for( u=0; u< (m_size-1); u++ )
		{
			// tesselating square [u,v]

			// 0, 1, 2
			pCurrTri->v[0] = m_size*(v+0) + (u+0);
			pCurrTri->v[1] = m_size*(v+0) + (u+1);
			pCurrTri->v[2] = m_size*(v+1) + (u+1);
			pCurrTri++;

			// 2, 3, 0
			pCurrTri->v[0] = m_size*(v+1) + (u+1);
			pCurrTri->v[1] = m_size*(v+1) + (u+0);
			pCurrTri->v[2] = m_size*(v+0) + (u+0);
			pCurrTri++;

		}
	}
}


void cBezierPatch::Load( cFile& file )
{
	// load ourselves from point in the file passed in.
	int pointIndex;

	char buff[1024];
	
	for( pointIndex=0; pointIndex<4; pointIndex++ )
	{
		file.ReadNonCommentedLine( buff, '#' );
		sscanf( buff, "%f %f %f %f %f %f %f %f %f %f %f %f",
			&m_ctrlPoints[pointIndex * 4 + 0].x,
			&m_ctrlPoints[pointIndex * 4 + 0].y,
			&m_ctrlPoints[pointIndex * 4 + 0].z,
			&m_ctrlPoints[pointIndex * 4 + 1].x,
			&m_ctrlPoints[pointIndex * 4 + 1].y,
			&m_ctrlPoints[pointIndex * 4 + 1].z,
			&m_ctrlPoints[pointIndex * 4 + 2].x,
			&m_ctrlPoints[pointIndex * 4 + 2].y,
			&m_ctrlPoints[pointIndex * 4 + 2].z,
			&m_ctrlPoints[pointIndex * 4 + 3].x,
			&m_ctrlPoints[pointIndex * 4 + 3].y,
			&m_ctrlPoints[pointIndex * 4 + 3].z);
	}

	// scale everything up a bit.
	for( int i=0; i<16;i++ )
	{
		m_ctrlPoints[i] = m_ctrlPoints[i] * 10.f;
	}

	// now we can initialize and tesselate the patch
	Init( 10 );
	Tesselate();
}


void cBezierPatch::Draw( bool bDrawNet )
{
	// hard code the control mesh lines
	static short netIndices[] =	{ 
		0, 1, 1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 
		8, 9, 9, 10, 10, 11, 12, 13, 13, 14, 14, 15,
		0, 4, 4, 8, 8, 12, 1, 5, 5, 9, 9, 13,
		2, 6, 6, 10, 10, 14, 3, 7, 7, 11, 11, 15 };

	Graphics()->GetDevice()->SetVertexShader( D3DFVF_VERTEX );
		
	if( bDrawNet )
	{
		sLitVertex v[16];
		for( int i=0; i<16; i++ )
		{
			v[i] = sLitVertex( m_ctrlPoints[i], 0 );
		}

		Graphics()->GetDevice()->DrawIndexedPrimitiveUP( 
			D3DPT_LINELIST,
			0,
			16,
			8,
			netIndices,
			D3DFMT_INDEX16,
			v,
			sizeof( sLitVertex ) );

	}

	Graphics()->GetDevice()->DrawIndexedPrimitiveUP( 
			D3DPT_TRIANGLELIST,			// Primtive Type
			0,							// Minimum Index
			m_size * m_size,			// Number of vertices
			2*(m_size-1)*(m_size-1),	// Number of Primitives
			m_triList,					// Index Data
			D3DFMT_INDEX16,				// Index format
			m_vertList,					// Vertex Data
			sizeof( sLitVertex ) );		// Vertex Stride

}


cBezierObject::cBezierObject()
{
	m_patchList = NULL;
}


cBezierObject::~cBezierObject()
{
	delete[] m_patchList;
}

void cBezierObject::Load( char* filename )
{
	char buff[1024];

	cFile file;
	try
	{
		file.Open( filename );
	}
	catch( cFileNotFound& error )
	{
		string out = 
			string("Error!\nThe Bezier Surface Patch file [") +
			error.Name() + 
			string("] \nwas not found.  Aborting Execution.");
		throw cGameError( (char*)out.c_str() );
	}


	file.ReadNonCommentedLine( buff, '#' );

	// read in the # of patches
	sscanf( buff, "%i\n", &m_nPatches );

	// create our patch list
	m_patchList = new cBezierPatch[ m_nPatches ];

	// read each of the patches in one at a time
	int patchIndex;
	for( patchIndex=0; patchIndex<m_nPatches; patchIndex++ )
	{
		// blank line before each patch
		file.ReadNonCommentedLine( buff, '#' );
		m_patchList[ patchIndex ].Load( file );
	}
}


void cBezierObject::Draw( const matrix4& mat, bool bDrawNet )
{
	int pIndex;

	// set up our coordinate space
	Graphics()->GetDevice()->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&mat );
	
	for( pIndex=0; pIndex<m_nPatches; pIndex++ )
	{
		m_patchList[ pIndex ].Draw( bDrawNet );
	}
}


