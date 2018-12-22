// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpWindowOpenGL.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpWindowOpenGL Class Implementation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpPch.h>
#include <GPG5_gpWindowOpenGL.h>

#include <GPG5_gpNodeManager.h>
#include <GPG5_gpEventManager.h>

// +---------------------------
// |	Constructor & Destructor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindowOpenGL::GPG5_gpWindowOpenGL
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpWindowOpenGL::GPG5_gpWindowOpenGL():
perspectiveRatio_ ( 1.0 )
{
	perspectiveRatio_ = (float) (width_) / (float) (height_);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindowOpenGL::~GPG5_gpWindowOpenGL
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpWindowOpenGL::~GPG5_gpWindowOpenGL()
{
}

// +---------------------------
// |	Context
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindowOpenGL::GetRenderingContext
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
HGLRC GPG5_gpWindowOpenGL::GetRenderingContext() const
{
	return renderingContext_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindowOpenGL::GetDeviceContext
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
HDC	GPG5_gpWindowOpenGL::GetDeviceContext() const
{
	return deviceContext_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindowOpenGL::SwapBuffers
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindowOpenGL::SwapBuffersWindow() const
{
	SwapBuffers(deviceContext_);			// bring backbuffer to foreground
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindowOpenGL::MakeCurrent
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindowOpenGL::MakeCurrent() const
{
	wglMakeCurrent(deviceContext_, renderingContext_);
}

// +--------------------------------------------------
// |	Virtual Methods of GPG5_gpWindow
// +--------------------------------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindowOpenGL::Initialize
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindowOpenGL::Initialize()
{
	deviceContext_ = GetDC(hwnd_);
	SetupPixelFormat();
	renderingContext_ = wglCreateContext(deviceContext_);
	wglMakeCurrent(deviceContext_, renderingContext_);
	endProcess_ = false;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindowOpenGL::Close
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
bool GPG5_gpWindowOpenGL::Close()
{
	return true;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindowOpenGL::Destroy
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindowOpenGL::Destroy()
{
	wglMakeCurrent(deviceContext_, NULL);
	wglDeleteContext(renderingContext_);
	ReleaseDC(hwnd_, deviceContext_);		
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindowOpenGL::Resize
// | Description     : 
// |
// | Parameters : 
// |   width         : 
// |   height        : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindowOpenGL::Resize(int width, int height)
{
	perspectiveRatio_ = (float) (width_) / (float) (height_);
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindowOpenGL::Process
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindowOpenGL::Process()
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.0f, 0.0f, 0.3f, 0.0f);

 	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,perspectiveRatio_ , 100.0f, 10000.0f);
	glViewport(0.0f, 0.0f, width_, height_);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	GPG5_gpNodeManager::GetInstance().ProcessNodes();
	
	glFlush();
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindowOpenGL::ProcessMessage
// | Description     : 
// |
// | Parameters : 
// |   message       : 
// |   wParam        : 
// |   lParam        : 
// | 
// +-------------------------------------------------------------
bool GPG5_gpWindowOpenGL::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	return ( GPG5_gpNodeManager::GetInstance().GetEventManager().SendEvent(hwnd_, message, wParam, lParam) );
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindowOpenGL::MainProcess
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
bool GPG5_gpWindowOpenGL::MainProcess()
{
	GPG5_gpNodeManager::GetInstance().LockMe();
	MSG msg;
	if (hwnd_)
	{
		if (PeekMessage(&msg, hwnd_, 0, 0, PM_REMOVE)) 
		{ 
			TranslateMessage(&msg);					// Find out what the message does
			DispatchMessage(&msg);						// Execute the message
		}
		else											// if there wasn't a message
		{ 
			wglMakeCurrent(deviceContext_, renderingContext_);
			Process();
			SwapBuffers(deviceContext_);
		} 
	}
	else
	{
		endProcess_ = true;
	}
	GPG5_gpNodeManager::GetInstance().UnLockMe();
	
	return endProcess_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindowOpenGL::MainLoopProcess
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPG5_gpWindowOpenGL::MainLoopProcess()
{
	MSG msg;
	while( ! endProcess_ )
	{
		//if (PeekMessage(&msg, hwnd_, 0, 0, PM_REMOVE)) 
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{ 
			TranslateMessage(&msg);					// Find out what the message does
			DispatchMessage(&msg);						// Execute the message
		}
		else											// if there wasn't a message
		{ 
			wglMakeCurrent(deviceContext_, renderingContext_);
			Process();
			SwapBuffers(deviceContext_);
		} 
	}
}


// +-------------------------------------------------------------
// |	Protected Methods
// +-------------------------------------------------------------

// +---------------------------
// |	Pixel Format
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpWindowOpenGL::SetupPixelFormat
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------

void GPG5_gpWindowOpenGL::SetupPixelFormat()
{
	int nPixelFormat;					// our pixel format index

	static PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR),	// size of structure
		1,								// default version
		PFD_DRAW_TO_WINDOW |			// window drawing support
		PFD_SUPPORT_OPENGL |			// OpenGL support
		PFD_DOUBLEBUFFER,				// double buffering support
		PFD_TYPE_RGBA,					// RGBA color mode
		32,								// 32 bit color mode
		0, 0, 0, 0, 0, 0,				// ignore color bits, non-palettized mode
		0,								// no alpha buffer
		0,								// ignore shift bit
		0,								// no accumulation buffer
		0, 0, 0, 0,						// ignore accumulation bits
		16,							// 16 bit z-buffer size
		0,								// no stencil buffer
		0,								// no auxiliary buffer
		PFD_MAIN_PLANE,					// main drawing plane
		0,								// reserved
		0, 0, 0 };						// layer masks ignored

	nPixelFormat = ChoosePixelFormat(deviceContext_, &pfd);	// choose best matching pixel format

	SetPixelFormat(deviceContext_, nPixelFormat, &pfd);		// set pixel format to device context
}
