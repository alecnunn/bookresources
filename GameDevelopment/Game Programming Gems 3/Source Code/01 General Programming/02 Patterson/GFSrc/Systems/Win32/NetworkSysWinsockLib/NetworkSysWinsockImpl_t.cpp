/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// NetworkSysWinsockImpl_t.cpp: implementation of the NetworkSysWinsockImpl_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GFInclude/Systems/Win32/NetworkSysWinsockImpl_t.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NetworkSysWinsockImpl_t::NetworkSysWinsockImpl_t()
{
	m_pSystems = 0;
}

NetworkSysWinsockImpl_t::~NetworkSysWinsockImpl_t()
{

}

bool NetworkSysWinsockImpl_t::Init( Systems_t *pSystems )
{
	m_pSystems = pSystems;
	return true;
}

void NetworkSysWinsockImpl_t::Shutdown()
{
}
