/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#include "stdafx.h"
#include "csg_bsp.h"
#include "DlgHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------------------
DlgHelp::DlgHelp(CWnd* pParent /*=NULL*/)
	: CDialog(DlgHelp::IDD, pParent)
{
}

//---------------------------------------------------------------------------------------
void DlgHelp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgHelp, CDialog)
	ON_BN_DOUBLECLICKED(IDOK, OnDoubleclickedOk)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------
void DlgHelp::OnOK() 
{
	CDialog::OnOK();
}

//---------------------------------------------------------------------------------------
void DlgHelp::OnDoubleclickedOk() 
{
}

//---------------------------------------------------------------------------------------
void DlgHelp::OnCancel() 
{
	CDialog::OnCancel();
}
