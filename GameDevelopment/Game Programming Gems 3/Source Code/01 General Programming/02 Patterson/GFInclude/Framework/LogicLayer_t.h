/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogicLayer_t.h: interface for the LogicLayer_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGICLAYER_T_H__C22005EF_ED9A_4889_9D58_CED36C0A617F__INCLUDED_)
#define AFX_LOGICLAYER_T_H__C22005EF_ED9A_4889_9D58_CED36C0A617F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class LogicLayer_t  
{
public:
	LogicLayer_t() {};
	virtual ~LogicLayer_t() {};

	virtual bool Connect( Systems_t *pSystems ) = 0;
	virtual void Disconnect() = 0;

	virtual void Update() = 0;

	virtual void SetExclusive( bool bEnable ) = 0;
	virtual bool IsExclusive() = 0;
};

typedef std::list<LogicLayer_t *> LogicLayerPtrList_t;

#endif // !defined(AFX_LOGICLAYER_T_H__C22005EF_ED9A_4889_9D58_CED36C0A617F__INCLUDED_)
