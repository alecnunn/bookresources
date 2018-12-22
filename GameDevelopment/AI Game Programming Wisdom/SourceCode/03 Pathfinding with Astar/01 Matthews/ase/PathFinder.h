//////////////////////////////////////////////////////////////////
// Class:	CAStar class (27/6/2001)
// File:	AStar.h
// Author:	James Matthews
//
// Implements the A* algorithm.
// 
//
// Please visit http://www.generation5.org/ for the latest
// in Artificial Intelligence news, interviews, articles and
// discussion forums.
//

#ifndef _ASTAR_H_
#define _ASTAR_H_

#include <memory.h>
#include "AsIncludes.h"

class CAStar {
	public:
		CAStar();
		~CAStar();

		_asFunc	 udCost;			// Called when cost value is need
		_asFunc  udValid;			// Called to check validity of a coordinate
		_asFunc  udNotifyChild;		// Called when child is added/checked (LinkChild)
		_asFunc	 udNotifyList;		// Called when node is added to Open/Closed list

		void	*m_pCBData;			// Data passed back to callback functions
		void	*m_pNCData;			// Data passed back to notify child functions

		bool	GeneratePath(int, int, int, int);
		int		Step();
		void	StepInitialize(int, int, int, int);
		void	SetRows(int r)		 { m_iRows = r;    }
		void	Reset() { m_pBest = NULL; }

		_asNode	*GetBestNode() { return m_pBest; }

	protected:
		int		m_iRows;			// Used to calculate node->number
		int		m_iSX, m_iSY, m_iDX, m_iDY, m_iDNum;

		_asNode	*m_pOpen;			// The open list
		_asNode	*m_pClosed;			// The closed list
		_asNode *m_pBest;			// The best node
		_asStack*m_pStack;			// Propagation stack

		// Functions.
		void	AddToOpen(_asNode *);
		void	ClearNodes();
		void	CreateChildren(_asNode *);
		void	LinkChild(_asNode *, _asNode *);
		void	UpdateParents(_asNode *);

		// Stack Functions.
		void	Push(_asNode *);
		_asNode *Pop();
		
		_asNode *CheckList(_asNode *, int);
		_asNode	*GetBest();
		
		// Inline functions.
		inline int Coord2Num(int x, int y) { return x * m_iRows + y; }
		inline int udFunc(_asFunc, _asNode *, _asNode *, int, void *);
};

#endif
