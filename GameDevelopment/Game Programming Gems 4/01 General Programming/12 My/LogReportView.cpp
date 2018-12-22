// LogReportView.cpp : implementation file
//

#include "stdafx.h"
#include "RTTIProps.h"
#include "RTTIPropsDoc.h"
#include "LogReportView.h"

#include "CustomMsgs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogReportView

IMPLEMENT_DYNCREATE(CLogReportView, CFormView)

CLogReportView::CLogReportView()
	: CFormView(CLogReportView::IDD)
{
	//{{AFX_DATA_INIT(CLogReportView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CLogReportView::~CLogReportView()
{
}

void CLogReportView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogReportView)
	DDX_Control(pDX, IDC_REPORT, m_Report);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLogReportView, CFormView)
	//{{AFX_MSG_MAP(CLogReportView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogReportView diagnostics

#ifdef _DEBUG
void CLogReportView::AssertValid() const
{
	CFormView::AssertValid();
}

void CLogReportView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CRTTIPropsDoc* CLogReportView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRTTIPropsDoc)));
	return (CRTTIPropsDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLogReportView message handlers

void CLogReportView::OnInitialUpdate() 
  {
	CFormView::OnInitialUpdate();

  ListView_SetExtendedListViewStyle(m_Report.m_hWnd,LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
  m_Report.InsertColumn(1,"ID",       LVCFMT_RIGHT, 50);    // not 0 to enable LVCFMT_RIGHT
  m_Report.InsertColumn(2,"Type",     LVCFMT_CENTER,70);
  m_Report.InsertColumn(3,"Message",  LVCFMT_LEFT, 100);
  m_Report.InsertColumn(4,"Time",     LVCFMT_RIGHT, 60);
  m_Report.InsertColumn(5,"Date",     LVCFMT_RIGHT, 60);
  m_Report.InsertColumn(6,"File",     LVCFMT_LEFT,  50);
  m_Report.InsertColumn(7,"Line",     LVCFMT_RIGHT, 50);

  // fill from doc

  CLogReportData LogData;
  CRTTIPropsDoc* pDoc = GetDocument();

  bool  boContinue = pDoc->GetFirstLogData(&LogData);
  while(boContinue)
    {
    OnUpdate(NULL,LOGREPORT_ADDTXT,(CObject*)&LogData);
    boContinue = pDoc->GetNextLogData(&LogData);
    }

  // now that the report exists, force resize

  RECT rect;
  GetWindowRect(&rect);
  OnSize(SIZE_RESTORED,rect.right-rect.left,rect.bottom-rect.top);
  }

//

void CLogReportView::OnSize(UINT nType, int cx, int cy) 
  {
	CFormView::OnSize(nType, cx, cy);
	
  if(m_Report.m_hWnd)
    {                                                       // otherwise not created yet
    m_Report.SetWindowPos(NULL,0,0,cx,cy,SWP_NOMOVE | SWP_NOZORDER);

    int iWidth = cx-20;                                     // border & vertical scrollbar
    iWidth -= m_Report.GetColumnWidth(0);                   // ID
    iWidth -= m_Report.GetColumnWidth(1);                   // Type
    if(iWidth < 100) iWidth = 100;
    m_Report.SetColumnWidth(2,iWidth);

    iWidth = cx-20;
    iWidth -= m_Report.GetColumnWidth(3);                   // Time
    iWidth -= m_Report.GetColumnWidth(4);                   // Date
    iWidth -= m_Report.GetColumnWidth(6);                   // Line
    if(iWidth < 50) iWidth = 50;
    m_Report.SetColumnWidth(5,iWidth);
    }
  }

//

void CLogReportView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
  {
  switch(lHint)
    {
    case LOGREPORT_ADDTXT : AddMessage((CLogReportData*)pHint); break;
    case LOGREPORT_CLEAR  : m_Report.DeleteAllItems(); break;
    }
  }

//

void CLogReportView::AddMessage(CLogReportData* pLogData)
  {
  if(!pLogData) return;
  CString strNum,strLine;
  DWORD   dwNum = pLogData->m_dwID;
  if(dwNum)              strNum .Format("%u",dwNum);
  if(pLogData->m_dwLine) strLine.Format("%u",pLogData->m_dwLine);

  // multiple lines ?

  char szTmp[1024];                                         // need to work on a copy to NOT modify the LogData
  strncpy(szTmp,(LPCTSTR)pLogData->m_strMsg,1024);
  szTmp[1023] = 0;

  const char* pszLine = szTmp;
  AddSubMessage(strchr(pszLine,'\n'));

  //

  if(!m_Report.m_hWnd) return;

  m_Report.InsertItem (0,  strNum);
  m_Report.SetItemText(0,1,pLogData->m_strLevel);
  m_Report.SetItemText(0,2,szTmp);
  m_Report.SetItemText(0,3,pLogData->m_strTime);
  m_Report.SetItemText(0,4,pLogData->m_strDate);
  m_Report.SetItemText(0,5,pLogData->m_strFile);
  m_Report.SetItemText(0,6,strLine);

  m_Report.EnsureVisible(0,false);
  }

//

void CLogReportView::AddSubMessage(const char* pszMsg)
  {
  if(!pszMsg) return;
  char* pszEnter = strchr(pszMsg,'\n');

  if(pszEnter)
    {
    *pszEnter = 0;
    AddSubMessage(pszEnter+1);
    }

  if(!*pszMsg) return;
  m_Report.InsertItem (0,  "");
  m_Report.SetItemText(0,2,pszMsg);
  }
