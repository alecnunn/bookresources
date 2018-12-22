/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\CommonSrc\SceneManagement
File:  IdleObj.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/


#if !defined(AFX_IDLEOBJ_H__A05D69E1_E6E9_11D1_9A27_006097777910__INCLUDED_)
#define AFX_IDLEOBJ_H__A05D69E1_E6E9_11D1_9A27_006097777910__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



class IdleObject
{
public:
	IdleObject();
	virtual ~IdleObject();


	virtual void OnIdle() {};

};

#endif // !defined(AFX_IDLEOBJ_H__A05D69E1_E6E9_11D1_9A27_006097777910__INCLUDED_)
