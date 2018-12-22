/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: Cell.cpp
 *    Desc: cell code for path-finding AI demo
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"
#include <assert.h>
#include "cell.h"
#include <wingdi.h>
#include <mmsystem.h>

// change these values to draw additional pieces of the world
bool g_bDrawPortals = false;
bool g_bDrawGraph = false;

cWorld::cWorld( char* filename )
{
	FILE* fp = fopen( filename, "r" );

	int nCells;

	// Read in the # of cells
	fscanf(fp, "%i\n", &nCells);

	int i;

	for( i=0; i<nCells; i++ )
	{
		// read in the leading \n
		fscanf(fp, "\n");

		cCell* pNew = new cCell( this );
		int nVerts, nWalls, nDoors, nItems;

		fscanf( fp, "%i %i %i %i\n", &nVerts, &nWalls, &nDoors, &nItems );

		int i2;
		// read in the verts
		for( i2=0; i2<nVerts; i2++ )
		{
			point2 currPt;
			fscanf(fp, "%f %f\n", &currPt.x, &currPt.y );
			if( currPt.x < pNew->m_min.x ) pNew->m_min.x = currPt.x;
			if( currPt.x > pNew->m_max.x ) pNew->m_max.x = currPt.x;
			if( currPt.y < pNew->m_min.y ) pNew->m_min.y = currPt.y;
			if( currPt.y > pNew->m_max.y ) pNew->m_max.y = currPt.y;
			pNew->m_vertList.push_back( currPt );
		}
		
		// read in the walls
		for( i2=0; i2<nWalls; i2++ )
		{
			sCellWall currWall;
			fscanf(fp, "%i %i\n", &currWall.v0, &currWall.v1 );
			pNew->m_wallList.push_back( currWall );
		}

		// read in the doors
		for( i2=0; i2<nDoors; i2++ )
		{
			sCellDoor currDoor;
			fscanf(fp, "%i %i %i\n", &currDoor.v0, &currDoor.v1, &currDoor.otherCell );
			pNew->m_doorList.push_back( currDoor );
		}

		m_cellList.push_back( pNew );

	}
	fclose( fp );

	// now that we built the world, build the graph.
	for( i=0; i<m_cellList.size(); i++ )
	{
		// add a node for the centroid of the cell
		point2 center = m_cellList[i]->FindCenter();

		int i2;
		// now step around the doors, creating nodes and making edges.
		for( i2=0; i2<m_cellList[i]->m_doorList.size(); i2++ )
		{

			cNode* doorNode;

			// find the opposing door, so we only create the door node once.
			sCellDoor* otherDoor = FindOtherDoor( i, i2 );
			
			if( !otherDoor ) 
				continue;
			
			if( otherDoor->doorNode )
			{
				// it's already created.
				doorNode = otherDoor->doorNode;
			}
			else
			{
				// we need to create it.
				point2 loc = m_cellList[i]->m_vertList[ m_cellList[i]->m_doorList[i2].v0 ] +
							 m_cellList[i]->m_vertList[ m_cellList[i]->m_doorList[i2].v1 ];
				loc = loc / 2;
				doorNode = new cNode( NULL, loc );
			}
			m_cellList[i]->m_doorList[i2].doorNode = doorNode;
			m_nodeList.push_back( doorNode );

		}

		// Now that all the door nodes, step through *again*, and throw edges across the doors
		for( i2=0; i2<m_cellList[i]->m_doorList.size(); i2++ )
		{
			for( int i3=0; i3<m_cellList[i]->m_doorList.size(); i3++ )
			{
				if( i3 != i2 )
				{
					// throw an edge across.
					cEdge* edge1 = new cEdge(	m_cellList[i],
												m_cellList[i]->m_doorList[i2].doorNode, 
												m_cellList[i]->m_doorList[i3].doorNode );
					m_cellList[i]->m_doorList[i2].doorNode->AddOutgoingEdge( edge1 );
				}
			}
		}
	}
}


void cCreature::Erase( HDC hdc, int xCenter, int yCenter, float scale )
{
	static HPEN whitePen = CreatePen( PS_SOLID, 2,  RGB(0xff,0xff,0xff) );

	SelectObject(hdc, whitePen );

	// draw ourselves.
	int x0, y0;

	x0 = xCenter + (int)(scale* m_loc.x);
	y0 = yCenter - (int)(scale* m_loc.y);

	int radius = (int)(scale/4);

	Ellipse( hdc, x0 - radius, y0 - radius, x0 + radius, y0 + radius );
}


