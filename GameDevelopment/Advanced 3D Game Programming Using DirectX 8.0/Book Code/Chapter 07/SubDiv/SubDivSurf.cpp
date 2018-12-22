/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: SubDivSurf.cpp
 *    Desc: Implementation of modified-butterfly scheme
 *          Subdivision Surfaces
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"
#include "SubDivSurf.h"

#include <algorithm>
using namespace std;

cSubDivSurf::cSubDivSurf( const char* filename )
{
	/**
	 * Nullify all of our dynamic data structures
	 */
	m_d3dTriList = NULL;

	m_pVertexBuffer = NULL;

	/**
	 * Load the model
	 */
	cModel model( filename );

	/**
	 * For the purposes of the sample, unitize the object.
	 * production code probably wouldn't want this.
	 */
	model.Scale( 1.f / model.GenRadius() );

	m_nVerts = model.NumVerts();
	m_nTris = model.NumTris();
	m_nEdges = m_nTris * 3 / 2;

	m_pVList = new sVert[ m_nVerts ];
	m_pEList = new sEdge[ m_nEdges ];
	m_pTList = new sTriangle[ m_nTris ];

	int i;

	// Load the vertices
	for( i=0; i<m_nVerts; i++ )
	{
		//m_pVList[i].m_vert = *(model.VertData() + i);
		CopyMemory( &m_pVList[i].m_vert, (model.VertData() + i), sizeof( sVertex ) );
		
		m_pVList[i].m_index = i;
	}

	// Load the triangles (simultaneously load the edges)
	int currEdge = 0;
	for( i=0; i<m_nTris; i++ )
	{
		sTri* pTri = (sTri*) (model.TriData() + i);
		int ind[3];

		ind[0] = pTri->v[0];
		ind[1] = pTri->v[1];
		ind[2] = pTri->v[2];

		m_pTList[i].Init( &m_pVList[ind[0]], &m_pVList[ind[1]], &m_pVList[ind[2]] );

		// only do it one way to avoid duplicates
		if( ind[0] > ind[1] )
			m_pEList[currEdge++].Init( &m_pVList[ind[0]], &m_pVList[ind[1]] );
		if( ind[1] > ind[2] )
			m_pEList[currEdge++].Init( &m_pVList[ind[1]], &m_pVList[ind[2]] );
		if( ind[2] > ind[0] )
			m_pEList[currEdge++].Init( &m_pVList[ind[2]], &m_pVList[ind[0]] );
	}

	CalcNormals();

	GenD3DData();
}



cSubDivSurf::~cSubDivSurf()
{
	/**
	 * Annihilate all of our dynamic data
	 */
	if( m_pVertexBuffer )
		m_pVertexBuffer->Release();
	m_pVertexBuffer = NULL;

	delete [] m_pVList;
	delete [] m_pEList;
	delete [] m_pTList;

	delete [] m_d3dTriList;
}



void cSubDivSurf::Draw( matrix4& mat )
{
	LPDIRECT3DDEVICE8 lpDevice = Graphics()->GetDevice();
	Graphics()->SetWorldMatrix( mat );

	HRESULT hr;

	// The index buffer
	LPDIRECT3DINDEXBUFFER8 pIndexBuffer = 0;

	// Create the index buffer
	lpDevice->CreateIndexBuffer(
		m_nTris * 3 * sizeof( WORD ),	// Size in bytes of buffer
		D3DUSAGE_WRITEONLY,				// Will only be writing to the buffer
		D3DFMT_INDEX16,					// 16 bit indices
		D3DPOOL_DEFAULT,				// Default memory pooling
		&pIndexBuffer );				// Address of the buffer pointer

	// Pointer to the index buffer data
	WORD* pData = 0;

	// Lock the index buffer
	pIndexBuffer->Lock( 0, 0, (BYTE**)&pData, 0 );

	// Copy the index data into the index buffer
	CopyMemory( pData, m_d3dTriList, m_nTris * 3 * sizeof( WORD ) );

	// Unlock the index buffer
	pIndexBuffer->Unlock();

	// Tell Direct3D to use the index buffer
	lpDevice->SetIndices( pIndexBuffer, 0 );
	
	// Attach the vertex buffer to rendering stream 0
	lpDevice->SetStreamSource( 0, m_pVertexBuffer, sizeof( sVertex ) );

	// Draw the primitive
	hr = lpDevice->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST,
		0,
		m_nVerts,
		0,
		m_nTris );

	if( FAILED( hr ) )
	{
		DP0("[cSubDivSurf::Draw]: DrawIndexedPrimitive failed!\n");
	}

	pIndexBuffer->Release();
}


