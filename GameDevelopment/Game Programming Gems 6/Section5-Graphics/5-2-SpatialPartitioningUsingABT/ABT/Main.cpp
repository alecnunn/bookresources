#include "OGLWindow.h"
#include <tchar.h>

/* ***************************************************************** *\
|  WinMain
|  *****************************************************************
|  Program entry point
\* ***************************************************************** */
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG					msg;					// Window Message Structure
	DWORD				dwTickCount;			// Used For The Tick Counter
	DWORD				dwLastTickCount;
	boolean				bFullscreen = false;	// fullscreen flag
	cOGLWindow			window(hInstance);

	// init const member
	window.m_szClassName = _T("OGLWNDCLASS");

	// show info
	MessageBox(HWND_DESKTOP, _T("Game Programming Gems 6 - ABT Demo\n")
							 _T("Use arrow and page up/down keys to move, W,A,S and D keys to rotate"),
							 _T("Information"), MB_OK | MB_ICONINFORMATION);


	// Ask The User If They Want To Start In FullScreen Mode?
	if (MessageBox (HWND_DESKTOP, _T("Would You Like To Run In Fullscreen Mode?"), _T("Start FullScreen?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		bFullscreen = true;
	}
	
	// register the window class
	if(!window.registerClass())
	{
		// Failure
		MessageBox(HWND_DESKTOP, _T("Error Registering Window Class!"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
		return -1;
	}

	boolean	bIsMessagePumpActive;
	boolean bIsProgramLooping = true;

	while(bIsProgramLooping)
	{
		// create the window
		if (window.create(_T("GPG 6 - ABT"), 800, 600, bFullscreen))
		{
			// At This Point We Should Have A Window That Is Setup To Render OpenGL
			if(!window.init())
			{
				// Failure
				system("PAUSE");
				PostQuitMessage(0);
				bIsProgramLooping = false;
			}
			else
			{	
				dwLastTickCount = GetTickCount();
				bIsMessagePumpActive = true;
				while(bIsMessagePumpActive)
				{
					// check for window messages
					if (PeekMessage(&msg, window, 0, 0, PM_REMOVE) != 0)
					{
						// Check For WM_QUIT Message
						if(msg.message != WM_QUIT)
						{
							DispatchMessage(&msg);
						}
						else
						{
							// end message loop
							bIsMessagePumpActive = false;
							bIsProgramLooping = false;
						}
					}
					else
					{
						if (!window.m_bIsVisible)
						{
							// wait for a message to come
							WaitMessage();
						}
						else
						{
							// application loop

							// update timing vars
							dwTickCount = GetTickCount();
							window.update(dwTickCount - dwLastTickCount);
							dwLastTickCount = dwTickCount;
							
							// draw the scene
							window.draw();
						}
					}
				}
			}

			// destroy the window
			window.destroy();
		}
		else
		{
			// Error Creating Window
			MessageBox(HWND_DESKTOP, _T("Error Creating Window"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
			bIsProgramLooping = false;
		}
	}

	// unregister window class
	window.unregisterClass();

	return 0;
}


