/***************************************************************
* Executable.h                                                 *
*                                                              *
* This file contains the prototypes for basic functions that   *
* are called by the OS when the executable starts.			   *
***************************************************************/

//include the definitions of the basic windows types
#include <windows.h>

//WinMain is called when the application is run.  It usually 
//takes care of the creation and functioning of the window.
//For our purposes, we create an instance of our application
//class and let that handle the window.
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT);

//This is the message handling function that is called by the 
//system.  We simply pass the messages to our application 
//object.
LRESULT WINAPI EntryMessageHandler(HWND hWnd, UINT Message, 
							  WPARAM wParam, LPARAM lParam);