/**
 * This is where the meat of the subdivision work is done.
 * Depending on the valence of the two endpoints of each edge,
 * the code will generate the new edge value
 */
void cSubDivSurf::GenNewVertLocs()
{
	for( int i=0; i<m_nEdges; i++ )
	{
		int val0 = m_pEList[i].m_v[0]->Valence();
		int val1 = m_pEList[i].m_v[1]->Valence();

		point3 loc;

		/**
		 * CASE 1: both vertices are of valence == 6
		 * Use the butterfly scheme
		 */
		if( val0 == 6 && val1 == 6 )
		{
			loc = m_pEList[i].CalcButterfly();
		}

		/**
		 * CASE 2: one of the vertices are of valence == 6
		 * Calculate the k-vertex for the non-6 vertex
		 */
		else if( val0 == 6 && val1 != 6 )
		{
			loc = m_pEList[i].CalcKVert(1,0);
		}

		else if( val0 != 6 && val1 == 6 )
		{
			loc = m_pEList[i].CalcKVert(0,1);
		}

		/**
		 * CASE 3: neither of the vertices are of valence == 6
		 * Calculate the k-vertex for each of them, and average
		 * the result
		 */
		else 
		{
			loc = ( m_pEList[i].CalcKVert(1,0) + 
				    m_pEList[i].CalcKVert(0,1) ) / 2.f;
		}

		m_pEList[i].m_newVLoc.m_vert = sVertex( 
			loc , point3::Zero );

		/**
		 * Assign the new vertex an index (this is useful later,
		 * when we start throwing vertex pointers around.  We
		 * could have implemented everything with indices, but
		 * the code would be much harder to read.  An extra dword
		 * per vertex is a small price to pay.)
		 */
		m_pEList[i].m_newVLoc.m_index = i + m_nVerts;
	}
}


