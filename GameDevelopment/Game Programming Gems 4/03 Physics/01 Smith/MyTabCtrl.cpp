// MyTabCtrl.cpp : implementation file
//
/////////////////////////////////////////////////////
// This class is provided as is and Ben Hill takes no
// responsibility for any loss of any kind in connection
// to this code.
/////////////////////////////////////////////////////
// Is is meant purely as a educational tool and may
// contain bugs.
/////////////////////////////////////////////////////
// ben@shido.fsnet.co.uk
// http://www.shido.fsnet.co.uk
/////////////////////////////////////////////////////
// Thanks to a mystery poster in the C++ forum on 
// www.codeguru.com I can't find your name to say thanks
// for your Control drawing code. If you are that person 
// thank you very much. I have been able to use some of 
// you ideas to produce this sample application.
/////////////////////////////////////////////////////

#include "stdafx.h"
//#include "MyTabExample.h"
#include "MyTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyTabCtrl

CMyTabCtrl::CMyTabCtrl()
{
	m_tabPages[0]=new CTabOneDlg;
	m_tabPages[1]=new CTabTwoDlg;
	m_tabPages[2]=new CTabThreeDlg;
	m_tabPages[3]=new CTabFourDlg;
	m_tabPages[4]=new CTabFiveDlg;
	m_tabPages[5]=new CTabSixDlg;
	m_tabPages[6]=new CTabSevenDlg;
	m_tabPages[7]=new CTabEightDlg;
	m_tabPages[8]=new CTabNineDlg;
	m_tabPages[9]=new CTabTenDLG;

	m_nNumberOfPages=10;
}

CMyTabCtrl::~CMyTabCtrl()
{
	for(int nCount=0; nCount < m_nNumberOfPages; nCount++){
		delete m_tabPages[nCount];
	}
}

void CMyTabCtrl::Init()
{
	m_tabCurrent=0;

	m_tabPages[0]->Create(IDD_TAB_ONE_DLG, this);
	m_tabPages[1]->Create(IDD_TAB_TWO_DLG, this);
	m_tabPages[2]->Create(IDD_TAB_THREE_DLG, this);
	m_tabPages[3]->Create(IDD_TAB_FOUR_DLG, this);
	m_tabPages[4]->Create(IDD_TAB_FIVE_DLG, this);
	m_tabPages[5]->Create(IDD_TAB_SIX_DLG, this);
	m_tabPages[6]->Create(IDD_TAB_SEVEN_DLG, this);
	m_tabPages[7]->Create(IDD_TAB_EIGHT_DLG, this);
	m_tabPages[8]->Create(IDD_TAB_NINE_DLG, this);
	m_tabPages[9]->Create(IDD_TAB_TEN_DLG, this);

	m_tabPages[0]->ShowWindow(SW_SHOW);
	m_tabPages[1]->ShowWindow(SW_HIDE);
	m_tabPages[2]->ShowWindow(SW_HIDE);
	m_tabPages[3]->ShowWindow(SW_HIDE);
	m_tabPages[4]->ShowWindow(SW_HIDE);
	m_tabPages[5]->ShowWindow(SW_HIDE);
	m_tabPages[6]->ShowWindow(SW_HIDE);
	m_tabPages[7]->ShowWindow(SW_HIDE);
	m_tabPages[8]->ShowWindow(SW_HIDE);
	m_tabPages[9]->ShowWindow(SW_HIDE);

	SetRectangle();
}

void CMyTabCtrl::SetRectangle()
{
	CRect tabRect, itemRect;
	int nX, nY, nXc, nYc;

	GetClientRect(&tabRect);
	GetItemRect(0, &itemRect);

	nX=itemRect.left;
	nY=itemRect.bottom+1;
	nXc=tabRect.right-itemRect.left-1;
	nYc=tabRect.bottom-nY-1;

	m_tabPages[0]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	for(int nCount=1; nCount < m_nNumberOfPages; nCount++){
		m_tabPages[nCount]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_HIDEWINDOW);
	}
}

BEGIN_MESSAGE_MAP(CMyTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CMyTabCtrl)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyTabCtrl message handlers

void CMyTabCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CTabCtrl::OnLButtonDown(nFlags, point);

	if(m_tabCurrent != GetCurFocus()){
		m_tabPages[m_tabCurrent]->ShowWindow(SW_HIDE);
		m_tabCurrent=GetCurFocus();
		m_tabPages[m_tabCurrent]->ShowWindow(SW_SHOW);
		m_tabPages[m_tabCurrent]->SetFocus();
	}
}
