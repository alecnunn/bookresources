// FoliageDemoDlg.h : header file
//

#if !defined(AFX_FOLIAGEDEMODLG_H__AD0C28FB_8DA4_42E6_B432_B54081AB4F90__INCLUDED_)
#define AFX_FOLIAGEDEMODLG_H__AD0C28FB_8DA4_42E6_B432_B54081AB4F90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CFoliageDemoDlg dialog

class CFoliageDemoDlg : public CDialog
{
// Construction
public:
	CFoliageDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFoliageDemoDlg)
	enum { IDD = IDD_FOLIAGEDEMO_DIALOG };
	CSliderCtrl	m_drawDistance;
	CSliderCtrl	m_frequency;
	CWorldView	m_worldView;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFoliageDemoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFoliageDemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCustomdrawFrequency(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawWidgetdistance(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FOLIAGEDEMODLG_H__AD0C28FB_8DA4_42E6_B432_B54081AB4F90__INCLUDED_)
