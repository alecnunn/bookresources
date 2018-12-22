// +-----------------------------------------------------------------------
// |
// |                         File Name: GPthread.h
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPthread Class Header.
// | 
// +-----------------------------------------------------------------------

#ifndef GPGPEAD_H
#define GPGPEAD_H
#pragma once

// +---------------------------
// |	Includes
// +---------------------------
#include <GPdefs.h>
#include <windows.h>

// +-----------------------------------------------------------------------
// |
// |  Class       : GPthread
// |  Description : 
// |								This class encapsulates a Win32 thread and provides its main
// |								functionallity.
// |
// +-----------------------------------------------------------------------
class _GP GPthread
{
	public:
		// +---------------------------
		// |	GPthread Function
		// +---------------------------
		typedef DWORD (WINAPI *GPthreadFuncPtr)(void* arg);

		// +---------------------------
		// |	Constructor & Destructor
		// +---------------------------
		GPthread(GPthreadFuncPtr pFun, void* pArg);
		virtual ~GPthread();

		// +---------------------------
		// |	Handle & Identifier
		// +---------------------------
		HANDLE GetHandle() const;
		DWORD  GetIdentifier() const;

		// +---------------------------
		// |	Priorioty
		// +---------------------------
		void SetPriority(int priority);
		int  GetPriority() const;

		// +---------------------------
		// |	Control
		// +---------------------------
		void Resume();
		void Suspend();  
		void WaitForFinish(DWORD milliseconds = 100);
		bool IsRunning() const;

	protected:
		// +---------------------------
		// |	Handle & Identifier
		// +---------------------------
		HANDLE handle_;
		DWORD  id_;     

	private:
		// +---------------------------
		// |	Copy Constructor
		// +---------------------------
		GPthread(const GPthread& ref); //Not Implemented
};


#endif //GPGPEAD_H