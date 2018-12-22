/***************************************************************
* Application.h                                                *
*                                                              *
* This file contains the header information for the class      *
* CHostApplication.  For this file to compile correctly, the   *
* path for the DX include files must be added to your path.    *
*                                                              *
* Changes:                                                     *
*	*  Added Direct3D device support to CHostApplication       *
*   *  Added device creation functions                         *
*   *  Added fullscreen function                               *
*   *  Added device destruction function                       *
*   *  Added function to enumerate available devices           *
*	*  Added several virtual functions to allow easy           *
*      subclassing  in later chapters.                         *
*   *  Removed previous comments and commented new code        *
*   *  Added the m_Continue Flag                               *
*   *  Removed previous comments and commented new code        *
***************************************************************/
#include <d3d8.h>
#include <d3dx8.h>

#include "DirectXFont.h"

class CHostApplication
{
public:
	CHostApplication();
	virtual ~CHostApplication();
	LRESULT WINAPI MessageHandler(HWND hWnd, UINT msg, 
								  WPARAM wParam, LPARAM lParam);

	void Go();

protected:
	//These new functions cover all aspects of device management
	HRESULT InitializeD3D();
	HRESULT CreateDevice(
					D3DDEVICE_CREATION_PARAMETERS *pCreateParms,
				    D3DPRESENT_PARAMETERS         *pPresentParms);
	HRESULT EasyCreateWindowed(HWND WindowHandle, 
							   D3DDEVTYPE DeviceType,
							   DWORD Behavior);
	HRESULT EasyCreateFullScreen(D3DDISPLAYMODE *pMode,
							     D3DDEVTYPE DeviceType, 
							     DWORD Behavior);
	HRESULT RestoreDevice();
	long EnumerateModes(D3DDISPLAYMODE *pModes, long ModeCount);
	HRESULT DestroyDevice();
	

	//These new functions will enable use to subclass the
	//CHostApplication class in later chapters.  Because they
	//are virtual, any time this class calls them, the
	//implementations in the child class will be called first.
	//The implementations in this class are empty.  The BOOL
	//values should return whether or not the child class wants
	//to continue.

	//These functions are called for any set up before the app
	//initializes, renders, or terminates
	virtual BOOL PreInitialize();
	virtual BOOL PreTerminate();
	virtual void PreRender();

	//These functions are called for any cleanup after the app
	//initializes, renders, or terminates
	virtual BOOL PostInitialize();
	virtual BOOL PostTerminate();
	virtual HRESULT PostRender();

	//These functions allow the child class the respond to 
	//certain important events.
	virtual BOOL HandleMessage(MSG *pMessage);
	virtual BOOL PreReset();
	virtual BOOL PostReset();
	virtual void Render();

	HWND m_hWnd;
	long m_WindowWidth;
	long m_WindowHeight;

	//so it can be used quickly and easily.
	LPDIRECT3DDEVICE8 m_pD3DDevice;

	//The pointer to our D3D object
	LPDIRECT3D8       m_pD3D;

	//These members are saved copied of the device creation
	//parameters.
	D3DPRESENT_PARAMETERS         m_PresentParameters;
	D3DDEVICE_CREATION_PARAMETERS m_CreationParameters;

	//This flag is set to tell the application whether or not
	//to continue running.
	BOOL m_Continue;

	//Font for status, etc.
	CDirectXFont m_Font;
	char m_TextBuffer[1000];

	//These low res times are in milliseconds
	long    m_LowResStartTime;
	long    m_LowResCurrentTime;

	//These low res times are in seconds
	float   m_LowResLastFrameTime;
	float   m_LowResFPS;

	//This is the high res frequency to convert 
	//clocks to microseconds
	__int64 m_HighResFrequency;

	//These high res times are in counter values
	//when needed, use the frequency to convert
	//(saves precision)
	__int64 m_HighResStartTime;
	__int64 m_HighResCurrentTime;

	//Used to retrieve values since the functions
	//require pointers as input parameters
	__int64 m_HighResTemp;

	//These high res times are in seconds
	double m_HighResLastFrameTime;
	double m_HighResFPS;

	//Draw it or not??
	BOOL m_DrawFPS;

	//These are moved into the base class
	D3DXMATRIX m_WorldMatrix;
	D3DXMATRIX m_ViewMatrix;
	D3DXMATRIX m_ProjectionMatrix;
};