eResult cSubDivSurf::Subdivide()
{

	/**
	 * We know how many components our subdivided model will have, calc them
	 */
	int nNewEdges = 2*m_nEdges + 3*m_nTris;
	int nNewVerts = m_nVerts + m_nEdges;
	int nNewTris = 4*m_nTris;

	/**
	 * If the model will have too many triangles (d3d can only handle 2^16), return
	 */
	if( nNewVerts >= 65536 || nNewTris >= 65536)
	{
		return resFalse;
	}

	/**
	 * Find the location of the new vertices.  Most of the hard work
	 * is done here.
	 */
	GenNewVertLocs();

	int i;
	sVert* inner[3]; // the vertices on the 3 edges (order: 0..1, 1..2, 2..0)


	// Allocate space for the subdivided data
	sVert* pNewVerts = new sVert[ nNewVerts ];
	sEdge* pNewEdges = new sEdge[ nNewEdges ];
	sTriangle* pNewTris = new sTriangle[ nNewTris ];


	//==========--------------------------  Step 1: Fill the vertex list
	// First batch - the original vertices
	for( i=0; i<m_nVerts; i++ )
	{
		pNewVerts[i].m_index = i;
		pNewVerts[i].m_vert = m_pVList[i].m_vert;
	}
	// Second batch - vertices from each edge
	for( i=0; i<m_nEdges; i++ )
	{
		pNewVerts[m_nVerts + i].m_index = m_nVerts + i;
		pNewVerts[m_nVerts + i].m_vert = m_pEList[i].m_newVLoc.m_vert;
	}

	//==========--------------------------  Step 2: Fill in the edge list
	int currEdge = 0;
	// First batch - the 2 edges that are spawned by each original edge
	for( i=0; i<m_nEdges; i++ )
	{
		pNewEdges[currEdge++].Init( 
			&pNewVerts[m_pEList[i].m_v[0]->m_index],
			&pNewVerts[m_pEList[i].m_newVLoc.m_index] );
		pNewEdges[currEdge++].Init( 
			&pNewVerts[m_pEList[i].m_v[1]->m_index],
			&pNewVerts[m_pEList[i].m_newVLoc.m_index] );
	}
	// Second batch - the 3 inner edges spawned by each original triangle
	for( i=0; i<m_nTris; i++ )
	{
		// find the inner 3 vertices of this triangle 
		// ( the new vertex of each of the triangles' edges )
		inner[0] = &m_pTList[i].m_v[0]->GetEdge( m_pTList[i].m_v[1] )->m_newVLoc;
		inner[1] = &m_pTList[i].m_v[1]->GetEdge( m_pTList[i].m_v[2] )->m_newVLoc;
		inner[2] = &m_pTList[i].m_v[2]->GetEdge( m_pTList[i].m_v[0] )->m_newVLoc;

		pNewEdges[currEdge++].Init( 
			&pNewVerts[inner[0]->m_index],
			&pNewVerts[inner[1]->m_index] );
		pNewEdges[currEdge++].Init( 
			&pNewVerts[inner[1]->m_index],
			&pNewVerts[inner[2]->m_index] );
		pNewEdges[currEdge++].Init( 
			&pNewVerts[inner[2]->m_index],
			&pNewVerts[inner[0]->m_index] );
	}


	//==========--------------------------  Step 3: Fill in the triangle list
	int currTri = 0;
	for( i=0; i<m_nTris; i++ )
	{
		// find the inner vertices
		inner[0] = &m_pTList[i].m_v[0]->GetEdge( m_pTList[i].m_v[1] )->m_newVLoc;
		inner[1] = &m_pTList[i].m_v[1]->GetEdge( m_pTList[i].m_v[2] )->m_newVLoc;
		inner[2] = &m_pTList[i].m_v[2]->GetEdge( m_pTList[i].m_v[0] )->m_newVLoc;

		// 0, inner0, inner2
		pNewTris[currTri++].Init( 
			&pNewVerts[m_pTList[i].m_v[0]->m_index],
			&pNewVerts[inner[0]->m_index], 
			&pNewVerts[inner[2]->m_index] ); 

		// 1, inner1, inner0
		pNewTris[currTri++].Init( 
			&pNewVerts[m_pTList[i].m_v[1]->m_index],
			&pNewVerts[inner[1]->m_index], 
			&pNewVerts[inner[0]->m_index] ); 

		// 2, inner2, inner1
		pNewTris[currTri++].Init( 
			&pNewVerts[m_pTList[i].m_v[2]->m_index],
			&pNewVerts[inner[2]->m_index], 
			&pNewVerts[inner[1]->m_index] ); 

		// inner0, inner1, inner2
		pNewTris[currTri++].Init( 
			&pNewVerts[inner[0]->m_index], 
			&pNewVerts[inner[1]->m_index], 
			&pNewVerts[inner[2]->m_index] ); 
	}

	//==========--------------------------  Step 4: Housekeeping

	// Swap out the old data sets for the new ones.
	
	delete [] m_pVList;
	delete [] m_pEList;
	delete [] m_pTList;

	m_nVerts = nNewVerts;
	m_nEdges = nNewEdges;
	m_nTris = nNewTris;

	m_pVList = pNewVerts;
	m_pEList = pNewEdges;
	m_pTList = pNewTris;

	// Calculate the vertex normals of the new mesh using face normal averaging
	CalcNormals();

	//==========--------------------------  Step 5: Make arrays so we can send the triangles in one batch

	delete [] m_d3dTriList;
	if( m_pVertexBuffer )
		m_pVertexBuffer->Release();
	m_pVertexBuffer = NULL;

	GenD3DData();

	return resAllGood;
}

point3 cSubDivSurf::sEdge::CalcButterfly()
{
	point3 out = point3::Zero;

	sVert* other[2];
	other[0] = GetOtherVert( m_v[0], m_v[1], NULL );
	other[1] = GetOtherVert( m_v[0], m_v[1], other[0] );

	// two main ones
	out += (1.f/2.f) * m_v[0]->m_vert.loc;
	out += (1.f/2.f) * m_v[1]->m_vert.loc;

	// top/bottom ones
	out += (1.f/8.f) * other[0]->m_vert.loc;
	out += (1.f/8.f) * other[1]->m_vert.loc;

	// outside 4 verts
	out += (-1.f/16.f) * GetOtherVert( other[0], m_v[0], m_v[1] )->m_vert.loc;
	out += (-1.f/16.f) * GetOtherVert( other[0], m_v[1], m_v[0] )->m_vert.loc;
	out += (-1.f/16.f) * GetOtherVert( other[1], m_v[0], m_v[1] )->m_vert.loc;
	out += (-1.f/16.f) * GetOtherVert( other[1], m_v[1], m_v[0] )->m_vert.loc;

	return out;
}

