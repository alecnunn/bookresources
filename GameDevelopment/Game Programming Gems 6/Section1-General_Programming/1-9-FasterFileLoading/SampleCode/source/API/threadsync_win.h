#ifndef _THREAD_SYNC_WIN_H_
#define _THREAD_SYNC_WIN_H_

#include <windows.h>

class ThreadSync
{
public:
	ThreadSync(){ InitializeCriticalSection(&m_Lock); }
	~ThreadSync(){ DeleteCriticalSection(&m_Lock); }

	inline void Lock(){ EnterCriticalSection(&m_Lock);	}
	inline void UnLock(){ LeaveCriticalSection(&m_Lock); }

protected:
	CRITICAL_SECTION m_Lock;
};

#endif