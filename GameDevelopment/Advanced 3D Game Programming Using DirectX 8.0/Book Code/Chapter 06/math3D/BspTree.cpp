/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: BspTree.cpp
 *    Desc: BSP tree code, node-based construction, 3D only
 *          uses dynamic polygons
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include <assert.h>
#include "BspTree.h"

cBspTree::cBspTree()
{
}

cBspTree::~cBspTree()
{
	// destroy the tree
}


void cBspTree::AddPolygon( const polygon<point3>& in )
{
	if( !m_pHead )
	{
		// if there's no tree, make a new one
		m_pHead = new cNode( in );
	}
	else
	{
		// otherwise add it to the tree
		m_pHead->AddPolygon( in );
	}
}

void cBspTree::AddPolygonList( vector< polygon<point3> >& in )
{
	if( !m_pHead )
	{
		// if there's no tree, make a new one
		m_pHead = new cNode( in );
	}
	else
	{
		/**
		 * Adding a list of polygons to 
		 * an existing tree is unimplemented 
		 * (exercise to the reader)
		 */
		assert( false );
	}
}


void cBspTree::TraverseTree( vector<polygon<point3>*>* polyList, const point3& loc )
{
	if( m_pHead )
	{
		// drop it down
		m_pHead->TraverseTree( polyList, loc );
	}
}


bool cBspTree::LineOfSight( const point3& a, const point3& b )
{
	assert( m_pHead ); // make sure there is a tree to test against

	return m_pHead->LineOfSight( a, b );
}


cBspTree::cNode::~cNode()
{
	// we could run into some stack overflow problems, but
	// the tree will most likely not be deep enough for us to worry about it.
	delete m_pFront;
	delete m_pBack;
}


cBspTree::cNode::cNode( bool bIsSolid ) : 
	m_bIsLeaf( true ), 
	m_bIsSolid( bIsSolid ),
	m_pFront( NULL ),
	m_pBack( NULL )
{
	// all done.
}

cBspTree::cNode::cNode( const polygon<point3>& in ) : 
	m_bIsLeaf( false ), 
	m_poly( in ),
	m_plane( in ),
	m_pFront( new cNode( false ) ),
	m_pBack( new cNode( true ) )
{
	// all done.
}

cBspTree::cNode::cNode( vector< polygon<point3> >& in ) :
	m_bIsLeaf( false )
{

	// if the list is empty, we're bombing out.
	assert( in.size() );

	// get the best index to use as a splitting plane
	int bestIndex = BestIndex( in );

	// we could remove the index from the vector, but that's slow.  
	// instead we'll just kind of ignore it during the next phase.
	// remove the best index
	polygon<point3> splitPoly = in[bestIndex];

	m_plane = plane3( splitPoly );
	m_poly = splitPoly;

	// take the rest of the polygons and divide them.
	vector< polygon<point3> > frontList, backList;

	int i;
	for( i=0; i<in.size(); i++ )
	{
		// ignore the polygon if it's the one we're using as the splitting plane
		if( i == bestIndex ) continue;

		// test the polygon against this node.
		ePListLoc res = m_plane.TestPoly( in[i] );

		polygon<point3> front(in[i].nElem + 5), back(in[i].nElem + 5); // used in PLIST_SPLIT

		switch( res )
		{
		case plistFront:
			// drop down the front
			frontList.push_back( in[i] );
			break;
		case plistBack:
			// drop down the back
			backList.push_back( in[i] );
			break;
		case plistSplit:
			// split the polygon, drop the halves down.
			m_plane.Split( in[i], &front, &back );
			frontList.push_back( front );
			backList.push_back( back );
			break;
		case plistCoplanar:
			// add the polygon to this node's list
			m_coplanarList.push_back( in[i] );
			break;
		}
	}

	// we're done processing the polygon list  Deal with them.
	if( frontList.size() )
	{
		m_pFront = new cNode( frontList );
	}
	else
	{
		m_pFront = new cNode( false );
	}
	if( backList.size() )
	{
		m_pBack = new cNode( backList );
	}
	else
	{
		m_pBack = new cNode( true );
	}

}

