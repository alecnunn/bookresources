/* Copyright (C) Ian Lewis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Ian Lewis, 2001"
 */
// DirectSoundAcceleratedMixer.h: interface for the CDirectSoundAcceleratedMixer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTSOUNDACCELERATEDMIXER_H__5E6AF022_2465_44A7_8A5A_E9FC01C161F7__INCLUDED_)
#define AFX_DIRECTSOUNDACCELERATEDMIXER_H__5E6AF022_2465_44A7_8A5A_E9FC01C161F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Mixer.h"
#include "dsutil.h"
class CDirectSoundAcceleratedChannel;

class CDirectSoundAcceleratedMixer : public CMixer  
{
private:

public:
	CDirectSoundAcceleratedMixer(int _nChannels);
	virtual ~CDirectSoundAcceleratedMixer();

	virtual void tick(time_t t); //perform housekeeping
};

#endif // !defined(AFX_DIRECTSOUNDACCELERATEDMIXER_H__5E6AF022_2465_44A7_8A5A_E9FC01C161F7__INCLUDED_)
