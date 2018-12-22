// CDlgEditCamera.cpp : implementation file
//

#include "stdafx.h"
#include "..\RTTIProps.h"
#include "DlgEditCamera.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// static members

int   CDlgEditCamera::iDir  = 4;

float CDlgEditCamera::fFov  = 60.f;
float CDlgEditCamera::fNear = 1.f;
float CDlgEditCamera::fFar  = 1000.f;

float CDlgEditCamera::fPosX = 0.f;
float CDlgEditCamera::fPosY = 0.f;
float CDlgEditCamera::fPosZ = 0.f;

/////////////////////////////////////////////////////////////////////////////
// CDlgEditCamera dialog


CDlgEditCamera::CDlgEditCamera(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditCamera::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgEditCamera)
	m_iDir = 4;
	m_fFar = 1000.0f;
	m_fFov = 60.0f;
	m_fNear = 1.0f;
	m_fPosX = 0.0f;
	m_fPosY = 0.0f;
	m_fPosZ = 0.0f;
	//}}AFX_DATA_INIT

  m_boInitFromStatic = true;
}


void CDlgEditCamera::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEditCamera)
	DDX_Radio(pDX, IDC_DIR0, m_iDir);
	DDX_Text(pDX, IDC_FAR, m_fFar);
	DDX_Text(pDX, IDC_FOV, m_fFov);
	DDX_Text(pDX, IDC_NEAR, m_fNear);
	DDX_Text(pDX, IDC_POSX, m_fPosX);
	DDX_Text(pDX, IDC_POSY, m_fPosY);
	DDX_Text(pDX, IDC_POSZ, m_fPosZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgEditCamera, CDialog)
	//{{AFX_MSG_MAP(CDlgEditCamera)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEditCamera message handlers

BOOL CDlgEditCamera::OnInitDialog() 
  {
	CDialog::OnInitDialog();
	
  // remember last chosen values

  if(m_boInitFromStatic)
    {
    m_iDir  = iDir;

    m_fFov  = fFov;
    m_fNear = fNear;
    m_fFar  = fFar;

    m_fPosX = fPosX;
    m_fPosY = fPosY;
    m_fPosZ = fPosZ;
  
    UpdateData(false);
    }

  //
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditCamera::OnOK() 
{
  UpdateData(true);

  // save chosen values

  if(m_boInitFromStatic)
    {
    iDir  = m_iDir;

    fFov  = m_fFov;
    fNear = m_fNear;
    fFar  = m_fFar;

    fPosX = m_fPosX;
    fPosY = m_fPosY;
    fPosZ = m_fPosZ;
    }

  //
	
	CDialog::OnOK();
}
