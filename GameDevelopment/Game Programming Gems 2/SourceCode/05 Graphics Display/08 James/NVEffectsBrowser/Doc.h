/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
// Doc.h : interface of the CNVEffectsDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOC_H__C20F718B_7614_4494_ADAA_DC6FF9B2DA5F__INCLUDED_)
#define AFX_DOC_H__C20F718B_7614_4494_ADAA_DC6FF9B2DA5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEffectMenuItem
{
public:
	CEffectMenuItem(EBProperty* pProperty, HMENU hMenu, UINT MenuID)
		: m_pProperty(pProperty),
			m_MenuID(MenuID),
			m_hMenu(hMenu)
	{}

	EBProperty* m_pProperty;
	HMENU m_hMenu;
	UINT m_MenuID;
};

typedef struct tagEffectFileData
{
	WIN32_FIND_DATA m_FindData;
	std::string m_strDirectory;	
	HINSTANCE hLib;
} EffectFileData;

typedef std::map<std::string, EffectFileData> tmapEffectFileData;

class CRenderWnd;

class CNVEffectsDoc : public CDocument
{
protected: // create from serialization only
	CNVEffectsDoc();
	DECLARE_DYNCREATE(CNVEffectsDoc)

// Attributes
public:

// Operations
public:
	void Tick();
	void OnIdle();
	bool SetCurrentEffect(EBEffect* pEffect);
	bool Start();
	bool MenuHandler(UINT nID);
	
	HRESULT InitDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT ConfirmDevice(EBEffect* pEffect);
	void MouseMove(HWND hWnd, int x, int y) { if (m_bValidEffect) m_pCurrentEffect->MouseMove(hWnd, x, y); }
	void MouseButton(HWND hWnd, eButtonID Button, bool bDown, int x, int y) { if (m_bValidEffect) m_pCurrentEffect->MouseButton(hWnd, Button, bDown, x, y); }
	void Keyboard(DWORD dwKey, UINT nFlags, bool bDown) { if (m_bValidEffect) m_pCurrentEffect->Keyboard(dwKey, nFlags, bDown); }
	bool FileFoundCallback(const WIN32_FIND_DATA& FindData, const std::string& strDirectory);
	bool DoTestPopup(int x, int y);
	
	// Accessors
	bool IsEffectValid() const { return m_bValidEffect; }
	const std::list<EBEffect*>* GetEffectsList() const {return &m_listEffects; }
	EBEffect* GetCurrentEffect() const { return m_pCurrentEffect; }

	EBTimer* GetTimer() const {	return m_pTimer; }

	// Returns stats from effect, if it reports them.
	bool GetTriangleCount(DWORD& TriangleCount) const
	{
		if (!m_pCurrentEffect)
			return false;

		EBProperty* pPolyProp = m_pCurrentEffect->GetProperty("TriangleCount");
		if (pPolyProp && pPolyProp->IsKindOf(EBTYPE_DWORD_PROP))
		{
			TriangleCount = (DWORD)pPolyProp->ReadValue(m_pCurrentEffect);
			return true;
		}

		return false;
	}

	bool GetVertexCount(DWORD& VertexCount) const
	{
		if (!m_pCurrentEffect)
			return false;

		EBProperty* pPolyProp = m_pCurrentEffect->GetProperty("VertexCount");
		if (pPolyProp && pPolyProp->IsKindOf(EBTYPE_DWORD_PROP))
		{
			VertexCount = (DWORD)pPolyProp->ReadValue(m_pCurrentEffect);
			return true;
		}

		return false;
	}

private:

	CRenderWnd* GetRenderWnd() const
	{
		ASSERT(m_pRenderView);
		ASSERT(m_pRenderView->GetCurWnd());
		return m_pRenderView->GetCurWnd();
	}


	void ChangeDevice();
	bool RebuildEffect(DWORD dwDirtyFlags);
	bool BuildEffectMenu(EBEffect* pEffect);

	tmapEffectFileData m_EffectFileData;

	CRenderView* m_pRenderView;
	CProgramFormView* m_pProgramFormView[2];
	CShaderTree* m_pTreeView;

	std::list<EBEffect*> m_listEffects;
	EBEffect* m_pCurrentEffect;
	EBTimer* m_pTimer;

	nv_objects::NVMovie* m_pMovie;
	bool m_bMovieRecording;

	std::map<UINT, CEffectMenuItem> m_mapMenuItem;
	
	CMenu m_EffectMenu;
	bool m_bEffectMenu;

	bool m_bValidEffect;
	bool m_bStarted;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNVEffectsDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void OnChangedViewList();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void OnCloseDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNVEffectsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CNVEffectsDoc)
	afx_msg void OnCaptureBeginmovie();
	afx_msg void OnUpdateCaptureBeginmovie(CCmdUI* pCmdUI);
	afx_msg void OnCaptureEndmovie();
	afx_msg void OnUpdateCaptureEndmovie(CCmdUI* pCmdUI);
	afx_msg void OnCaptureSaveimage();
	afx_msg void OnUpdateCaptureSaveimage(CCmdUI* pCmdUI);
	afx_msg void OnDeviceForcesoftwarevp();
	afx_msg void OnUpdateDeviceForcesoftwarevp(CCmdUI* pCmdUI);
	afx_msg void OnDeviceReference();
	afx_msg void OnUpdateDeviceReference(CCmdUI* pCmdUI);
	afx_msg void OnFileHardwaredevice();
	afx_msg void OnUpdateFileHardwaredevice(CCmdUI* pCmdUI);
	afx_msg void OnOptionsExpandall();
	afx_msg void OnUpdateOptionsExpandall(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#define IDM_MENUFREE1 (WM_USER + 1000)

class CEffectSearch : public nv_objects::NVFileSearch
{
public:
	virtual bool FileFoundCallback(const WIN32_FIND_DATA& FindData, const std::string& strDirectory);
	CNVEffectsDoc* m_pDoc;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOC_H__C20F718B_7614_4494_ADAA_DC6FF9B2DA5F__INCLUDED_)
