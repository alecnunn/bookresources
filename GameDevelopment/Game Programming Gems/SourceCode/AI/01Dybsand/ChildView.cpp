/* Copyright (C) Eric Dybsand, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Eric Dybsand, 2000"
 */
// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"		// standard headers
#include "GameGems.h"	// this application
#include "ChildView.h"	// this class
#include "FSMclass.h"	// declare FSMclass which is the FSM
#include "FSMstate.h"	// declare FSMstate which are the states of the FSM
#include "TestFSMDlg.h"	// declare TestFSMDlg which is a dialog class for testing FSM

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// this is debug only stuff
#if LOG_FILE
FILE *fpDebug;			// log file
#endif


/////////////////////////////////////////////////////////////////////////////
// CChildView::CreateFSM() - create the FSMclass object and initialize it
// with FSMstate objects created from enum data

void CChildView::CreateFSM()
{
	m_pFSMclass = NULL;

	// create the FSMclass object
	try
	{
		// FSMclass( int iStateID )
		m_pFSMclass = new FSMclass(STATE_ID_UNCARING);
	}
	catch( ... )
	{
		throw;
	}

	// now create FSMstate objects and initialize FSMclass with them
	FSMstate *pFSMstate = NULL;

	// create the STATE_ID_UNCARING
	try
	{
		// FSMstate( int iStateID, unsigned usTransitions )
		pFSMstate = new FSMstate( STATE_ID_UNCARING, 3 );
	}
	catch( ... )
	{
		throw;
	}
	// now add state transitions to this state
	pFSMstate->AddTransition( INPUT_ID_PLAYER_SEEN, STATE_ID_ANNOYED );
	pFSMstate->AddTransition( INPUT_ID_PLAYER_ATTACKS, STATE_ID_MAD );
	pFSMstate->AddTransition( INPUT_ID_MONSTER_HURT, STATE_ID_RAGE );
	// now add this state to the FSM
	m_pFSMclass->AddState( pFSMstate );

	// create the STATE_ID_MAD
	try
	{
		// FSMstate( int iStateID, unsigned usTransitions )
		pFSMstate = new FSMstate( STATE_ID_MAD, 2 );
	}
	catch( ... )
	{
		throw;
	}
	// now add state transitions to this state
	pFSMstate->AddTransition( INPUT_ID_MONSTER_HURT, STATE_ID_RAGE );
	pFSMstate->AddTransition( INPUT_ID_MONSTER_HEALED, STATE_ID_UNCARING );
	// now add this state to the FSM
	m_pFSMclass->AddState( pFSMstate );

	// create the STATE_ID_RAGE
	try
	{
		// FSMstate( int iStateID, unsigned usTransitions )
		pFSMstate = new FSMstate( STATE_ID_RAGE, 2 );
	}
	catch( ... )
	{
		throw;
	}
	// now add state transitions to this state
	pFSMstate->AddTransition( INPUT_ID_MONSTER_HURT, STATE_ID_BERSERK );
	pFSMstate->AddTransition( INPUT_ID_MONSTER_HEALED, STATE_ID_ANNOYED );
	// now add this state to the FSM
	m_pFSMclass->AddState( pFSMstate );

	// create the STATE_ID_BERSERK
	try
	{
		// FSMstate( int iStateID, unsigned usTransitions )
		pFSMstate = new FSMstate( STATE_ID_BERSERK, 2 );
	}
	catch( ... )
	{
		throw;
	}
	// now add state transitions to this state
	pFSMstate->AddTransition( INPUT_ID_MONSTER_HURT, STATE_ID_BERSERK );
	pFSMstate->AddTransition( INPUT_ID_MONSTER_HEALED, STATE_ID_RAGE );
	// now add this state to the FSM
	m_pFSMclass->AddState( pFSMstate );

	// create the STATE_ID_ANNOYED
	try
	{
		// FSMstate( int iStateID, unsigned usTransitions )
		pFSMstate = new FSMstate( STATE_ID_ANNOYED, 3 );
	}
	catch( ... )
	{
		throw;
	}
	// now add state transitions to this state
	pFSMstate->AddTransition( INPUT_ID_PLAYER_GONE, STATE_ID_UNCARING );
	pFSMstate->AddTransition( INPUT_ID_MONSTER_HEALED, STATE_ID_UNCARING );
	pFSMstate->AddTransition( INPUT_ID_PLAYER_ATTACKS, STATE_ID_RAGE );
	// now add this state to the FSM
	m_pFSMclass->AddState( pFSMstate );
}

/////////////////////////////////////////////////////////////////////////////
// CChildView

CChildView::CChildView()
{
	CreateFSM();
}

CChildView::~CChildView()
{
	if( m_pFSMclass != NULL )
		delete m_pFSMclass;

#if LOG_FILE
    fprintf(fpDebug, "stopped - aiDEBUG.TXT closed \n" );
    fclose( fpDebug );
#endif
}


BEGIN_MESSAGE_MAP(CChildView,CWnd )
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_PAINT()
	ON_COMMAND(IDM_TESTFSM, OnTestFSM)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

#if LOG_FILE
	// open a logging file
	fpDebug = fopen("aiDEBUG.TXT", "wt");
	fprintf(fpDebug, "starting - aiDEBUG.TXT opened \n" );
#endif

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CWnd::OnPaint() for painting messages
}


/////////////////////////////////////////////////////////////////////////////
// CChildView::OnTestFSM() - opens a dialog box to use to test the FSM

void CChildView::OnTestFSM() 
{
	TestFSMDlg dlg( this, m_pFSMclass );
	dlg.DoModal();
}
