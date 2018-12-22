// DlgRotation.cpp : implementation file
//

#include "stdafx.h"
#include "..\RTTIProps.h"
#include "DlgRotation.h"

#include "VectorMatrix.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRotation dialog


CDlgRotation::CDlgRotation(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRotation::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRotation)
	m_fEulerX = 0.0f;
	m_fEulerY = 0.0f;
	m_fEulerZ = 0.0f;
	//}}AFX_DATA_INIT
}


void CDlgRotation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRotation)
	DDX_Text(pDX, IDC_EULERX, m_fEulerX);
	DDX_Text(pDX, IDC_EULERY, m_fEulerY);
	DDX_Text(pDX, IDC_EULERZ, m_fEulerZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRotation, CDialog)
	//{{AFX_MSG_MAP(CDlgRotation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRotation message handlers

void CDlgRotation::OnOK() 
  {
  m_qRot.Set(0.f,0.f,0.f,1.f);
  UpdateData();

  GPG4::CMat4x4 m4Rot;
  m4Rot.RotationXYZ(GPG4::CVect3D(DEG2RAD(m_fEulerX),DEG2RAD(m_fEulerY),DEG2RAD(m_fEulerZ)));
  m_qRot.FromRotation(m4Rot);
	
	CDialog::OnOK();
  }
