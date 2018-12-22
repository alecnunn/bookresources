/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AVLayerConfigObj_GamePause_t.h: interface for the AVLayerConfigObj_GamePause_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVLAYERCONFIGOBJ_GAMEPAUSE_T_H__EE8689FC_FDC3_4350_B36B_FD4CEBCEBEB3__INCLUDED_)
#define AFX_AVLAYERCONFIGOBJ_GAMEPAUSE_T_H__EE8689FC_FDC3_4350_B36B_FD4CEBCEBEB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/AVLayerConfig_t.h"

class AVLayerConfigObj_GamePause_t : public AVLayerConfig_t  
{
public:
	AVLayerConfigObj_GamePause_t();
	virtual ~AVLayerConfigObj_GamePause_t();

	virtual void Configure( AVLayer_t *pAVLayer );
};

#endif // !defined(AFX_AVLAYERCONFIGOBJ_GAMEPAUSE_T_H__EE8689FC_FDC3_4350_B36B_FD4CEBCEBEB3__INCLUDED_)
