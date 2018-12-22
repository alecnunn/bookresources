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

#include <math.h>
#include "PathFinder.h"

CAStar::CAStar() 
{
	m_pOpen = m_pClosed = NULL;
	m_pStack = NULL;
	m_pBest = NULL;

	udCost = NULL;
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

	temp->g = 0;
	temp->h = abs(dx-sx) + abs(dy-sy);
	temp->f = temp->g + temp->h;
	temp->number = Coord2Num(sx, sy);

	m_pOpen = temp;

	udFunc(udNotifyList, NULL, m_pOpen, ASNL_STARTOPEN, m_pNCData);
	udFunc(udNotifyChild, NULL, temp, 0, m_pNCData);
}

int CAStar::Step()
{
	if (!(m_pBest = GetBest()))
		return -1;

	if (m_pBest->number == m_iDNum) 
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
	int x = node->x, y = node->y;

	for (int i=-1;i<2;i++) {
		for (int j=-1;j<2;j++) {
			temp.x = x+i;
			temp.y = y+j;
			if (i == 0 && j == 0 || !udFunc(udValid, node, &temp, 0, m_pCBData)) continue;

			LinkChild(node, &temp);
		}
	}
}

void CAStar::LinkChild(_asNode *node, _asNode *temp) 
{
	int x = temp->x;
	int y = temp->y;
	int g = node->g + udFunc(udCost, node, temp, 0, m_pCBData);
	int num = Coord2Num(x,y);

	_asNode *check = NULL;

	if (check = CheckList(m_pOpen, num)) {
		node->children[node->numchildren++] = check;

		// A better route found, so update
		// the node and variables accordingly.
		if (g < check->g) {
			check->parent = node;
			check->g = g;
			check->f = g + check->h;
			udFunc(udNotifyChild, node, check, 1, m_pNCData);
		} else {
			udFunc(udNotifyChild, node, check, 2, m_pNCData);
		}
	} else if (check = CheckList(m_pClosed, num)) {
		node->children[node->numchildren++] = check;

		if (g < check->g) {
			check->parent = node;
			check->g = g;
			check->f = g + check->h;

			udFunc(udNotifyChild, node, check, 3, m_pNCData);

			// The fun part...
			UpdateParents(check);
		} else {
			udFunc(udNotifyChild, node, check, 4, m_pNCData);
		}
	} else {
		_asNode *newnode = new _asNode(x,y);
		newnode->parent = node;
		newnode->g = g;
		newnode->h = abs(x-m_iDX) + abs(y-m_iDY);
		newnode->f = newnode->g + newnode->h;
		newnode->number = Coord2Num(x,y);

		AddToOpen(newnode);

		node->children[node->numchildren++] = newnode;

		udFunc(udNotifyChild, node, newnode, 5, m_pNCData);
	}
}

_asNode *CAStar::CheckList(_asNode *node, int num) 
{
	while (node) {
		if (node->number == num) return node;

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

	while(node) {
		if (addnode->f > node->f) {
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

void CAStar::UpdateParents(_asNode *node) 
{
	int g = node->g, c = node->numchildren;

	_asNode *kid = NULL;
	for (int i=0;i<c;i++) {
		kid = node->children[i];
		if (g+1 < kid->g) {
			kid->g = g+1;
			kid->f = kid->g + kid->h;
			kid->parent = node;
			
			Push(kid);
		}
	}

	_asNode *parent;
	while (m_pStack) {
		parent = Pop();
		c = parent->numchildren;
		for (int i=0;i<c;i++) {
			kid = parent->children[i];
			
			if (parent->g+1 < kid->g) {
				kid->g = parent->g + udFunc(udCost, parent, kid, 0, m_pCBData);
				kid->f = kid->g + kid->h;
				kid->parent = parent;

				Push(kid);
			}
		}
	}
}

void CAStar::Push(_asNode *node)
{

	if (!m_pStack) {
		m_pStack = new _asStack;
		m_pStack->data = node;
		m_pStack->next = NULL;
	} else {
		_asStack *temp = new _asStack;

		temp->data = node;
		temp->next = m_pStack;
		m_pStack = temp;
	}
}

_asNode *CAStar::Pop() 
{
	_asNode *data = m_pStack->data;
	_asStack *temp = m_pStack;

	m_pStack = temp->next;
	
	delete temp;

	return data;
}

int CAStar::udFunc(_asFunc func, _asNode *param1, _asNode *param2, int data, void *cb)
{
	if (func) return func(param1, param2, data, cb);

	return 1;
}
