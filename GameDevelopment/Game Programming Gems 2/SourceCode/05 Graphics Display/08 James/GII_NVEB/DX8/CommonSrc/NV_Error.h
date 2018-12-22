/*********************************************************************NVMH2****
Path:  C:\DEV\devrel\NV_SDK_4\DX8\CommonSrc
File:  NV_Error.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:
//	Greg James, 1996
//	Well, it's been modified a bit since then =)

******************************************************************************/


#ifndef	__MYERROR__
#define	__MYERROR__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


void DoError( char * str, bool quit );	// BOOL is quit/don't quit

void OkMsgBox( char * szCaption, char * szFormat, ... );

void FDebug( char * szFormat, ... );

#define NULLCHECK(q, msg,quit) {if(q==NULL) { DoError(msg, quit); }}
#define IFNULLRET(q, msg)	   {if(q==NULL) { FDebug(msg); return;}}

#define FAILRET(hres, msg) {if(FAILED(hres)){FDebug("*** %s   HRESULT: %d\n",msg, hres);return hres;}}
#define HRESCHECK(q, msg)	 {if(FAILED(q)) { FDebug(msg); return;}}

#endif
