/*
 * Main include file
 */

#ifndef _MAIN_H
#define _MAIN_H

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <windowsx.h>
#include <winsock.h>
#include <commctrl.h>

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#pragma warning(disable:4786) // identifier truncated to 255 characters

#include <set>
#include <map>
#include <string>
#include <list>
#include <vector>
#include <deque>

using namespace std;

typedef int            int32;
typedef unsigned int   uint32;
typedef short          int16;
typedef unsigned short uint16;
typedef unsigned char  byte;
typedef unsigned int   uint;
typedef _int64         int64;

// Custom window messages
const uint32 WM_SOCKET_NOTIFY = WM_APP;

#include "WinUtil.h"

#include "Timer.h"

#include "Debug.h"
#include "Config.h"
#include "Resource.h"

#include "Action.h"
#include "Message.h"
#include "Endpoint.h"
#include "Host.h"

#include "Socket.h"
#include "Filter.h"
#include "Network.h"
#include "WinsockError.h"
#include "UDPSocket.h"
#include "TCPSocket.h"
#include "ListenSocket.h"
#include "UDPFilter.h"
#include "TCPFilter.h"

#include "WindowResizer.h"
#include "ListView.h"
#include "Interface.h"


struct app_type
{
   Debug     *debug;
   Config    *config;
   Interface *inter;
   Network   *network;
   CTimer    *timer;
};

extern app_type *app;

#endif
