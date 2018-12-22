/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AVLayerConfigObj_Game_t.h: interface for the AVLayerConfigObj_Game_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVLAYERCONFIGOBJ_GAME_T_H__A9AEA5A9_0FCE_40F9_B63C_6393D34BF78A__INCLUDED_)
#define AFX_AVLAYERCONFIGOBJ_GAME_T_H__A9AEA5A9_0FCE_40F9_B63C_6393D34BF78A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/AVLayerConfig_t.h"

class AVLayerConfigObj_Game_t : public AVLayerConfig_t  
{
public:
	AVLayerConfigObj_Game_t();
	virtual ~AVLayerConfigObj_Game_t();

	virtual void Configure( AVLayer_t *pAVLayer );
};

#endif // !defined(AFX_AVLAYERCONFIGOBJ_GAME_T_H__A9AEA5A9_0FCE_40F9_B63C_6393D34BF78A__INCLUDED_)
