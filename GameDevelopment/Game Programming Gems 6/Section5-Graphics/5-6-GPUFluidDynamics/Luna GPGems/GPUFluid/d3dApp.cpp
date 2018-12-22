//=============================================================================
// d3dApp.h by Frank Luna (C) 2005 All Rights Reserved.
//=============================================================================

#include "d3dApp.h"

D3DApp* gd3dApp              = 0;
IDirect3DDevice9* gd3dDevice = 0;

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Don't start processing messages until the application has been created.
	if( gd3dApp != 0 )
		return gd3dApp->msgProc(msg, wParam, lParam);
	else
		return DefWindowProc(hwnd, msg, wParam, lParam);
}

D3DApp::D3DApp(HINSTANCE hInstance, std::string winCaption, D3DDEVTYPE devType, DWORD requestedVP)
{
	mMainWndCaption = winCaption;
	mDevType        = devType;
	mRequestedVP    = requestedVP;
	
	mhAppInst   = hInstance;
	mhMainWnd   = 0;
	md3dObject  = 0;
	mAppPaused  = false;
	ZeroMemory(&md3dPP, sizeof(md3dPP));

	initMainWindow();
	initDirect3D();
}

D3DApp::~D3DApp()
{
	ReleaseCOM(md3dObject);
	ReleaseCOM(gd3dDevice);
}

HINSTANCE D3DApp::getAppInst()
{
	return mhAppInst;
}

HWND D3DApp::getMainWnd()
{
	return mhMainWnd;
}

void D3DApp::initMainWindow()
{
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = MainWndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = mhAppInst;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = "D3DWndClassName";

	if( !RegisterClass(&wc) )
	{
		MessageBox(0, "RegisterClass FAILED", 0, 0);
		PostQuitMessage(0);
	}

	// Default to a window with a client area rectangle of 800x600.

	RECT R = {0, 0, 800, 600};
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	mhMainWnd = CreateWindow("D3DWndClassName", mMainWndCaption.c_str(), 
		WS_OVERLAPPEDWINDOW, 100, 100, R.right, R.bottom, 
		0, 0, mhAppInst, 0); 

	if( !mhMainWnd )
	{
		MessageBox(0, "CreateWindow FAILED", 0, 0);
		PostQuitMessage(0);
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);
}

void D3DApp::initDirect3D()
{
	// Step 1: Create the IDirect3D9 object.

    md3dObject = Direct3DCreate9(D3D_SDK_VERSION);
	if( !md3dObject )
	{
		MessageBox(0, "Direct3DCreate9 FAILED", 0, 0);
		PostQuitMessage(0);
	}


	// Step 2: Verify hardware support for specified formats in windowed and full screen modes.
	
	D3DDISPLAYMODE mode;
	md3dObject->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);
	HR(md3dObject->CheckDeviceType(D3DADAPTER_DEFAULT, mDevType, mode.Format, mode.Format, true));
	HR(md3dObject->CheckDeviceType(D3DADAPTER_DEFAULT, mDevType, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, false));

	// Step 3: Check for requested vertex processing and pure device.

	D3DCAPS9 caps;
	HR(md3dObject->GetDeviceCaps(D3DADAPTER_DEFAULT, mDevType, &caps));

	DWORD devBehaviorFlags = 0;
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		devBehaviorFlags |= mRequestedVP;
	else
		devBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	if( caps.DevCaps & D3DDEVCAPS_PUREDEVICE &&
		mRequestedVP != D3DCREATE_SOFTWARE_VERTEXPROCESSING)
			devBehaviorFlags |= D3DCREATE_PUREDEVICE;

	// Step 4: Fill out the D3DPRESENT_PARAMETERS structure.

	md3dPP.BackBufferWidth            = 0; 
	md3dPP.BackBufferHeight           = 0;
	md3dPP.BackBufferFormat           = D3DFMT_UNKNOWN;
	md3dPP.BackBufferCount            = 1;
	md3dPP.MultiSampleType            = D3DMULTISAMPLE_NONE;
	md3dPP.MultiSampleQuality         = 0;
	md3dPP.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	md3dPP.hDeviceWindow              = mhMainWnd;
	md3dPP.Windowed                   = true;
	md3dPP.EnableAutoDepthStencil     = true; 
	md3dPP.AutoDepthStencilFormat     = D3DFMT_D24S8;
	md3dPP.Flags                      = 0;
	md3dPP.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	md3dPP.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;


	// Step 5: Create the device.

	HR(md3dObject->CreateDevice(
		D3DADAPTER_DEFAULT, // primary adapter
		mDevType,           // device type
		mhMainWnd,          // window associated with device
		devBehaviorFlags,   // vertex processing
	    &md3dPP,            // present parameters
	    &gd3dDevice));      // return created device
}

int D3DApp::run()
{
	MSG  msg;
    msg.message = WM_NULL;

	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	float secsPerCnt = 1.0f / (float)cntsPerSec;

	__int64 prevTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);

	while(msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if(PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
		{
            TranslateMessage( &msg );
            DispatchMessage( &msg );
		}
		// Otherwise, do animation/game stuff.
		else
        {	
			// If the application is paused then free some CPU cycles to other 
			// applications and then continue on to the next frame.
			if( mAppPaused )
			{
				Sleep(20);
				continue;
			}

			if( !isDeviceLost() )
			{
				__int64 currTimeStamp = 0;
				QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
				float dt = (currTimeStamp - prevTimeStamp)*secsPerCnt;

				updateScene(dt);
				drawScene();

				// Prepare for next iteration: The current time stamp becomes
				// the previous time stamp for the next iteration.
				prevTimeStamp = currTimeStamp;
			}
        }
    }
	return (int)msg.wParam;
}

