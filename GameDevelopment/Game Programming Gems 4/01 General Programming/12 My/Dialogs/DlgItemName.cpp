// dlgitemname.cpp : implementation file
//

#include "stdafx.h"
#include "..\RTTIProps.h"
#include "DlgItemName.h"

#include "EditorScene.h"
#include "EditorSceneItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgItemName dialog


CDlgItemName::CDlgItemName(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgItemName::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgItemName)
	m_strName = _T("");
	//}}AFX_DATA_INIT
  m_pEditorScene = NULL;
}


void CDlgItemName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgItemName)
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDV_MaxChars(pDX, m_strName, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgItemName, CDialog)
	//{{AFX_MSG_MAP(CDlgItemName)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgItemName message handlers

BOOL CDlgItemName::OnInitDialog() 
  {
	CDialog::OnInitDialog();

	if(!m_pEditorScene) OnCancel();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

//

void CDlgItemName::OnOK() 
  {
  CString strOldName = m_strName;
  UpdateData(true);

  // already existing name ?

  if(m_strName.IsEmpty() || m_pEditorScene->GetItem((LPCTSTR)m_strName))
    {
    AfxMessageBox("this name is already used ; choose another one",MB_OK);
    m_strName = strOldName;
    UpdateData(false);
    ((CEdit*)GetDlgItem(IDC_NAME))->SetSel(0,-1);
    return;
    }

  //

	CDialog::OnOK();
  }

//

void CDlgItemName::OnChangeName() 
  {
  CString strOldName = m_strName;
  UpdateData(true);

  int iPos = m_strName.FindOneOf("&<>\\\"");                // the chrs &<>\" are forbidden
  if(iPos != -1)
    {
    m_strName = strOldName;
    UpdateData(false);
    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_NAME);
    if(pEdit) pEdit->SetSel(m_strName.GetLength(),m_strName.GetLength());
    }
  }
