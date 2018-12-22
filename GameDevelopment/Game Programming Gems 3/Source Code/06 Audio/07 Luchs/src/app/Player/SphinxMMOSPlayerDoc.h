// SphinxMMOSPlayerDoc.h : Schnittstelle der Klasse CSphinxMMOSPlayerDoc
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEMODOC_H__07C612F3_9DAD_40FC_960C_1ABA6DA864F2__INCLUDED_)
#define AFX_DEMODOC_H__07C612F3_9DAD_40FC_960C_1ABA6DA864F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSphinxMMOSPlayerDoc : public CDocument
{
protected: // Nur aus Serialisierung erzeugen
	CSphinxMMOSPlayerDoc();
	DECLARE_DYNCREATE(CSphinxMMOSPlayerDoc)

// Attribute
public:
	FLOAT*	m_pfFaderSignal; // input per slider
	FLOAT*	m_pWaveTable;	// loaded samples
	UINT32 m_nSampleCount;

	CString		m_strCurrentFile;

// Operationen
public:
//	atomos::IGAPatch*		CreatePatch(ATOM_CHAR* pszName);
//	atomos::IGAProcessor*	CreateProcessor(atomos::IGAPatch* pPatch, UINT32 idClass);


// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CSphinxMMOSPlayerDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CSphinxMMOSPlayerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:


// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CSphinxMMOSPlayerDoc)
	afx_msg void OnFileReload();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_DEMODOC_H__07C612F3_9DAD_40FC_960C_1ABA6DA864F2__INCLUDED_)
