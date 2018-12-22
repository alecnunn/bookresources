/* Copyright (C) Ian Lewis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Ian Lewis, 2001"
 */
// Wave.h: interface for the CWave class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVE_H__21041404_44A1_4191_BDBA_F89B0162F060__INCLUDED_)
#define AFX_WAVE_H__21041404_44A1_4191_BDBA_F89B0162F060__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WaveDesc.h"

/*
Wave
The basic building block of the audio
infrastructure. Contains raw audio data
whose structure is defined by the
CWaveDesc member.
*/

class CWave  
{
protected:
	CWaveDesc description;//describes the structure of the wave data

	BYTE* data;
public:
	CWave();
	virtual ~CWave();

	//access functions
	const CWaveDesc& getDescription() const {return description;}
	BYTE* getData(){return data;}
};

#endif // !defined(AFX_WAVE_H__21041404_44A1_4191_BDBA_F89B0162F060__INCLUDED_)
