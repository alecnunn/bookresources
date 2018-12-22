// (c) Kenny Mitchell - Westwood Studios EA 2002

#ifndef _MY_DEBUG_H
#define _MY_DEBUG_H

#include <d3d8.h>
#include <stdio.h>

// debug failure handling
inline void Fail_Message(char* msg, char* file, int line)
{
	char str[255];
	sprintf(str,"%s in %s at line %d\n", msg, file, line);
	OutputDebugString(str);
	DebugBreak();
}
#define FAILMSG(msg) if (FAILED(result)) { Fail_Message(msg,__FILE__,__LINE__); }


#endif