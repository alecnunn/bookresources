/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/
#ifndef __NVUTIL_H

#include "nvinc.h"

namespace nv_objects
{


// Lifted from MS presentation....

//
// Usage: SetThreadName (-1, “MainThread”);
//
typedef struct tagTHREADNAME_INFO
{
	DWORD	dwType; 	// must be 0x1000
	LPCSTR	szName;	// pointer to name (in user addr space)
	DWORD	dwThreadID;	// thread ID (-1=caller thread)
	DWORD	dwFlags;	// reserved for future use, must be zero
} THREADNAME_INFO;


class NVUtil
{
public:
	// Give the current thread a useful name so that the debugger makes it clear which is which
	static void SetThreadName( DWORD dwThreadID, LPCSTR szThreadName)
	{
		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = szThreadName;
		info.dwThreadID = dwThreadID;
		info.dwFlags = 0;

		__try 
		{
			RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info );
		}
		__except(EXCEPTION_CONTINUE_EXECUTION)
		{
		
		}
	}
};

//
// Avoid the annoying stack that comes with DebugBreak()
// Usage: if (b) { DebugBreak(); }
//
#ifdef _X86
	#define Break() do {	\
		asm int 3		\
	} while (0)
#else // _X86
#define Break() do {	\
DebugBreak();	\
} while (0)
#endif // _X86

//
// Use RTTI in debug builds to verify pointers to abstract types
// Use instead of static_cast<>()
//
// Usage:
//	CBar* pBar = checked_cast<CBar*> (pFoo);
//

template <class TypeFrom, class TypeTo>
TypeTo* checked_cast (TypeFrom* p) {
	ASSERT (dynamic_cast<TypeTo*> (p));
	return static_cast<TypeTo*> (p);
}

// 
// Verify that a pointer to a COM object is still valid
//
// Usage: 
//	VERIFY_COM_INTEFACE (pFoo);
//

template <class Q> 
void VERIFY_COM_INTERFACE (Q* p) {

#ifdef DEBUG
	p->AddRef();
	p->Release();
#endif

}

//
// Verify that a BSTR really is a BSTR and is handled correctly
// Warning! Fails on BSTRs which contain embedded nuls
// Usage: 
//	VERIFY_BSTR (bstrName)
//

#ifdef _DEBUG
	#define VERIFY_BSTR(bstr) do { \
		ASSERT (SysStringLen (bstr) == wcslen (bstr); \
	} while (0)
#else
	#define VEIFY_BSTR(bstr) do { } while (0)
#endif


// 
// A variation on ASSERT() with some additional functionality. Useful in paint
// code Set __ignore_assert=1 to ignore
// Usage: 
//	ASSERT (n != 7);
//

#ifndef ASSERT
#ifdef _DEBUG
#define ASSERT(exp) do { \
	static bool __ignore_assert = false; \
	if (!exp && !__ignore_assert) { \
		char sz[127]; \
		sprintf (_sz,_ \
			__FILE__ "("__%d__"): Assert Failed: %s", \
			__LINE__, #exp); \
		if (DebuggerPresent()) {\
			OutputDebugString (sz); \
			Break() \
		} else {\
			if (MessageBox (NULL, "Break To Debugger?", sz, MB_YESNO)) {\
				Break(); \
				OutputDebugString (sz); \
			} \
		} \
	} \
} while (0)
#else // DEBUG
#define ASSERT(exp) do {} while (0)
#endif // DEBUG
#endif // ASSERT

//
// Usage:
// 	verify_write_ptr (pFoo);
//

template <class Q> 
void
verify_write_ptr (Q* p) 
{
	ASSERT(!IsBadWritePtr (p, sizeof (Q)));
}		


}; // namespace nv_objects

#endif	// __NVUTIL_H
  