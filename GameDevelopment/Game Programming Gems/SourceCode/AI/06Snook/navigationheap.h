/* Copyright (C) Greg Snook, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg Snook, 2000"
 */
#ifndef NAVIGATIONHEAP_H
#define NAVIGATIONHEAP_H
/****************************************************************************************\
	NavigationHeap.h

	NavigationHeap component interface for the Navimesh sample program.
	Included as part of the Game Programming Gems sample code.

	Created 3/18/00 by Greg Snook
	greg@mightystudios.com
    ------------------------------------------------------------------------------------- 
	Notes/Revisions:

\****************************************************************************************/
#ifndef NAVIGATIONCELL_H
#include "navigationcell.h"
#endif

#ifndef STD_VECTOR_H
#define STD_VECTOR_H
#pragma warning(disable : 4786)
#include <vector>
#endif

#ifndef STD_LIST_H
#define STD_LIST_H
#pragma warning(disable : 4786)
#include <list>
#endif

#ifndef STD_ALGO_H
#define STD_ALGO_H
#pragma warning(disable : 4786)
#include <algorithm>
#endif

#include <function.h>

/*	NavigationNode
------------------------------------------------------------------------------------------
	
	A NavigationNode represents an entry in the NavigationHeap. It provides some simple
	operators to classify it against other NavigationNodes when the heap is sorted.
	
------------------------------------------------------------------------------------------
*/
class NavigationNode
{
public:
	NavigationNode(): cell(0), cost(0){}
	~NavigationNode() {}

	NavigationCell* cell; // pointer to the cell in question
	float cost;        // (g + h) in A* represents the cost of traveling through this cell

	inline bool operator < (const NavigationNode& b )
	{
		// To compare two nodes, we compare the cost or `f' value, which is the
		// sum of the g and h values defined by A*.
		return (cost < (b.cost));
	}

	inline bool operator > (const NavigationNode& b )
	{
		// To compare two nodes, we compare the cost or `f' value, which is the
		// sum of the g and h values defined by A*.
		return (cost > (b.cost));
	}

	inline bool operator == (const NavigationNode& b )
	{
		// Two nodes are equal if their components are equal
		return ((cell == b.cell) && (cost == b.cost));
	}
};

//
// The STL requires some external operators as well
//
inline bool operator < ( const NavigationNode& a, const NavigationNode& b )
{
	return (a.cost < b.cost);
}

inline bool operator > ( const NavigationNode& a, const NavigationNode& b )
{
	return (a.cost > b.cost);
}

inline bool operator == ( const NavigationNode& a, const NavigationNode& b )
{
	return ((a.cell == b.cell) && (a.cost == b.cost));
}


/*	NavigationHeap
------------------------------------------------------------------------------------------
	
	A NavigationHeap is a priority-ordered list facilitated by the STL heap functions.
	This class is also used to hold the current path finding session ID and the desired 
	goal point for NavigationCells to query. Thanks to Amit J. Patel for detailing the use
	of STL heaps in this way. It's much faster than a linked list or multimap approach.
	
------------------------------------------------------------------------------------------
*/


class NavigationHeap
{
public:

	// ----- ENUMERATIONS & CONSTANTS -----
	// greater<NavigationNode> is an STL thing to create a 'comparison' object out of
	// the greater-than operator, and call it comp.
	typedef std::vector<NavigationNode> Container;
	std::greater<NavigationNode> comp;


	// ----- CREATORS ---------------------

	NavigationHeap();
	~NavigationHeap();

	// ----- OPERATORS --------------------

	// ----- MUTATORS ---------------------
	void Setup(int SessionID, const vector3& Goal);
	void AddCell(NavigationCell* pCell);
	void AdjustCell(NavigationCell* pCell);

	// ----- ACCESSORS --------------------
	bool NotEmpty()const;
	void GetTop(NavigationNode& n);
	int SessionID()const;
	const vector3& Goal()const;

private:

	// ----- DATA -------------------------
	Container m_Nodes;
	int m_SessionID;
	vector3 m_Goal;

