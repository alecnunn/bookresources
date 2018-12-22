/* Copyright (C) William van der Sterren, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William van der Sterren, 2002"
 */

/* Copyright (C) James Matthews, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) James Matthews, 2001"
 */


//////////////////////////////////////////////////////////////////
// Class:	CAStar class (27/6/2001)
// File:	AStar.cpp
// Author:	James Matthews
//
// Implements the A* algorithm.
// 
//
// Please visit http://www.generation5.org/ for the latest
// in Artificial Intelligence news, interviews, articles and
// discussion forums.
//

#include <cmath>
#include "PathFinder.h"

CAStar::CAStar() 
: udTerrainCost(0),
  udTerrainHeuristic(0),
  udThreatCost(0),
  udThreatHeuristic(0)
{
	m_pOpen = m_pClosed = NULL;
	m_pStack = NULL;
	m_pBest = NULL;

	udValid = NULL;
	udNotifyChild = NULL;
	udNotifyList = NULL;
}

CAStar::~CAStar() 
{
	ClearNodes();
}

void CAStar::ClearNodes() 
{
	_asNode *temp = NULL, *temp2 = NULL;

	if (m_pOpen) {
		while (m_pOpen) {
			temp = m_pOpen->next;

			delete m_pOpen;

			m_pOpen = temp;
		}
	}

	if (m_pClosed) {
		while (m_pClosed) {
			temp = m_pClosed->next;

			delete m_pClosed;

			m_pClosed = temp;
		}
	}
}

/////////////////////////////////////////////////
// CAStar::GeneratePath(int, int, int, int)
//
// Main A* algorithm. The step functions are used
// to keep code redundancy to a minimum.
//

bool CAStar::GeneratePath(int sx, int sy, int dx, int dy) 
{
	StepInitialize(sx, sy, dx, dy);
	
	int retval = 0;
	while (retval == 0) {
		retval = Step();
	};
	
	if (retval == -1 || !m_pBest) {
		m_pBest = NULL;
		return false;
	}

	return true;
}

void CAStar::StepInitialize(int sx, int sy, int dx, int dy)
{
	ClearNodes();
	
	m_iSX = sx; m_iSY = sy; m_iDX = dx; m_iDY = dy;
	m_iDNum = Coord2Num(dx,dy);

	_asNode *temp = new _asNode(sx, sy);
  _asNode dest(dx, dy);

	temp->SetG(0);
	temp->SetH(udTerrainHeuristic(temp, &dest) + udThreatHeuristic(temp, &dest)); 
  temp->UpdateF();

	temp->SetID(Coord2Num(sx, sy));

	m_pOpen = temp;

	udFunc(udNotifyList, NULL, m_pOpen, ASNL_STARTOPEN, m_pNCData);
	udFunc(udNotifyChild, NULL, temp, 0, m_pNCData);
}

int CAStar::Step()
{
	if (!(m_pBest = GetBest()))
		return -1;

  // check for arrival at destination
	if ( m_pBest->GetID() == m_iDNum ) 
		return 1;

	CreateChildren(m_pBest);

	return 0;
}

_asNode *CAStar::GetBest() 
{
	if (!m_pOpen) return NULL;

	_asNode *temp = m_pOpen, *temp2 = m_pClosed;
	m_pOpen = temp->next;

	udFunc(udNotifyList, NULL, temp, ASNL_DELETEOPEN, m_pNCData);

	m_pClosed = temp;
	m_pClosed->next = temp2;

	udFunc(udNotifyList, NULL, m_pClosed, ASNL_ADDCLOSED, m_pNCData);

	return temp;
}

void CAStar::CreateChildren(_asNode *node) 
{
	_asNode temp;
	int x = node->GetX(); 
  int y = node->GetY();

	for (int i=-1;i<2;i++) {
		for (int j=-1;j<2;j++) {
			temp.SetXY(x+i, y+j);

			if (i == 0 && j == 0 || !udFunc(udValid, node, &temp, 0, m_pCBData)) continue;

			LinkChild(node, &temp);
		}
	}
}

void CAStar::LinkChild(_asNode *node, _asNode *temp) 
{
	int   x   = temp->GetX();
	int   y   = temp->GetY();
	float g   =   node->GetG() + udTerrainCost(node, temp) + udThreatCost(node, temp);
  float t   = temp->GetThreatAimQuality();
	int   id  = Coord2Num(x,y);

	_asNode *check = NULL;

	if (check = CheckList(m_pOpen, id)) {
		node->AddChild(check);

		// A better route found, so update
		// the node and variables accordingly.
		if ( g < check->GetG() ) {
			check->SetParent(node);
      check->SetThreatAimQuality(t);
			check->SetG(g);
      check->UpdateF();
			udFunc(udNotifyChild, node, check, ASNC_OPENADD_UP, m_pNCData);
		} else {
			udFunc(udNotifyChild, node, check, ASNC_OPENADD, m_pNCData);
		}
	} 
  else if (check = CheckList(m_pClosed, id)) {
		node->AddChild(check);

		if ( g < check->GetG() ) {
			check->SetParent(node);
      check->SetThreatAimQuality(t);
			check->SetG(g);
      check->UpdateF();

			udFunc(udNotifyChild, node, check, ASNC_CLOSEDADD_UP, m_pNCData);

			// The fun part...
      //! \todo remove UpdateParents call
			//UpdateParents(check);
		} else {
			udFunc(udNotifyChild, node, check, ASNC_CLOSEDADD, m_pNCData);
		}
	} 
  else 
    {
      _asNode dest(m_iDX, m_iDY);
		  _asNode *newnode = new _asNode(x,y);

		  newnode->SetParent(node);
      newnode->SetThreatAimQuality(t);
		  newnode->SetG(g);
		  newnode->SetH(udTerrainHeuristic(newnode, &dest) + udThreatHeuristic(newnode, &dest));
      newnode->UpdateF();
      newnode->SetID(Coord2Num(x,y));

		  AddToOpen(newnode);

		  node->AddChild(newnode);

		  udFunc(udNotifyChild, node, newnode, ASNC_NEWADD, m_pNCData);
	  }
}

_asNode *CAStar::CheckList(_asNode *node, int anID) 
{
	while (node) {
		if (node->GetID() == anID) return node;

		node = node->next;
	}

	return NULL;
}

void CAStar::AddToOpen(_asNode *addnode) 
{
	_asNode *node = m_pOpen;
	_asNode *prev = NULL;

	if (!m_pOpen) {
		m_pOpen = addnode;
		m_pOpen->next = NULL;

		udFunc(udNotifyList, NULL, addnode, ASNL_STARTOPEN, m_pNCData);

		return;
	}

	while( node ) {
		if ( addnode->GetF() > node->GetF() ) {
			prev = node;
			node = node->next;
		} else {
			if (prev) {
				prev->next = addnode;
				addnode->next = node;
				udFunc(udNotifyList, prev, addnode, ASNL_ADDOPEN, m_pNCData);
			} else {
				_asNode *temp = m_pOpen;

				m_pOpen = addnode;
				m_pOpen->next = temp;
				udFunc(udNotifyList, temp, addnode, ASNL_STARTOPEN, m_pNCData);
			}

			return;
		}
	}

	prev->next = addnode;
	udFunc(udNotifyList, prev, addnode, ASNL_ADDOPEN, m_pNCData);
}

int CAStar::udFunc(_asFunc func, _asNode *param1, _asNode *param2, int data, void *cb)
{
	if (func) return func(param1, param2, data, cb);

	return 1;
}

