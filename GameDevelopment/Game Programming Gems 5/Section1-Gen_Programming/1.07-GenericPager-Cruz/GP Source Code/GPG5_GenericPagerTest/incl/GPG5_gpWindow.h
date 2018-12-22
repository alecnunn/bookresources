// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpWindow.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpWindow Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPG5_GPWINDOW_H
#define GPG5_GPWINDOW_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include <windows.h>
#include <string>

// +-----------------------------------------------------------------------
// |
// |  Class       : GPG5_gpWindow
// |  Description : 
// |								This class encapsutaltes a Win32 Window
// |
// +-----------------------------------------------------------------------
class GPG5_gpWindow
{
	public:
		// +---------------------------
		// |	Constructor & Destructor
		// +---------------------------
		GPG5_gpWindow();
		virtual ~GPG5_gpWindow();
		
		// +---------------------------
		// |	Handle
		// +---------------------------
		HWND GetHwnd() const;

		// +---------------------------
		// |	Parent
		// +---------------------------
		void SetParent(HWND parentHwnd);
		HWND GetParent() const;

		// +---------------------------
		// |	Name
		// +---------------------------
		void SetName(const std::string& name);
		const std::string& GetName() const;

		// +---------------------------
		// |	Class Name
		// +---------------------------
		void SetRegisterClassName(const std::string& name);
		const std::string& GetRegisterClassName() const;

		// +---------------------------
		// |	Position & Size
		// +---------------------------
		void SetPosition(int posX, int posY);
		void GetPosition(int& posX, int& posY) const;
		void SetSize(int width, int height);
		void GetSize(int& width, int& height) const;
		
		// +---------------------------
		// |	Style
		// +---------------------------
		void SetClassStyle(DWORD classStyle);
		DWORD GetClassStyle() const;
		void SetStyle(DWORD style);
		DWORD GetStyle() const;
		void SetExStyle(DWORD exStyle);
		DWORD GetExStyle() const;
		
		// +---------------------------
		// |	Cursor
		// +---------------------------
		void EnableCursor(bool showCursor);

		// +---------------------------
		// |	Create, Show & Hide
		// +---------------------------
		void CreateWin();	
		void Show();
		void Hide();

		// +---------------------------
		// |	Resize, Close & Destroy
		// +---------------------------
		void DoResize(int width, int height);
		void DoClose();
		void DoDestroy();
		
		// +--------------------------------------------------
		// |	Virtual Methods to Implement by a derived class
		// +--------------------------------------------------
		virtual void Initialize();				
		virtual bool Close(); //If true, destroy the window
		virtual void Destroy();			
		virtual void Resize(int width, int height);
		virtual void Process();
		virtual bool ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);

		// +---------------------------
		// |	Main Process
		// +---------------------------
		virtual bool MainProcess();
		virtual void MainLoopProcess();

	protected:
		// +---------------------------
		// |	Handle
		// +---------------------------
		HWND hwnd_;		

		// +---------------------------
		// |	Parent
		// +---------------------------
		HWND parentHwnd_;

		// +---------------------------
		// |	Name
		// +---------------------------
		std::string name_;		
		
		// +---------------------------
		// |	Class Name
		// +---------------------------
		std::string className_;
		
		// +---------------------------
		// |	Style
		// +---------------------------
		DWORD classStyle_;	
		DWORD style_;				
		DWORD exStyle_;			
		
		// +---------------------------
		// |	Position & Size
		// +---------------------------
		int posX_;					
		int posY_;					
		int bits_;					
		int width_;					
		int height_;				

		// +---------------------------
		// |	Cursor
		// +---------------------------
		bool showCursor_;

		// +---------------------------
		// |	Main Process
		// +---------------------------
		bool endProcess_;
};


#endif //GPG5_GPWINDOW_H