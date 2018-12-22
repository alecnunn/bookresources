/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogicLayerCommand_t.h: interface for the LogicLayerCommand_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGICLAYERCOMMAND_T_H__AA5D9677_E242_47D1_93D5_0B6A91AF4C8F__INCLUDED_)
#define AFX_LOGICLAYERCOMMAND_T_H__AA5D9677_E242_47D1_93D5_0B6A91AF4C8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class LogicLayer_t;

class LogicLayerCommand_t  
{
public:
	enum LogicLayerCommandID_t {
		INVALID,
		PUSH_BACK,
		PUSH_FRONT,
		REMOVE,
		REMOVE_ALL,
	};

	LogicLayerCommand_t( LogicLayerCommandID_t nID, LogicLayer_t *pLogicLayer )
	{
		m_nID = nID;
		m_pLogicLayer = pLogicLayer;
	}
	virtual ~LogicLayerCommand_t() {}

	LogicLayerCommandID_t m_nID;
	LogicLayer_t *m_pLogicLayer;
};

typedef std::vector<LogicLayerCommand_t> LogicLayerCommandVector_t;

#endif // !defined(AFX_LOGICLAYERCOMMAND_T_H__AA5D9677_E242_47D1_93D5_0B6A91AF4C8F__INCLUDED_)
