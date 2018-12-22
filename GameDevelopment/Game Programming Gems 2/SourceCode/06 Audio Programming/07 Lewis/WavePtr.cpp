/* Copyright (C) Ian Lewis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Ian Lewis, 2001"
 */
// WavePtr.cpp: implementation of the CWavePtr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LowLevelAPI.h"
#include "WavePtr.h"
#include "Wave.h"
#include "AudioBuffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWavePtr::CWavePtr(CWave* p):pWave(p)
{

}

CWavePtr::~CWavePtr()
{

}

bool CWavePtr::finished()
{
	return(!loop && position >= pWave->getDescription().nSamples);
}


//CAUTION: this code assumes that the buffer passed in
//will always be smaller than the TOTAL size of the
//wave file.
void CWavePtr::getNext(CAudioBuffer &buffer)
{
	//before we do anything more expensife, check to see
	//if this wave is even playing. If not, then fill
	//the buffer with zeroes.
	if(finished())
	{
		memset(buffer.getData(),0,buffer.getLength());
	}

	unsigned int len=buffer.getLength();
	//throughout this function we need to convert between
	//bytes and samples, so save off that conversion factor
	unsigned int bytesPerSample=pWave->getDescription().bytesPerSample;
	
	//if we're not hitting the end of the wave, filling the 
	//buffer can just use a memcpy.
	if(position+len < pWave->getDescription().nSamples)
	{
		memcpy(buffer.getData(),pWave->getData()+position*bytesPerSample,len);
		position+=len/bytesPerSample;
	}
	else
	{
		//get as much data as we can from the end of the wave
		unsigned int remainingSamples=pWave->getDescription().nSamples - position;
		unsigned int leftoverSpace=len/bytesPerSample-remainingSamples;

		memcpy(buffer.getData(),
				pWave->getData()+position*bytesPerSample,
				remainingSamples*bytesPerSample);
		
		position+=remainingSamples;

		//if we're looping, then we need to go back and copy from the 
		//beginning.
		if(loop)
		{
			memcpy(buffer.getData(),
					pWave->getData(),
					leftoverSpace*bytesPerSample);
			position=leftoverSpace;
		}
		else
		{
			memset(buffer.getData(),0,leftoverSpace*bytesPerSample);
		}
	}
}


