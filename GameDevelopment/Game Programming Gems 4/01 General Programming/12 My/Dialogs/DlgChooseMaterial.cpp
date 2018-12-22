// DlgChooseMaterial.cpp : implementation file
//

#include "stdafx.h"
#include "..\RTTIProps.h"
#include "DlgChooseMaterial.h"

#include "EditorSceneItem.h"
#include "EditorMaterial.h"
#include "EngineMaterial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgChooseMaterial dialog


CDlgChooseMaterial::CDlgChooseMaterial(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChooseMaterial::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgChooseMaterial)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

  m_pSceneItem = NULL;
  m_pSelected  = NULL;
}


void CDlgChooseMaterial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgChooseMaterial)
	DDX_Control(pDX, IDC_MATLIST, m_MatList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgChooseMaterial, CDialog)
	//{{AFX_MSG_MAP(CDlgChooseMaterial)
	ON_BN_CLICKED(IDC_NOMAT, OnNomat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgChooseMaterial message handlers

BOOL CDlgChooseMaterial::OnInitDialog() 
  {
	CDialog::OnInitDialog();
  ListView_SetExtendedListViewStyle(m_MatList.m_hWnd,LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
  m_MatList.InsertColumn(0,"Material",LVCFMT_LEFT,250);

  FillList(m_pSceneItem);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

// recursive

void CDlgChooseMaterial::FillList(GPG4::CEditorSceneItem* pItem)
  {
  if(!pItem) return;
  GPG4::CEditorObj*      pObj   = pItem->GetObject();
  GPG4::CEditorMaterial* pEdMat = DYNAMIC_CAST(GPG4::CEditorMaterial,pObj);

  if(pEdMat)
    {
    GPG4::CEngineMaterial* pMat = DYNAMIC_CAST(GPG4::CEngineMaterial,pEdMat->GetEngineObj());

    CStdString strName = pEdMat->GetName();
    if(!strName.empty() && pMat)
      {
      int nItem = m_MatList.InsertItem(LVIF_TEXT | LVIF_PARAM,0,strName.c_str(),0,0,0,LPARAM(pMat));
      if((nItem != -1) && (pMat == m_pSelected))
        {
        m_MatList.SetItemState(nItem,LVIS_SELECTED,LVIS_SELECTED);
        }
      }
    }

  GPG4::CEditorSceneItem* pChild = pItem->GetFirstChild();
  while(pChild)
    {
    FillList(pChild);
    pChild = pItem->GetNextChild();
    }
  }

//

void CDlgChooseMaterial::OnNomat() 
  {
  m_pSelected = NULL;
	CDialog::OnOK();
  }

//

void CDlgChooseMaterial::OnOK() 
  {
  m_pSelected  = NULL;

  POSITION pos = m_MatList.GetFirstSelectedItemPosition();
  if(pos != NULL)
    {
    int nItem = m_MatList.GetNextSelectedItem(pos);
    GPG4::CEngineMaterial* pMat = (GPG4::CEngineMaterial*)m_MatList.GetItemData(nItem);
    if(pMat) m_pSelected = pMat;
    }
	
	CDialog::OnOK();
  }
