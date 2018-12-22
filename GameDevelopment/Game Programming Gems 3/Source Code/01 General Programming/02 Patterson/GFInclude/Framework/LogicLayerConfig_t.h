/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// LogicLayerConfig_t.h: interface for the LogicLayerConfig_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGICLAYERCONFIG_T_H__9CE42460_3180_46B2_A339_12B753E3868B__INCLUDED_)
#define AFX_LOGICLAYERCONFIG_T_H__9CE42460_3180_46B2_A339_12B753E3868B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class LogicLayer_t;

class LogicLayerConfig_t  
{
public:
	LogicLayerConfig_t() {};
	virtual ~LogicLayerConfig_t() {};

	virtual void Configure( LogicLayer_t *pLogicLayer ) = 0;
};

#endif // !defined(AFX_LOGICLAYERCONFIG_T_H__9CE42460_3180_46B2_A339_12B753E3868B__INCLUDED_)