void cBspTree::cNode::AddPolygon( const polygon<point3>& in )
{
	if( m_bIsLeaf )
	{
		// reinitialize ourselves as a node
		*this = cNode( in );
	}
	else
	{
		// test the polygon against this node.
		ePListLoc res = this->m_plane.TestPoly( in );

		polygon<point3> front(in.nElem+1), back(in.nElem+1); // used in PLIST_SPLIT
		switch( res )
		{
		case plistFront:
			// drop down the front
			m_pFront->AddPolygon( in );
			break;
		case plistBack:
			// drop down the back
			m_pBack->AddPolygon( in );
			break;
		case plistSplit:
			// split the polygon, drop the halves down.
			m_plane.Split( in, &front, &back );
			m_pFront->AddPolygon( front );
			m_pBack->AddPolygon( back );
			break;
		case plistCoplanar:
			// add the polygon to this node's list
			m_coplanarList.push_back( in );
			break;
		}
	}
}

void cBspTree::cNode::TraverseTree( vector< polygon<point3>* >* polyList, const point3& loc )
{
	if( m_bIsLeaf )
	{
		// do nothing.
	}
	else
	{
		// test the loc against the current node
		ePointLoc res = m_plane.TestPoint( loc );

		int i;
		switch( res )
		{
		case ptFront:
			// get back, us, front
			m_pBack->TraverseTree( polyList, loc );
			polyList->push_back( &m_poly ); // the poly at this node
			for( i=0; i<m_coplanarList.size(); i++ )
			{
				polyList->push_back( &m_coplanarList[i] );
			}
			m_pFront->TraverseTree( polyList, loc );
			break;

		case ptBack:
			// get front, us, back
			m_pFront->TraverseTree( polyList, loc );
			polyList->push_back( &m_poly ); // the poly at this node
			for( i=0; i<m_coplanarList.size(); i++ )
			{
				polyList->push_back( &m_coplanarList[i] );
			}
			m_pBack->TraverseTree( polyList, loc );
			break;

		case ptCoplanar:
			// get front, back, us
			m_pFront->TraverseTree( polyList, loc );
			m_pBack->TraverseTree( polyList, loc );
			polyList->push_back( &m_poly ); // the poly at this node
			for( i=0; i<m_coplanarList.size(); i++ )
			{
				polyList->push_back( &m_coplanarList[i] );
			}
			break;

		}
	}
}

int cBspTree::cNode::BestIndex( vector< polygon<point3> >& polyList )
{
	/**
	 * The current hueristic is blind least-split
	 */
	// run through the list, searching for the best one.
	int maxCheck; // the highest polygon we'll bother testing (10% of total)
	maxCheck = (int)(polyList.size() * percentageToCheck);
	if( !maxCheck ) maxCheck = 1;

	int i, i2;
	int bestSplits = 100000;
	int bestIndex = -1;
	int currSplits;
	plane3 currPlane;
	for( i=0; i<maxCheck; i++ )
	{
		currSplits = 0;
		currPlane = plane3( polyList[i] );
		ePListLoc res;

		for( i2=0; i2< polyList.size(); i2++ )
		{
			if( i == i2 ) continue;

			res = currPlane.TestPoly( polyList[i2] );
			if( res == plistSplit )
				currSplits++;
		}
		if( currSplits < bestSplits )
		{
			bestSplits = currSplits;
			bestIndex = i;
		}
	}
	assert( bestIndex >= 0 );
	return bestIndex;
}


bool cBspTree::cNode::LineOfSight( const point3& a, const point3& b )
{
	if( m_bIsLeaf )
	{
		return !m_bIsSolid; // if we land in a solid node, then there is no line of sight
	}

	// FIXME: not correctly handling both verts coplanar
	ePointLoc aLoc = m_plane.TestPoint( a );
	ePointLoc bLoc = m_plane.TestPoint( b );

	point3 split;

	if( aLoc == ptCoplanar && bLoc == ptCoplanar )
	{
		// for sake of something better to do, be conservative
		//return false;
		return m_pFront->LineOfSight( a, b );
	}

	if( aLoc == ptFront && bLoc == ptBack )
	{
		//split, then return the logical 'or' of both sides
		split = m_plane.Split( a, b );

		return m_pFront->LineOfSight( a, split ) && m_pBack->LineOfSight( b, split );

	}

	if( aLoc == ptBack && bLoc == ptFront )
	{
		//split, then return the logical 'or' of both sides
		split = m_plane.Split( a, b );

		return m_pFront->LineOfSight( b, split ) && m_pBack->LineOfSight( a, split );
	}


	if( aLoc == ptFront || bLoc == ptFront ) // the other == POINT_COLPLANAR or POINT_FRONT
	{
		// drop down the front
		return m_pFront->LineOfSight( a, b );
	}

	else // they're both on the back side
	{
		// drop down the front
		return m_pBack->LineOfSight( a, b );
	}


	return true;
}

