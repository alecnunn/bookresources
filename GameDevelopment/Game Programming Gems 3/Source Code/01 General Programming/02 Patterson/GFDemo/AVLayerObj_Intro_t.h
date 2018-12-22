/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AVLayerObj_Intro_t.h: interface for the AVLayerObj_Intro_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVLAYEROBJ_INTRO_T_H__FB441620_76ED_4E9D_9006_352387E4E345__INCLUDED_)
#define AFX_AVLAYEROBJ_INTRO_T_H__FB441620_76ED_4E9D_9006_352387E4E345__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/AVLayer_t.h"
#include "GFInclude/Objects/TextBox2D_t.h"
class Systems_t;

class AVLayerObj_Intro_t : public AVLayer_t  
{
public:
	AVLayerObj_Intro_t();
	virtual ~AVLayerObj_Intro_t();

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

#endif // !defined(AFX_AVLAYEROBJ_INTRO_T_H__FB441620_76ED_4E9D_9006_352387E4E345__INCLUDED_)
