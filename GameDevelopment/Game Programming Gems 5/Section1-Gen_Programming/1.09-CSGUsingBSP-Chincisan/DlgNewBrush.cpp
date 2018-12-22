/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/
#include "stdafx.h"
#include "csg_bsp.h"
#include "DlgNewBrush.h"

//---------------------------------------------------------------------------------------
// CDlgNewBrush dialog
CDlgNewBrush::CDlgNewBrush(CWnd* pParent)
        : CDialog(CDlgNewBrush::IDD, pParent)
{
}

//---------------------------------------------------------------------------------------
void CDlgNewBrush::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SPIN3, m_splr);
    DDX_Control(pDX, IDC_SPIN2, m_splv);
    DDX_Control(pDX, IDC_EDIT3, m_radstrips);
    DDX_Control(pDX, IDC_EDIT2, m_vstrips);
}

//---------------------------------------------------------------------------------------
// Message map
BEGIN_MESSAGE_MAP(CDlgNewBrush, CDialog)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
    ON_BN_CLICKED(IDOK, OnOk)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------
BOOL CDlgNewBrush::OnInitDialog()
{
    CDialog::OnInitDialog();
    m_radstrips.SetLimitText(2);
    m_vstrips.SetLimitText(2);
    if(_brshSett._isSolid){
        CheckRadioButton(RB_SOLID, RB_CUTALL, RB_SOLID);
    }
    else{
        if(_brshSett._cutAll){
            CheckRadioButton(RB_SOLID, RB_CUTALL, RB_CUTALL);
        }
        else{
            CheckRadioButton(RB_SOLID, RB_CUTALL, RB_CUT);
        }
    }

    if(_brshSett._tm == TM_LIN)
        CheckRadioButton(RB_LINEAR, RB_CYLINDRIC, RB_LINEAR);
    else if (_brshSett._tm == TM_CYL)
        CheckRadioButton(RB_LINEAR, RB_CYLINDRIC, RB_CYLINDRIC);

    CheckDlgButton(CK_SPLITABLE, _brshSett._splitable ? BST_CHECKED : BST_UNCHECKED);
    SetDlgItemInt(IDC_EDIT2, _brshSett._vstrips);
    SetDlgItemInt(IDC_EDIT3, _brshSett._radstrips);
    if(_brshSett._radstrips==-1){
        m_vstrips.EnableWindow(0);
        GetDlgItem(IDC_STATICRS)->EnableWindow(0);
        SetDlgItemInt(IDC_STATICRS,0);
    }
    else
        SetDlgItemInt(IDC_EDIT1,_brshSett._radstrips);

    if(_brshSett._vstrips==-1){
        m_radstrips.EnableWindow(0);
        GetDlgItem(IDC_STATICVS)->EnableWindow(0);
    }
    else
        SetDlgItemInt(IDC_EDIT2,_brshSett._vstrips);
    return TRUE;

}

//---------------------------------------------------------------------------------------
void CDlgNewBrush::OnDestroy()
{
    CDialog::OnDestroy();
}

//---------------------------------------------------------------------------------------
void CDlgNewBrush::OnButton1()
{
}

//---------------------------------------------------------------------------------------
void CDlgNewBrush::OnOk()
{
    _brshSett._isSolid   = IsDlgButtonChecked(RB_SOLID);
    _brshSett._cutAll    = IsDlgButtonChecked(RB_CUTALL);
    _brshSett._thikness  = 0;
    _brshSett._splitable = IsDlgButtonChecked(CK_SPLITABLE);
    _brshSett._reversed  = IsDlgButtonChecked(CK_REVERSE);
    _brshSett._vstrips   = GetDlgItemInt(IDC_EDIT2);
    _brshSett._radstrips = GetDlgItemInt(IDC_EDIT3);

    if(IsDlgButtonChecked(RB_LINEAR)){
        _brshSett._tm = TM_LIN;
    }
    else if(IsDlgButtonChecked(RB_CYLINDRIC)){
        _brshSett._tm = TM_CYL;
    }
    EndDialog(IDOK);
}

//---------------------------------------------------------------------------------------
void CDlgNewBrush::OnCancel()
{
    CDialog::OnCancel();
}

//---------------------------------------------------------------------------------------
void CDlgNewBrush::OnAutodetail()
{
}

