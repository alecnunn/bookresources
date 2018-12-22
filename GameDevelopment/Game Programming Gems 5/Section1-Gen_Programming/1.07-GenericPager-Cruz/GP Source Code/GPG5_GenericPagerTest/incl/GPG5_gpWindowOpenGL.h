// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpWindowOpenGL.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpWindowOpenGL Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPG5_GPWINDOWOPENGL_H
#define GPG5_GPWINDOWOPENGL_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpWindow.h>

// +-----------------------------------------------------------------------
// |
// |  Class       : GPG5_gpWindowOpenGL
// |  Description : 
// |								This class encapsutaltes a Win32 OpenGL Window
// |
// +-----------------------------------------------------------------------
class GPG5_gpWindowOpenGL: public GPG5_gpWindow
{
	public:
		// +---------------------------
		// |	Constructor & Destructor
		// +---------------------------
		GPG5_gpWindowOpenGL();
		virtual ~GPG5_gpWindowOpenGL();

		// +---------------------------
		// |	Context
		// +---------------------------
		HGLRC GetRenderingContext() const;
		HDC	GetDeviceContext() const;
		void SwapBuffersWindow() const;				
		void MakeCurrent() const;				
	
		// +--------------------------------------------------
		// |	Virtual Methods of GPG5_gpWindow
		// +--------------------------------------------------
		virtual void Initialize();				
		virtual bool Close();
		virtual void Destroy();			
		virtual void Resize(int width, int height);
		virtual void Process();
		virtual bool ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);

		virtual bool MainProcess();
		virtual void MainLoopProcess();

	protected:
		// +---------------------------
		// |	Pixel Format
		// +---------------------------
		virtual void SetupPixelFormat();

		// +---------------------------
		// |	Context
		// +---------------------------
		HGLRC renderingContext_;
		HDC	deviceContext_;

		// +---------------------------
		// |	View Ratio
		// +---------------------------
		float perspectiveRatio_;

	private:

};

#endif //GPG5_GPWINDOWOPENGL_H