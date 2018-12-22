/**********************************************************************
 	MODULE: SkelDemo
	
	DESCRIPTION: This module is a demo of how to pratically use MRC data
				 to finally render the animation in an OpenGl window.				 	
	
	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2001
 **********************************************************************/

#pragma once
#include "resource.h"

GLvoid KillGLWindow(GLvoid);
//==============================================================================================================
//////////////////////////////////////////////////////////////////////////////
// Main Windows Callback Process
//////////////////////////////////////////////////////////////////////////////
static LRESULT CALLBACK WndProc(	HWND	hWnd,							// Handle For This Window
							UINT	uMsg,									// Message For This Window
							WPARAM	wParam,									// Additional Message Information
							LPARAM	lParam)									// Additional Message Information
{																			
	switch (uMsg)															// Check For Windows Messages
	{																		
		case WM_ACTIVATE:													// Watch For Window Activate Message
		{																	
			if (!HIWORD(wParam))											// Check Minimization State
			{																
				active=TRUE;												// Program Is Active
				
			}																
			else															
			{																
				active=FALSE;												// Program Is No Longer Active
			}																
																			
			return 0;														// Return To The Message Loop
		}																	
																			
		case WM_SYSCOMMAND:													// Intercept System Commands
		{																	
			switch (wParam)													// Check System Calls
			{																
				case SC_SCREENSAVE:											// Screensaver Trying To Start?
				case SC_MONITORPOWER:										// Monitor Trying To Enter Powersave?
				return 0;													// Prevent From Happening
			}																
			break;															// Exit
		}																	
																			
		case WM_CLOSE:														// Did We Receive A Close Message?
		{																	
			PostQuitMessage(0);												// Send A Quit Message
			return 0;														// Jump Back
		}																	
																			
		case WM_KEYDOWN:													// Is A Key Being Held Down?
		{																	
			keys[wParam] = TRUE;											// If So, Mark It As TRUE
			return 0;														// Jump Back
		}																	
																			
		case WM_KEYUP:														// Has A Key Been Released?
		{																	
			keys[wParam] = FALSE;											// If So, Mark It As FALSE
			return 0;														// Jump Back
		}																	
																			
		case WM_SIZE:														// Resize The OpenGL Window
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));					// LoWord=Width, HiWord=Height
			return 0;														// Jump Back
		}
	}


	return DefWindowProc(hWnd,uMsg,wParam,lParam);							// Pass All Unhandled Messages To DefWindowProc
}


//==============================================================================================================
BOOL CALLBACK DialogProc(
						 HWND hwndDlg,  // handle to dialog box
						 UINT uMsg,     // message
						 WPARAM wParam, // first message parameter
						 LPARAM lParam  // second message parameter
						 )
{
	switch (uMsg)															// Check For Windows Messages
	{																		
		
		case WM_INITDIALOG:
		{
			return 0;
		}

		case WM_COMMAND:
		{																	
			UINT wNotifyCode = HIWORD(wParam);			// notification code 
			int idButton = LOWORD(wParam);				// item, control, or accelerator identifier 
			HWND hwndButton = (HWND) lParam;			// handle of control 

			
			if (wNotifyCode==BN_CLICKED)
			{
				switch (idButton)
				{		
					case IDC_CHECK_NORM:
						if (SendMessage(hwndButton,BM_GETCHECK,0,0)==BST_CHECKED)
							normals=true;
						else normals=false;
						break;
					
					case IDC_CHECK_SKEL:
						if (SendMessage(hwndButton,BM_GETCHECK,0,0)==BST_CHECKED)
							skel=true;
						else skel=false;
						break;
				
					case IDC_BUTTON_ROTATE:
						rotate=!rotate;
						if (rotate)
							SendMessage(hwndButton,WM_SETTEXT,0,(LPARAM)TEXT ("Stop"));
						else 
							SendMessage(hwndButton,WM_SETTEXT,0,(LPARAM)TEXT ("Rotate"));
						break;
				}
			}
			

			SetFocus(hWnd);
			return 1;
		}
	}
	return 0;
}
//==============================================================================================================

 

