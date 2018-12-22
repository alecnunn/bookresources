/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: Model.h
 *    Desc: Generic model class, loads .03D files
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _MODEL_H
#define _MODEL_H

#include <vector>
#include <string>
#include "..\math3d\tri.h"
#include "..\math3d\mathD3D.h"


#define FVF_TYPE ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 )


class cModel  
{
	struct vertex
	{
		point3 loc;
		point3 norm;
		ulong diff;
		float u, v;
	};


	typedef tri<unsigned short> sTri;

	std::vector< sTri >		m_tris;
	std::vector< vertex >	m_verts;

	std::string				m_name;



public:

	cModel( const char* filename );
	cModel( const char* name, int nVerts, int nTris );


	float GenRadius();
	void Scale( float amt );

	void Draw( const matrix4& mat );

	//==========--------------------------  Access functions.

	int NumVerts(){ return m_verts.size(); }
	int NumTris(){ return m_tris.size(); }
	const char* Name(){ return m_name.c_str(); }

	/**
	 * Some other classes may end up using cModel
	 * to assist in their file parsing.  Because of this
	 * give them a way to get at the vertex and triangle 
	 * data.
	 */
	vertex* VertData(){ return &m_verts[0]; }
	sTri* TriData(){ return &m_tris[0]; }

};

#endif // _MODEL_H
