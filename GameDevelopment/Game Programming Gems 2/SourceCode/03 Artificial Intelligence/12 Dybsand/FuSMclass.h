/* Copyright (C) Eric Dybsand, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Eric Dybsand, 2001"
 */
// FuSMclass.h: interface for the FuSMclass class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FUSMCLASS_H__7041B021_E922_11D4_8C04_0080C8FE83CE__INCLUDED_)
#define AFX_FUSMCLASS_H__7041B021_E922_11D4_8C04_0080C8FE83CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// forward reference
class FuSMstate;
//
// this is the declaration of the generic fuzzy state machine class
//

// Create a Standard Template Library <map> for containing the states
//
// NOTE: if STL is not supported on your system, then I apologize for
// this inconvenience.  You will need to substitute its usage with a
// similar container class.  The STL <map> is an indexable hash table
// lookup container class, that provides fast access of generic objects,
// and since STL is a standardized container class on most compilers,
// I felt it would provide the best convenient container.
//
typedef map< int, FuSMstate*, less<int> > FuzzyState_Map;
typedef FuzzyState_Map::value_type FSM_VT;

// Create a Standard Template Library <list> for membership states 
//
typedef list< FuSMstate* > FuzzyState_List;

class FuSMclass  
{
	FuzzyState_Map m_map;		// map containing all the fuzzy states of this FuSM
	FuzzyState_List m_list;		// list containing only membership state
	FuzzyState_List::iterator m_itList; // iterator for use in accessing member states

	int m_iCurrentInput;		// value received that triggered transitions to new membership fuzzy states

public:
	FuSMclass();
	~FuSMclass();

	int GetInput(void) { return m_iCurrentInput; }

	FuSMstate *GetState( int iStateID );	// return the FuSMstate object pointer
	void AddState( FuSMstate *pState );	// add a FuSMstate object pointer to the map
	void DeleteState( int iStateID );	// delete a FuSMstate object pointer from the map

	void StateTransition( int iInput );	// perform fuzzy state transitions based on input
	FuSMstate *GetNextFuzzyStateMember(void); // allow access to member fuzzy states
};

#endif // !defined(AFX_FUSMCLASS_H__7041B021_E922_11D4_8C04_0080C8FE83CE__INCLUDED_)
