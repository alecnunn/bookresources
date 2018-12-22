/**********************************************************************
 	MODULE: SkelDemo
	
	DESCRIPTION: This module is a demo of how to pratically use MRC data
				 to finally render the animation in an OpenGl window.				 	
	
	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2001
 **********************************************************************/

#include "std.h"
#include "OpGL.h"
#include "ImportMRC\MRC.h"
#include "skin.h"


//==============================================================================================================
//////////////////////////////////////////////////////////////////////////////
// Global Variables
//////////////////////////////////////////////////////////////////////////////
HDC			hDC=NULL;														// Private GDI Device Context
HGLRC		hRC=NULL;														// Permanent Rendering Context
HWND		hWnd=NULL;														// Holds Main Window Handle
HWND		hDLGWnd=NULL;													// Holds FPS Window Handle

HINSTANCE	hInstance;														// Holds The Instance Of The Application
																			
bool	keys[256];															// Array Used For The Keyboard Routine (0 init)
bool	active=TRUE;														// Window Active Flag Set To TRUE By Default
bool	fullscreen=TRUE;													// Fullscreen Flag Set To Fullscreen Mode By Default

bool	normals=FALSE;		//display normals
bool	skel=FALSE;			//display skeletal structure
bool	rotate=FALSE;		//rotate character animation

Clock t;

_Skin* pSkin;

//==============================================================================================================


//==============================================================================================================
bool InitGameData() {
	//opening MRC file
	FILE* mrc=fopen("Data\\TEST.mrc","rb");
	if (!mrc) return false;
	//loading MRC data (DLL function)
	MRC_t* MRCdata = MRCLoad (mrc);
	if (!MRCdata) return false;
	
	// object initialization
	pSkin = new _Skin(MRCdata);

	return true;
}

//==============================================================================================================

#include "WinProc.h" 
//==============================================================================================================
//////////////////////////////////////////////////////////////////////////////
// Main Windows Application Entry Point
//////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(	HINSTANCE	hInstance,									// Instance
					HINSTANCE	hPrevInstance,								// Previous Instance
					LPSTR		lpCmdLine,									// Command Line Parameters
					int			nCmdShow)									// Window Show State
{																			
	MSG		msg;															// Windows Message Structure
	BOOL	done=FALSE;														// Bool Variable To Exit Loop

	// Ask The User Which Screen Mode They Prefer
	if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;													// Windowed Mode
	}

	// Create Our OpenGL Window
	if (!CreateGLWindow("Skeletal animation DEMO - MARCO TOMBESI",800,600,32,fullscreen))
	{
		return 0;															// Quit If Window Was Not Created
	}																		

///////////////////////
//init game data:
	if (!InitGameData())
	{
		MessageBox(NULL,"Can't Load OBJECT data from .MRC file!!.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return 0;
	}
//////////////////////

	while(!done)															// Loop That Runs While done=FALSE
	{																		
		if (PeekMessage(&msg,hWnd,0,0,PM_REMOVE))							// Is There A Message Waiting?
		{																	
			if (msg.message==WM_QUIT)										// Have We Received A Quit Message?
			{																
				done=TRUE;													// If So done=TRUE
			}																
			else															// If Not, Deal With Window Messages
			{																
				TranslateMessage(&msg);										// Translate The Message
				DispatchMessage(&msg);										// Dispatch The Message
			}																
		}																	
		else																// If There Are No Messages
		{
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if (active && keys[VK_ESCAPE])									// Active?  Was There A Quit Received?
			{																
				done=TRUE;													// ESC or DrawGLScene Signalled A Quit
			}																
			else															// Not Time To Quit, Update Screen
			{																
				t.Update();												// TIMER SETTING FOR LAST FRAME

				DrawGLScene();												// Reduce Wasted Cycles While Minimized (Draw Scene)

				SwapBuffers(hDC);											// Swap Buffers (Double Buffering)
			}																
																			
			//////////////////////////////////////////////////////////////////////////////
			if (keys[VK_F1])												// Is F1 Being Pressed?
			{																
				keys[VK_F1]=FALSE;											// If So Make Key FALSE
				KillGLWindow();												// Kill Our Current Window
				fullscreen=!fullscreen;										// Toggle Fullscreen / Windowed Mode
				// Recreate Our OpenGL Window
				if (!CreateGLWindow("Skeletal animation DEMO - MARCO TOMBESI",800,600,32,fullscreen))
				{
					return 0;												// Quit If Window Was Not Created
				}
			
			}
		}
	}

	// Shutdown
	
	KillGLWindow();															// Kill The Window
	
	return (msg.wParam);													// Exit The Program
}
//==============================================================================================================

