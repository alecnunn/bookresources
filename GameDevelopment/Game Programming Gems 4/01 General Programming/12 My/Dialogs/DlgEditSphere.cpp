// DlgEditSphere.cpp : implementation file
//

#include "stdafx.h"
#include "..\RTTIProps.h"
#include "DlgEditSphere.h"

#include "EngineMaterial.h"
#include "DlgChooseMaterial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// static members

float CDlgEditSphere::fPosX = 0.f;
float CDlgEditSphere::fPosY = 0.f;
float CDlgEditSphere::fPosZ = 0.f;

float CDlgEditSphere::fRadius = 1.f;
UINT  CDlgEditSphere::uiLOD   = 8;

BOOL  CDlgEditSphere::boNewMat = TRUE;

/////////////////////////////////////////////////////////////////////////////
// CDlgEditSphere dialog


CDlgEditSphere::CDlgEditSphere(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditSphere::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgEditSphere)
	m_uiLOD = 0;
	m_boNewMat = FALSE;
	m_fPosX = 0.0f;
	m_fPosY = 0.0f;
	m_fPosZ = 0.0f;
	m_fRadius = 0.0f;
	m_strMaterial = _T("None");
	//}}AFX_DATA_INIT

  m_boInitFromStatic = true;
  m_pMat  = NULL;
  m_pItem = NULL;
}


void CDlgEditSphere::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEditSphere)
	DDX_Text(pDX, IDC_LOD, m_uiLOD);
	DDX_Check(pDX, IDC_NEWMAT, m_boNewMat);
	DDX_Text(pDX, IDC_POSX, m_fPosX);
	DDX_Text(pDX, IDC_POSY, m_fPosY);
	DDX_Text(pDX, IDC_POSZ, m_fPosZ);
	DDX_Text(pDX, IDC_RADIUS, m_fRadius);
	DDX_Text(pDX, IDC_MATNAME, m_strMaterial);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgEditSphere, CDialog)
	//{{AFX_MSG_MAP(CDlgEditSphere)
	ON_BN_CLICKED(IDC_CHOOSEMAT, OnChoosemat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEditSphere message handlers

BOOL CDlgEditSphere::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  // remember last chosen values

  if(m_boInitFromStatic)
    {
    m_fPosX = fPosX;
    m_fPosY = fPosY;
    m_fPosZ = fPosZ;

    m_fRadius  = fRadius;
    m_uiLOD    = uiLOD;
    
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

void CDlgEditSphere::OnOK() 
  {
  UpdateData(true);

  // save chosen values

  if(m_boInitFromStatic)
    {
    fPosX = m_fPosX;
    fPosY = m_fPosY;
    fPosZ = m_fPosZ;

    fRadius  = m_fRadius;
    uiLOD    = m_uiLOD;

    boNewMat = m_boNewMat;
    }

  //

	CDialog::OnOK();
  }

//

void CDlgEditSphere::OnChoosemat() 
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

void CDlgEditSphere::UpdateMatName()
  {
  m_strMaterial = "None";
  if(m_pMat)
    {
    m_strMaterial = m_pMat->GetName();
    if(m_strMaterial.IsEmpty()) m_strMaterial = "UNNAMED";
    }
  UpdateData(false);
  }
