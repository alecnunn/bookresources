/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
// TLDialog.cpp : implementation file
//

#include "stdafx.h"
#include "linkerdialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTLDialog dialog


CTLDialog::CTLDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CTLDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTLDialog)
	m_dwNumDirectionalLights = 0;
	m_dwNumPointLights = 0;
	m_TexGen = -1;
	m_bEyeSpace = FALSE;
	m_bSpecular = FALSE;
	m_bLocalViewer = FALSE;
	m_bLighting = FALSE;
	m_bTextures = FALSE;
	m_bWhiteLight = FALSE;
	m_bSpecularRDotL = FALSE;
	m_Fog = -1;
	//}}AFX_DATA_INIT
}


void CTLDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTLDialog)
	DDX_Control(pDX, IDC_SPIN_NUMPOINT, m_PointSpin);
	DDX_Control(pDX, IDC_SPIN_NUMDIRECTIONAL, m_DirectionalSpin);
	DDX_Text(pDX, IDC_EDIT_NUMDIRECTIONAL, m_dwNumDirectionalLights);
	DDV_MinMaxDWord(pDX, m_dwNumDirectionalLights, 0, 99);
	DDX_Text(pDX, IDC_EDIT_NUMPOINT, m_dwNumPointLights);
	DDV_MinMaxDWord(pDX, m_dwNumPointLights, 0, 99);
	DDX_Radio(pDX, IDC_TEXGEN_NONE, m_TexGen);
	DDX_Check(pDX, IDC_LIGHTING_EYESPACE, m_bEyeSpace);
	DDX_Check(pDX, IDC_LIGHTING_SPECULAR, m_bSpecular);
	DDX_Check(pDX, IDC_LIGHTING_LOCALVIEWER, m_bLocalViewer);
	DDX_Check(pDX, IDC_ENABLELIGHTING, m_bLighting);
	DDX_Check(pDX, IDC_ENABLETEXTURES, m_bTextures);
	DDX_Check(pDX, IDC_WHITELIGHT, m_bWhiteLight);
	DDX_Check(pDX, IDC_LIGHTING_SPECULARREFLECT, m_bSpecularRDotL);
	DDX_Radio(pDX, IDC_FOGNONE, m_Fog);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTLDialog, CDialog)
	//{{AFX_MSG_MAP(CTLDialog)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTLDialog message handlers

int CTLDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

BOOL CTLDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_DirectionalSpin.SetRange(0, 99);	
	m_PointSpin.SetRange(0, 99);	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
