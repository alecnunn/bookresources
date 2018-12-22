/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#ifndef __BASUTILS_H__
#define __BASUTILS_H__

#include "baseos.h"
#include "basemath.h"
#include "baseutils.h"
#include <direct.h>

//---------------------------------------------------------------------------------------
#ifdef _DEBUG
INLN void _cdecl TRACEX(LPCSTR lpszFormat, ...)
{
    static char sss[1024] = "";
    va_list args;
    va_start(args, lpszFormat);

    int nBuf;
    CHAR szBuffer[512];

    nBuf = _vsnprintf(szBuffer, sizeof(szBuffer) / sizeof(CHAR), lpszFormat, args);
    assert(nBuf < sizeof(szBuffer));//Output truncated as it was > sizeof(szBuffer)

    //if(strcmp(sss ,szBuffer))
    {
        OutputDebugStringA(szBuffer);
        strcpy(sss, szBuffer);
    }

    va_end(args);
}

#else // _DEBUG no debug here
INLN void _cdecl RTRACE(LPCSTR lpszFormat, ...)
{
    static char sss[1024] = "";
    va_list args;
    va_start(args, lpszFormat);

    int nBuf;
    CHAR szBuffer[512];

    nBuf = _vsnprintf(szBuffer, sizeof(szBuffer) / sizeof(CHAR), lpszFormat, args);
    assert(nBuf < sizeof(szBuffer));//Output truncated as it was > sizeof(szBuffer)

    //if(strcmp(sss ,szBuffer))
    {
        OutputDebugStringA(szBuffer);
        strcpy(sss, szBuffer);
    }

    va_end(args);
}

    #define MEM_START(_l_) 
    #define MEM_END()
    #define TIME_START()
    #define TIME_END(_text)

#ifdef MS_WIN32
	#define TRACEX	 (void*)0
	#define TRACEMAT (void*)0
	#define FFSTR    (void*)0
	#define FFSTR3   (void*)0
#else
	#define TRACEX	 //
	#define TRACEMAT //
	#define FFSTR    //
	#define FFSTR3   //
#endif //
#endif //_DEBUG


#define DECLARE_TIME()      static LARGE_INTEGER _Start;\
                            static LARGE_INTEGER _Freq;\
                            static LARGE_INTEGER _End;\
                            QueryPerformanceFrequency(&_Freq);\
                            QueryPerformanceCounter(&_Start)
#define WAIT_HERE(msec)     QueryPerformanceCounter(&_End);\
                            while( (_End.QuadPart - _Start.QuadPart)*1000 / _Freq.QuadPart < msec)\
							{\
								 QueryPerformanceCounter(&_End);\
								 Sleep(0);\
							}
#define TRACETIME(msec)     QueryPerformanceCounter(&_End);\
                            TRACEX("%s %d ms\r\n",msec ,(_End.QuadPart - _Start.QuadPart)*1000 / _Freq.QuadPart) 
#define READ_CLOCK()        QueryPerformanceCounter(&_Start)


INLN void stripcrlf(char* ln){
    while(ln[0] && (ln[strlen(ln)-1]=='\n' ||ln[strlen(ln)-1]=='\r'))
        ln[strlen(ln)-1]=0;
}

class Cfms
{
public:
    Cfms(char* p,...){
        va_list args;
        va_start(args, p);
        _vsnprintf(_s, sizeof(_s) / sizeof(char), p, args);
        va_end(args);
    }
    operator const char*(){return _s;}


    char _s[64];
};
#define	MKSTR	(const char*)Cfms

#endif //__BASUTILS_H__
