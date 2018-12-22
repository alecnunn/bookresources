#if !defined(AFX_PLUGINFDN4_H__D9E6D4D5_7E77_495D_8AD8_BDB77A4C048F__INCLUDED_)
#define AFX_PLUGINFDN4_H__D9E6D4D5_7E77_495D_8AD8_BDB77A4C048F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlugInFDN4.h : header file
//

#include "cDspFDN4.h"
#include "PlugIn.h"

/////////////////////////////////////////////////////////////////////////////
// CPlugInFDN4 dialog

class CPlugInFDN4 : public CPlugIn
{
// Construction
public:
	CPlugInFDN4(CWnd* pParent = NULL);   // standard constructor
	void CreateFromTemplate( CWnd *parent ) { Create( IDD_FDN4, parent ); }

	std::complex<float> TransferFunction( const std::complex<float> &z ) const; 
	void Process( cSoundBuffer<float> &buffer );
	void ProcessStereo( cSoundBuffer<float> &bufferL, cSoundBuffer<float> &bufferR );
	void Reset();

// Dialog Data
	//{{AFX_DATA(CPlugInFDN4)
	enum { IDD = IDD_FDN4 };
	CComboBox	m_SelectPreset;
	CComboBox	m_FeedbackMatrix;
	CEdit	m_Modulation[2];
	CEdit	m_Rotation[2];
	CEdit	m_HalfLife[2];
	CEdit	m_DelayLength[4];
	CEdit	m_Mix[2];
	CButton	m_MuteInput;
	CButton	m_Bypass;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlugInFDN4)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	cDspFDN4 m_FDN4;
	
	virtual BOOL OnInitDialog();
	afx_msg void OnInitLengthsAndParameters();
	afx_msg void OnSetParameters();
	afx_msg void OnSelectPreset();
	
	// Generated message map functions
	//{{AFX_MSG(CPlugInFDN4)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLUGINFDN4_H__D9E6D4D5_7E77_495D_8AD8_BDB77A4C048F__INCLUDED_)
