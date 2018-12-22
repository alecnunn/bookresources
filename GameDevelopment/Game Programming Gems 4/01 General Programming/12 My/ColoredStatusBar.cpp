// ColoredStatusBar.cpp : implementation file
//

#include "stdafx.h"
#include "ColoredStatusBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColoredStatusBar

CColoredStatusBar::CColoredStatusBar()
{
}

CColoredStatusBar::~CColoredStatusBar()
{
}


BEGIN_MESSAGE_MAP(CColoredStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(CColoredStatusBar)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColoredStatusBar message handlers

void CColoredStatusBar::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
  {
	// Attach to a CDC object
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	dc.SetBkMode(TRANSPARENT);

	// Get the pane rectangle and calculate text coordinates
	CRect rect(&lpDrawItemStruct->rcItem);

  if(lpDrawItemStruct->itemID == 0)
    {
    if(!m_strText.empty())
      {
      dc.SetTextAlign(TA_UPDATECP);
      dc.MoveTo(rect.left+2, rect.top);

  	  dc.SetTextColor(RGB(0, 0, 0));
      DrawColoredString(dc,m_strText.c_str());

      dc.SetTextAlign(TA_NOUPDATECP);
      }
    }

	// Detach from the CDC object, otherwise the hDC will be
	// destroyed when the CDC object goes out of scope
	dc.Detach();
  }

//

void CColoredStatusBar::DrawColoredString(CDC& dc,const char* pszTxt)
  {
  if(!pszTxt) return;

  char* pszInk = strstr(pszTxt,"$INK");
  if(pszInk) *pszInk = 0;

  dc.TextOut(0,0,pszTxt);                                   // substring before $INK
  if(!pszInk) return;

  pszInk += strlen("$INK");
  char* pszEnd = strchr(pszInk,'$');
  if(!pszEnd) return;                                       // error

  *pszEnd = 0;
  if(0 == strcmp(pszInk,"black")) dc.SetTextColor(RGB(  0,  0,  0));
  if(0 == strcmp(pszInk,"blue"))  dc.SetTextColor(RGB(  0,  0,255));

  DrawColoredString(dc,pszEnd+1);
  }
