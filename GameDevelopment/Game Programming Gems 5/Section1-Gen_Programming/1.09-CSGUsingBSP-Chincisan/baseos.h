/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#ifndef __BASEOS_H__
#define __BASEOS_H__

//---------------------------------------------------------------------------------------
#pragma pack(push, 8)

//---------------------------------------------------------------------------------------
// Defines for particular compilers
#ifdef WIN32
   #ifdef __GNUC__
       #define MG_WIN32              // for ming compiler
   #else    
       #define MS_WIN32              // for ms compiler
   #endif//
   #include <windows.h>
#else
   #define _LINUX

typedef unsigned long  DWORD;
typedef int            BOOL;
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef char*          LPSTR;
typedef const char*    LPCSTR;
typedef int            SOCKET;
typedef void*          HANDLE;
   #define FALSE           0
   #define TRUE            1
#endif

//---------------------------------------------------------------------------------------
#ifdef WIN32
	#pragma warning (disable: 4786)
    #pragma warning (disable: 4006)
#endif

#define _XIOSBASE_
   #define _XLOCNUM_
   #define _STREAMBUF_
   #define _IOS_
   #define _OSTREAM_
   #define _ISTREAM_
   #define _IOSTREAM_

//---------------------------------------------------------------------------------------
// stl
#include <iostream>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <stdio.h>
#include <string.h>
#include <cassert>
#include <stdlib.h>
#include <time.h>

//---------------------------------------------------------------------------------------
#ifndef WIN32
	#include <stdarg.h>
	#include <time.h>
#endif //

//---------------------------------------------------------------------------------------
using namespace std;

//---------------------------------------------------------------------------------------
#ifdef _BDL_PREC
    typedef	double REAL;
#else
    typedef	float REAL;
	#pragma warning(disable: 4305)
#endif //


//---------------------------------------------------------------------------------------
// dll exporting 
#ifdef _USRDLL 
	#define EX_IN __declspec(dllexport) 
#else
	#define EX_IN __declspec(dllimport) 
#endif

//---------------------------------------------------------------------------------------
// compiler section
#ifdef MS_WIN32 // microsoft win32
    #define INLN        __forceinline
    #define REG
    #define NO_VT       __declspec(novtable)
    #define VIRT
    #define NAKED_      __declspec( naked )
    #define ASM_        __asm
    #define TIME()		GetTickCount()
    #define SLEEP(i_)   Sleep(i_)
    #define nvtclass    class __declspec(novtable)
	#define OUT_
#else 
    #define INLN        inline
    #define REG         register
    #define NO_VT   
    #define VIRT
    #define NAKED_  
    #define ASM_        asm
    #define nvtclass    class 
	#define OUT_		
INLN DWORD TIME()       {time_t   t; time(&t);return t*1000; }

//---------------------------------------------------------------------------------------
INLN void Sleep(int i)   {;}
    #define max(a_,b_)  ((a_)>=(b_) ? (a_) : (b_))
    #define min(a_,b_)  ((a_)> (b_) ? (b_) : (a_))
#endif

//---------------------------------------------------------------------------------------
// helper macros
#define VIRT            // dummy macro for virtual function
#define ABSTRACT        // dummy macro for abstract classes
#define STATIC          // dummy macro for static funtions
#define THRFN           // dummy macro for function on TLS

//---------------------------------------------------------------------------------------
// assertion
#ifdef WIN32
#ifndef _ASSERT
    #define _ASSERT(__e)    __e ? (void*)0 : DebugBreak()
#endif 

#ifndef ASSERT
    #define ASSERT(__e)    __e ? (void*)0 : DebugBreak()
#endif 

#ifndef _ASSERTE
    #define _ASSERTE(__e)    __e ? (void*)0 : DebugBreak()
#endif
#endif//WIN32

#pragma pack(pop)

#endif //__BASEOS_H__


