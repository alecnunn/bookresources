// stdafx 
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0401
// Author:		Frank Luchs
// History:
//				1996-05-07	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

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