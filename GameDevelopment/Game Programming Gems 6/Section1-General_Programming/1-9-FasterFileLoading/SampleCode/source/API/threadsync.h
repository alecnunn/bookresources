#ifndef _THREAD_SYNC_H_
#define _THREAD_SYNC_H_

#ifdef _MSC_VER
	#include "threadsync_win.h"
#endif //WIN32

#ifdef LINUX
	#include "threadsync_linux.h"
#endif //LINUX


#endif