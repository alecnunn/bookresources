/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// NetworkSysWinsockImpl_t.h: interface for the NetworkSysWinsockImpl_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORKSYSWINSOCKIMPL_T_H__84D93B74_BB43_4680_96A4_4ED094D51E2A__INCLUDED_)
#define AFX_NETWORKSYSWINSOCKIMPL_T_H__84D93B74_BB43_4680_96A4_4ED094D51E2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Systems/NetworkSys_t.h"

class NetworkSysWinsockImpl_t : public NetworkSys_t  
{
public:
	NetworkSysWinsockImpl_t();
	virtual ~NetworkSysWinsockImpl_t();

	virtual bool Init( Systems_t *pSystems );
	virtual void Shutdown();

protected:
	Systems_t *m_pSystems;
};

#endif // !defined(AFX_NETWORKSYSWINSOCKIMPL_T_H__84D93B74_BB43_4680_96A4_4ED094D51E2A__INCLUDED_)
