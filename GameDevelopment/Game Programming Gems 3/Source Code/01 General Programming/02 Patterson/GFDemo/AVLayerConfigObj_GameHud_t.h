/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AVLayerConfigObj_GameHud_t.h: interface for the AVLayerConfigObj_GameHud_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVLAYERCONFIGOBJ_GAMEHUD_T_H__1792A42B_4CC5_45DB_AB24_E5AF0BBD0786__INCLUDED_)
#define AFX_AVLAYERCONFIGOBJ_GAMEHUD_T_H__1792A42B_4CC5_45DB_AB24_E5AF0BBD0786__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/AVLayerConfig_t.h"

class AVLayerConfigObj_GameHud_t : public AVLayerConfig_t  
{
public:
	AVLayerConfigObj_GameHud_t();
	virtual ~AVLayerConfigObj_GameHud_t();

	virtual void Configure( AVLayer_t *pAVLayer );
};

#endif // !defined(AFX_AVLAYERCONFIGOBJ_GAMEHUD_T_H__1792A42B_4CC5_45DB_AB24_E5AF0BBD0786__INCLUDED_)
