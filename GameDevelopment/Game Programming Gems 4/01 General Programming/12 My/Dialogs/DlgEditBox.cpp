// DlgEditBox.cpp : implementation file
//

#include "stdafx.h"
#include "..\RTTIProps.h"
#include "DlgEditBox.h"

#include "EngineMaterial.h"
#include "DlgChooseMaterial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// static members

float CDlgEditBox::fDimX = 1.f;
float CDlgEditBox::fDimY = 1.f;
float CDlgEditBox::fDimZ = 1.f;

float CDlgEditBox::fPosX = 0.f;
float CDlgEditBox::fPosY = 0.f;
float CDlgEditBox::fPosZ = 0.f;

int   CDlgEditBox::iPosX = 1;
int   CDlgEditBox::iPosY = 1;
int   CDlgEditBox::iPosZ = 1;

BOOL  CDlgEditBox::boNewMat = TRUE;

/////////////////////////////////////////////////////////////////////////////
// CDlgEditBox dialog


CDlgEditBox::CDlgEditBox(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgEditBox)
	m_fDimX = 0.0f;
	m_fDimY = 0.0f;
	m_fDimZ = 0.0f;
	m_fPosX = 0.0f;
	m_fPosY = 0.0f;
	m_fPosZ = 0.0f;
	m_iPosX = 1;
	m_iPosY = 1;
	m_iPosZ = 1;
	m_boNewMat = FALSE;
	m_strMaterial = _T("None");
	//}}AFX_DATA_INIT

  m_boInitFromStatic = true;
  m_pMat  = NULL;
  m_pItem = NULL;
}


void CDlgEditBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEditBox)
	DDX_Text(pDX, IDC_DIMX, m_fDimX);
	DDX_Text(pDX, IDC_DIMY, m_fDimY);
	DDX_Text(pDX, IDC_DIMZ, m_fDimZ);
	DDX_Text(pDX, IDC_POSX, m_fPosX);
	DDX_Text(pDX, IDC_POSY, m_fPosY);
	DDX_Text(pDX, IDC_POSZ, m_fPosZ);
	DDX_Radio(pDX, IDC_POSXMIN, m_iPosX);
	DDX_Radio(pDX, IDC_POSYMIN, m_iPosY);
	DDX_Radio(pDX, IDC_POSZMIN, m_iPosZ);
	DDX_Check(pDX, IDC_NEWMAT, m_boNewMat);
	DDX_Text(pDX, IDC_MATNAME, m_strMaterial);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgEditBox, CDialog)
	//{{AFX_MSG_MAP(CDlgEditBox)
	ON_BN_CLICKED(IDC_CHOOSEMAT, OnChoosemat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEditBox message handlers

BOOL CDlgEditBox::OnInitDialog() 
  {
	CDialog::OnInitDialog();

  // remember last chosen values

  if(m_boInitFromStatic)
    {
    m_fDimX = fDimX;
    m_fDimY = fDimY;
    m_fDimZ = fDimZ;

    m_fPosX = fPosX;
    m_fPosY = fPosY;
    m_fPosZ = fPosZ;
  
    m_iPosX = iPosX;
    m_iPosY = iPosY;
    m_iPosZ = iPosZ;

    m_boNewMat = boNewMat;
    }

  else
    {
    UpdateMatName();
    }

  UpdateData(false);

  //

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

//

void CDlgEditBox::OnOK() 
  {
  UpdateData(true);

  // save chosen values

  if(m_boInitFromStatic)
    {
    fDimX = m_fDimX;
    fDimY = m_fDimY;
    fDimZ = m_fDimZ;

    fPosX = m_fPosX;
    fPosY = m_fPosY;
    fPosZ = m_fPosZ;
  
    iPosX = m_iPosX;
    iPosY = m_iPosY;
    iPosZ = m_iPosZ;

    boNewMat = m_boNewMat;
    }

  //

	CDialog::OnOK();
  }

//

void CDlgEditBox::OnChoosemat() 
  {
  CDlgChooseMaterial dlg;
  dlg.m_pSceneItem = m_pItem;
  dlg.m_pSelected  = m_pMat;
  if(dlg.DoModal() != IDOK) return;

  UpdateData(true);
  m_pMat = dlg.m_pSelected;
  m_boNewMat = false;
  UpdateMatName();
  }

//

void CDlgEditBox::UpdateMatName()
  {
  m_strMaterial = "None";
  if(m_pMat)
    {
    m_strMaterial = m_pMat->GetName();
    if(m_strMaterial.IsEmpty()) m_strMaterial = "UNNAMED";
    }
  UpdateData(false);
  }
