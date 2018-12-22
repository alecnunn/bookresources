// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpEventManager.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpEventManager Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPG5_GPEVENTMANAGER_H
#define GPG5_GPEVENTMANAGER_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpBaseNodeManager.h>

// +-----------------------------------------------------------------------
// |
// |  Class       : GPG5_gpEventManager
// |  Description : 
// |								This class sends the all the events that receives to all the
// |								GPG5_gpNode which has been added to it. This class is a member
// |								of GPG5_gpNodeManager, and the GPG5_gpWindow and GPG5_gpWindowOpenGL
// |								use it to send the received events.
// |
// +-----------------------------------------------------------------------
class GPG5_gpEventManager:public GPG5_gpBaseNodeManager
{
	public:
		// +---------------------------
		// |	Constructor & Destructor
		// +---------------------------
		GPG5_gpEventManager();
		virtual ~GPG5_gpEventManager();

		// +---------------------------
		// |	Events
		// +---------------------------
		bool SendEvent(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	protected:

	private:
};

#endif //GPG5_GPEVENTMANAGER_H