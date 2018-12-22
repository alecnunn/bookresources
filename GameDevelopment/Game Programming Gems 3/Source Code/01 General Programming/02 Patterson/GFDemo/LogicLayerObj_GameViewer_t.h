/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogicLayerObj_GameViewer_t.h: interface for the LogicLayerObj_GameViewer_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGICLAYEROBJ_GAMEVIEWER_T_H__42D47083_5062_4E7D_BD50_862DB2AD069E__INCLUDED_)
#define AFX_LOGICLAYEROBJ_GAMEVIEWER_T_H__42D47083_5062_4E7D_BD50_862DB2AD069E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\GFINCLUDE\FRAMEWORK\LogicLayer_t.h"
class Systems_t;
class AVLayerObj_GameViewer_t;

class LogicLayerObj_GameViewer_t : public LogicLayer_t  
{
public:
	LogicLayerObj_GameViewer_t();
	virtual ~LogicLayerObj_GameViewer_t();

	virtual bool Connect( Systems_t *pSystems );
	virtual void Disconnect();

	virtual void Update();

	virtual void SetExclusive( bool bEnable );
	virtual bool IsExclusive();
protected:
	Systems_t *m_pSystems;
	AVLayerObj_GameViewer_t *m_pAVLayer;
	bool m_bExclusive;
};

#endif // !defined(AFX_LOGICLAYEROBJ_GAMEVIEWER_T_H__42D47083_5062_4E7D_BD50_862DB2AD069E__INCLUDED_)
