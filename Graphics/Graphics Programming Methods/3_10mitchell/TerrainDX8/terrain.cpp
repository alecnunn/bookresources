// GPM - Pixel Shader Optimizations for Terrain Rendering Demo
// Kenny Mitchell
// Copyright Electronic Arts 2002, 2003

#include "terrain.h"

// global application data
LPDIRECT3DDEVICE8 DEV=NULL;
HWND hWnd=NULL;
HINSTANCE hInstance=NULL;
const char APPLICATION_NAME[]="Outdoor Lighting Demo - Kenny Mitchell (c) Copyright Electronic Arts 2002, 2003";

WNDCLASS		Application::WndClass;
LPDIRECT3D8	Application::D3D=NULL;

Input_Class*	Application::Input=NULL;
Camera_Class*	Application::Camera=NULL;
Scene_Class*	Application::Scene=NULL;
CD3DFont*		Application::Font=NULL;


// Sample entry point
INT WINAPI WinMain
(
	HINSTANCE thisInstance,
	HINSTANCE, 
	LPSTR, 
	INT 
)
{
	hInstance=thisInstance;

	// Initialize app
	if (Application::Setup())
	{ 
		// run the sample
		Application::Run();
	}

	// shutdown everything and exit the app
	Application::Shutdown();

	return 0;
}

// process
void Application::Process()
{
	Input->Update();
	Camera->Update();

	if (Input->Get_Key(VK_TAB)) Font->Toggle_Text_Display();
}

// render
void Application::Render()
{
   if (!DEV)
		return;

	// Clear the backbuffer
	DEV->Clear
	(
		0, 
		NULL, 
		D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
		D3DCOLOR_XRGB(160,160,255),
		1.0f, 
		0 
	);

	// Begin the scene
	DEV->BeginScene();

	// Rendering of scene objects can happen here
	Scene->Render();

static bool wire_frame=false;

	if (Input->Get_Key('W'))	wire_frame=true;
	if (Input->Get_Key('S'))	wire_frame=false;

	DEV->SetRenderState(D3DRS_FILLMODE, wire_frame ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

	if (wire_frame)
	{
		Font->DrawText(10,260,"W/S - Fill Mode (wireframe)");
	}
	else
	{
		Font->DrawText(10,260,"W/S - Fill Mode (solid)");
	}

static bool fog_enable=true;

	if (Input->Get_Key('R'))	fog_enable=true;
	if (Input->Get_Key('F'))	fog_enable=false;

	DEV->SetRenderState(D3DRS_FOGENABLE, fog_enable);

	if (fog_enable)
	{
		Font->DrawText(10,280,"R/F - Fog (enabled)");
	}
	else
	{
		Font->DrawText(10,280,"R/F - Fog (disabled)");
	}

	Font->DrawText(10,0,"TAB - Toggle Text Display");

	// End the scene
	DEV->EndScene();

	// Present the backbuffer contents to the display
	DEV->Present
	( 
		NULL, 
		NULL, 
		NULL, 
		NULL 
	);
}


// Set up D3D
bool Application::Setup()
{
	// Register the window class
	memset(&WndClass,0,sizeof(WNDCLASS));

	WndClass.style=CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc=MsgProc;
	WndClass.hInstance=hInstance;
	WndClass.lpszClassName=APPLICATION_NAME; 

	RegisterClass(&WndClass);

	// Create the application's window
	hWnd=CreateWindow
	( 
		APPLICATION_NAME, 
		APPLICATION_NAME, 
		WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, 768, 768,
		GetDesktopWindow(), 
		NULL, 
		WndClass.hInstance, 
		NULL 
	);

	// Show the window
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);


	// Create the D3D object, which is needed to create the D3DDevice.
	D3D=Direct3DCreate8(D3D_SDK_VERSION);

	if (!D3D) 
		return false;

	// Get the current desktop display mode
	D3DDISPLAYMODE display_mode;

	HRESULT result=D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &display_mode);
	FAILMSG("Failed to get display mode");

	// Set up the structure used to create the D3DDevice.
	D3DPRESENT_PARAMETERS present_params; 

	memset(&present_params, 0, sizeof(D3DPRESENT_PARAMETERS));

	present_params.Windowed=TRUE;
	present_params.SwapEffect=D3DSWAPEFFECT_COPY_VSYNC;
	present_params.BackBufferFormat=display_mode.Format;
	present_params.EnableAutoDepthStencil=true;
	present_params.AutoDepthStencilFormat=D3DFMT_D16;

	// Create the Direct3D device.
	result=D3D->CreateDevice
	(
		D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_HAL, 
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&present_params, 
		&DEV 
	);
	FAILMSG("Failed to create d3d device");

	// setup device state

	// set up filtering
	DEV->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	DEV->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	DEV->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	DEV->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	DEV->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	DEV->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	DEV->SetTextureStageState(2, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	DEV->SetTextureStageState(2, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	DEV->SetTextureStageState(2, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	DEV->SetTextureStageState(3, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	DEV->SetTextureStageState(3, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	DEV->SetTextureStageState(3, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

	// set up fog
	DEV->SetRenderState(D3DRS_FOGENABLE, TRUE);
	DEV->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_XRGB(160,160,255));

	DEV->SetRenderState(D3DRS_ZENABLE, TRUE);

//	DEV->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	// set up app objects
	Scene=new Scene_Class;

	Font=new CD3DFont(_T("Arial"), 12, 0);//D3DFONT_BOLD);
	Font->InitDeviceObjects(DEV);
	Font->RestoreDeviceObjects();

	Camera=new Camera_Class;
	Input=new Input_Class;
	Camera->Set_Input(Input);

	return true;
}



// shutdown application
void Application::Shutdown()
{
	Safe_Delete(Font);
	Safe_Delete(Scene);
	Safe_Delete(Camera);
	Safe_Delete(Input);

	Safe_Release(DEV);
	Safe_Release(D3D);

	UnregisterClass(APPLICATION_NAME, WndClass.hInstance);
}


// run the windows app
void Application::Run()
{
	// Enter the message loop
	MSG  msg;
	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	while (msg.message!=WM_QUIT)
	{
		// Use PeekMessage() if the app is active, so we can use idle time to
		// render the scene. Else, use GetMessage() to avoid eating CPU time.
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
         // Translate and dispatch the message
			TranslateMessage(&msg);
         DispatchMessage(&msg);
		}
		else
		{
			// process application
			Process();

			// render the scene
			Render();
		}
	}
}

void Application::Process_Input_Msg
(
	UINT msg, 
	WPARAM wParam
)
{
	if (Input)
	{
		Input->Process_Msg(msg, wParam); 
	}
}

// The window's message handler
LRESULT WINAPI MsgProc
( 
	HWND hWnd, 
	UINT msg, 
	WPARAM wParam, 
	LPARAM lParam 
)
{
	Application::Process_Input_Msg(msg, wParam);

	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}