void cCreature::Draw( HDC hdc, int xCenter, int yCenter, float scale )
{
	static HPEN redPen = CreatePen( PS_SOLID, 2,  RGB(0xff,00,00) );

	SelectObject(hdc, redPen );

	// draw ourselves.
	int x0, y0;

	x0 = xCenter + (int)(scale* m_loc.x);
	y0 = yCenter - (int)(scale* m_loc.y);

	int radius = (int)(scale/4);

	Ellipse( hdc, x0 - radius, y0 - radius, x0 + radius, y0 + radius );
}


void cCell::Draw( HDC hdc, int xCenter, int yCenter, float scale )
{
	static HPEN blackPen = CreatePen( PS_SOLID, 2,  RGB(00,00,00) );
	static HPEN bluePen = CreatePen( PS_SOLID, 2,  RGB(00,00,0xFF) );

	int i;

	if( g_bDrawPortals )
	{
		SelectObject(hdc, bluePen );
		for( i=0; i<m_doorList.size(); i++ )
		{
			int x0, x1, y0, y1;

			x0 = xCenter + (int)(scale* m_vertList[ m_doorList[i].v1 ].x);
			y0 = yCenter - (int)(scale* m_vertList[ m_doorList[i].v1 ].y);
			x1 = xCenter + (int)(scale* m_vertList[ m_doorList[i].v0 ].x);
			y1 = yCenter - (int)(scale* m_vertList[ m_doorList[i].v0 ].y);
			MoveToEx( hdc, x0, y0, NULL );
			LineTo( hdc, x1, y1 );
		}
	}

	SelectObject(hdc, blackPen );
	for( i=0; i<m_wallList.size(); i++ )
	{
		int x0, x1, y0, y1;
		x0 = xCenter + (int)(scale* m_vertList[ m_wallList[i].v1 ].x);
		y0 = yCenter - (int)(scale* m_vertList[ m_wallList[i].v1 ].y);
		x1 = xCenter + (int)(scale* m_vertList[ m_wallList[i].v0 ].x);
		y1 = yCenter - (int)(scale* m_vertList[ m_wallList[i].v0 ].y);
		MoveToEx( hdc, x0, y0, NULL );
		LineTo( hdc, x1, y1 );
	}
}

void cWorld::Draw( HDC hdc, int xCenter, int yCenter, float scale )
{
	int i;

	// draw the graph first
	if( g_bDrawGraph )
	{
		for( i=0; i<m_nodeList.size(); i++ )
			m_nodeList[i]->Draw( hdc, xCenter, yCenter, scale );
	}

	for( i=0; i<m_cellList.size(); i++ )
		m_cellList[i]->Draw( hdc, xCenter, yCenter, scale );
}



void cNode::Draw( HDC hdc, int xCenter, int yCenter, float scale )
{
	static HPEN greenPen = CreatePen( PS_SOLID, 2,  RGB(00,0xFF,00) );

	SelectObject(hdc, greenPen );

	// draw ourselves.
	int x0, y0;

	x0 = xCenter + (int)(scale* m_loc.x);
	y0 = yCenter - (int)(scale* m_loc.y);

	Ellipse( hdc, x0 - 5, y0 - 5, x0 + 5, y0 + 5 );

	for( int i=0; i< m_edgeList.size(); i++ )
	{
		m_edgeList[i]->Draw( hdc, xCenter, yCenter, scale );
	}
}


void cEdge::Draw( HDC hdc, int xCenter, int yCenter, float scale )
{
	// draw ourselves.
	static HPEN greenPen = CreatePen( PS_SOLID, 2,  RGB(00,0xFF,00) );
	SelectObject(hdc, greenPen );

	int x0, x1, y0, y1;

	x0 = xCenter + (int)(scale* m_pFrom->m_loc.x);
	y0 = yCenter - (int)(scale* m_pFrom->m_loc.y);
	x1 = xCenter + (int)(scale* m_pTo->m_loc.x);
	y1 = yCenter - (int)(scale* m_pTo->m_loc.y);
	MoveToEx( hdc, x0, y0, NULL );
	LineTo( hdc, x1, y1 );
}


void cWorld::InitShortestPath()
{
	// initialize all nodes to have (relatively) infinite cost
	for( int i=0; i<m_nodeList.size(); i++ )
	{
		m_nodeList[i]->m_fCost	= REALLY_BIG;
		m_nodeList[i]->m_bVisited = false;
	}
}

