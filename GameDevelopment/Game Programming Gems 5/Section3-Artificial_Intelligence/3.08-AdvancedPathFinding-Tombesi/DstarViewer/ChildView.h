/**********************************************************************

	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2003
 **********************************************************************/
// ChildView.h : interface of the CChildView class
//


#pragma once

#include <wchar.h>
#include "Painter.h"
#include "../DStar.h"
#include "Resource.h"


// CChildView window
enum OperationState
{
	OP_Obstacle,
	OP_Clear,
	OP_Start,
	OP_End,
	OP_Play,
	OP_Step,
	OP_Idle
};

enum EdgeType
{
	Edge_Clear = 1,
	Edge_Obstacle = 100000000
};


// SetBaseDim dialog
class SetBaseDim : public CDialog
{

public:
	SetBaseDim();   // standard constructor

//Overrides
public:
	void OnOK();
	
// Dialog Data
	enum { IDD = IDD_SETBASE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};


class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributes
public:
	OperationState	m_operation_state;
	Painter			m_Painter;
	unsigned		m_quad_cnt ;
	DStar		*	m_pDStar;
	UINT_PTR		m_timer_play;

// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOperationsSetupmap();
	afx_msg void OnOperationsClearmap();
	afx_msg void OnOperationsSetobstacle();
	afx_msg void OnOperationsSetclear();
	afx_msg void OnOperationsPlay();
	afx_msg void OnOperationsStep();
	afx_msg void OnSetBase();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
};




