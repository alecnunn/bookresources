/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AVLayerObj_GameEditor_t.h: interface for the AVLayerObj_GameEditor_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVLAYEROBJ_GAMEEDITOR_T_H__7AA2E385_E815_4594_8DD0_C23F77E01AF7__INCLUDED_)
#define AFX_AVLAYEROBJ_GAMEEDITOR_T_H__7AA2E385_E815_4594_8DD0_C23F77E01AF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/AVLayer_t.h"
#include "GFInclude/Objects/TextBox2D_t.h"

class AVLayerObj_GameEditor_t : public AVLayer_t  
{
public:
	AVLayerObj_GameEditor_t();
	virtual ~AVLayerObj_GameEditor_t();

	virtual bool Connect( Systems_t *pSystems );
	virtual void Disconnect();

	virtual void Update();
	virtual void RenderVisual();
	virtual void RenderAudio();

	// configurable
	TextBox2D_t m_ModeTextBox;
	TextBox2D_t m_InfoTextBox;
protected:
	Systems_t *m_pSystems;
};

#endif // !defined(AFX_AVLAYEROBJ_GAMEEDITOR_T_H__7AA2E385_E815_4594_8DD0_C23F77E01AF7__INCLUDED_)
