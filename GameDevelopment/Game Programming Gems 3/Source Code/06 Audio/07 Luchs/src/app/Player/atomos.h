#ifndef _atomos_
#define _atomos_

// interface definitions for base library
#include "..\\..\\lib\\atombase\\atombase_interfaces.h"

// interface definitions for plugins to use:
//#include "atomload_interfaces.h"


extern void InitializeAtomos();
extern atomos::IEngine* g_pEngine;



#endif // _atomos_