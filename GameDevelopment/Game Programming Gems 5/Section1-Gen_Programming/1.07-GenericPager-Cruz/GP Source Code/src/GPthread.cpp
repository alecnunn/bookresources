// +-----------------------------------------------------------------------
// |
// |                         File Name: GPthread.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPthread Class Implementation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------
#include <GPpch.h>
#include <GPthread.h>

// +---------------------------
// |	Constructor & Destructor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPthread::GPthread
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPthread::GPthread( GPthreadFuncPtr pFun, void* pArg):
handle_ ( 0 ),
id_			(-1)
{
  handle_ = CreateThread 
							(
								0,
								0,
								pFun,
								pArg,
								CREATE_SUSPENDED,
								&id_
							); 
}

// +-------------------------------------------------------------
// |
// | Function        : GPthread::~GPthread
// | Description     : Destroys the Win32 kernel object
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPthread::~GPthread()
{ 
	CloseHandle( handle_ );
}

// +---------------------------
// |	Handle & Identifier
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPthread::GetHandle
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
HANDLE GPthread::GetHandle() const
{
	return handle_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPthread::GetIdentifier
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
DWORD  GPthread::GetIdentifier() const
{
	return id_;
}

// +---------------------------
// |	Priorioty
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPthread::SetPriority
// | Description     : Change priority of the Win32 thread
// |
// | Parameters : 
// |   priority      : can be one of the following:
// |                   THREAD_PRIORITY_ABOVE_NORMAL, 
// |                   THREAD_PRIORITY_BELOW_NORMAL,
// |                   THREAD_PRIORITY_HIGHEST
// |                   THREAD_PRIORITY_IDLE 
// |                   THREAD_PRIORITY_LOWEST
// |                   THREAD_PRIORITY_NORMAL
// |                   THREAD_PRIORITY_TIME_CRITICAL 
// | 
// +-------------------------------------------------------------
void GPthread::SetPriority(int priority)
{
	SetThreadPriority(handle_, priority);
}

// +-------------------------------------------------------------
// |
// | Function        : GPthread::GetPriority
// | Description     : Return priority of the Win32 thread
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
int  GPthread::GetPriority() const
{
	return GetThreadPriority(handle_);
}

// +---------------------------
// |	Control
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPthread::Suspend
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPthread::Suspend()
{
  SuspendThread(handle_);
}

// +-------------------------------------------------------------
// |
// | Function        : GPthread::Resume
// | Description     : Start/Restart a suspended thread
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPthread::Resume()
{ 
	ResumeThread( handle_ );
}

// +-------------------------------------------------------------
// |
// | Function        : GPthread::WaitForFinish
// | Description     : 
// |
// | Parameters : 
// |   milliseconds  : 
// | 
// +-------------------------------------------------------------
void GPthread::WaitForFinish(DWORD milliseconds)
{
	WaitForSingleObject( handle_, milliseconds );
}

// +-------------------------------------------------------------
// |
// | Function        : GPthread::IsRunning
// | Description     : Return the running state of the thread
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
bool GPthread::IsRunning() const
{
	DWORD exitCode;
  GetExitCodeThread( handle_, &exitCode );
  return (exitCode == STILL_ACTIVE);
}


