// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpNodeWindow.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpNodeWindow Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPG5_gpNODEWINDOW_H
#define GPG5_gpNODEWINDOW_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpNodeQuad.h>
#include <GPwindow.h>

// +-----------------------------------------------------------------------
// |
// |  Class       : GPG5_gpNodeWindow
// |  Description : 
// |								This class RE-implements the methods:
// |										virtual void Init(const GPindex& index);
// |										virtual void UpdateRadius(int radius);
// |								of GPwindow.
// |
// |								These class is NOT REALLY NEEDED. The only purpose of its 
// |								use, it is to provide an easy way to draw, over the world,
// |								the size of a GPwindow ( to draw its frame ).
// |
// |								The only purpose of the RE-implementation of these methos
// |								is to update some information (position and size) for the
// |								window frame drawing when these methods are called by the
// |								Generic Pager.
// |
// +-----------------------------------------------------------------------
class GPG5_gpNodeWindow: public GPG5_gpNodeQuad, public GPwindow
{
	public:
		// +---------------------------
		// |	Constructor & Destructor
		// +---------------------------
		GPG5_gpNodeWindow();
		virtual ~GPG5_gpNodeWindow();

		// +---------------------------
		// |	Active
		// +---------------------------
		void Active(bool isActive);
		bool IsActive() const;

		// +--------------------------------------------------
		// |	Virtual Methods of GPG5_gpNode
		// +--------------------------------------------------
		virtual void SetPosition(const GPG5_gpVector& position);
		virtual bool ProcessEvent(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		virtual void Process();

		// +--------------------------------------------------
		// |	Virtual Methods of GPworld
		// +--------------------------------------------------
		virtual void Init(const GPindex& index);
		virtual void UpdateRadius(int radius);

	protected:
		// +---------------------------
		// |	Update GP Window
		// +---------------------------
		void UpdateGPwindow();

		// +---------------------------
		// |	Active
		// +---------------------------
		bool isActive_;
		GPG5_gpVector originalColor_;
		double originalLineWidth_;
		
		double speed_;

	private:

};

#endif //GPG5_gpNODEWINDOW_H