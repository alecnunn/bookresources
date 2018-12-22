// +-----------------------------------------------------------------------
// |
// |                         File Name: GPlock.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This files contains the GPlock Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPLOCK_H
#define GPLOCK_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include <GPdefs.h>
#include <windows.h>

// +-----------------------------------------------------------------------
// |
// |  Class       : GPlock
// |  Description : 
// |								This class provides an easy to use encapsulation of 
// |								a Win32 CRITICAL_SECTION.
// |
// +-----------------------------------------------------------------------
class _GP GPlock
{
	public:
		// +---------------------------
		// |	Constructor & Destructor
		// +---------------------------
		GPlock();
		virtual ~GPlock();
		
		// +---------------------------
		// |	Lock & UnLock
		// +---------------------------
		void Lock();
		void UnLock();

		// +---------------------------
		// |	CRITICAL_SECTION
		// +---------------------------
		CRITICAL_SECTION& GetCriticalSection() const;
	
	private:
		// +---------------------------
		// |	CRITICAL_SECTION
		// +---------------------------
		CRITICAL_SECTION criticalSection_;  		

};

#endif //GPLOCK_H