/* Copyright (C) Eric Dybsand, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Eric Dybsand, 2001"
 */
// TestFuSMDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GameGems.h"
#include "TestFuSMDlg.h"
#include "FuSMclass.h"
#include "FuSMstate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// strings for display of IDs during testing
#define FUSM_ID_STRING_LENGTH	15
char *pzFuSMStringIDs[NUMBER_OF_IDS_USED] = {
	"               ",
	"Berserk        ",
	"Raged          ",
	"Mad            ",
	"Annoyed        ",
	"Uncaring       ",
	"Player Seen    ",
	"Player Attacks ",
	"Player Gone    ",
	"Monster Hurt   ",
	"Monster Healed "
};

#if LOG_FILE
extern FILE *fpDebug;			// log file
#endif

/////////////////////////////////////////////////////////////////////////////
// TestFuSMDlg dialog


TestFuSMDlg::TestFuSMDlg(CWnd* pParent, FuSMclass *pFuSMclass)
	: CDialog(TestFuSMDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(TestFuSMDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pFuSMclass = pFuSMclass;
}


void TestFuSMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TestFuSMDlg)
	DDX_Control(pDX, IDC_LIST1, m_lbStates);
	DDX_Control(pDX, IDC_INPUTSCOMBO, m_cbInputs);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TestFuSMDlg, CDialog)
	//{{AFX_MSG_MAP(TestFuSMDlg)
	ON_CBN_SELCHANGE(IDC_INPUTSCOMBO, OnSelchangeInputscombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TestFuSMDlg message handlers

BOOL TestFuSMDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// initialize input values list box
	m_cbInputs.ResetContent();

	// go through the list of possible inputs and add them to the dropdown combo box
	CString sLine;
	for( int i=INPUT_ID_PLAYER_SEEN; i<NUMBER_OF_IDS_USED; ++i )
	{
		// get the string associated with this input value
		sLine = pzFuSMStringIDs[i];
		int nSel = m_cbInputs.AddString( (const char *)sLine);

		// store the id of the input string for access later
		m_cbInputs.SetItemData(nSel, (DWORD)i);
		sLine.Empty();
	}

	// initialize state list box
	m_lbStates.ResetContent();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void TestFuSMDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
// OnSelchangeInputscombo() 
// find the input, apply the input to the states determining which
// are active and to what degree, and then add the active states
// to the state list box
/////////////////////////////////////////////////////////////////////////////

void TestFuSMDlg::OnSelchangeInputscombo() 
{
	// get input value id from combo box
	int iSel = m_cbInputs.GetCurSel();
	if( iSel < 0 )
		return;
	int iID = (int)m_cbInputs.GetItemData(iSel);

	// NOTE: in order to remain totally compatible with the FSM usage
	// (found in TestFSMDlg) of the enum, and to keep the example as
	// simple as possible, we are going to convert the input action 
	// id (iID above) into a rating input to use later.  You may wish
	// to deploy a different approach to determine the value of an
	// input action.
	//
	// create a discrete "dislike rating" input for action inputs
	// positive = dislike more, negative = dislike less
	int iDislikeInput = 0;
	switch( iID )
	{
	case INPUT_ID_PLAYER_SEEN:
		iDislikeInput = 10;
		break;
	case INPUT_ID_PLAYER_ATTACKS:
		iDislikeInput = 30;
		break;
	case INPUT_ID_PLAYER_GONE:
		iDislikeInput = -10;
		break;
	case INPUT_ID_MONSTER_HURT:
		iDislikeInput = +50;
		break;
	case INPUT_ID_MONSTER_HEALED:
		iDislikeInput = -20;
	default:
		break;
	}

	// make sure the active state list box is cleared
	m_lbStates.ResetContent();

	// apply the fuzzy input value (the "dislike rating" from above)
	m_pFuSMclass->StateTransition(iDislikeInput);

	// now fill the active state list box with active states
	CString sLine;
	CString sHow;
	FuSMstate *pFuzzyState = NULL;
	char szLine[FUSM_ID_STRING_LENGTH*2];

	while( (pFuzzyState = m_pFuSMclass->GetNextFuzzyStateMember()) != NULL )
	{
		// crudely translate the degree of membership into a string description
		if( pFuzzyState->GetDegreeOfMembership() < 25 )
		{
			sHow = "is Sort of ";
		}
		else if( pFuzzyState->GetDegreeOfMembership() < 50 )
		{
			sHow = "is Partially ";
		}
		else if( pFuzzyState->GetDegreeOfMembership() < 75 )
		{
			sHow = "is Mostly ";
		}
		else if( pFuzzyState->GetDegreeOfMembership() < 99 )
		{
			sHow = "is Very ";
		}
		else
		{
			sHow = "is Totally ";
		}

		// construct a line showing degree of membership and name of state
		wsprintf( szLine, "%d -> %s %s", pFuzzyState->GetDegreeOfMembership(),
			(const char *)sHow, pzFuSMStringIDs[pFuzzyState->GetID()] );

		// and add that line of text to the list box
		iSel = m_lbStates.AddString( (const char *)szLine);
		sHow.Empty();
	}
}

// end of TestFuSMDlg.cpp
