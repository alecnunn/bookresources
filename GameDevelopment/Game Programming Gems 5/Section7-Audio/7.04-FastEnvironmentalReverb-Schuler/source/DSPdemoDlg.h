// DSPdemoDlg.h : header file
//

#if !defined(AFX_DSPDEMODLG_H__40F1AA6A_3781_45AF_B028_6907B743C43B__INCLUDED_)
#define AFX_DSPDEMODLG_H__40F1AA6A_3781_45AF_B028_6907B743C43B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cDsp0Pole.h"
#include "cDspDiskRead.h"
#include "cSoundOut.h"

#include "afxtempl.h"
#include "PlugInFDN4.h"

/////////////////////////////////////////////////////////////////////////////
// CDSPdemoDlg dialog

class CDSPdemoDlg : public CDialog
{
// Construction
public:
	CDSPdemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDSPdemoDlg)
	enum { IDD = IDD_DSPDEMO_DIALOG };
	CScrollBar	m_ScrollFreq;
	CButton	m_CheckLowRange;
	CButton	m_CheckStereo;
	CEdit	m_EditProperties;
	CEdit	m_EditSamplerate;
	CStatic	m_TransferGraph;
	CStatic	m_EffectArea;
	CButton	m_InputPathButton;
	CButton	m_StopButton;
	CButton	m_PlayButton;
	CEdit	m_VolumeEdit;
	CSliderCtrl	m_VolumeSlider;
	CEdit	m_InputPath;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDSPdemoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CArray< CPlugIn *, CPlugIn * > m_EffectList;
	CPlugInFDN4 m_PlugInFDN4;
	CPlugIn *m_CurrentEffect;
	
	unsigned m_TimerID;
	unsigned m_GraphUpdateTimer;
	HICON m_hIcon;
	cSoundOut m_SoundOut;
	cDspDiskRead m_DspDiskRead;
	cDsp0Pole m_Dsp0Pole;
	cDsp0Pole m_Dsp0PoleR;
	bool m_Playing;
	bool m_TransferGraphNeedsUpdate;
	unsigned m_Samplerate;
	bool m_InputIsStereo;
	unsigned m_DataOffset;

	void UpdateTransferGraph();
	
	// Generated message map functions
	//{{AFX_MSG(CDSPdemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAbout();
	afx_msg void OnDestroy();
	afx_msg void OnInputClick();
	afx_msg void OnPlay();
	afx_msg void OnStop();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKillfocusVolumeEdit();
	afx_msg void OnKillfocusEditSamplerate();
	afx_msg void OnKillfocusInputPath();
	afx_msg void OnCheckLowRange();
	afx_msg void OnReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DSPDEMODLG_H__40F1AA6A_3781_45AF_B028_6907B743C43B__INCLUDED_)
