/* Copyright (C) Ian Lewis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Ian Lewis, 2001"
 */
// DirectSoundAcceleratedChannel.h: interface for the CDirectSoundAcceleratedChannel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTSOUNDACCELERATEDCHANNEL_H__9BACE94F_178E_4D23_A2DC_BD7E383FECC0__INCLUDED_)
#define AFX_DIRECTSOUNDACCELERATEDCHANNEL_H__9BACE94F_178E_4D23_A2DC_BD7E383FECC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Mixer.h"

class CDirectSoundAcceleratedChannel : public CMixer::channel  
{
public:
	CDirectSoundAcceleratedChannel();
	virtual ~CDirectSoundAcceleratedChannel();

	//perform any platform-dependent housekeeping
	virtual void tick(time_t t,CMixer* parent);

	//returns true if wave format is compatible
	virtual bool canPlay(const CWaveDesc& soundType);

};

#endif // !defined(AFX_DIRECTSOUNDACCELERATEDCHANNEL_H__9BACE94F_178E_4D23_A2DC_BD7E383FECC0__INCLUDED_)
