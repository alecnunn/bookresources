//-----------------------------------------------------------------------------
// File: App.h
//
// Desc: Header file App sample app
//-----------------------------------------------------------------------------
#ifndef _APP_H
#define _APP_H

#if _MSC_VER >= 1000
#pragma once
#endif
#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file
#endif



//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
// TODO: change "DirectX AppWizard Apps" to your name or the company name
#define DXAPP_KEY        TEXT("Software\\DirectX AppWizard Apps\\App")

// Struct to store the current input state

class Scene;
class Input;
class Camera;
class Sun;
class Atmosphere;
class Anim;
class Movie;
class State;

//-----------------------------------------------------------------------------
// Name: class CAppForm
// Desc: CFormView-based class which allows the UI to be created with a form
//       (dialog) resource. This class manages all the controls on the form.
//-----------------------------------------------------------------------------
class CAppForm : public CFormView, public CD3DApplication
{
public:
	inline Camera *GetCamera() {return m_pCamera;}
	inline Sun *GetSun() {return m_pSun;}
	inline Atmosphere *GetAtmosphere() {return m_pAtmosphere;};
	inline Scene *GetScene() {return m_pScene;};


private:
	Scene*					m_pScene;
	Input*					m_pInput;
	Camera*					m_pCamera;
	Sun*					m_pSun;
	Atmosphere *			m_pAtmosphere;
	Anim *					m_pAnim;
	Movie *					m_pMovie;
	bool					m_bPlayMode;
	FILE *					m_fpDump;
	FILE *					m_fpRead;

	State*					m_pCurKeyState;

		
private:
	void DumpAnim();
	void DumpMovie();
	void ReadMovie();

public:
    BOOL                    m_bLoadingApp;          // TRUE, if the app is loading
    CD3DFont*               m_pFont;                // Font for drawing text

	void UpdateInput();

private:

    HWND    m_hwndRenderWindow;
    HWND    m_hwndRenderFullScreen;
    HWND    m_hWndTopLevelParent;

    HRESULT ConfirmDevice( D3DCAPS8*,DWORD,D3DFORMAT );
    HRESULT OneTimeSceneInit();
    HRESULT InitDeviceObjects();
    HRESULT RestoreDeviceObjects();
    HRESULT FrameMove();
    HRESULT Render();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();
    HRESULT FinalCleanup();
    virtual HRESULT AdjustWindowForChange();

    HRESULT RenderText();

    VOID    UpdateUIForDeviceCapabilites();



protected:
    DECLARE_DYNCREATE(CAppForm)

             CAppForm();
    virtual  ~CAppForm();

public:
    BOOL IsReady() { return m_bReady; }
    TCHAR* PstrFrameStats() { return m_strFrameStats; }
    VOID RenderScene() { Render3DEnvironment(); }
    HRESULT CheckForLostFullscreen();

    //{{AFX_DATA(CAppForm)
	enum { IDD = IDD_FORMVIEW };

	int		m_SliderSunIntensity;
	int		m_SliderSunTheta;
	int		m_SliderSunPhi;
	int		m_SliderBeta;
	float	m_EditSunIntensityScale;
	float	m_EditBetaRayScale;
	float	m_EditBetaMieScale;
	int		m_SliderBetaMie;
	int		m_SliderBetaRay;
	int		m_SliderHGg;
	int		m_SliderMultExtinction;
	int		m_SliderMultInscattering;
	int		m_CheckMultExtinction;
	int		m_SliderTime;
	BOOL	m_bDemo;
	//}}AFX_DATA

    //{{AFX_VIRTUAL(CAppForm)
    virtual void OnInitialUpdate();
    protected:
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

public:
    //{{AFX_MSG(CAppForm)
    afx_msg void OnToggleFullScreen();
    afx_msg void OnChangeDevice();
	afx_msg void OnCustomdrawSliderSunTheta(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSliderSunPhi(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSliderSunIntensity(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateEditSunIntensityScale();
	afx_msg void OnUpdateEditBetaRayScale();
	afx_msg void OnUpdateEditBetaMieScale();
	afx_msg void OnCustomdrawSliderBetaMie(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSliderBetaRay(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSliderHgG(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSliderMultExtinction(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSliderMultInscattering(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckMultExtinction();
	afx_msg void OnCustomdrawSliderTime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPlay();
	afx_msg void OnHelp();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()


private:

	// UI related
	void UpdateUI();

	float UIGetSunIntensity();
	float UIGetSunIntensityScale();
	float UIGetSunTheta();
	float UIGetSunPhi();

	float UIGetBetaMieMultiplier();
	float UIGetBetaMieScale();
	float UIGetBetaRayMultiplier();
	float UIGetBetaRayScale();
	float UIGetHGg();

	float UIGetExtinctionMultiplier();
	float UIGetInscatteringMultiplier();
	float UIGetTime();

	float ThetaPhi2Time(float theta, float phi);

};





//-----------------------------------------------------------------------------
// Name: class CAppDoc
// Desc: Overridden CDocument class needed for the CFormView
//-----------------------------------------------------------------------------
class CAppDoc : public CDocument
{
protected:
    DECLARE_DYNCREATE(CAppDoc)

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAppDoc)
    public:
    //}}AFX_VIRTUAL

// Implementation
    //{{AFX_MSG(CAppDoc)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};




//-----------------------------------------------------------------------------
// Name: class CAppFrameWnd
// Desc: CFrameWnd-based class needed to override the CFormView's window style
//-----------------------------------------------------------------------------
class CAppFrameWnd : public CFrameWnd
{
protected:
    DECLARE_DYNCREATE(CAppFrameWnd)
public:
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAppFrameWnd)
    public:
    virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
    virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
    //}}AFX_VIRTUAL

protected:
    //{{AFX_MSG(CAppFrameWnd)
    afx_msg void OnChangeDevice();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};




//-----------------------------------------------------------------------------
// Name: class CApp
// Desc: Main MFC application class derived from CWinApp.
//-----------------------------------------------------------------------------
class CApp : public CWinApp
{
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CApp)
    public:
    virtual BOOL InitInstance();
    virtual BOOL OnIdle( LONG );
    //}}AFX_VIRTUAL

// Implementation
    //{{AFX_MSG(CApp)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


CAppForm *GetApp();


#endif



