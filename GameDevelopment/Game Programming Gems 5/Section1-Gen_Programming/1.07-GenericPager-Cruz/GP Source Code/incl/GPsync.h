// +-----------------------------------------------------------------------
// |
// |                         File Name: GPsync.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPsync Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPSYNC_H
#define GPSYNC_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include <GPdefs.h>
#include <windows.h>

// +-----------------------------------------------------------------------
// |
// |  Class       : GPsync
// |  Description : 
// |								This class is a "singleton" that synchronize all the GP.
// |								All the GP synchronizes with the same (and unique) CRITICAL SECTION
// |								It is also possible to assing to this class an external CRITICAL
// |								SECTION in order to synchronize the GP with an other external thread.
// |
// +-----------------------------------------------------------------------
class _GP GPsync
{
	public:
		// +---------------------------
		// |	Destructor
		// +---------------------------
		~GPsync();

		// +---------------------------
		// |	Instance. Singleton Method
		// +---------------------------
		static GPsync& GetInstance();
		
		// +---------------------------
		// |	Synchronization
		// +---------------------------
		void SetCriticalSection(CRITICAL_SECTION* pCriticalSection=NULL);
		CRITICAL_SECTION* GetCriticalSection() const;
		void Lock();
		void UnLock();

	protected:
		// +---------------------------
		// |	Synchronization
		// +---------------------------
		CRITICAL_SECTION* pCriticalSection_;
		bool ownCriticalSection_;

	private:
		// +---------------------------
		// |	Constructor & Copy Constructor 
		// +---------------------------
		GPsync();
		GPsync(const GPsync& ref); //Not Implemented
};

#endif //GPSYNC_H