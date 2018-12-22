#include "priv_coreprecompiled.h"

/*
Copyright 2001 Bruce Dawson, Cygnus Software
For more information, visit http://www.cygnus-software.com/papers/
or e-mail: comments@cygnus-software.com

This code may be redistributed as long as this notice is kept intact.
*/

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>

// DBWin32 has a limit of about 500 characters that you can copy over at one time.
// OutputDebugString has a larger limit - around 1024 characters - but we limit
// ourselves to the DBWin32 limit.
#define	MAXPRINTSIZE	500

typedef BOOL	(__stdcall * tIsDebuggerPresent)();
typedef BOOL	(WINAPI *tReadProcessMemory)(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, DWORD nSize, LPDWORD lpNumberOfBytesRead);

// Return true if running under a debugger.
bool UniversalIsDebuggerPresent()
{
	// We might as well cache the addresses of Kernel32 and
	// IsDebuggerPresent, to make this function perform more
	// efficiently. However, we should not cache the results
	// of IsDebuggerPresent(), because that may change if the
	// user attaches to the process after it starts.
	static HINSTANCE Kernel32 = LoadLibrary("kernel32.dll");
	static tIsDebuggerPresent pIsDebuggerPresent = (tIsDebuggerPresent)GetProcAddress(Kernel32, "IsDebuggerPresent");
	assert(Kernel32);

	bool Result = false;

	if (pIsDebuggerPresent)
	{
		if (pIsDebuggerPresent())
			Result = true;
	}
	else
	{
		// This is a quirky, undocumented and unguaranteed method of identifying that we are running under
		// a debugger under Windows 95. ReadProcessMemory, as a member of Kernel32, should always be
		// contained within it, which means that it should always have an address higher than it. However,
		// when running under a debugger on Win95 it _doesn't_. I don't know why, but isn't that
		// convenient!
		// How did I discover this? It's a long story. It turns out it has been separately documented,
		// by Matt Pietrek and probably others.
		// Since Win98 and WinNT both have the IsDebuggerPresent() function, this hacked technique only
		// needs to work on Win95 - which it does. Bruce, June 1998.
		// There is also a variable in the task information block, accessible off of the FS segment,
		// which can be used to get the same information.
		tReadProcessMemory pReadProcessMemory = (tReadProcessMemory)GetProcAddress(Kernel32, "ReadProcessMemory");
		if ((void *)pReadProcessMemory < (void *)Kernel32)	//lint !e611	Ignore suspicious cast.
			Result = true;
	}

	return Result;
}

// Simple class to hold HANDLEs and automatically close them at the end of scope.
class AutoHandle
{
public:
	AutoHandle(HANDLE handle)
		: m_handle(handle)
	{
	}
	~AutoHandle()
	{
		if (m_handle)
			CloseHandle(m_handle);
	}
	operator HANDLE()
	{
		return m_handle;
	}
private:
	HANDLE	m_handle;
	AutoHandle& operator=(const AutoHandle&);	// Unimplemented.
	AutoHandle(const AutoHandle&);				// Unimplemented.
};

// This is a replacement for OutputDebugString that also sends messages to
// DBWin32 under Win9x as well as NT, and avoids calling OutputDebugString
// when running under the debugger.
void OutputDebugStringW95( const char* achBuffer)
{
	// DO NOT DO ANY ASSERTS OR C run time calls in this code - it has to
	// be safe to call this from ANYWHERE!!!

	/* 
	Do a regular OutputDebugString so that the output is 
	still seen in the debugger window when running under
	the debugger.

	Don't call OutputDebugString when not running under
	the debugger, because that fails inside of microthreads.
	*/
	if (UniversalIsDebuggerPresent())
	{
		// Use the explicit names so that if a project wants to
		// globally #define OutputDebugString() to point at
		// OutputDebugStringW95 there won't be any recursion
		// here.
		#ifdef _UNICODE
		::OutputDebugStringW(achBuffer);
		#else
		::OutputDebugStringA(achBuffer);
		#endif

		// The normal OutputDebugString behaviour on WinNT is to go either
		// to the debugger or to DBWin32. Not both. We will get that behaviour
		// if we do a return here, in the NT case. However, it is actually
		// very cool to have the output going to both DBWin32 and the debugger,
		// so I don't do a return.
	}

	/* See if DBWIN is open and waiting */
	AutoHandle heventDBWIN = OpenEvent(EVENT_MODIFY_STATE, FALSE, "DBWIN_BUFFER_READY");
	if ( !heventDBWIN )
		return;			

	/* get a handle to the data synch object */
	AutoHandle heventData = OpenEvent(EVENT_MODIFY_STATE, FALSE, "DBWIN_DATA_READY");
	if ( !heventData )
		return;			

	// Open the shared memory buffer.
	AutoHandle hSharedFile = CreateFileMapping((HANDLE)-1, NULL, PAGE_READWRITE, 0, 4096, "DBWIN_BUFFER");
	if (!hSharedFile) 
		return;

	LPSTR lpszSharedMem = (LPSTR)MapViewOfFile(hSharedFile, FILE_MAP_WRITE, 0, 0, MAXPRINTSIZE);
	if (!lpszSharedMem) 
		return;

	/* wait for buffer event */
	(void)WaitForSingleObject(heventDBWIN, INFINITE);

	/* write it to the shared memory */
	*((int *)lpszSharedMem) = _getpid();
	// Use lstrcpyn because it is safer - it won't overwrite the buffer.
	lstrcpyn(lpszSharedMem + sizeof(DWORD), achBuffer, MAXPRINTSIZE);

	/* signal data ready event */
	(void)SetEvent(heventData);
}

