/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AVLayerConfigObj_GameViewer_t.h: interface for the AVLayerConfigObj_GameViewer_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVLAYERCONFIGOBJ_GAMEVIEWER_T_H__44FE0B67_2B17_4C7D_8C17_F71958DDCAF2__INCLUDED_)
#define AFX_AVLAYERCONFIGOBJ_GAMEVIEWER_T_H__44FE0B67_2B17_4C7D_8C17_F71958DDCAF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/AVLayerConfig_t.h"

class AVLayerConfigObj_GameViewer_t : public AVLayerConfig_t  
{
public:
	AVLayerConfigObj_GameViewer_t();
	virtual ~AVLayerConfigObj_GameViewer_t();

	virtual void Configure( AVLayer_t *pAVLayer );
};

#endif // !defined(AFX_AVLAYERCONFIGOBJ_GAMEVIEWER_T_H__44FE0B67_2B17_4C7D_8C17_F71958DDCAF2__INCLUDED_)
