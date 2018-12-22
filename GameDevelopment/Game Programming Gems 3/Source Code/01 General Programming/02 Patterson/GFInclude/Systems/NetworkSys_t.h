/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// NetworkSys_t.h: interface for the NetworkSys_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORKSYS_T_H__5A3258AF_5E97_47B7_9C1F_9D5DEEFCD193__INCLUDED_)
#define AFX_NETWORKSYS_T_H__5A3258AF_5E97_47B7_9C1F_9D5DEEFCD193__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Systems_t;

class NetworkSys_t  
{
public:
	NetworkSys_t() {};
	virtual ~NetworkSys_t() {};

	virtual bool Init( Systems_t *pSystems ) = 0;
	virtual void Shutdown() = 0;
};

#endif // !defined(AFX_NETWORKSYS_T_H__5A3258AF_5E97_47B7_9C1F_9D5DEEFCD193__INCLUDED_)