	// ----- HELPER FUNCTIONS -------------
	Container::iterator FindNodeInterator(NavigationCell* pCell);

	// ----- UNIMPLEMENTED FUNCTIONS ------

	NavigationHeap( const NavigationHeap& Src);
	NavigationHeap& operator=( const NavigationHeap& Src);

};

//- Inline Functions ---------------------------------------------------------------------

//= CREATORS =============================================================================

/*	NavigationHeap
------------------------------------------------------------------------------------------
	
	Default Object Constructor
	
------------------------------------------------------------------------------------------
*/
inline NavigationHeap::NavigationHeap()
{

}

/*	~NavigationHeap
------------------------------------------------------------------------------------------
	
	Default Object Destructor
	
------------------------------------------------------------------------------------------
*/
inline NavigationHeap::~NavigationHeap()
{
}

//= OPERATORS ============================================================================

//= MUTATORS =============================================================================

//:	Setup
//----------------------------------------------------------------------------------------
//
//	Initialize the heap and record our new session info 
//
//-------------------------------------------------------------------------------------://
inline void NavigationHeap::Setup(int SessionID, const vector3& Goal)
{
	m_Goal = Goal;
	m_SessionID = SessionID;
	m_Nodes.clear();
}

//:	AddCell
//----------------------------------------------------------------------------------------
//
//	Add a cell to the heap, sorting it in by it's cost value 
//
//-------------------------------------------------------------------------------------://
inline void NavigationHeap::AddCell(NavigationCell* pCell)
{
	NavigationNode NewNode;

	NewNode.cell = pCell;
	NewNode.cost = pCell->PathfindingCost();

	m_Nodes.push_back(NewNode);
	std::push_heap( m_Nodes.begin(), m_Nodes.end(), comp );
}

//:	AdjustCell
//----------------------------------------------------------------------------------------
//
//	Adjust a cell in the heap to reflect it's updated cost value. NOTE: Cells may only 
//	sort up in the heap. 
//
//-------------------------------------------------------------------------------------://
inline void NavigationHeap::AdjustCell(NavigationCell* pCell)
{
	Container::iterator iter = FindNodeInterator(pCell);

	if (iter!=m_Nodes.end())
	{
		// update the node data
		(*iter).cell = pCell;
		(*iter).cost = pCell->PathfindingCost();
	
		// reorder the heap
		std::push_heap( m_Nodes.begin(), iter+1, comp );
	}
}


//= ACCESSORS ============================================================================

//:	NotEmpty
//----------------------------------------------------------------------------------------
//
//	Returns true if the heap is not empty 
//
//-------------------------------------------------------------------------------------://
inline bool NavigationHeap::NotEmpty()const
{
	return(m_Nodes.size() ? true:false);
}

//:	GetTop
//----------------------------------------------------------------------------------------
//
//	Pop the top off the heap and remove the best value for processing. 
//
//-------------------------------------------------------------------------------------://
inline void NavigationHeap::GetTop(NavigationNode& n)
{
	n = m_Nodes.front();
	std::pop_heap( m_Nodes.begin(), m_Nodes.end(), comp );
	m_Nodes.pop_back();
}

//:	FindNodeInterator
//----------------------------------------------------------------------------------------
//
//	Search the container for a given cell. May be slow, so don't do this unless nessesary. 
//
//-------------------------------------------------------------------------------------://
inline NavigationHeap::Container::iterator NavigationHeap::FindNodeInterator(NavigationCell* pCell)
{
	for( Container::iterator i = m_Nodes.begin(); i != m_Nodes.end(); ++i )
	{
	  if( (*i).cell == pCell )
		  return i;
	}
	return m_Nodes.end();
}

inline int NavigationHeap::SessionID()const
{
	return(m_SessionID);
}

inline 	const vector3& NavigationHeap::Goal()const
{
	return(m_Goal);
}
//- End of NavigationHeap ----------------------------------------------------------------

//****************************************************************************************

#endif  // end of file      ( NavigationHeap.h )

