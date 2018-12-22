/* Copyright (C) Ian Lewis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Ian Lewis, 2001"
 */
// DirectSoundAcceleratedChannel.cpp: implementation of the CDirectSoundAcceleratedChannel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LowLevelAPI.h"
#include "DirectSoundAcceleratedChannel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirectSoundAcceleratedChannel::CDirectSoundAcceleratedChannel()
{
	
}

CDirectSoundAcceleratedChannel::~CDirectSoundAcceleratedChannel()
{

}

//perform any platform-dependent housekeeping
void CDirectSoundAcceleratedChannel::tick(time_t t,CMixer* parent)
{
}

//returns true if wave format is compatible
bool CDirectSoundAcceleratedChannel::canPlay(const CWaveDesc& soundType)
{
	return false;
}
