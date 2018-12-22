/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AVLayerConfigObj_GameWin_t.h: interface for the AVLayerConfigObj_GameWin_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVLAYERCONFIGOBJ_GAMEWIN_T_H__8B1B0EF7_BF1A_4737_9D08_BD9E88EAF5EE__INCLUDED_)
#define AFX_AVLAYERCONFIGOBJ_GAMEWIN_T_H__8B1B0EF7_BF1A_4737_9D08_BD9E88EAF5EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/AVLayerConfig_t.h"

class AVLayerConfigObj_GameWin_t : public AVLayerConfig_t  
{
public:
	AVLayerConfigObj_GameWin_t();
	virtual ~AVLayerConfigObj_GameWin_t();

	virtual void Configure( AVLayer_t *pAVLayer );
};

#endif // !defined(AFX_AVLAYERCONFIGOBJ_GAMEWIN_T_H__8B1B0EF7_BF1A_4737_9D08_BD9E88EAF5EE__INCLUDED_)
