/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: Cell.h
 *    Desc: Code that defines cells and the nodes/edges between them
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _CELL_H
#define _CELL_H

#include <vector>
#include <stack>
using namespace std;

#include <windows.h>
#include <math.h>
#include "point2.h"

#define REALLY_BIG 999999.9f

#define PI 3.14159f

#define MAX_ANGLE (PI/1)
#define SPEED 0.22f

// globals
extern bool g_bDrawWalls;
extern bool g_bDrawPortals;
extern bool g_bDrawGraph;


// structures for the mad cell action

class cCell;
class cWorld;
class cEdge;
class cNode;
class cCreature;
struct sPath;



struct sCellWall
{
	int v0, v1;
};

struct sCellDoor
{
	int v0, v1;
	int otherCell;

	cNode* doorNode;

	sCellDoor() : doorNode( NULL ) {}
};

class cCell
{
	vector< point2 > m_vertList;
	vector< sCellWall > m_wallList;
	vector< sCellDoor > m_doorList;

	point2 m_min, m_max;

	cNode* m_pNode; // this is to help the demo

	cWorld* m_pParent; // a pointer to the world that contains us

public:

	cCell( cWorld* pParent )
	: m_min( 1000, 1000 )
	, m_max( -1000, -1000 )
	, m_pParent( pParent)
	{
	}

	~cCell()
	{
		// we don't have to do anything
	}

	void Draw( HDC hdc, int xCenter, int yCenter, float scale );

	point2 FindCenter()
	{
		point2 out( 0.f, 0.f );
		for( int i=0; i<m_vertList.size(); i++ )
		{
			out = out + m_vertList[i];
		}
		out = out / (float) i;
		return out;
	}

	bool ContainsPoint( point2 &pt )
	{
		if( pt.x > m_max.x ) return false;
		if( pt.x < m_min.x ) return false;
		if( pt.y > m_max.y ) return false;
		if( pt.y < m_min.y ) return false;
		return true;
	}

	cNode* AddTempNode( point2& loc );

	friend class cWorld;
};

class cWorld
{
	vector< cCell* > m_cellList;
	vector< cNode* > m_nodeList;
	vector< cEdge* > m_edgeList; // just referenced, we never delete the contents (nodes do that)
public:
	cWorld( char* filename );
	~cWorld();
	
	void Draw( HDC hdc, int xCenter, int yCenter, float scale );

	sCellDoor* FindOtherDoor( int cellIndex, int doorIndex )
	{
		// could theoretically return NULL if it's a one way door.
		cCell* pOtherCell = m_cellList[ m_cellList[ cellIndex ]->m_doorList[ doorIndex ].otherCell ];
		int otherIndex = m_cellList[ cellIndex ]->m_doorList[ doorIndex ].otherCell;

		if( !pOtherCell ) return NULL; // bad door.

		int i;
		sCellDoor* out = NULL;
		for( i=0; i< pOtherCell->m_doorList.size(); i++ )
		{
			if( pOtherCell->m_doorList[i].otherCell == cellIndex ) // hit.
			{
				out = &pOtherCell->m_doorList[i];
				break;
			}
		}
		return out;
	}

	cNode* FindCellNode( int cellIndex );

	int FindContainingCell( point2 &pt )
	{
		for( int i=0; i<m_cellList.size(); i++ )
		{
			if( m_cellList[i]->ContainsPoint( pt ) )
				return i;
		}
		return -1;
	}

	cNode* AddTempNode( point2& loc )
	{
		int nodeIndex = FindContainingCell( loc );
		if( nodeIndex < 0 ) return NULL;

		cNode* newNode = m_cellList[ nodeIndex ]->AddTempNode( loc );
		m_nodeList.push_back( newNode );
		return newNode;
	}

	cNode* FindCheapestNode();
	void InitShortestPath();
	void ShortestPath( sPath* pPath, cNode *pTo, cNode* pFrom );

	friend class cCell;
};


/**
 * Graph code 
 */

class cEdge;

class cNode
{
	vector< cEdge* >	m_edgeList; // All outgoing edges from this node
	point2				m_loc;
	cCell*				m_pParent; // may be null (For doorway nodes)

public:

	float			m_fCost; // used in shortest path
	bool			m_bVisited;
	cNode*			m_pPrev; // previous node in the shortest path

	cNode( cCell* parent, point2 loc ) 
	: m_pParent( parent )
	, m_loc( loc )
	{
	}

	~cNode();

	void AddOutgoingEdge( cEdge* edge )
	{
		m_edgeList.push_back( edge );
	}

	static void AddDualEdge( cCell* pCell, cNode* to, cNode* from );

	void Draw( HDC hdc, int xCenter, int yCenter, float scale );

	void Relax();

	friend class cEdge;
	friend class cCreature;
};

class cEdge
{
	float m_fWeight;
	cNode *m_pFrom, *m_pTo;
	cCell *m_pCell; // each edge lies in exactly one cell.
public:

	cEdge( cCell* pCell, cNode* from, cNode* to)
	: m_pCell( pCell )
	, m_pFrom( from )
	, m_pTo( to )
	{
		// find the weight
		point2 distVec = from->m_loc - to->m_loc;
		
		m_fWeight = distVec.Length();
	}

	void Draw( HDC hdc, int xCenter, int yCenter, float scale );

	friend class cNode;
};

struct sPath
{
	stack< cNode* > m_nodeStack;
	void Clear()
	{ 
		while( !m_nodeStack.empty() )
			m_nodeStack.pop(); 
	}
	void Add( cNode* node ){ m_nodeStack.push(node); }
};


class cCreature
{

public:

	point2	m_loc;

	bool	m_bFollowingPath;
	sPath	m_path;

	void Draw( HDC hdc, int xCenter, int yCenter, float scale );
	void Erase( HDC hdc, int xCenter, int yCenter, float scale );

	void Walk();
};


#endif //_CELL_H