point3 cSubDivSurf::sEdge::CalcKVert(int prim, int sec)
{
	int valence = m_v[prim]->Valence();

	point3 out = point3::Zero;

	out += (3.f / 4.f) * m_v[prim]->m_vert.loc;

	if( valence < 3 )
		assert( false );

	else if( valence == 3 )
	{
		for( int i=0; i<m_v[prim]->m_edgeList.size(); i++ )
		{
			sVert* pOther = m_v[prim]->m_edgeList[i]->Other( m_v[prim] );
			if( pOther == m_v[sec] )
				out += (5.f/12.f) * pOther->m_vert.loc;
			else
				out += (-1.f/12.f) * pOther->m_vert.loc;
		}
	}

	else if( valence == 4 )
	{
		out += (3.f/8.f) * m_v[sec]->m_vert.loc;

		sVert* pTemp = GetOtherVert( m_v[0], m_v[1], NULL );
		// get the one after it
		sVert* pOther = GetOtherVert( m_v[prim], pTemp, m_v[sec] );

		out += (-1.f/8.f) * pOther->m_vert.loc;
	}

	else // valence >= 5 
	{
		sVert* pCurr = m_v[sec];
		sVert* pLast = NULL;
		sVert* pTemp;
		for( int i=0; i< valence; i++ )
		{
			float weight = 
				((1.f/4.f) + (float)cos( 2 * PI * (float)i / (float)valence ) + 
				 (1.f/2.f) * (float)cos(4*PI*(float)i/(float)valence)) / (float)valence;

			out += weight * pCurr->m_vert.loc;

			pTemp = GetOtherVert( m_v[prim], pCurr, pLast );
			pLast = pCurr;
			pCurr = pTemp;
		}
	}
	return out;
}


void cSubDivSurf::CalcNormals()
{
	int i;

	// reset all vertex normals
	for( i=0; i<m_nVerts; i++ )
	{
		m_pVList[i].m_vert.norm = point3::Zero;
	}

	// find all triangle normals
	for( i=0; i<m_nTris; i++ )
	{
		m_pTList[i].m_normal = plane3(
			m_pTList[i].m_v[0]->m_vert.loc,
			m_pTList[i].m_v[1]->m_vert.loc,
			m_pTList[i].m_v[2]->m_vert.loc).n;

		// add the normal to each vertex
		m_pTList[i].m_v[0]->m_vert.norm += m_pTList[i].m_normal;
		m_pTList[i].m_v[1]->m_vert.norm += m_pTList[i].m_normal;
		m_pTList[i].m_v[2]->m_vert.norm += m_pTList[i].m_normal;
	}

	// reset all vertex normals
	for( i=0; i<m_nVerts; i++ )
	{
		m_pVList[i].m_vert.norm.Normalize();
	}

}

void cSubDivSurf::GenD3DData()
{

	/**
	 * Create a vertex buffer
	 */

	HRESULT hr;
	hr = Graphics()->GetDevice()->CreateVertexBuffer(
		m_nVerts * sizeof( sVertex ),
		D3DUSAGE_WRITEONLY,
		D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1,
		D3DPOOL_DEFAULT,
		&m_pVertexBuffer );

    if( FAILED( hr ))
	{
        throw cGameError("Vertex Buffer creation failed!\n");
	}

	m_d3dTriList = new sTri[ m_nTris ];

	sVertex* pVert;

	// Lock the vertex buffer
	hr = m_pVertexBuffer->Lock(
		0,
		0,
		(BYTE**)&pVert,
		0 );

	if( FAILED( hr ))
	{
		throw cGameError("VB Lock failed\n");
	}

	int i;

	// Copy data into the buffer
	for( i=0; i<m_nVerts; i++ )
	{
		*pVert++ = m_pVList[i].m_vert;
	}
	m_pVertexBuffer->Unlock();

	for( i=0; i<m_nTris; i++ )
	{
		m_d3dTriList[i].v[0] = m_pTList[i].m_v[0]->m_index;
		m_d3dTriList[i].v[1] = m_pTList[i].m_v[1]->m_index;
		m_d3dTriList[i].v[2] = m_pTList[i].m_v[2]->m_index;
	}
}

