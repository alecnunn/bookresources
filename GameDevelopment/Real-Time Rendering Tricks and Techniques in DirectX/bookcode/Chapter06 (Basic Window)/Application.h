/***************************************************************
* Application.h                                                *
*                                                              *
* This file contains the header information for the class      *
* CHostApplication, which will become the host application for *
* all of our 3D work.  It is a very simple windows application *
* because most of the real work will be done by other classes. *
* For this file to compile correctly, the path for the DX      *
* include files must be added to your path.                    *
***************************************************************/

//Include this header file.  It includes the basic 
//windows headers that make our most simple app work.
#include <windows.h>

class CHostApplication
{
public:
	//The constructor handles the construction of the 
	//application object.  The destructor will be used
	//to cleanup anything...
	CHostApplication();
	virtual ~CHostApplication();

	//This is the message handling function for the application.
	//All windows messages will be passed to and interpreted by 
	//this function.  
	LRESULT WINAPI MessageHandler(HWND hWnd, UINT msg, 
								  WPARAM wParam, LPARAM lParam);

	//This function starts the actual processing of the
	//application.  For now, it just pumps messages
	void Go();

protected:
	//This is a handle to our window.  This will be passed to
	//the 3D device when a windowed device is created.
	HWND m_hWnd;

	//Some simple window size settings
	long m_WindowWidth;
	long m_WindowHeight;
};