cNode* cWorld::FindCheapestNode()
{
	// ideally, we would implement a slightly more advanced
	// data structure to hold the nodes, like a heap.
	// since our levels are so simple, we can deal with a
	// linear algorithm.

	float fBestCost = REALLY_BIG;
	cNode* pOut = NULL;
	for( int i=0; i<m_nodeList.size(); i++ )
	{
		if( !m_nodeList[i]->m_bVisited )
		{
			if( m_nodeList[i]->m_fCost < fBestCost ) // new cheapest node
			{
				fBestCost = m_nodeList[i]->m_fCost;
				pOut = m_nodeList[i];
			}
		}
	}

	// if we haven't found a node yet, something is wrong with the graph.
	assert( pOut );

	return pOut;
}

void cNode::Relax()
{
	this->m_bVisited = true;

	for( int i=0; i<m_edgeList.size(); i++ )
	{
		cEdge* pCurr = m_edgeList[i];
		if( pCurr->m_fWeight + this->m_fCost < pCurr->m_pTo->m_fCost )
		{
			// relax the 'to' node
			pCurr->m_pTo->m_pPrev = this;
			pCurr->m_pTo->m_fCost = pCurr->m_fWeight + this->m_fCost;
		}
	}
}

void cWorld::ShortestPath( sPath* pPath, cNode *pTo, cNode* pFrom )
{

	// easy out.
	if( pTo == pFrom ) return;

	InitShortestPath();

	pFrom->m_fCost = 0.f;

	bool bDone = false;
	cNode* pCurr;
	while( 1 )
	{
		pCurr = FindCheapestNode();
		if( !pCurr )
			return; // no path can be found.
		if( pCurr == pTo )
			break; // We found the shortest path

		pCurr->Relax(); // relax this node
	}

	// now we construct the path.

	// empty the path first.
	while( !pPath->m_nodeStack.empty() ) pPath->m_nodeStack.pop();

	pCurr = pTo;
	while( pCurr != pFrom )
	{
		pPath->m_nodeStack.push( pCurr );
		pCurr = pCurr->m_pPrev;
	}
}

cNode* cWorld::FindCellNode( int cellIndex )
{
	return m_cellList[cellIndex]->m_pNode;
}


void cCreature::Walk()
{
	// determine the time delta
	static float fLastTime = (float)timeGetTime();
	float fCurrTime = (float)timeGetTime();

	float fTimeDelta = fCurrTime - fLastTime;
	fLastTime = fCurrTime;

	if( fTimeDelta < 0.0001 ) fTimeDelta = 0.001f; // no good.

	float vMult = 30.f * fTimeDelta / (1000.f);

	if( vMult > 1.f ) vMult = 1.f;

	if( m_bFollowingPath )
	{
		// see if there are any more nodes.
		if( m_path.m_nodeStack.empty() )
		{
			m_bFollowingPath = false;
			return;
		}
		cNode* pNode = m_path.m_nodeStack.top();

		float distToNode = point2::Dist( pNode->m_loc, this->m_loc );

		// if we're closer than 0.05, pop it off the stack
		if( distToNode < 0.05f )
		{
			m_path.m_nodeStack.pop();
		}
		else
		{
			// move towards it.
			point2 dir = pNode->m_loc - this->m_loc;
			dir.Normalize();
			
			if( distToNode < vMult * SPEED )
			{
				m_loc = m_loc + distToNode * dir;
			}
			else
			{
				m_loc = m_loc + vMult * SPEED * dir;
			}
		}
	}
}

void cNode::AddDualEdge( cCell* pCell, cNode* to, cNode* from )
{
	cEdge* newEdge1 = new cEdge( pCell, to, from );
	to->AddOutgoingEdge( newEdge1 );

	cEdge* newEdge2 = new cEdge( pCell, from, to );
	from->AddOutgoingEdge( newEdge2 );
}


cNode* cCell::AddTempNode( point2& loc )
{
	// create the node, then throw edges to each of the other nodes in this cell.
	cNode* out = new cNode( this, loc );

	for( int i=0; i< m_doorList.size(); i++ )
	{
		cNode::AddDualEdge( this, out, m_doorList[i].doorNode );
	}

	return out;
}


cNode::~cNode()
{
	// delete all of our outgoing edges.
	int loop = m_edgeList.size();
	while( loop-- )
		delete m_edgeList[ loop ];
}


cWorld::~cWorld()
{
	// delete all of our nodes (deleting them will in turn delete all of the
	// edges in the graph
	int loop = m_nodeList.size();
	while( loop-- )
		delete m_nodeList[ loop ];

	loop = m_cellList.size();
	while( loop-- )
		delete m_cellList[ loop ];
}