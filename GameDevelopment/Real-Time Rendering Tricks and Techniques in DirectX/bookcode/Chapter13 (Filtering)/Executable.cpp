/***************************************************************
* Executable.cpp                                               *
*                                                              *
* This file contains the prototypes for basic functions that   *
* are called by the OS when the executable starts.			   *
* To compile correctly, this file must be linked with:         *
* kernel32.lib                                                 *
* user32.lib                                                   *
*                                                              *
* Changes:                                                     *
*	Changed the Application class                              *
***************************************************************/

//include the header file
#include "executable.h"

//Include the definition of the class we wish to instantiate.
//This class will change as new classes are built.
#include "TextureStateApplication.h"

//Create a global application pointer so the system calls can
//be relayed to the application's message handler.  This 
//pointer will change as we create child classes of 
//CHostApplication.
static CTextureStateApplication *g_pTextureStateApplication = NULL; 

//WinMain is called when the application is run.  It usually 
//takes care of the creation and functioning of the window.
//For our purposes, we create an instance of our application
//class and let that handle the window.
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	//Create a new instance of the application
	g_pTextureStateApplication = new CTextureStateApplication();

	//Run until the application returns from Go!
    g_pTextureStateApplication->Go();

	//Clean up the application
	delete (g_pTextureStateApplication);

	//End the program.
	return 0;
}

//This is the message handling function that is called by the 
//system.  We simply pass the messages to our application 
//object.
LRESULT WINAPI EntryMessageHandler(HWND hWnd, UINT Message, 
							  WPARAM wParam, LPARAM lParam)
{
	//Technically, we should probably check to make sure 
	//that the pointer is not NULL.  However, the chances
	//of this function getting called AND the pointer being
	//bad are fairly slim, so we'll take our chances and not
	//spend the cycles double checking...
	return g_pTextureStateApplication->MessageHandler(hWnd, Message, 
											          wParam, lParam);
}
