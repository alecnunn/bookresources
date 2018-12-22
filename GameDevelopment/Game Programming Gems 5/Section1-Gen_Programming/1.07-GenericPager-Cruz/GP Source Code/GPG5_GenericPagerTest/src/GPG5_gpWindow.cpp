// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpWindow.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpWindow Class Implementation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpPch.h>
#include <GPG5_gpWindow.h>

// +-------------------------------------------------------------
// |	Global
// +-------------------------------------------------------------
LRESULT CALLBACK GPG5_gpWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// +---------------------------
// |	Constructor & Destructor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::GPG5_gpWindow
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpWindow::GPG5_gpWindow():
classStyle_ (CS_HREDRAW | CS_VREDRAW),
style_			(WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS),
exStyle_		(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE),
		
posX_				(100),
posY_				(100),
bits_				(32),
width_			(800),
height_			(600),

showCursor_		(true),
endProcess_		(false),

hwnd_		(NULL),
parentHwnd_	(NULL)
{
	name_ = "GPG5_gpWindow";
	className_ = "GPG5_gpWindow";
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::~GPG5_gpWindow
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpWindow::~GPG5_gpWindow()
{
}

// +---------------------------
// |	Handle
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::GetHwnd
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
HWND GPG5_gpWindow::GetHwnd() const
{
	return hwnd_;
}

// +---------------------------
// |	Parent
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::SetParent
// | Description     : 
// |
// | Parameters : 
// |   parentHwnd    : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindow::SetParent(HWND parentHwnd)
{
	parentHwnd_ = parentHwnd;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::GetParent
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
HWND GPG5_gpWindow::GetParent() const
{
	return parentHwnd_;
}

// +---------------------------
// |	Name
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::SetName
// | Description     : 
// |
// | Parameters : 
// |   name          : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindow::SetName(const std::string& name)
{
	name_ = name;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::GetName
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const std::string& GPG5_gpWindow::GetName() const
{
	return name_;
}

// +---------------------------
// |	Class Name
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::SetRegisterClassName
// | Description     : 
// |
// | Parameters : 
// |   name          : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindow::SetRegisterClassName(const std::string& name)
{
	className_ = name;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::GetRegisterClassName
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
const std::string& GPG5_gpWindow::GetRegisterClassName() const
{
	return className_;
}

// +---------------------------
// |	Position & Size
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::SetPosition
// | Description     : 
// |
// | Parameters : 
// |   posX          : 
// |   posY          : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindow::SetPosition(int posX, int posY)
{
	posX_ = posX;
	posY_	= posY;

	SetWindowPos(hwnd_, NULL, posX_, posY_, 0, 0, SWP_NOSIZE);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::GetPosition
// | Description     : 
// |
// | Parameters : 
// |   posX          : 
// |   posY          : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindow::GetPosition(int& posX, int& posY) const
{
	posX = posX_;
	posY = posY_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::SetSize
// | Description     : 
// |
// | Parameters : 
// |   width         : 
// |   height        : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindow::SetSize(int width, int height)
{
	width_ = width;
	height_ = height;

	SetWindowPos(hwnd_, NULL, width_,  height_, 0, 0, SWP_NOMOVE);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::GetSize
// | Description     : 
// |
// | Parameters : 
// |   width         : 
// |   height        : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindow::GetSize(int& width, int& height) const
{
	width  = width_;
	height = height_;
}

// +---------------------------
// |	Style
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::SetClassStyle
// | Description     : 
// |
// | Parameters : 
// |   classStyle    : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindow::SetClassStyle(DWORD classStyle)
{
	classStyle_ = classStyle;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::GetClassStyle
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
DWORD GPG5_gpWindow::GetClassStyle() const
{
	return classStyle_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::SetStyle
// | Description     : 
// |
// | Parameters : 
// |   style         : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindow::SetStyle(DWORD style)
{
	style_ = style;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::GetStyle
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
DWORD GPG5_gpWindow::GetStyle() const
{
	return style_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::SetExStyle
// | Description     : 
// |
// | Parameters : 
// |   exStyle       : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindow::SetExStyle(DWORD exStyle)
{
	exStyle_ = exStyle;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::GetExStyle
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
DWORD GPG5_gpWindow::GetExStyle() const
{
		return exStyle_;
}

// +---------------------------
// |	Cursor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::EnableCursor
// | Description     : 
// |
// | Parameters : 
// |   showCursor    : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindow::EnableCursor(bool showCursor)
{
	showCursor_ = showCursor;
	ShowCursor(showCursor_);
}

// +---------------------------
// |	Create, Show & Hide
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::CreateWin
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindow::CreateWin()
{
	HINSTANCE hInstance = GetModuleHandle(0);

	WNDCLASSEX windowClass;		// window class

	windowClass.cbSize				= sizeof(WNDCLASSEX);
	windowClass.style					= classStyle_;
	windowClass.lpfnWndProc		= GPG5_gpWindowProc;
	windowClass.cbClsExtra		= 0;
	windowClass.cbWndExtra		= 0;
	windowClass.hInstance			= hInstance;
	windowClass.hIcon					= LoadIcon(NULL, IDI_APPLICATION);	
	windowClass.hCursor				= LoadCursor(NULL, IDC_ARROW);		
	windowClass.hbrBackground	= (HBRUSH) (COLOR_APPWORKSPACE);
	windowClass.lpszMenuName	= NULL;								
	windowClass.lpszClassName	= className_.data();
	windowClass.hIconSm				= LoadIcon(NULL, IDI_WINLOGO);		

	RegisterClassEx(&windowClass);

	hwnd_ = CreateWindowEx(exStyle_,
						  className_.data(),
						  name_.data(),
						  style_,
						  posX_, posY_,									
							width_,height_,	
						  parentHwnd_,
						  NULL,		
						  hInstance,
						  NULL);

	RECT rect;
	GetWindowRect(hwnd_,&rect);
	AdjustWindowRectEx(&rect, style_, FALSE, exStyle_);

	SetWindowLong(hwnd_,GWL_USERDATA,(LONG) this);

	Initialize();
}


// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::Show
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindow::Show()
{
	ShowWindow(hwnd_, SW_SHOWNORMAL);
	UpdateWindow(hwnd_);
	SetFocus(hwnd_);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::Hide
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindow::Hide()
{
	//To do
}

// +---------------------------
// |	Resize, Close & Destroy
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::DoResize
// | Description     : 
// |
// | Parameters : 
// |   width         : 
// |   height        : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindow::DoResize(int width, int height)
{
	width_ = width;
	height_ = height;
	
	Resize(width_, height_);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::DoClose
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindow::DoClose()
{
	 if ( Close() )
	 {
		ClipCursor(NULL);
		DestroyWindow(hwnd_);
		endProcess_ = true;
	 }
	 else
	 {
		 Hide();
	 }
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::DoDestroy
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindow::DoDestroy()
{
	hwnd_ = NULL;
	Destroy();
//	UnregisterClass(className_, GetModuleHandle(0));		// Free the window class
}

// +--------------------------------------------------
// |	Virtual Methods to Implement by a derived class
// +--------------------------------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::Initialize
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindow::Initialize()
{
	//To Redefine
}


// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::Close
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
bool GPG5_gpWindow::Close()
{
	//To Redefine
	return true;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::Destroy
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindow::Destroy()
{
	//To Redefine
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::Resize
// | Description     : 
// |
// | Parameters : 
// |   width         : 
// |   height        : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindow::Resize(int width, int height)
{
	//To Redefine
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::Process
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindow::Process()
{
	//To Redefine
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::ProcessMessage
// | Description     : 
// |
// | Parameters : 
// |   message       : 
// |   wParam        : 
// |   lParam        : 
// | 
// +-------------------------------------------------------------
bool GPG5_gpWindow::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	//To Redefine
	return true;
}

// +---------------------------
// |	Main Process
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::Process
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
bool GPG5_gpWindow::MainProcess()
{
	MSG msg;
	if (hwnd_)
	{
		if (PeekMessage(&msg, hwnd_, 0, 0, PM_REMOVE)) 
		{ 
			TranslateMessage(&msg);	// Find out what the message does
			DispatchMessage(&msg);	// Execute the message
		}
		else											// if there wasn't a message
		{ 
			Process();
		} 
	}
	else
	{
		endProcess_ = true;
	}
	return endProcess_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindow::MainLoopProcess
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindow::MainLoopProcess()
{
	MSG msg;
	while( ! endProcess_ )
	{
		if (PeekMessage(&msg, hwnd_, 0, 0, PM_REMOVE)) 
		{ 
			TranslateMessage(&msg);					// Find out what the message does
			DispatchMessage(&msg);						// Execute the message
		}
		else											// if there wasn't a message
		{ 
			//Process();
		} 
	}
}

// +-------------------------------------------------------------
// |	Global
// +-------------------------------------------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindowProc
// | Description     : 
// |
// | Parameters : 
// |   hwnd          : 
// |   message       : 
// |   wParam        : 
// |   lParam        : 
// | 
// +-------------------------------------------------------------
LRESULT CALLBACK GPG5_gpWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LONG    lRet = 0; 

	GPG5_gpWindow* genericPagerWindow= (GPG5_gpWindow*) GetWindowLong(hwnd,GWL_USERDATA);
	
	if ( genericPagerWindow && genericPagerWindow->ProcessMessage(message, wParam, lParam) )
	{
		switch (message)
		{ 
			case WM_CREATE:					
				{
					break; 
				}
			case WM_DESTROY:
				{
					genericPagerWindow->DoDestroy();
					break; 
				}

			case WM_CLOSE:
				{
					genericPagerWindow->DoClose();				
					break; 
				}

			case WM_SIZE:										
				{
					genericPagerWindow->DoResize(LOWORD(lParam),HIWORD(lParam));
					break; 
				}
				
			case WM_KEYDOWN:
				{
					if(wParam == VK_ESCAPE)
					{
						//PostQuitMessage (0);
						genericPagerWindow->DoClose();				
					}
					break;
				}
				
			default:
				{
					lRet = DefWindowProc (hwnd, message, wParam, lParam); 
					break; 
				}
		}
	}
	else
	{
		lRet = DefWindowProc (hwnd, message, wParam, lParam); 
	}

	return lRet;										
}


