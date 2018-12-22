// PropView.cpp : implementation file
//

#include "stdafx.h"
#include "RTTIProps.h"
#include "RTTIPropsDoc.h"
#include "PropView.h"

#include "CustomMsgs.h"

#define IDC_GRID 100
#include "GridCtrl\GridCtrl.h"
#include "GridCtrl\GridBtnCell.h"
#include "GridCtrl\GridCellCombo.h"

#include "Persistent.h"
#include "Properties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropView

IMPLEMENT_DYNCREATE(CPropView, CFormView)

CPropView::CPropView()
	: CFormView(CPropView::IDD)
{
	//{{AFX_DATA_INIT(CPropView)
  m_strName = _T("Name");
	//}}AFX_DATA_INIT

  m_pGrid = NULL;
  m_pObj  = NULL;
}

CPropView::~CPropView()
{
  delete m_pGrid;
}

void CPropView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropView)
	DDX_Control(pDX, IDC_NAME, m_Name);
  DDX_Control(pDX, IDC_TAB, m_Tab);
  DDX_Text(pDX, IDC_NAME, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropView, CFormView)
	//{{AFX_MSG_MAP(CPropView)
  ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	//}}AFX_MSG_MAP
  ON_NOTIFY(GVN_BEGINLABELEDIT, IDC_GRID, OnGridBeginEdit)
  ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnGridEndEdit)
  ON_NOTIFY(NM_CLICK, IDC_GRID, OnGridClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropView diagnostics

#ifdef _DEBUG
void CPropView::AssertValid() const
{
	CFormView::AssertValid();
}

void CPropView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CRTTIPropsDoc* CPropView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRTTIPropsDoc)));
	return (CRTTIPropsDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPropView message handlers

void CPropView::OnInitialUpdate() 
  {
  CFormView::OnInitialUpdate();
  
  // grid

  RECT rect;

  if(!m_pGrid)
    {
    m_pGrid = new CGridCtrl(15,4,1,0);
    if(!m_pGrid) return;
    GetClientRect(&rect);
    m_pGrid->SetDefCellHeight(16);
    m_pGrid->Create(rect,this,IDC_GRID);

    m_pGrid->SetColumnWidth(0,150);
    m_pGrid->SetColumnWidth(1,100);
    m_pGrid->SetColumnWidth(2,200);
    m_pGrid->SetColumnWidth(3,150);

    m_pGrid->GetCell(0,0)->SetText("Property");
    m_pGrid->GetCell(0,1)->SetText("Type");
    m_pGrid->GetCell(0,2)->SetText("Value");
    m_pGrid->GetCell(0,3)->SetText("Help");

    ResetGridItemData();
    }

  // now that the tab & grid exist, force resize

  GetWindowRect(&rect);
  OnSize(SIZE_RESTORED,rect.right-rect.left,rect.bottom-rect.top);

  // combos & buttons

  m_BtnDataBase.SetGrid(m_pGrid);

  // an object has been set before the view was initialized ?

  if(m_pObj)
    {
    GPG4::CPersistent* pObj = m_pObj;
    m_pObj = NULL;
    SetObject(pObj);
    }
  }

//

void CPropView::ResetGridItemData()
  {
  if(!m_pGrid) return;
  int iRow = m_pGrid->GetRowCount();

  for(int i = 0; i < iRow; i++)
    {
    m_pGrid->SetItemData(i,0,(LPARAM)NULL);
    m_pGrid->SetItemData(i,1,(LPARAM)NULL);
    }
  }

//

void CPropView::OnSize(UINT nType, int cx, int cy) 
  {
  CFormView::OnSize(nType, cx, cy);
  RECT rect;
  GetClientRect(&rect);

  // resize tab

  if(m_Tab.m_hWnd)
    {
    int iSizeX = rect.right-rect.left;
    int iSizeY = rect.bottom-rect.top;
    if((iSizeX > 0) && (iSizeY > 0)) m_Tab.SetWindowPos(NULL,0,0,iSizeX,iSizeY,SWP_NOZORDER);
    }

  // resize grid

  if(m_pGrid)
    {
    int iSizeX = rect.right -7;
    int iSizeY = rect.bottom-28;
    if((iSizeX > 0) && (iSizeY > 0)) m_pGrid->SetWindowPos(NULL,3,24,iSizeX,iSizeY,SWP_NOZORDER);
    }

  // resize name

  ResizeName();
  }

//

void CPropView::ResizeName()
  {
  // resize & move Name

  if(m_Name.m_hWnd && m_Tab.m_hWnd)
    {
    RECT wnd;
    GetWindowRect(&wnd);

    int  iCount = m_Tab.GetItemCount();
    RECT rect;
    if(iCount) m_Tab.GetItemRect(iCount-1,&rect);
    else rect.bottom = rect.left = rect.right = rect.top = 0;

    RECT save;
    m_Name.GetWindowRect(&save);
    int iSizeX = wnd.right-wnd.left-(rect.right+10)-10;
    int iSizeY = save.bottom-save.top;
    if((iSizeX > 0) && (iSizeY > 0)) m_Name.SetWindowPos(&wndTop,rect.right+10,4,iSizeX,iSizeY,0);
    }
  }

//
// msgs
//

void CPropView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
  {
  switch(lHint)
    {
    case PROP_UPDATE : 
      {
      if(!pHint) { Update(); return; }

      // check tabs recursively

      GPG4::CPersistent* pTab = m_pObj;

      while(pTab)
        {
        if(pTab == (GPG4::CPersistent*)pHint)
          {
          Update();
          return;
          }
        pTab = GetNextTab(pTab);
        }
      break;
      }
                            
    //

    case SCENE_SELECTION : SetObject((GPG4::CPersistent*)pHint);   break;
    }
  }

// set object & init tabs

bool CPropView::SetObject(GPG4::CPersistent* pObj)
  {
  if(m_pObj == pObj) return true;                           // already set
  m_pObj = pObj;

  if(!m_Tab.m_hWnd)  return true;
  m_Tab.DeleteAllItems();

  if(m_pGrid) 
    {                                                       // delete the content of all rows
    m_pGrid->SetRowCount(1);
    m_pGrid->SetRowCount(15);
    ResetGridItemData();
    }

  if(!m_pObj) return true;

  // tabs

  GPG4::CPersistent* pTab = m_pObj;
  if(IS_KIND_OF(GPG4::CEditorSceneItem,pTab))               // skip CEditorSceneItem
    pTab = GetNextTab(pTab);

  DWORD  dwTab = 0;
  TCITEM tcItem;
  tcItem.mask = TCIF_PARAM;
  m_Tab.InsertItem(dwTab,"All");
  tcItem.lParam = (LPARAM)pTab;
  m_Tab.SetItem(dwTab++,&tcItem);

  while(pTab)
    {
    m_Tab.InsertItem(dwTab, pTab->GetEditName());
    tcItem.lParam = (LPARAM)pTab;
    m_Tab.SetItem(dwTab,&tcItem);

    dwTab++;
    pTab = GetNextTab(pTab);
    }

  m_Tab.SetCurSel(0);
  ResizeName();

  // fill report

  Update();
  return true;
  }

//

void CPropView::Update()
  {
  FillTab();

  if(!m_pObj) m_Name.SetWindowText("");
  else        m_Name.SetWindowText(m_pObj->GetName());
  }

//

GPG4::CPersistent* CPropView::GetNextTab(GPG4::CPersistent* pObj)
  {
  GPG4::CEditorSceneItem* pItem = DYNAMIC_CAST(GPG4::CEditorSceneItem,pObj);
  if(pItem) return pItem->GetObject();

  GPG4::CEditorObj* pEdObj = DYNAMIC_CAST(GPG4::CEditorObj,pObj);
  if(pEdObj) return pEdObj->GetEngineObj();

  return NULL;
  }

//

void CPropView::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
  {
	*pResult = 0;
  if(!m_pGrid) return;

  // delete grid content

  m_pGrid->SetRowCount(1);
  m_pGrid->SetRowCount(15);
  ResetGridItemData();

  FillTab();
  }

// called to fill a tab

void CPropView::FillTab()
  {
  if(!m_pGrid) return;

  CCellID cell = m_pGrid->GetFocusCell();
  if(m_pGrid->IsValid(cell))
    {
    CGridCellBase* pCell = m_pGrid->GetCell(cell.row,cell.col);
    if(pCell) pCell->EndEdit();                             // end editing
    }

  // fill grid

  if(!m_Tab.m_hWnd) return;
  int sel_index = m_Tab.GetCurSel();
  TCITEM tcItem;
  tcItem.mask = TCIF_PARAM;
  m_Tab.GetItem(sel_index,&tcItem);
  GPG4::CPersistent* pTab = (GPG4::CPersistent*)tcItem.lParam;
  DWORD dwRow = 0;

  while(pTab)
    {
    const GPG4::CRTTI* pRTTI = pTab->GetRTTI();

    while(pRTTI)
      {
      GPG4::CExtraData* pData  = pRTTI->GetExtraData();
      GPG4::CExtraProp* pExtra = DYNAMIC_CAST(GPG4::CExtraProp,pData);
      if(pExtra)
        {
        GPG4::CPropList* pProps = pExtra->GetPropList();
        if(pProps)
          {
          GPG4::CProperty* pItem = pProps->GetFirstProp();
          while(pItem && !PropToBeShown(pTab,pItem)) pItem = pProps->GetNextProp();

          if(pItem)
            {                                               // at least 1 exposed item
            if(dwRow)
              {                                             // otherwise don't add an empty line as the 1st one
              if(dwRow >= DWORD(m_pGrid->GetRowCount()-1)) m_pGrid->InsertRow(NULL);
              m_pGrid->SetItemData(dwRow+1,0,(LPARAM)NULL);
              dwRow++;
              }

            FillProps(pTab,pProps,&dwRow);
            }
          }
        }

      pRTTI = pRTTI->GetBaseRTTI();
      if(pRTTI == &GPG4::CPersistent::m_RTTI) break;        // stop at CPersistent
      }

    if(sel_index) pTab = NULL;
    else          pTab = GetNextTab(pTab);                  // "All" tab => recurse
    }

  m_pGrid->Invalidate();
  }

// add properties to the grid

void CPropView::FillProps(GPG4::CPersistent* pObj,GPG4::CPropList* pDesc,DWORD* pdwRow)
  {
  if(!pObj || !pDesc || !pdwRow) return;
  GPG4::CProperty* pItem = pDesc->GetFirstProp();
  DWORD dwRow = *pdwRow;

  while(pItem)
    {
    if(PropToBeShown(pObj,pItem))
      {
      if(dwRow >= DWORD(m_pGrid->GetRowCount()-1)) m_pGrid->InsertRow(NULL);

      // boolean : true/false button

      if((pItem->GetType() == "Bool") && !PropReadOnly(pObj,pItem))
        {
        m_pGrid->SetCellType(dwRow+1,2,RUNTIME_CLASS(CGridBtnCell));
        CGridBtnCell* pCell = (CGridBtnCell*)m_pGrid->GetCell(dwRow+1,2);
        pCell->SetBtnDataBase(&m_BtnDataBase);
        pCell->SetupBtns( 0,                                // zero-based index of image to draw
                          DFC_BUTTON,                       // type of frame control to draw e.g. DFC_BUTTON
                          DFCS_BUTTONPUSH,                  // like DrawFrameControl()'s nState  e.g. DFCS_BUTTONCHECK
                          CGridBtnCellBase::CTL_ALIGN_CENTER,
                                                            // horizontal alignment of control image
                          0,                                // fixed width of control or 0 for size-to-fit
                          FALSE,                            // T=btn is member of a radio group
                          pItem->GetValue(pObj));           // Text to insert centered in button; if NULL no text
        }

      // special editing : button

      if((pItem->GetCtrlType() == GPG4::CProperty::BUTTON) && !PropReadOnly(pObj,pItem))
        {
        m_pGrid->SetCellType(dwRow+1,2,RUNTIME_CLASS(CGridBtnCell));
        CGridBtnCell* pCell = (CGridBtnCell*)m_pGrid->GetCell(dwRow+1,2);
        pCell->SetBtnDataBase(&m_BtnDataBase);
        pCell->SetupBtns( 0,
                          DFC_BUTTON,
                          DFCS_BUTTONPUSH,
                          CGridBtnCellBase::CTL_ALIGN_CENTER,
                          0,
                          FALSE,
                          SpecialGetValue(pObj,pItem));     // this comes with SpecialEditing
        }

      // property's data

      m_pGrid->SetItemText(dwRow+1,0,pItem->GetName());
      m_pGrid->SetItemText(dwRow+1,1,pItem->GetType());
      m_pGrid->SetItemText(dwRow+1,2,pItem->GetValue(pObj));
      m_pGrid->SetItemText(dwRow+1,3,pItem->GetHelp());

      m_pGrid->SetItemData(dwRow+1,0,(LPARAM)pItem);
      m_pGrid->SetItemData(dwRow+1,1,(LPARAM)pObj);

      // read only

      if(PropReadOnly(pObj,pItem))
        {                                                   // not editable
        CGridCellBase* pCell = m_pGrid->GetCell(dwRow+1,2);
        pCell->SetBackClr(RGB(255,255,192));
        }

      // color

      if(pItem->GetCtrlType() == GPG4::CProperty::COLOR)
        {
        GPG4::CPropVect4D* pPropV4 = DYNAMIC_CAST(GPG4::CPropVect4D,pItem);
        if(pPropV4)
          {
          GPG4::CVect4D v4Color = pPropV4->Get(pObj);
          CGridCellBase* pCell  = m_pGrid->GetCell(dwRow+1,3);
          pCell->SetBackClr(RGB(v4Color.r*255,v4Color.g*255,v4Color.b*255));
          }
        }

      dwRow++;
      }

    pItem = pDesc->GetNextProp();
    }

  *pdwRow = dwRow;
  }

//

bool CPropView::PropToBeShown(GPG4::CPersistent* pObj,GPG4::CProperty* pProp)
  {
  if(!pObj || !pProp) return false;
  if(!pObj->IsPropExposed(pProp)) return false;
  if(!GetDocument()->GetReadOnlyProps() && PropReadOnly(pObj,pProp)) return false;
  return true;
  }

//

bool CPropView::PropReadOnly(GPG4::CPersistent* pObj,GPG4::CProperty* pProp)
  {
  if(!pObj || !pProp) return true;
  return pObj->IsPropReadOnly(pProp);
  }

//
// grid

void CPropView::OnGridBeginEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
  {
  NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
  BOOL   boAllowEdit = (pItem->iColumn == 2);

  GPG4::CProperty*   pProp = (GPG4::CProperty*)  m_pGrid->GetItemData(pItem->iRow,0);
  GPG4::CPersistent* pObj  = (GPG4::CPersistent*)m_pGrid->GetItemData(pItem->iRow,1);

  if(!pProp || !pObj) boAllowEdit = FALSE;
  else
    {
    if(!pProp->GetSize())        boAllowEdit = FALSE;       // empty line
    if(PropReadOnly(pObj,pProp)) boAllowEdit = FALSE;
    }

  *pResult = (boAllowEdit ? 0 : -1);
  }

//

void CPropView::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
  {
  NM_GRIDVIEW*  pItem = (NM_GRIDVIEW*)pNotifyStruct;
  CString      strNew = m_pGrid->GetItemText(pItem->iRow,pItem->iColumn);
  BOOL boAcceptChange = FALSE;

  GPG4::CProperty*   pProp = (GPG4::CProperty*)  m_pGrid->GetItemData(pItem->iRow,0);
  GPG4::CPersistent* pObj  = (GPG4::CPersistent*)m_pGrid->GetItemData(pItem->iRow,1);

  if(pProp && pObj)
    {
    boAcceptChange = ModifyProp(pObj,pProp,(LPCTSTR)strNew);

    if(boAcceptChange)
      {
      strNew = pProp->GetValue(pObj);                       // re-read
      Update();

      GetDocument()->UpdateAllViews(this,PROP_CHANGED,(CObject*)pObj);
      GetDocument()->SetModifiedFlag();

      CString strMsg;
      strMsg.Format("property [%s] of [%s] set to [%s]",pProp->GetName().c_str(),pObj->GetName().c_str(),pProp->GetValue(pObj).c_str());
      _DLOG2_((*GetDocument()->GetLogger()),strMsg,"grid op");
      }
    }

  *pResult = (boAcceptChange ? 0 : -1);
  }

//

void CPropView::OnGridClick(NMHDR *pNotifyStruct, LRESULT* pResult)
  {
  NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
  if(pItem->iColumn != 2) return;                           // only 2nd column can be clicked

  CGridBtnCell* pGridBtnCell = (CGridBtnCell*)m_pGrid->GetCell(pItem->iRow,pItem->iColumn);
  if(!pGridBtnCell || !pGridBtnCell->IsKindOf(RUNTIME_CLASS(CGridBtnCell))) return;

  GPG4::CProperty*   pProp = (GPG4::CProperty*)  m_pGrid->GetItemData(pItem->iRow,0);
  GPG4::CPersistent* pObj  = (GPG4::CPersistent*)m_pGrid->GetItemData(pItem->iRow,1);
  if(!pProp || !pObj)          return;
  if(PropReadOnly(pObj,pProp)) return;

  CStdString strOld = pProp->GetValue(pObj);
  CStdString strNew;

  if(pProp->GetType() == "Bool")
    {
    if(strOld == "false") strNew = "true";
    else                  strNew = "false";
    }

  else
    {                                                       // special editing
    if(!SpecialEditing(pObj,pProp,&strNew)) return;
    }

  if(!ModifyProp(pObj,pProp,strNew)) return;                // true = modif handled
  strNew = pProp->GetValue(pObj);                           // re-read

  Update();

  GetDocument()->UpdateAllViews(this,PROP_CHANGED,(CObject*)pObj);
  GetDocument()->SetModifiedFlag();

  CString strMsg;
  if(pProp->GetType() == "Bool")
    strMsg.Format("property [%s] of [%s] set to [%s]",pProp->GetName().c_str(),pObj->GetName().c_str(),strNew.c_str());
  else
    strMsg.Format("property [%s] of [%s] set to [%s]",pProp->GetName().c_str(),pObj->GetName().c_str(),SpecialGetValue(pObj,pProp).c_str());
  _DLOG2_((*GetDocument()->GetLogger()),strMsg,"grid op");

  *pResult = 0;
  }

// eventually update other properties, do some stuff

bool CPropView::ModifyProp(GPG4::CPersistent* pObj,GPG4::CProperty* pProp,const CStdString& strNew)
  {
  if(!pObj || !pProp) return false;

  // process modif

  bool boRes = pObj->ModifyProp(pProp,strNew);              // true = modif handled
  if(!boRes) boRes = pProp->SetValue(pObj,strNew);          // this is for all "non-special" props
  if(!boRes) return false;

  // doc/UI updates (can only be done here)

  if(pObj->IsPropOfRTTI(pProp,&GPG4::CEditorObj::m_RTTI))
    {
    if(pProp->GetName() == "Selected")
      {
      GetDocument()->ModifySelection(DYNAMIC_CAST(GPG4::CEditorObj,pObj));
      GetDocument()->UpdateAllViews(NULL,DOC_SELECTION_CHANGED,(CObject*)pObj);
      }
    }

  return true;
  }

//

bool CPropView::SpecialEditing(GPG4::CPersistent* pObj,GPG4::CProperty* pProp,CStdString* pstrNew)
  {
  if(!pObj || !pProp || !pstrNew) return false;
  GPG4::CEditorObj* pEdObj = DYNAMIC_CAST(GPG4::CEditorObj,pObj);
  if(!pEdObj)
    {
    GPG4::CEngineObj* pEngObj = DYNAMIC_CAST(GPG4::CEngineObj,pObj);
    if(!pEngObj) return false;
    pEdObj = pEngObj->GetEditorObj();
    if(!pEdObj)  return false;
    }

  return pEdObj->SpecialEditing(pProp,pstrNew,GetDocument());
  }

//

CStdString CPropView::SpecialGetValue(GPG4::CPersistent* pObj,GPG4::CProperty* pProp)
  {
  CStdString strRes = pProp->GetValue(pObj);
  if(!pObj || !pProp) return strRes;

  GPG4::CEditorObj* pEdObj = DYNAMIC_CAST(GPG4::CEditorObj,pObj);
  if(!pEdObj)
    {
    GPG4::CEngineObj* pEngObj = DYNAMIC_CAST(GPG4::CEngineObj,pObj);
    if(!pEngObj) return strRes;
    pEdObj = pEngObj->GetEditorObj();
    if(!pEdObj)  return strRes;
    }

  return pEdObj->SpecialGetValue(pProp);
  }
