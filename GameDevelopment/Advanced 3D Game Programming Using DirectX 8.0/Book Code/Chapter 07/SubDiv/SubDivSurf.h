/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: SubDivSurf.h
 *    Desc: Class to handle subdivision surfaces.
 *          Loads .o3d files
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _SUBDIVSURF_H
#define _SUBDIVSURF_H

#include <vector>
#include <algorithm>

#include <assert.h>
#include <d3d.h>

#include "..\math3d\tri.h"

typedef tri<unsigned short> sTri;

class cSubDivSurf  
{
	struct sVert;
	struct sEdge;
	struct sTriangle;

	int			m_nVerts;
	sVert*		m_pVList;

	int			m_nEdges;
	sEdge*		m_pEList;

	int			m_nTris;
	sTriangle*	m_pTList;

	/**
	 * These fields are to accelerate drawing with indexed primitives
	 */
	sTri*	m_d3dTriList;
	LPDIRECT3DVERTEXBUFFER8 m_pVertexBuffer;

	/**
	 * Subdivision Surface vertex (name 'sVertex' is used in D3D code)
	 */
	struct sVert
	{
		/**
		 * These two arrays describe the adjascency information 
		 * for a vertex. Each vertex knows who all of it's neighboring 
		 * edges and triangles are. an important note is that these 
		 * lists aren't sorted.  We need to search through the list 
		 * when we need to get a specific adjascent triangle.
		 * This is, of course, inefficient.  Consider sorted insertion
		 * an excercise to the reader.
		 */
		std::vector< sTriangle* >	m_triList;
		std::vector< sEdge* >		m_edgeList;

		/**
		 * position/normal information for the vertex
		 */
		sVertex	m_vert;

		/**
		 * Each Vertex knows it's position in the array it lies in.  
		 * This helps when we're constructing the arrays of subdivided data.
		 */
		int		m_index;

		void AddEdge( sEdge* pEdge )
		{
			assert( 0 == std::count( 
				m_edgeList.begin(), 
				m_edgeList.end(), 
				pEdge ) );
			m_edgeList.push_back( pEdge );
		}

		void AddTri( sTriangle* pTri )
		{
			assert( 0 == std::count( 
				m_triList.begin(), 
				m_triList.end(), 
				pTri ) );
			m_triList.push_back( pTri );
		}

		/**
		 * Valence == How many other vertices are connected to this one
		 * which said another way is how many edges the vert has.
		 */
		int Valence()
		{
			return m_edgeList.size();
		}

		sVert() : 
			m_triList( 0 ),
			m_edgeList( 0 )
		{
		}

		/**
		 * Given a Vertex that we know we are attached to, this function 
		 * searches the list of adjascent edges looking for the one that 
		 * contains the imputted vertex.  Asserts if there is no edge for 
		 * that vertex.
		 */
		sEdge*	GetEdge( sVert* pOther )
		{
			for( int i=0; i<m_edgeList.size(); i++ )
			{
				if( m_edgeList[i]->Contains( pOther ) )
					return m_edgeList[i];
			}
			assert(false); // didn't have it!
			return NULL;
		}
	};
	
	/**
	 * Edge structure that connects two vertices in a SubSurf
	 */
	struct sEdge
	{
		sVert*	m_v[2];

		/**
		 * When we perform the subdivision calculations on all the edges
		 * the result is held in this newVLoc strucure.  Never has any
		 * connectivity information, just location and color.
		 */
		sVert		m_newVLoc;

		/**
		 * true == one of the edges' vertices is the inputted vertex
		 */
		bool Contains( sVert* pVert )
		{
			return (m_v[0] == pVert) || m_v[1] == pVert;
		}

		/**
		 * retval = the other vertex than the inputted one
		 */
		sVert* Other( sVert* pVert )
		{
			return (m_v[0] == pVert) ? m_v[1] : m_v[0];
		}

		void Init( sVert* v0, sVert* v1 )
		{
			m_v[0] = v0;
			m_v[1] = v1;

			/**
			 * Note that the edge notifies both of it's vertices that it's 
			 * connected to them.
			 */
			m_v[0]->AddEdge( this );
			m_v[1]->AddEdge( this );
		}

		/**
		 * This function takes into consideration the two triangles that
		 * share this edge.  It returns the third vertex of the first 
		 * triangle it finds that is not equal to 'notThisOne'.  So if
		 * want one, notThisOne is passed as NULL.  If we want the other
		 * one, we pass the result of the first execution.
		 */
		sVert* GetOtherVert( sVert* v0, sVert* v1, sVert* notThisOne )
		{
			sTriangle* pTri;
			for( int i=0; i<v0->m_triList.size(); i++ )
			{
				pTri = v0->m_triList[i];
				if( pTri->Contains( v0 ) && pTri->Contains( v1 ) )
				{
					if( pTri->Other( v0, v1 ) != notThisOne )
						return pTri->Other( v0, v1 );
				}
			}
			// when we support boundary edges, we shouldn't assert
			assert(false);
			return NULL;
		}

		/**
		 * Calculate the K-Vertex location of 'prim' vertex.  For triangles
		 * of valence !=6
		 */
		point3 CalcKVert( int prim, int sec );
		/**
		 * Calculate the location of the subdivided point using the 
		 * butterfly method.  
		 * for edges with both vertices of valence == 6
		 */
		point3 CalcButterfly();
	};

	/**
	 * Subdivision surface triangle
	 */
	struct sTriangle
	{
		/**
		 * The three vertices of this triangle
		 */
		sVert*	m_v[3];
		point3	m_normal;

		void Init( sVert* v0, sVert* v1, sVert* v2 )
		{
			m_v[0] = v0;
			m_v[1] = v1;
			m_v[2] = v2;

			/**
			 * Note that the triangle notifies all 3 of it's vertices 
			 * that it's connected to them.
			 */
			m_v[0]->AddTri( this );
			m_v[1]->AddTri( this );
			m_v[2]->AddTri( this );
		}

		/**
		 * true == the triangle contains the inputted vertex
		 */
		bool Contains( sVert* pVert )
		{
			return pVert == m_v[0] || pVert == m_v[1] || pVert == m_v[2];
		}

		/**
		 * retval = the third vertex (first and second are inputted).
		 * asserts out if inputted values aren't part of the triangle
		 */
		sVert* Other( sVert* v1, sVert* v2 )
		{
			assert( Contains( v1 ) && Contains( v2 ) );
			for( int i=0; i<3; i++ )
			{
				if( m_v[i] != v1 && m_v[i] != v2 )
					return m_v[i];
			}
			assert(false); // something bad happened;
			return NULL;
		}
	};

	/**
	 * Called by Subdivide.  Steps through the edges and calculates
	 * the new vertex locations
	 */
	void GenNewVertLocs();


	void CalcNormals();
	void GenD3DData();

public:

	/**
	 * This function performs a destructive subdivision of a model.
	 * To be able to return to the original model, we need to cache
	 * an extra copy and hold it around.  For the purposes of this sample
	 * this isn't required, but it would be a nice feature.
	 * retval of res_False means that it maxed out the # of vertices D3D
	 * can handle
	 */
	eResult Subdivide();

	cSubDivSurf( const char* filename );
	virtual ~cSubDivSurf();

	void Draw( matrix4& mat );

	/**
	 * Other high-level functions can ask us what our triangle count is
	 */
	int NumTris()
	{
		return m_nTris;
	}

};

#endif // _SUBDIVSURF_H
