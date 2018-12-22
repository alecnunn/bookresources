
// GFTypes.h

#ifndef GFTYPES_H
#define GFTYPES_H

#pragma warning( disable : 4786 )

#include <vector>
#include <list>
#include <map>
#include <string>
#include <deque>

typedef std::string String_t;

#define GF_ENABLE_LOGCALLS 1
#define GF_ENABLE_ERRORCALLS 1
#define GF_ENABLE_ASSERTCALLS 1
#define GF_ENABLE_HALTCALLS 1

#if GF_ENABLE_LOGCALLS == 1
#include "GFInclude/Systems/LogSys_t.h"
#define LOGCALL(x) x
#else
#define LOGCALL(x)
#endif

#if GF_ENABLE_ERRORCALLS == 1
#include "GFInclude/Systems/ErrorSys_t.h"
#define ERRORCALL(x) x
#else
#define ERRORCALL(x)
#endif

#if GF_ENABLE_ASSERTCALLS == 1
#include "GFInclude/Systems/ErrorSys_t.h"
#define ASSERTCALL(x) x
#else
#define ASSERTCALL(x)
#endif

#if GF_ENABLE_HALTCALLS == 1
#include "GFInclude/Systems/ErrorSys_t.h"
#define HALTCALL(x) x
#else
#define HALTCALL(x)
#endif

// define this to 1 if we want to check for memory leaks
#define GFX_MEMORY_CHECKER 0

#if GFX_MEMORY_CHECKER == 1

#define _CRTDBG_MAP_ALLOC 
#include <stdlib.h> 
#include <crtdbg.h> 

#ifdef _DEBUG 
#define MYDEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__) 
#define DEBUG_MALLOC(size) _malloc_dbg(size, _NORMAL_BLOCK, __FILE__, __LINE__) 
#define new MYDEBUG_NEW 
#define malloc DEBUG_MALLOC 
#endif // _DEBUG 

// 2. Call the following function at the very beginnning of the excutable function (e.g. _tWinMain). 
//_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); 
// 2a. and call the following function at the very last of the excution. 
//_CrtDumpMemoryLeaks(); 

#endif // GFX_MEMORY_CHECKER

#endif // GFTYPES_H