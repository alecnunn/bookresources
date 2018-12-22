#ifndef	MICROTHREAD_H
#define	MICROTHREAD_H

#include "core/core.h"

// I want to have exception handling disabled, and micro threads practically
// require it. The STL expects exceptions to be enabled, and complains if
// they are disabled. This will only cause a problem if something happens
// that would trigger an exception - in which case I'm SOL anyway.
// The only thing that should throw an exception is out of memory, and I
// don't plan to run out of memory.
#pragma warning(disable : 4530)

namespace MicroThread
{
	// Low level micro-thread functions and types:
	class MicroThreadInfo;

	// Micro thread calling convention
	#define	MICROTHREADCALL	__cdecl
	// Function signature for micro thread functions.
	typedef void (MICROTHREADCALL *tMicroThreadFunction)(void* pData);

	// Create a micro thread, ready for scheduling with SwitchToMicroThread().
	MicroThreadInfo* CreateMicroThread(const tMicroThreadFunction scriptFunction, void* const pData = 0);
	// SwitchToMicroThread returns true if the thread is still running, false
	// if the thread ran to completion, in which case IsDead() will now
	// return true.
	bool SwitchToMicroThread(MicroThreadInfo* pMicroThread);
	// Threads can be deleted at any time, except when they are running - i.e;
	// a micro thread cannot delete itself. They should be deleted once they
	// have died, to free up resources.
	void DeleteMicroThread(MicroThreadInfo* pMicroThread);
	// This function can be called from within a running micro thread to get
	// a pointer to the threads information block. It will return zero at all
	// other times.
	MicroThreadInfo* GetActiveMicroThread();

	// Functions to call from within micro threads:
	// MicroThreadSleep() yields control to the main thread.
	void MicroThreadSleep();
	// MicroThreadSleep(int Frames) simply calls MicroThreadSleep()
	// Frames times.
	void MicroThreadSleep(int Frames);
	// You can use this to return from scripts, or you can just return
	// from the initial function.
	void MicroThreadReturn();

	class MicroThreadInfo
	{
	public:
		MicroThreadInfo()
			: m_dead(false)
		{
		}
		// Mark a thread as dead if you want it to no longer be run, even
		// if you call SwitchToMicroThread().
		virtual void MarkAsDead();
		// Find out how many bytes of stack are used by this thread.
		// Note: you cannot call this function on a thread that is
		// currently running - well, you can but it will just return
		// zero.
		virtual size_t MicroThreadStackSize() const = 0;
		// Is the thread dead? If so, you might as well delete it because
		// it won't ever get run.
		bool IsDead() const {return m_dead;}

	protected:
		// The virtual destructor must be protected rather than private, otherwise
		// the destructor in the derived class will not be called.
		// The destructor is not public because MicroThreadInfo
		// cannot be deleted, it must be destroyed with DestroyScript.
		// You can destroy a script at any time, except when it is actually
		// running - i.e.; a script may not destroy itself.
		virtual ~MicroThreadInfo()
		{
		}
	private:
		MicroThreadInfo& operator=(const MicroThreadInfo&);	// Not implemented.
		MicroThreadInfo(const MicroThreadInfo&);	// Not implemented.
		bool	m_dead;
		friend void DeleteMicroThread(MicroThreadInfo* pThread);
	};
};	// End MicroThread namespace



namespace MicroThreadManager
{

	// Micro-thread management functions and types - optional.
	class ManagedMicroThreadInfo;

	// Function to create a micro thread. Threads created with this function
	// must be destroyed with DeleteManagedMicroThread.
	ManagedMicroThreadInfo* CreateManagedMicroThread(MicroThread::tMicroThreadFunction MicroThreadFunction, void* pData = 0);
	// Function to give a time slice to all managed micro-threads.
	void RunManagedMicroThreads(const bool deleteDeadMicroThreads = true);
	void DeleteManagedMicroThread(ManagedMicroThreadInfo* pManagedThread);
	// Warning - do not delete the thread returned by this function!!!
	// However you may schedule it.
	MicroThread::MicroThreadInfo* pGetMicroThreadInfo(ManagedMicroThreadInfo* pManagedThread);

	void DeleteAllManagedMicroThreads();
	int GetNumManagedMicroThreads();
	size_t ManagedMicroThreadStackSize();

}; // End MicroThreadManager namespace

// WARNING: For mysterious reasons that I don't entirely understand, if you try to
// start the micro-threads you can run into troubles on NT when not running
// under a debugger. As soon as you call OutputDebugString with ESP set to a
// non-standard location - BOOM! Your application exits. Luckily it works fine
// under a debugger, so the trick is to not call OutputDebugString when not running
// under the debugger. I automatically detect this (if you use dprintf) so this works
// quite well. The only reason you might need to call OutputDebugString when not
// under the debugger is to send text to DBWin32, which traps OutputDebugString on
// NT, so I made dprintf() talk directly to DBWin32.
//
// So, be sure not to use OutputDebugString - use dprintf instead. That's not so
// bad - dprintf is more powerful and more portable.

#endif	// SCRIPTDATA_H
