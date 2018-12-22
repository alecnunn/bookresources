/* Copyright (C) Eric Dybsand, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Eric Dybsand, 2001"
 */
// FuSMstate.h: interface for the FuSMstate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FUSMSTATE_H__7041B022_E922_11D4_8C04_0080C8FE83CE__INCLUDED_)
#define AFX_FUSMSTATE_H__7041B022_E922_11D4_8C04_0080C8FE83CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
// this is the definition of an individual fuzzy state
//
class FuSMstate  
{
	int m_iStateID;		// the unique ID of this state
	int m_iLowRange;	// range of int values to represent membership in this state
	int m_iHighRange;
	int m_iValueOfMembership;	// current amount of membership, for this fuzzy state
	int m_iDegreeOfMembership;	// current percentage of truth, for this fuzzy state

public:
	FuSMstate( int iStateID, int iLowRange, int iHighRange );
	virtual ~FuSMstate();

	// access the state ID
	int GetID() { return m_iStateID; }
	// access membership ranges
	void GetMembershipRanges( int& iLow, int& iHigh );
	// access percent of truth of current membership for this state
	int GetDegreeOfMembership( void ) { return m_iDegreeOfMembership; }
	// access value of current membership for this state
	int GetValueOfMembership( void ) { return m_iValueOfMembership; }
	// transition this fuzzy state
	bool DoTransition( int iInputValue );
};

#endif // !defined(AFX_FUSMSTATE_H__7041B022_E922_11D4_8C04_0080C8FE83CE__INCLUDED_)
