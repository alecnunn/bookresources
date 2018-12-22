/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AVLayerConfigObj_GameEditor_t.h: interface for the AVLayerConfigObj_GameEditor_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVLAYERCONFIGOBJ_GAMEEDITOR_T_H__0B1CAF19_0A48_493F_81F0_8337DB401E75__INCLUDED_)
#define AFX_AVLAYERCONFIGOBJ_GAMEEDITOR_T_H__0B1CAF19_0A48_493F_81F0_8337DB401E75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/AVLayerConfig_t.h"

class AVLayerConfigObj_GameEditor_t : public AVLayerConfig_t  
{
public:
	AVLayerConfigObj_GameEditor_t();
	virtual ~AVLayerConfigObj_GameEditor_t();

	virtual void Configure( AVLayer_t *pAVLayer );
};

#endif // !defined(AFX_AVLAYERCONFIGOBJ_GAMEEDITOR_T_H__0B1CAF19_0A48_493F_81F0_8337DB401E75__INCLUDED_)
