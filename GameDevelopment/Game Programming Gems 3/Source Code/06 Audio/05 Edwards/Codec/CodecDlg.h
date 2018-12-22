// CodecDlg.h
//
// main CDialog window

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "soundmanager.h"

class CCodecDlg : public CDialog
{
public:
	CCodecDlg(CWnd* pParent = NULL);

	//{{AFX_DATA(CCodecDlg)
	enum { IDD = IDD_CODEC_DIALOG };
	CSliderCtrl	m_Rate;
	CButton	m_DumpBtn;
	CSliderCtrl	m_OscaLevel;
	CSliderCtrl	m_OscaEnv;
	CSliderCtrl	m_OscfVoice;
	CSliderCtrl	m_OscfMod;
	CSliderCtrl	m_OscfConst;
	CButton	m_SaveBtn;
	CButton	m_ClearBtn;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CCodecDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void			StoreFMParms();

protected:	
	HICON			m_hIcon;
	SoundManager	*soundmanager;
	QVOX			*qvox;
	QVOX_Client		*player;
	Speaker			*speaker;
	int				edit_channel;		// current channel being edited
	char			filename[512];		// WAV filename
	char			spkfilename[512];	// SPK filename
	OscFM			*edit_osc;
	int				rate;				// speech speed

	void			ShowFMParms();

	//{{AFX_MSG(CCodecDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSave();
	afx_msg void OnModulator();
	afx_msg void OnCarrier();
	afx_msg void OnOscSawtooth();
	afx_msg void OnOscSine();
	afx_msg void OnOscSquare();
	afx_msg void OnOscTri();
	afx_msg void OnOscfRatioSw();
	afx_msg void OnChangeOscfRat1();
	afx_msg void OnChangeOscfRat2();
	afx_msg void OnBrowse();
	afx_msg void OnOutFmmod();
	afx_msg void OnOutFmout();
	afx_msg void OnOutSpeech();
	afx_msg void OnSaveSpk();
	afx_msg void OnBrowseSpk();
	afx_msg void OnDump();
	afx_msg void OnDumpSpk();
	afx_msg void OnRateSw();
	afx_msg void OnDeep();
	afx_msg void OnDeepSw();
	afx_msg void OnOscPulse();
	afx_msg void OnOscNoise();
	afx_msg void OnResidualSw();
	afx_msg void OnShallowSw();
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