LRESULT D3DApp::msgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Is the application in a minimized or maximized state?
	static bool minOrMaxed = false;

	RECT clientRect = {0, 0, 0, 0};
	switch( msg )
	{

	// WM_ACTIVE is sent when the window is activated or deactivated.
	// We pause the game when the main window is deactivated and 
	// unpause it when it becomes active.
	case WM_ACTIVATE:
		if( LOWORD(wParam) == WA_INACTIVE )
			mAppPaused = true;
		else
			mAppPaused = false;
		return 0;


	// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		if( gd3dDevice )
		{
			md3dPP.BackBufferWidth  = LOWORD(lParam);
			md3dPP.BackBufferHeight = HIWORD(lParam);

			if( wParam == SIZE_MINIMIZED )
			{
				mAppPaused = true;
				minOrMaxed = true;
			}
			else if( wParam == SIZE_MAXIMIZED )
			{
				mAppPaused = false;
				minOrMaxed = true;
				onLostDevice();
				HR(gd3dDevice->Reset(&md3dPP));
				onResetDevice();
			}
			// Restored is any resize that is not a minimize or maximize.
			// For example, restoring the window to its default size
			// after a minimize or maximize, or from dragging the resize
			// bars.
			else if( wParam == SIZE_RESTORED )
			{
				mAppPaused = false;

				// Are we restoring from a mimimized or maximized state, 
				// and are in windowed mode?  Do not execute this code if 
				// we are restoring to full screen mode.
				if( minOrMaxed && md3dPP.Windowed )
				{
					onLostDevice();
					HR(gd3dDevice->Reset(&md3dPP));
					onResetDevice();
				}
				else
				{
					// No, which implies the user is resizing by dragging
					// the resize bars.  However, we do not reset the device
					// here because as the user continuously drags the resize
					// bars, a stream of WM_SIZE messages is sent to the window,
					// and it would be pointless (and slow) to reset for each
					// WM_SIZE message received from dragging the resize bars.
					// So instead, we reset after the user is done resizing the
					// window and releases the resize bars, which sends a
					// WM_EXITSIZEMOVE message.
				}
				minOrMaxed = false;
			}
		}
		return 0;


	// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
	// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		GetClientRect(mhMainWnd, &clientRect);
		md3dPP.BackBufferWidth  = clientRect.right;
		md3dPP.BackBufferHeight = clientRect.bottom;
		onLostDevice();
		HR(gd3dDevice->Reset(&md3dPP));
		onResetDevice();

		return 0;

	// WM_CLOSE is sent when the user presses the 'X' button in the
	// caption bar menu.
	case WM_CLOSE:
		DestroyWindow(mhMainWnd);
		return 0;

	// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			enableFullScreenMode(false);
		else if( wParam == 'F' )
			enableFullScreenMode(true);
		return 0;
	}
	return DefWindowProc(mhMainWnd, msg, wParam, lParam);
}

void D3DApp::enableFullScreenMode(bool enable)
{
	// Switch to fullscreen mode.
	if( enable )
	{
		// Are we already in fullscreen mode?
		if( !md3dPP.Windowed ) 
			return;

		int width  = GetSystemMetrics(SM_CXSCREEN);
		int height = GetSystemMetrics(SM_CYSCREEN);

		md3dPP.BackBufferFormat = D3DFMT_X8R8G8B8;
		md3dPP.BackBufferWidth  = width;
		md3dPP.BackBufferHeight = height;
		md3dPP.Windowed         = false;

		// Change the window style to a more fullscreen friendly style.
		SetWindowLongPtr(mhMainWnd, GWL_STYLE, WS_POPUP);

		// If we call SetWindowLongPtr, MSDN states that we need to call
		// SetWindowPos for the change to take effect.  In addition, we 
		// need to call this function anyway to update the window dimensions.
		SetWindowPos(mhMainWnd, HWND_TOP, 0, 0, width, height, SWP_NOZORDER | SWP_SHOWWINDOW);	
	}
	// Switch to windowed mode.
	else
	{
		// Are we already in windowed mode?
		if( md3dPP.Windowed ) 
			return;

		RECT R = {0, 0, 800, 600};
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		md3dPP.BackBufferFormat = D3DFMT_UNKNOWN;
		md3dPP.BackBufferWidth  = 800;
		md3dPP.BackBufferHeight = 600;
		md3dPP.Windowed         = true;
	
		// Change the window style to a more windowed friendly style.
		SetWindowLongPtr(mhMainWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

		// If we call SetWindowLongPtr, MSDN states that we need to call
		// SetWindowPos for the change to take effect.  In addition, we 
		// need to call this function anyway to update the window dimensions.
		SetWindowPos(mhMainWnd, HWND_TOP, 100, 100, R.right, R.bottom, SWP_NOZORDER | SWP_SHOWWINDOW);
	}

	// Reset the device with the changes.
	onLostDevice();
	HR(gd3dDevice->Reset(&md3dPP));
	onResetDevice();
}

bool D3DApp::isDeviceLost()
{
	// Get the state of the graphics device.
	HRESULT hr = gd3dDevice->TestCooperativeLevel();

	// If the device is lost and cannot be reset yet then
	// sleep for a bit and we'll try again on the next 
	// message loop cycle.
	if( hr == D3DERR_DEVICELOST )
	{
		Sleep(20);
		return true;
	}
	// Driver error, exit.
	else if( hr == D3DERR_DRIVERINTERNALERROR )
	{
		MessageBox(0, "Internal Driver Error...Exiting", 0, 0);
		PostQuitMessage(0);
		return true;
	}
	// The device is lost but we can reset and restore it.
	else if( hr == D3DERR_DEVICENOTRESET )
	{
		onLostDevice();
		HR(gd3dDevice->Reset(&md3dPP));
		onResetDevice();
		return false;
	}
	else
		return false;
}