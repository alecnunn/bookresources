/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// TimeSys_t.h: interface for the TimeSys_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMESYS_T_H__308732FE_E66F_4E61_889C_26529F32A993__INCLUDED_)
#define AFX_TIMESYS_T_H__308732FE_E66F_4E61_889C_26529F32A993__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Systems_t;

class TimeSys_t  
{
public:
	TimeSys_t() {};
	virtual ~TimeSys_t() {};

	virtual bool Init( Systems_t *pSystems ) = 0;
	virtual void Shutdown() = 0;
};

#endif // !defined(AFX_TIMESYS_T_H__308732FE_E66F_4E61_889C_26529F32A993__INCLUDED_)
