/* Copyright (C) Eric Dybsand, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Eric Dybsand, 2001"
 */
// GameGems.h : main header file for the GAMEGEMS application
//

#if !defined(AFX_GAMEGEMS_H__F6FF27C4_D314_11D3_911B_0080C8FE83CE__INCLUDED_)
#define AFX_GAMEGEMS_H__F6FF27C4_D314_11D3_911B_0080C8FE83CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

//
// these headers declare STL support for the application
//
#pragma warning(disable:4786)  // don't warn about browse name overflow.

#include <list>
#include <vector>
#include <deque>
#include <map>
#include <queue>
#include <utility>
#include <iterator>
#include <algorithm>
#include <functional>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <strstream>
#include <sstream>
#include <exception>
#include <cmath>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// STATE_IDs and INPUT_IDs used for testing FSMclass and FSMstate for Gems

enum
{
	STATE_ID_BERSERK=1,
	STATE_ID_RAGE,
	STATE_ID_MAD,
	STATE_ID_ANNOYED,
	STATE_ID_UNCARING,
	INPUT_ID_PLAYER_SEEN,
	INPUT_ID_PLAYER_ATTACKS,
	INPUT_ID_PLAYER_GONE,
	INPUT_ID_MONSTER_HURT,
	INPUT_ID_MONSTER_HEALED,
	NUMBER_OF_IDS_USED
};

#ifdef _DEBUG
#define LOG_FILE	1
#else
#define LOG_FILE	0
#endif

/////////////////////////////////////////////////////////////////////////////
// CGameGemsApp:
// See GameGems.cpp for the implementation of this class
//

class CGameGemsApp : public CWinApp
{
public:
	CGameGemsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGameGemsApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CGameGemsApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMEGEMS_H__F6FF27C4_D314_11D3_911B_0080C8FE83CE__INCLUDED_)
