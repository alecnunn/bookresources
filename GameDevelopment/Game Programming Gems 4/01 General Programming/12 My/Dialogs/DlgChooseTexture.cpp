// DlgChooseTexture.cpp : implementation file
//

#include "stdafx.h"
#include "..\RTTIProps.h"
#include "DlgChooseTexture.h"

#include "EditorSceneItem.h"
#include "EditorTexture.h"
#include "EngineTexture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgChooseTexture dialog


CDlgChooseTexture::CDlgChooseTexture(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChooseTexture::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgChooseTexture)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

  m_pSceneItem = NULL;
  m_pSelected  = NULL;
}


void CDlgChooseTexture::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgChooseTexture)
	DDX_Control(pDX, IDC_TEXLIST, m_TexList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgChooseTexture, CDialog)
	//{{AFX_MSG_MAP(CDlgChooseTexture)
	ON_BN_CLICKED(IDC_NOTEX, OnNotex)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgChooseTexture message handlers

BOOL CDlgChooseTexture::OnInitDialog() 
  {
	CDialog::OnInitDialog();
  ListView_SetExtendedListViewStyle(m_TexList.m_hWnd,LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
  m_TexList.InsertColumn(0,"Texture",LVCFMT_LEFT,250);

  FillList(m_pSceneItem);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

// recursive

void CDlgChooseTexture::FillList(GPG4::CEditorSceneItem* pItem)
  {
  if(!pItem) return;
  GPG4::CEditorObj*       pObj = pItem->GetObject();
  GPG4::CEditorTexture* pEdTex = DYNAMIC_CAST(GPG4::CEditorTexture,pObj);

  if(pEdTex)
    {
    GPG4::CEngineTexture* pTex = DYNAMIC_CAST(GPG4::CEngineTexture,pEdTex->GetEngineObj());

    CStdString strName = pEdTex->GetName();
    if(!strName.empty() && pTex)
      {
      int nItem = m_TexList.InsertItem(LVIF_TEXT | LVIF_PARAM,0,strName.c_str(),0,0,0,LPARAM(pTex));
      if((nItem != -1) && (pTex == m_pSelected))
        {
        m_TexList.SetItemState(nItem,LVIS_SELECTED,LVIS_SELECTED);
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

void CDlgChooseTexture::OnNotex() 
  {
  m_pSelected = NULL;
	CDialog::OnOK();
  }

//

void CDlgChooseTexture::OnOK() 
  {
  m_pSelected  = NULL;

  POSITION pos = m_TexList.GetFirstSelectedItemPosition();
  if(pos != NULL)
    {
    int nItem = m_TexList.GetNextSelectedItem(pos);
    GPG4::CEngineTexture* pTex = (GPG4::CEngineTexture*)m_TexList.GetItemData(nItem);
    if(pTex) m_pSelected = pTex;
    }
	
	CDialog::OnOK();
  }
