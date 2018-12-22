// +-----------------------------------------------------------------------
// |
// |                         File Name: GPG5_gpEventManager.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPG5_gpEventManager Class Implementation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------
#include <GPG5_gpPch.h>
#include <GPG5_gpEventManager.h>

#include <GPG5_gpNode.h>

// +---------------------------
// |	Constructor & Destructor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpEventManager::GPG5_gpEventManager
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpEventManager::GPG5_gpEventManager()
{
}

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpEventManager::~GPG5_gpEventManager
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPG5_gpEventManager::~GPG5_gpEventManager()
{
}

// +---------------------------
// |	Events
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPG5_gpEventManager::SendEvent
// | Description     : 
// |									 This method sendS the events. In order to continue,
// |									 the return values must be true.
// |
// | Parameters : 
// |   hwnd          : 
// |   message       : 
// |   wParam        : 
// |   lParam        : 
// | 
// +-------------------------------------------------------------
bool GPG5_gpEventManager::SendEvent(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LockMe();

	GPG5_gpNodeList::iterator it		= nodeList_.begin();
	GPG5_gpNodeList::iterator itEnd = nodeList_.end();

	bool returnValue = true;
	while ( ( it != itEnd ) && returnValue )
	{
		returnValue = (*it)->ProcessEvent(hwnd, message, wParam, lParam);
		it++;
	}

	UnLockMe();

	return returnValue;
}
