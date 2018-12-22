#ifndef _SHARED_NETWORK_PLATFORM_H
#define _SHARED_NETWORK_PLATFORM_H

#ifdef WIN32
#include "win32/NetworkPlatform.h"
#endif

#ifdef LINUX
#include "linux/NetworkPlatform.h"
#endif
 
#endif // _SHARED_NETWORK_PLATFORM_H
