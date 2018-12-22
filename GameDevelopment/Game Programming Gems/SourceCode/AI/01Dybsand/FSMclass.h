/* Copyright (C) Eric Dybsand, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Eric Dybsand, 2000"
 */
// FSMclass.h: interface for the FSMclass class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FSMCLASS_H__F6FF27D1_D314_11D3_911B_0080C8FE83CE__INCLUDED_)
#define AFX_FSMCLASS_H__F6FF27D1_D314_11D3_911B_0080C8FE83CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// forward reference
class FSMstate;

// Create a Standard Template Library <map> for containing the states
//
// NOTE: if STL is not supported on your system, then I apologize for
// this inconvenience.  You will need to substitute its usage with a
// similar container class.  The STL <map> is an indexable hash table
// lookup container class, that provides fast access of generic objects,
// and since STL is a standardized container class on most compilers,
// I felt it would provide the best convenient container.
//
typedef map< int, FSMstate*, less<int> > State_Map;
typedef State_Map::value_type SM_VT;

//
// this is the declaration of the generic finite state machine class
//
class FSMclass  
{
	State_Map m_map;		// map containing all states of this FSM
	int m_iCurrentState;	// the m_iStateID of the current state

public:
	FSMclass( int iStateID );	// set initial state of the FSM
	~FSMclass();				// clean up memory usage

	// return the current state ID
	int GetCurrentState() { return m_iCurrentState; }
	// set current state
	void SetCurrentState( int iStateID ) { m_iCurrentState = iStateID; }

	FSMstate *GetState( int iStateID );	// return the FSMstate object pointer
	void AddState( FSMstate *pState );	// add a FSMstate object pointer to the map
	void DeleteState( int iStateID );	// delete a FSMstate object pointer from the map

	int StateTransition( int iInput );	// perform a state transition based on input and current state
};

#endif // !defined(AFX_FSMCLASS_H__F6FF27D1_D314_11D3_911B_0080C8FE83CE__INCLUDED_)