//==============================================================================================================
//==============================================================================================================
// Creates The OpenGL Window
// Parameters:
//	title			- Title To Appear At The Top Of The Window
//	width			- Width Of The GL Window Or Fullscreen Mode
//	height			- Height Of The GL Window Or Fullscreen Mode
//	bits			- Number Of Bits To Use For Color (8/16/24/32)
//	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)
//////////////////////////////////////////////////////////////////////////////
BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;												// Holds The Results After Searching For A Match
	WNDCLASS	wc;															// Windows Class Structure
	DWORD		dwExStyle;													// Window Extended Style
	DWORD		dwStyle;													// Window Style
	RECT		WindowRect;													// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;												// Set Left Value To 0
	WindowRect.right=(long)width;											// Set Right Value To Requested Width
	WindowRect.top=(long)0;													// Set Top Value To 0
	WindowRect.bottom=(long)height;											// Set Bottom Value To Requested Height
																			
	fullscreen=fullscreenflag;												// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);							// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;				// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;								// WndProc Handles Messages
	wc.cbClsExtra		= 0;												// No Extra Window Data
	wc.cbWndExtra		= 0;												// No Extra Window Data
	wc.hInstance		= hInstance;										// Set The Instance
	wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));	// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_CROSS);						// Load The Arrow Pointer
	wc.hbrBackground	= NULL;												// No Background Required For GL
	wc.lpszMenuName		= NULL;												// We Don't Want A Menu
	wc.lpszClassName	= "OP GL render screen";							// Set The Class Name
																			
	if (!RegisterClass(&wc))												// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;														// Return FALSE
	}																		
																			
	if (fullscreen)															// Attempt Fullscreen Mode?
	{																		
		DEVMODE dmScreenSettings;											// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));				// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);					// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;							// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;							// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;								// Selected Bits Per Pixel
		dmScreenSettings.dmDisplayFrequency = 75;							// refresh rate in Hz
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","WARNING",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;											// Windowed Mode Selected.
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;												// Return FALSE
			}																
		}																	
	}																		
																			
	if (fullscreen)															// Are We Still In Fullscreen Mode?
	{																		
		dwExStyle=WS_EX_APPWINDOW;											// Window Extended Style
		dwStyle=WS_POPUP;													// Windows Style
		ShowCursor(FALSE);													// Hide Mouse Pointer
	}																		
	else																	
	{																		
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;						// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;										// Windows Style
	}																		
																			
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);				// Adjust Window To True Requested Size
																			
	////=========START The OPEN GL Window============////
	if (!(hWnd=CreateWindowEx(	dwExStyle,									// Extended Style For The Window
								"OP GL render screen",						// Class Name
								title,										// Window Title
								dwStyle |									// Defined Window Style
								WS_CLIPSIBLINGS |							// Required Window Style
								WS_CLIPCHILDREN,							// Required Window Style
								0, 0,										// Window Position
								WindowRect.right-WindowRect.left,			// Calculate Window Width
								WindowRect.bottom-WindowRect.top,			// Calculate Window Height
								NULL,										// No Parent Window
								NULL,										// No Menu
								hInstance,									// Instance
								NULL))										// Dont Pass Anything To WM_CREATE
								||
	////=========START The FPS box===================////
		!(hDLGWnd=CreateDialog(hInstance,
								MAKEINTRESOURCE(IDD_USERBAR),
								hWnd,
								DialogProc))
								
			)																
	//=================================================//
	{
		KillGLWindow();														// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;														// Return FALSE
	}																		

	

	static	PIXELFORMATDESCRIPTOR pfd=										// pfd Tells Windows How We Want Things To Be
	{																		
		sizeof(PIXELFORMATDESCRIPTOR),										// Size Of This Pixel Format Descriptor
		1,																	// Version Number
		PFD_DRAW_TO_WINDOW |												// Format Must Support Window
		PFD_SUPPORT_OPENGL |												// Format Must Support OpenGL
		PFD_GENERIC_ACCELERATED |
		PFD_DOUBLEBUFFER,													// Must Support Double Buffering
		PFD_TYPE_RGBA,														// Request An RGBA Format
		bits,																// Select Our Color Depth
		0, 0, 0, 0, 0, 0,													// Color Bits Ignored
		0,																	// No Alpha Buffer
		0,																	// Shift Bit Ignored
		0,																	// No Accumulation Buffer
		0, 0, 0, 0,															// Accumulation Bits Ignored
		16,																	// 16Bit Z-Buffer (Depth Buffer)  
		0,																	// No Stencil Buffer
		0,																	// No Auxiliary Buffer
		PFD_MAIN_PLANE,														// Main Drawing Layer
		0,																	// Reserved
		0, 0, 0																// Layer Masks Ignored
	};																		
																			
	if (!(hDC=GetDC(hWnd)))													// Did We Get A Device Context?
	{																		
		KillGLWindow();														// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;														// Return FALSE
	}																		
																			
	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))							// Did Windows Find A Matching Pixel Format?
	{																		
		KillGLWindow();														// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;														// Return FALSE
	}																		
																			
	if(!SetPixelFormat(hDC,PixelFormat,&pfd))								// Are We Able To Set The Pixel Format?
	{																		
		KillGLWindow();														// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;														// Return FALSE
	}																		
																			
	if (!(hRC=wglCreateContext(hDC)))										// Are We Able To Get A Rendering Context?
	{																		
		KillGLWindow();														// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;														// Return FALSE
	}																		
																			
	if(!wglMakeCurrent(hDC,hRC))											// Try To Activate The Rendering Context
	{																		
		KillGLWindow();														// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;														// Return FALSE
	}																		
																			
	ShowWindow(hWnd,SW_SHOW);												// Show The Window
	SetForegroundWindow(hWnd);												// Slightly Higher Priority
	SetFocus(hWnd);															// Sets Keyboard Focus To The Window

	if (!InitGL())															// Initialize Our Newly Created GL Window
	{																		
		KillGLWindow();														// Reset The Display
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;														// Return FALSE
	}																		
	
	ReSizeGLScene(width, height);											// Set Up Our Perspective GL Screen																		

	return TRUE;															// Success
}





//==============================================================================================================
//////////////////////////////////////////////////////////////////////////////
// Kill The OpenGL Window
//////////////////////////////////////////////////////////////////////////////
GLvoid KillGLWindow(GLvoid)													// Properly Kill The Window
{																			
	if (fullscreen)															// Are We In Fullscreen Mode?
	{																		
		ChangeDisplaySettings(NULL,0);										// If So Switch Back To The Desktop
		ShowCursor(TRUE);													// Show Mouse Pointer
	}																		
																			
	if (hRC)																// Do We Have A Rendering Context?
	{																		
		if (!wglMakeCurrent(NULL,NULL))										// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))											// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;															// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))										// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;															// Set DC To NULL
	}

	if (hDLGWnd && ! DestroyWindow(hDLGWnd))
	{
		MessageBox(NULL,"Could Not Release hDLGWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDLGWnd=NULL;														// Set hDLGWnd To NULL
	}


	if (hWnd && !DestroyWindow(hWnd))										// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;															// Set hWnd To NULL
	}

	if (!UnregisterClass("OP GL render screen",hInstance))					// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;														// Set hInstance To NULL
	}

}

//==============================================================================================================