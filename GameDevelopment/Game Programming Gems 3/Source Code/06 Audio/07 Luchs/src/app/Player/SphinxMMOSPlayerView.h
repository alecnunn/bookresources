// SphinxMMOSPlayerView.h : Schnittstelle der Klasse CSphinxMMOSPlayerView
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEMOVIEW_H__9A66CAE0_E890_4727_9B9F_96B5B1967DED__INCLUDED_)
#define AFX_DEMOVIEW_H__9A66CAE0_E890_4727_9B9F_96B5B1967DED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSphinxMMOSPlayerView : public CFormView
{
protected: // Nur aus Serialisierung erzeugen
	CSphinxMMOSPlayerView();
	DECLARE_DYNCREATE(CSphinxMMOSPlayerView)

public:
	//{{AFX_DATA(CSphinxMMOSPlayerView)
	enum { IDD = IDD_DEMO_FORM };
	CButton	m_static1;
	CSliderCtrl	m_slider1;
	CEdit	m_edit1;
	CStatic	m_staticPatch;
	CTreeCtrl	m_treePatches;
	//}}AFX_DATA

// Attribute
public:
	CSphinxMMOSPlayerDoc* GetDocument();

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CSphinxMMOSPlayerView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual void OnInitialUpdate(); // das erste mal nach der Konstruktion aufgerufen
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CSphinxMMOSPlayerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:


	void UpdatePatchList();


	CImageList	m_Images;
	FLOAT32		m_fFaderValue;


// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CSphinxMMOSPlayerView)
	afx_msg void OnEngineStart();
	afx_msg void OnUpdateEngineStart(CCmdUI* pCmdUI);
	afx_msg void OnEngineStop();
	afx_msg void OnUpdateEngineStop(CCmdUI* pCmdUI);
	afx_msg void OnSelchangedTreePatches(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBtnTrigger();
	afx_msg void OnEngineRecord();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // Testversion in SphinxMMOSPlayerView.cpp
inline CSphinxMMOSPlayerDoc* CSphinxMMOSPlayerView::GetDocument()
   { return (CSphinxMMOSPlayerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_DEMOVIEW_H__9A66CAE0_E890_4727_9B9F_96B5B1967DED__INCLUDED_)
