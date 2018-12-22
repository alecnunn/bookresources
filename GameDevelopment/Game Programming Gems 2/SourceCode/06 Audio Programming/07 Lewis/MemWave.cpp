/* Copyright (C) Ian Lewis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Ian Lewis, 2001"
 */
// MemWave.cpp: implementation of the CMemWave class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LowLevelAPI.h"
#include "MemWave.h"
#include "dsutil.h"
#include "mmreg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemWave::CMemWave(char* filename)
{
	CWaveFile wfile;
	wfile.Open(filename,NULL,WAVEFILE_READ);
	WAVEFORMATEX* pwfx=wfile.GetFormat();	
	
	//for the purposes of this sample, allow ONLY 
	//normal PCM waves to be loaded.
	ASSERT(pwfx->wFormatTag==WAVE_FORMAT_PCM);

	//fill in the description from the WAVEFORMATEX tag
	description.bitsPerSample=pwfx->wBitsPerSample;
		//kludge: WAVE doesn't say how bits are packed into
		//bytes, so assume the most common case
	description.bytesPerSample=pwfx->wBitsPerSample/8;
	description.cif=1;//again, assume normal PCM encoding
	description.isSigned=true;
	description.littleEndian=true;
	description.nBytes=wfile.GetSize();
	description.nChannels=pwfx->nChannels;
	//we ASSERTed WAVE_FORMAT_PCM, so nSamples 
	//can be computed in a straightforward manner
	description.nSamples=description.nBytes/description.bytesPerSample;
	description.priority=0;
	description.sampleRate=pwfx->nSamplesPerSec;

	//now read the file into memory
	data=new BYTE[description.nBytes];
	DWORD nRead;
	wfile.Read(data,description.nBytes,&nRead);
	ASSERT(nRead==description.nBytes);
}

CMemWave::~CMemWave()
{
	if(data)
		delete[]data;
}
