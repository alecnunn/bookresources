/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/
#ifndef __DLG_NERWBRUSH_H__
#define __DLG_NERWBRUSH_H__

//----------------------------------------------------------------------------------------
#include "resource.h"
#include "baseos.h"
#include "brush.h"

//----------------------------------------------------------------------------------------
// Structure (new brush initial properties)
struct SNewBrush
{
    SNewBrush(){
        _tm = TM_LIN;
    }
    BOOL    _isSolid;
    REAL    _thikness;
    BOOL    _splitable;
    BOOL    _reversed;
    int     _vstrips;
    int     _radstrips;
    BOOL	_cutAll;
    TEX_MAP _tm;
};


//----------------------------------------------------------------------------------------
// CDlgNewBrush dialog
class CDlgNewBrush : public CDialog
{
    // Construction
public:
    CDlgNewBrush(CWnd* pParent = NULL);   
    enum { IDD = DLG_NEWBRUSH };
    CSpinButtonCtrl	m_splr;             
    CSpinButtonCtrl	m_splv;             
    CEdit	        m_radstrips;
    CEdit	        m_vstrips;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    
    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
    afx_msg void OnButton1();
    afx_msg void OnOk();
    virtual void OnCancel();
    afx_msg void OnAutodetail();
    DECLARE_MESSAGE_MAP()
public:
    SNewBrush   _brshSett;
};

#endif // 
