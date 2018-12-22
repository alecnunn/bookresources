/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AudioSysDSoundImpl_t.h: interface for the AudioSysDSoundImpl_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUDIOSYSDSOUNDIMPL_T_H__140DF409_DF19_4DED_BF06_40A4BBE3C07A__INCLUDED_)
#define AFX_AUDIOSYSDSOUNDIMPL_T_H__140DF409_DF19_4DED_BF06_40A4BBE3C07A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Systems/AudioSys_t.h"

class AudioSysDSoundImpl_t : public AudioSys_t  
{
public:
	AudioSysDSoundImpl_t();
	virtual ~AudioSysDSoundImpl_t();

	virtual bool Init( Systems_t *pSystems );
	virtual void Shutdown();

	virtual bool BeginRender();
	virtual void EndRender();
};

#endif // !defined(AFX_AUDIOSYSDSOUNDIMPL_T_H__140DF409_DF19_4DED_BF06_40A4BBE3C07A__INCLUDED_)
