/* Copyright (C) Eric Dybsand, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Eric Dybsand, 2000"
 */
// FSMstate.h: interface for the FSMstate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FSMSTATE_H__F6FF27D0_D314_11D3_911B_0080C8FE83CE__INCLUDED_)
#define AFX_FSMSTATE_H__F6FF27D0_D314_11D3_911B_0080C8FE83CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
// this is the definition of an individual state, its inputs and transition arrays
//
class FSMstate  
{
	unsigned m_usNumberOfTransistions;	// maximum number of states supported by this state
	int *m_piInputs;					// input array for tranistions
	int *m_piOutputState;				// output state array
	int m_iStateID;						// the unique ID of this state

public:
	// constructor accepts an ID for this state and the number of transitions to support
	FSMstate( int iStateID, unsigned usTransitions );
	// destructor cleans up allocated arrays
	~FSMstate();

	// access the state ID
	int GetID() { return m_iStateID; }
	// add a state transition to the array
	void AddTransition( int iInput, int iOutputID );
	// remove a state transation from the array
	void DeleteTransition( int iOutputID );
	// get the output state and effect a transistion
	int GetOutput( int iInput );
};

#endif // !defined(AFX_FSMSTATE_H__F6FF27D0_D314_11D3_911B_0080C8FE83CE__INCLUDED_)
