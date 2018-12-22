/* Copyright (C) Ian Lewis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Ian Lewis, 2001"
 */
// DirectSoundAcceleratedMixer.cpp: implementation of the CDirectSoundAcceleratedMixer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LowLevelAPI.h"
#include "DirectSoundAcceleratedMixer.h"
#include "DirectSoundAcceleratedChannel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirectSoundAcceleratedMixer::CDirectSoundAcceleratedMixer(int _nChannels):
	CMixer(_nChannels)
{
	for(int i=0;i<channels.size();i++)
		channels[i]=new CDirectSoundAcceleratedChannel;
}

CDirectSoundAcceleratedMixer::~CDirectSoundAcceleratedMixer()
{
	for(int i=0;i<channels.size();i++)
		delete channels[i];
}

void CDirectSoundAcceleratedMixer::tick(time_t t)
{
}