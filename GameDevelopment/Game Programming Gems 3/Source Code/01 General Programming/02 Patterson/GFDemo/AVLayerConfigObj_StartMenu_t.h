/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AVLayerConfigObj_StartMenu_t.h: interface for the AVLayerConfigObj_StartMenu_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVLAYERCONFIGOBJ_STARTMENU_T_H__1BD92376_711E_486A_9C42_134BF48AF4FF__INCLUDED_)
#define AFX_AVLAYERCONFIGOBJ_STARTMENU_T_H__1BD92376_711E_486A_9C42_134BF48AF4FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/AVLayerConfig_t.h"

class AVLayerConfigObj_StartMenu_t : public AVLayerConfig_t  
{
public:
	AVLayerConfigObj_StartMenu_t();
	virtual ~AVLayerConfigObj_StartMenu_t();

	virtual void Configure( AVLayer_t *pAVLayer );
};

#endif // !defined(AFX_AVLAYERCONFIGOBJ_STARTMENU_T_H__1BD92376_711E_486A_9C42_134BF48AF4FF__INCLUDED_)
