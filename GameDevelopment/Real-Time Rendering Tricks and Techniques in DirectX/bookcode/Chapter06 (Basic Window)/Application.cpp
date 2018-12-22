/***************************************************************
* Application.cpp                                              *
*                                                              *
* This file contains the implementation of the                 *
* CHostApplication class, as well as the global entry          *
* functions called by the system to actually start the app.    *
* To compile correctly, this file must be linked with:         *
* kernel32.lib                                                 *
* user32.lib                                                   *
***************************************************************/

//Include the class definition
#include "Executable.h"
#include "Application.h"

CHostApplication::CHostApplication()
{
	//Set the default window size
	m_WindowWidth  = 640;
	m_WindowHeight = 480;
}

void CHostApplication::Go()
{
    //Most of the details of these parameters are not important
	//to us.  The short story is that we are defining a window 
	//class with very basic parameters that will be used to
	//create our application window later.  The important 
	//parameters are the MessageHandler function, which will be
	//used to process messages, and the class name, which will
	//be used to identify the class.
	WNDCLASSEX WindowClass = {sizeof(WNDCLASSEX), 
							  CS_CLASSDC,
							  EntryMessageHandler,
							  0,
							  0, 
							  GetModuleHandle(NULL),
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  "Host Application",
							  NULL};

    //Register the new class so it can be used later.
	RegisterClassEx(&WindowClass);

    //This call creates the window, using the window class we 
	//defined above. We specify the window title and size.  The
	//rest of the parameters specify that our window is a basic
	//window and a child of the main desktop window.
    m_hWnd = CreateWindow("Host Application",
						  "Host Application", 
						  WS_OVERLAPPEDWINDOW,
						  0,
						  0,
						  m_WindowWidth,
						  m_WindowHeight,
						  GetDesktopWindow(),
						  NULL,
						  WindowClass.hInstance,
						  NULL);

    //If we successfully created the window, show the window.
	if(m_hWnd)
		ShowWindow(m_hWnd, SW_SHOW);
	else 
		return;

	//This while loop is a message pump.  It ensures that the 
	//application is continually taking messages from the 
	//message queue and sending them to the message handler 
	//function.  The pump forces the underlying mechanics of 
	//the app to call MessageHandler.  We do not call it 
	//directly.
	MSG Message; 
	PeekMessage(&Message, 0, 0, 0, PM_REMOVE);

	while (Message.message != WM_QUIT)
    {
		TranslateMessage(&Message);
        DispatchMessage(&Message);
		PeekMessage(&Message, 0, 0, 0, PM_REMOVE);
    }
}

CHostApplication::~CHostApplication()
{
	if (m_hWnd)
		DestroyWindow(m_hWnd);
}

//This is the message handling function that is entrusted with
//responding to messages ranging from mouse and keyboard input 
//to system messages.
LRESULT WINAPI CHostApplication::MessageHandler(HWND hWnd, 
												UINT Message, 
												WPARAM wParam,
												LPARAM lParam)
{
	//This is where we decide which messages to respond to and
	//which messages should simply be passed to the default 
	//handler.
	switch(Message)
    {
        //For now, we just handle the destroy message.  We 
		//respond by telling the app to shut down.
		case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    //Any messages that get this far are handled by the default
	//message handler.
	return DefWindowProc(hWnd, Message, wParam, lParam);
}




