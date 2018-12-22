// SphinxMMOSPlayerMainFrame.h : Schnittstelle der Klasse CSphinxMMOSPlayerMainFrame
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEMOMAINFRAME_H__DCE4C661_E4FC_41C3_B47E_0A2E7C74ED58__INCLUDED_)
#define AFX_DEMOMAINFRAME_H__DCE4C661_E4FC_41C3_B47E_0A2E7C74ED58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSphinxMMOSPlayerMainFrame : public CFrameWnd
{
	
protected: // Nur aus Serialisierung erzeugen
	CSphinxMMOSPlayerMainFrame();
	DECLARE_DYNCREATE(CSphinxMMOSPlayerMainFrame)

// Attribute
public:

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CSphinxMMOSPlayerMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CSphinxMMOSPlayerMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // Eingebundene Elemente der Steuerleiste
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CSphinxMMOSPlayerMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_DEMOMAINFRAME_H__DCE4C661_E4FC_41C3_B47E_0A2E7C74ED58__INCLUDED_)
