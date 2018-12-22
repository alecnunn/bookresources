/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AVLayerConfigObj_SystemConfig_t.h: interface for the AVLayerConfigObj_SystemConfig_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVLAYERCONFIGOBJ_SYSTEMCONFIG_T_H__94B509B4_8B16_43A9_AF53_DC8016E7D4BE__INCLUDED_)
#define AFX_AVLAYERCONFIGOBJ_SYSTEMCONFIG_T_H__94B509B4_8B16_43A9_AF53_DC8016E7D4BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/AVLayerConfig_t.h"

class AVLayerConfigObj_SystemConfig_t : public AVLayerConfig_t  
{
public:
	AVLayerConfigObj_SystemConfig_t();
	virtual ~AVLayerConfigObj_SystemConfig_t();

	virtual void Configure( AVLayer_t *pAVLayer );
};

#endif // !defined(AFX_AVLAYERCONFIGOBJ_SYSTEMCONFIG_T_H__94B509B4_8B16_43A9_AF53_DC8016E7D4BE__INCLUDED_)
