//=============================================================================
// d3dApp.h by Frank Luna (C) 2005 All Rights Reserved.
//
// Contains the base Direct3D application class which provides the
// framework interface for the sample applications.  Clients are to derive 
// from D3DApp, override the framework methods, and instantiate only a single
// instance of the derived D3DApp class.  At the same time, the client should
// set the global application  pointer (gd3dApp) to point to the one and only
// instance, e.g., gd3dApp = new HelloD3DApp(hInstance);
// 
//=============================================================================

#ifndef D3DAPP_H
#define D3DAPP_H

#include "d3dUtil.h"
#include <string>

class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance, std::string winCaption, D3DDEVTYPE devType, DWORD requestedVP);
	virtual ~D3DApp();

	HINSTANCE getAppInst();
	HWND      getMainWnd();

	// Framework methods.  Derived client class overrides these methods to 
	// implement specific application requirements.
	virtual bool checkDeviceCaps()     { return true; }
	virtual void onLostDevice()        {}
	virtual void onResetDevice()       {}
	virtual void updateScene(float dt) {}
	virtual void drawScene()           {}

	// Override these methods only if you do not like the default window creation,
	// direct3D device creation, window procedure, or message loop.  In general,
	// for the sample programs of this book, we will not need to modify these.
	virtual void initMainWindow();
	virtual void initDirect3D();
	virtual int run();
	virtual LRESULT msgProc(UINT msg, WPARAM wParam, LPARAM lParam);

	void enableFullScreenMode(bool enable);
	bool isDeviceLost();

protected:
	// Derived client class can modify these data members in the constructor to 
	// customize the application.  
	std::string mMainWndCaption;
	D3DDEVTYPE  mDevType;
	DWORD       mRequestedVP;
	
	// Application, Windows, and Direct3D data members.
	HINSTANCE             mhAppInst;
	HWND                  mhMainWnd;
	IDirect3D9*           md3dObject;
	bool                  mAppPaused;
	D3DPRESENT_PARAMETERS md3dPP;
};

// Globals for convenient access.
extern D3DApp* gd3dApp;
extern IDirect3DDevice9* gd3dDevice;

#endif // D3DAPP_H