// Get the 'time' in seconds, with unspecified precision that should be at least
// one millisecond. The start time is unspecified.
double GetSeconds()
{
	// On some (old) versions of Windows NT, multiple CPUs are not properly
	// handled in regards to keeping timers consistent.  However I believe
	// that this is no longer a problem, so we should be able to use
	// QueryPerformanceCounter() on machines that report that it works.
	static LARGE_INTEGER	iFrequency;
	static BOOL FreqResult = QueryPerformanceFrequency(&iFrequency);
	LARGE_INTEGER			iTime;
	BOOL CounterResult = QueryPerformanceCounter(&iTime);
	if ((FreqResult == TRUE) && (CounterResult == TRUE))
	{
		double Time = (double)iTime.QuadPart / (double)iFrequency.QuadPart;
		return Time;
	}

	// It is not guaranteed that QueryPerformanceCounter() will work on all
	// machines. Therefore we need this fallback code. It will probably
	// never be used.
	
	// The problem with these functions is that they wrap around after 49.7 days.  Bummer.
	//	return timeGetTime() / 1000.0;

	SYSTEMTIME	SysTime;
	GetSystemTime(&SysTime);
	FILETIME	FileTime;
	(void)SystemTimeToFileTime(&SysTime, &FileTime);	// Ignoring return value.
	double Result = (FileTime.dwHighDateTime * 65536.0 * 65536.0 + FileTime.dwLowDateTime) / 10000000.0;
	return Result;
}

// Do a debug print.
void rdprintv(const char *format, va_list ap)
{
	char tempstr[8000] = "";
	// I subtract one from the buffer length to ensure proper behaviour on
	// the boundary case.
	if (_vsnprintf(tempstr, sizeof(tempstr) - 1, format,ap) < 0) 
	{
		// would have exceeded max
		tempstr[ sizeof(tempstr) - 2 ] = '\n'; // force a new-line
	}
	// BUG! _vsnprintf() does not always null terminate the output. If the number
	// of characters printed, not including the terminating null, exactly fits in
	// the buffer, it returns the 'success' code (positive) even though a badly
	// formed string is in the buffer. Yucch.  Bruce, Feb 1998.
	tempstr[ sizeof(tempstr) - 1 ] = '\0'; // force a terminating null
	/*
	BUGBUG
	Code to create a longer string for test purposes.
	for (int i = 0; i < 20; ++i)
	{
		sprintf(tempstr + strlen(tempstr), "%d ", i);
		strcat(tempstr, "This is just a test of a reasonably long string - wahoo!\n");
	}*/
	// Print in small pieces, to avoid hitting the DBWin32 or OutputDebugString
	// limitations.
	char *outputstring = tempstr;
	while (outputstring[0])
	{
		char	copy[MAXPRINTSIZE];
		lstrcpyn(copy, outputstring, MAXPRINTSIZE);
		OutputDebugStringW95(copy);
		outputstring += strlen(copy);
	}
}

void rdprintf(const char* format, ...)
{
	va_list ap;

	va_start(ap,format);
	rdprintv(format, ap);
	va_end(ap);
}
