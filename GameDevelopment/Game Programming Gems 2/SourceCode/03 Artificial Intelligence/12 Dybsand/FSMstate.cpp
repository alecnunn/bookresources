/* Copyright (C) Eric Dybsand, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Eric Dybsand, 2001"
 */
// FSMstate.cpp: implementation of the FSMstate class.  Unoptimized.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameGems.h"
#include "FSMstate.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// AddTransition() - accept an input transition threshhold and the
// output state ID to associate with it
//////////////////////////////////////////////////////////////////////

void FSMstate::AddTransition( int iInput, int iOutputID )
{
	// the m_piInputs[] and m_piOutputState[] are not sorted
	// so find the first non-zero offset in m_piOutputState[]
	// and use that offset to store the input and OutputID
	// within the m_piInputs[] and m_piOutputState[]
	for( int i=0; i<m_usNumberOfTransistions; ++i )
	{
		if( !m_piOutputState[i] )
			break;
	}
	// only a valid offset is used
	if( i < m_usNumberOfTransistions )
	{
		m_piOutputState[i] = iOutputID;
		m_piInputs[i] = iInput;
	}
}

//////////////////////////////////////////////////////////////////////
// GetOutput() - accepts an input transition value and finds the 
// input transition value stored in m_piInputs[] that is associated
// with an output state ID and returns that output state ID
//
// NOTE: this function acts as a state transition function and could
// be replaced with a different transition approach depending on the
// needs of your FSM
//////////////////////////////////////////////////////////////////////

int FSMstate::GetOutput( int iInput )
{
	int iOutputID = m_iStateID;	// output state to be returned

	// for each possible transistion
	for( int i=0; i<m_usNumberOfTransistions; ++i )
	{
		// zeroed output state IDs indicate the end of the array
		if( !m_piOutputState[i] )
			break;
		// state transition function: look for a match with the input value
		if( iInput == m_piInputs[i] )
		{
			iOutputID = m_piOutputState[i];	// output state id
			break;
		}
	}
	// returning either this m_iStateID to indicate no output 
	// state was matched by the input (ie. no state transition
	// can occur) or the transitioned output state ID
	return( iOutputID );
}

//////////////////////////////////////////////////////////////////////
// DeleteTransition() - remove an output state ID and its associated
// input transition value from the arrays and zero out the slot used
//////////////////////////////////////////////////////////////////////

void FSMstate::DeleteTransition( int iOutputID )
{
	// the m_piInputs[] and m_piOutputState[] are not sorted
	// so find the offset of the output state ID to remove
	for( int i=0; i<m_usNumberOfTransistions; ++i )
	{
		if( m_piOutputState[i] == iOutputID )
			break;
	}

	// test to be sure the offset is valid
	if( i >= m_usNumberOfTransistions )
		return;

	// remove this output ID and its input transition value
	m_piInputs[i] = 0; 
	m_piOutputState[i] = 0;

	// since the m_piInputs[] and m_piOutputState[] are not 
	// sorted, then we need to shift the remaining contents
	for( ; i<(m_usNumberOfTransistions-1); ++i )
	{
		if( !m_piOutputState[i] )
			break;

		m_piInputs[i] = m_piInputs[i+1];
		m_piOutputState[i] = m_piOutputState[i+1];
	}
	// and make sure the last used offset is cleared
	m_piInputs[i] = 0; 
	m_piOutputState[i] = 0;
}

//////////////////////////////////////////////////////////////////////
// FSMstate() - create a new instance and allocate arrays
//////////////////////////////////////////////////////////////////////

FSMstate::FSMstate( int iStateID, unsigned usTransitions )
{
	// don't allow 0 transitions
	if( !usTransitions )
		m_usNumberOfTransistions = 1;
	else
		m_usNumberOfTransistions = usTransitions;

	// save off id and number of transitions
	m_iStateID = iStateID;
	
	// now allocate each array
	try
	{
		m_piInputs = new int[m_usNumberOfTransistions];
		for( int i=0; i<m_usNumberOfTransistions; ++i )
			m_piInputs[i] = 0;
	}
	catch( ... )
	{
		throw;
	}

	try
	{
		m_piOutputState = new int[m_usNumberOfTransistions];
		for( int i=0; i<m_usNumberOfTransistions; ++i )
			m_piOutputState[i] = 0;
	}
	catch( ... )
	{
		delete [] m_piInputs;
		throw;
	}
}

//////////////////////////////////////////////////////////////////////
// ~FSMstate() - clean up by returning memory used for arrays
//////////////////////////////////////////////////////////////////////

FSMstate::~FSMstate()
{
	delete [] m_piInputs;
	delete [] m_piOutputState;
}

// end of FSMstate.cpp
