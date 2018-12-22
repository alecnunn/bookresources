/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: Model.cpp
 *    Desc: Generic model class, loads .o3d files
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"
#include "Model.h"
#include "file.h"

using namespace std;

cModel::cModel( const char* name, int nVerts, int nTris ) :
	m_name( name )
{
	int i;

	m_verts.reserve( nVerts );
	vertex vert;
	for( i=0; i<nVerts; i++ )
	{
		m_verts.push_back( vert );
	}

	m_tris.reserve( nTris );
	sTri tri;
	for( i=0; i<nTris; i++ )
	{
		m_tris.push_back( tri );
	}
}


cModel::cModel( const char* filename )
{
	int i;

	cFile file;
	file.Open( filename );

	queue<string> m_tokens;

	file.TokenizeNextNCLine( &m_tokens, '#' );

	// first token is the name.
	m_name = m_tokens.front();
	m_tokens.pop();

	// next is the number of fields in the vertex info
	int nVertexFields = atoi( m_tokens.front().c_str() );
	m_tokens.pop();

	// next is the triangle offset
	int offset = atoi( m_tokens.front().c_str() );
	m_tokens.pop();

	// next is the number of vertices
	int nVerts = atoi( m_tokens.front().c_str() );
	m_tokens.pop();

	// next is the number of triangles
	int nTris = atoi( m_tokens.front().c_str() );
	m_tokens.pop();

	if( !m_tokens.empty() )
	{
		// additional info in the header
	}

	
	// Reserve space in the vector for all the verts.
	// This will speed up all the additions, since only
	// one resize will be done.
	m_verts.reserve( nVerts );
	for( i=0; i<nVerts; i++ )
	{
		//while( !m_tokens.empty() ) m_tokens.pop();
		file.TokenizeNextNCLine( &m_tokens, '#' );

		vertex curr;

		// Vertex data is guarenteed
		curr.loc.x = atof( m_tokens.front().c_str() );
		m_tokens.pop();
		curr.loc.y = atof( m_tokens.front().c_str() );
		m_tokens.pop();
		curr.loc.z = atof( m_tokens.front().c_str() );
		m_tokens.pop();

		// hack to load color
		if( nVertexFields == 4 )
		{
			curr.diff = atoi( m_tokens.front().c_str() );

			m_tokens.pop();
		}


		// Load normal data if nfields is 6 or 8
		if( nVertexFields == 6 || nVertexFields == 8 )
		{
			curr.norm.x = atof( m_tokens.front().c_str() );
			m_tokens.pop();
			curr.norm.y = atof( m_tokens.front().c_str() );
			m_tokens.pop();
			curr.norm.z = atof( m_tokens.front().c_str() );
			m_tokens.pop();
		}
		else
		{
			curr.norm.Assign( 0, 0, 0 );
		}

		// Load texture data if nfields is 5 or 8
		if( nVertexFields == 5 || nVertexFields == 8 )
		{
			curr.u = atof( m_tokens.front().c_str() );
			m_tokens.pop();
			curr.v = atof( m_tokens.front().c_str() );
			m_tokens.pop();
		}
		else
		{
			curr.u = 0.f;
			curr.v = 0.f;
		}

		m_verts.push_back( curr );
	}

	// Reserve space in the vector for all the verts.
	// This will speed up all the additions, since only
	// one resize will be done.
	m_tris.reserve( nTris );
	for( i=0; i<nTris; i++ )
	{
		m_tokens.empty();
		file.TokenizeNextNCLine( &m_tokens, '#' );

		sTri tri;

		// vertex data is guaranteed
		tri.v[0] = atoi( m_tokens.front().c_str() ) - offset;
		m_tokens.pop();
		tri.v[1] = atoi( m_tokens.front().c_str() ) - offset;
		m_tokens.pop();
		tri.v[2] = atoi( m_tokens.front().c_str() ) - offset;
		m_tokens.pop();

		m_tris.push_back( tri );
	}

	if( nVertexFields == 3 || nVertexFields == 4 || nVertexFields == 5 )
	{
		// Normals weren't provided.  Generate our own.

		// First set all the normals to zero.
		for( i=0; i<nVerts; i++ )
		{
			m_verts[i].norm.Assign( 0,0,0 );
		}

		// Then go through and add each triangle's normal 
		// to each of it's verts.
		for( i=0; i<nTris; i++ )
		{
			plane3 plane(
				m_verts[ m_tris[i].v[0] ].loc,
				m_verts[ m_tris[i].v[1] ].loc,
				m_verts[ m_tris[i].v[2] ].loc );

			m_verts[ m_tris[i].v[0] ].norm += plane.n;
			m_verts[ m_tris[i].v[1] ].norm += plane.n;
			m_verts[ m_tris[i].v[2] ].norm += plane.n;
		}

		// Finally normalize all of the normals
		for( i=0; i<nVerts; i++ )
		{
			m_verts[i].norm.Normalize();
		}

	}


}



void cModel::Scale( float amt )
{
	int size = m_verts.size();
	for( int i=0; i<size; i++ )
	{
		m_verts[i].loc *= amt;
	}
}


void cModel::Draw( const matrix4& mat )
{
	Graphics()->SetWorldMatrix( mat );

	Graphics()->GetDevice()->SetVertexShader( FVF_TYPE );

	Graphics()->GetDevice()->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST,
		0,
		m_verts.size(),
		m_tris.size(),
		&m_tris[0],
		D3DFMT_INDEX16,
		&m_verts[0],
		sizeof( vertex ) );	
}


float cModel::GenRadius()
{
	float best = 0.f;
	int size = m_verts.size();
	for( int i=0; i<size; i++ )
	{
		float curr = m_verts[i].loc.Mag();
		if( curr > best )
			best = curr;
	}
	return best;
}