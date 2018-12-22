/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogicLayerObj_GameLose_t.h: interface for the LogicLayerObj_GameLose_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGICLAYEROBJ_GAMELOSE_T_H__8E96CDDD_19FE_43EE_80B9_072BBC216023__INCLUDED_)
#define AFX_LOGICLAYEROBJ_GAMELOSE_T_H__8E96CDDD_19FE_43EE_80B9_072BBC216023__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/LogicLayer_t.h"
class Systems_t;
class AVLayerObj_GameLose_t;

class LogicLayerObj_GameLose_t : public LogicLayer_t  
{
public:
	LogicLayerObj_GameLose_t();
	virtual ~LogicLayerObj_GameLose_t();

	virtual bool Connect( Systems_t *pSystems );
	virtual void Disconnect();

	virtual void Update();

	virtual void SetExclusive( bool bEnable );
	virtual bool IsExclusive();
protected:
	Systems_t *m_pSystems;
	AVLayerObj_GameLose_t *m_pAVLayer;
	bool m_bExclusive;
};

#endif // !defined(AFX_LOGICLAYEROBJ_GAMELOSE_T_H__8E96CDDD_19FE_43EE_80B9_072BBC216023__INCLUDED_)
