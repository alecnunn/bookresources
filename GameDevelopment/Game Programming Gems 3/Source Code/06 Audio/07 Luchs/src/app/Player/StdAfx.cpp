// stdafx.cpp 
#include "stdafx.h"


#define SPHINXMMOS

#ifdef SPHINXMMOS
// link the atombase library for SphinxMMOS 
#ifdef _DEBUG
#	pragma message("library is linking with atombased.lib")
#	pragma comment(lib, "..\\..\\..\\bin\\atombased.lib")
#else
#	pragma message("library is linking with atombase.lib")
#	pragma comment(lib, "..\\..\\..\\bin\\atombase.lib")
#endif
#else
// link the atombase library for ATOMOS and SPHINX MM 
#ifdef _DEBUG
#	pragma message("library is linking with atombased.lib")
#	pragma comment(lib, "atombased.lib")
#else
#	pragma message("library is linking with atombase.lib")
#	pragma comment(lib, "atombase.lib")
#endif
#endif // SPHINXMMOS