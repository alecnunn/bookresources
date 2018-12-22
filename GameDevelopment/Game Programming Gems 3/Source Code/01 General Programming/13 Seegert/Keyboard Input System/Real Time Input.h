#ifndef __REAL_TIME_INPUT_
#define __REAL_TIME_INPUT_

/* disable some warnings */
#pragma warning (disable : 4786)

/* windows headers */
#include <windows.h>
#include <direct.h>

/* directx headers */
#include <dinput.h>

/* stl headers */
#include <algorithm>
#include <map>
#include <string>
#include <vector>

/* resource headers */
#include "String Tables\resource.h"

/* namespaces */
using namespace std;

/* type definitions */
typedef unsigned long	uint32;
typedef unsigned short	uint16;
typedef unsigned char	uint8;
typedef long			int32;
typedef short			int16;
typedef char			int8;

/* enumerations */
typedef uint8			ISModifierState;
enum
{
	kismsNone	= 0x00,
	kismsCtrl	= 0x01,
	kismsAlt	= 0x02,
	kismsShift	= 0x04
};


#endif /*__REAL_TIME_INPUT_*/
