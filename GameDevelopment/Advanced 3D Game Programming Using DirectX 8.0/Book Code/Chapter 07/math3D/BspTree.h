/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: BspTree.h
 *    Desc: BSP tree code, node-based construction, 3D only
 *          uses dynamic polygons
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _BSPTREE_H
#define _BSPTREE_H

#include "point3.h"
#include "polygon.h"
#include "plane3.h"

#include <vector>
using std::vector;

const float percentageToCheck = .1f; // 10%

/**
 * This code expects the set of polygons we're giving it to be
 * closed, forming a continuous skin.  If it's not, weird things
 * may happen.
 */
class cBspTree
{
public:

	// construction/destruction
	cBspTree();
	~cBspTree();

	// we need to handle copying
	cBspTree( const cBspTree &in );
	cBspTree& operator=( const cBspTree &in );

	// add a polygon to the tree
	void AddPolygon( const polygon<point3>& in );
	void AddPolygonList( vector< polygon<point3> >& in );

	void TraverseTree( vector< polygon<point3>* >* polyList, const point3& loc );

	bool LineOfSight( const point3& a, const point3& b );

protected:

private:


	class cNode
	{

		cNode*		m_pFront; // pointer to front subtree
		cNode*		m_pBack; // pointer to back subtree

		polygon<point3>	m_poly; // 
		plane3		m_plane;
		bool		m_bIsLeaf;
		bool		m_bIsSolid;
		
		// fixme: should be 2 lists, parallel and anti-parallel
		vector< polygon<point3> >	m_coplanarList; // list of coplanar polygons

		static int BestIndex( vector< polygon<point3> >& polyList );

	public:
		cNode( bool bIsSolid ); // leaf constructor
		cNode( const polygon<point3>& in ); // node constructor
		cNode( vector< polygon<point3> >& in ); // node constructor
		~cNode();

		// we need to handle copying
		cNode( const cNode &in );
		cNode& operator=( const cNode &in );

		void AddPolygon( const polygon<point3>& in );

		void TraverseTree( vector< polygon<point3>* >* polyList, const point3& loc );

		bool IsLeaf()
		{
			return m_bIsLeaf;
		}

		bool LineOfSight( const point3& a, const point3& b );
	};

	cNode* m_pHead; // root node of the tree

};


inline cBspTree::cBspTree( const cBspTree &in )
{
	// clone the tree
	if( in.m_pHead )
		m_pHead = new cNode( *in.m_pHead );
	else
		m_pHead = NULL;
}

inline cBspTree& cBspTree::operator=( const cBspTree &in )
{
	if( &in != this )
	{
		// delete the tree if we have one already
		if( m_pHead )
			delete m_pHead;

		// clone the tree
		if( in.m_pHead )
			m_pHead = new cNode( *in.m_pHead );
		else
			m_pHead = NULL;
	}
		
	return *this;
}


inline cBspTree::cNode::cNode( const cNode &in )
{
	m_poly = in.m_poly;
	m_plane = in.m_plane;
	m_bIsLeaf = in.m_bIsLeaf;
	m_bIsSolid = in.m_bIsSolid;

	// clone the trees
	m_pFront = NULL;
	if( in.m_pFront )
		m_pFront = new cNode( *in.m_pFront );

	m_pBack = NULL;
	if( in.m_pBack )
		m_pBack = new cNode( *in.m_pBack );
}


inline cBspTree::cNode& cBspTree::cNode::operator=( const cNode &in )
{
	if( &in != this )
	{
		// delete the subtrees if we have them already
		if( m_pFront )
			delete m_pFront;
		if( m_pBack )
			delete m_pBack;

		// copy all the data over
		m_poly = in.m_poly;
		m_plane = in.m_plane;
		m_bIsLeaf = in.m_bIsLeaf;
		m_bIsSolid = in.m_bIsSolid;

		// clone the trees
		m_pFront = NULL;
		if( in.m_pFront )
			m_pFront = new cNode( *in.m_pFront );

		m_pBack = NULL;
		if( in.m_pBack )
			m_pBack = new cNode( *in.m_pBack );
	}
	return *this;
}

#endif //_BSPTREE_H