/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// FrameSys_t.h: interface for the FrameSys_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRAMESYS_T_H__E0CEDCFA_2D52_4AF7_B769_30F7CD4A2549__INCLUDED_)
#define AFX_FRAMESYS_T_H__E0CEDCFA_2D52_4AF7_B769_30F7CD4A2549__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Systems_t;

class FrameSys_t  
{
public:
	FrameSys_t() {};
	virtual ~FrameSys_t() {};

	virtual bool Init( Systems_t *pSystems ) = 0;
	virtual void Shutdown() = 0;

	virtual bool StartFrameSync() = 0;
	virtual void EndFrameSync() = 0;
	virtual bool StartAsync() = 0;
	virtual void EndAsync() = 0;
};

#endif // !defined(AFX_FRAMESYS_T_H__E0CEDCFA_2D52_4AF7_B769_30F7CD4A2549__INCLUDED_)
