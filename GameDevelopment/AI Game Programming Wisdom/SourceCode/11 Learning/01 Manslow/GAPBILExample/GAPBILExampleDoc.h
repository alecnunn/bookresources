//GAPBILExample
//Copyright John Manslow
//29/09/2001

// GAPBILExampleDoc.h : interface of the CGAPBILExampleDoc class
//
/////////////////////////////////////////////////////////////////////////////

class CGAPBILExampleDoc : public CDocument
{
protected: // create from serialization only
	CGAPBILExampleDoc();
	DECLARE_DYNCREATE(CGAPBILExampleDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGAPBILExampleDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGAPBILExampleDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CGAPBILExampleDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
