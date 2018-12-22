/* Copyright (C) Eric Dybsand, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Eric Dybsand, 2000"
 */
// TestFSMDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GameGems.h"
#include "TestFSMDlg.h"	// this class
#include "FSMclass.h"	// declare FSMclass which is the FSM

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// strings for display of IDs during testing
#define ID_STRING_LENGTH	15
char *pzStringIDs[NUMBER_OF_IDS_USED] = {
	"               ",
	"Berserk        ",
	"Rage           ",
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
// TestFSMDlg dialog


TestFSMDlg::TestFSMDlg(CWnd* pParent, FSMclass *pFSMclass)
	: CDialog(TestFSMDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(TestFSMDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pFSMclass = pFSMclass;

	// IDs for current and output states
	m_iCurrentState = m_pFSMclass->GetCurrentState();	
	m_iOutputState = 0;
}


void TestFSMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TestFSMDlg)
	DDX_Control(pDX, IDC_OUTPUTSTATE, m_editOutputState);
	DDX_Control(pDX, IDC_CURRENTSTATE, m_editCurrentState);
	DDX_Control(pDX, IDC_INPUTSCOMBO, m_cbInputs);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TestFSMDlg, CDialog)
	//{{AFX_MSG_MAP(TestFSMDlg)
	ON_BN_CLICKED(IDOK, OnAccept)
	ON_BN_CLICKED(IDCANCEL, OnExit)
	ON_CBN_SELCHANGE(IDC_INPUTSCOMBO, OnSelchangeInputscombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TestFSMDlg message handlers

// apply the input value to the current state and update displays and log
//
void TestFSMDlg::OnAccept() 
{
	// first record the transition to the log by getting
	// the string of the current state
	CString sCurrentState = pzStringIDs[m_iCurrentState];

	// get input value id
	int iSel = m_cbInputs.GetCurSel();
	if( iSel < 0 )
		return;
	int iID = (int)m_cbInputs.GetItemData(iSel);
	// and a string description of input
	CString sInput = pzStringIDs[iID];
	// and string description of the output state
	CString sOutputState = pzStringIDs[m_iOutputState];

#if LOG_FILE
    fprintf(fpDebug, "Current state: %s Input: %s Output state: %s \n",
	(LPCSTR)sCurrentState, (LPCSTR)sInput, (LPCSTR)sOutputState );
#endif

	sCurrentState.Empty();
	sOutputState.Empty();

	// update now
	m_iCurrentState = m_iOutputState;
	m_iOutputState = 0;

	// update the edit controls with new state strings
	sCurrentState = pzStringIDs[m_iCurrentState];
	m_editCurrentState.SetWindowText( (LPCTSTR)sCurrentState );
	sOutputState = pzStringIDs[m_iOutputState];
	m_editOutputState.SetWindowText( (LPCTSTR)sOutputState );
	// reset the input value list box
	m_cbInputs.SetCurSel( -1 );
	// do we need to tell the dialog?
	UpdateData( false );
}

void TestFSMDlg::OnExit() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}

void TestFSMDlg::OnSelchangeInputscombo() 
{
	// make sure the FSM and the test dialog agree with what
	// is the current state of the FSM
	m_pFSMclass->SetCurrentState(m_iCurrentState);

	// get input value id
	int iSel = m_cbInputs.GetCurSel();
	if( iSel < 0 )
		return;
	int iID = (int)m_cbInputs.GetItemData(iSel);

	// have the FSM do the transition to an output state
	m_iOutputState = m_pFSMclass->StateTransition(iID);
	// and get string description of output state and 
	CString sOutputState = pzStringIDs[m_iOutputState];
	// update the edit control window
	m_editOutputState.SetWindowText( (LPCTSTR)sOutputState );
	// do we need to tell the dialog?
	UpdateData( false );
}

BOOL TestFSMDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// initialize state edit controls
	CString sCurrentState = pzStringIDs[m_iCurrentState];
	m_editCurrentState.SetWindowText( (LPCTSTR)sCurrentState );
	CString sOutputState = pzStringIDs[m_iOutputState];
	m_editOutputState.SetWindowText( (LPCTSTR)sOutputState );

	// initialize input values list box
	m_cbInputs.ResetContent();

	CString sLine;
	for( int i=INPUT_ID_PLAYER_SEEN; i<NUMBER_OF_IDS_USED; ++i )
	{
		// get the string associated with this input value
		sLine = pzStringIDs[i];
		int nSel = m_cbInputs.AddString( (const char *)sLine);

		// store the id of the input string for access later
		m_cbInputs.SetItemData(nSel, (DWORD)i);
		sLine.Empty();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
