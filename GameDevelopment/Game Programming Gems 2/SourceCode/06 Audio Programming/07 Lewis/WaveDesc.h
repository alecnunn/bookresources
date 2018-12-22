/* Copyright (C) Ian Lewis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Ian Lewis, 2001"
 */
// WaveDesc.h: interface for the CWaveDesc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVEDESC_H__D7648BFA_EFA9_4BA9_8F04_CD7F129C6658__INCLUDED_)
#define AFX_WAVEDESC_H__D7648BFA_EFA9_4BA9_8F04_CD7F129C6658__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
WaveType
Definitions for different types of data storage
*/
typedef enum
{
	WAVE_TYPE_PCM,
	WAVE_TYPE_ADPCM,
	WAVE_TYPE_MP3
}WaveType;

/*
CWaveDesc
Holds a description of audio source material
that is used by several other classes to 
classify and prioritize audio data.
*/
struct CWaveDesc  
{
	unsigned int bitsPerSample;
	unsigned int bytesPerSample;//bytes per (uncompressed) sample
	unsigned int nChannels;//number of distinct audio data channels, e.g. 2 for stereo, 6 for AC3
	unsigned int cif;//channel interleave factor, e.g. Redbook stereo audio has
			//an interleave of 1 because the data alternates channels
			//every other sample. A certain console platform has an interleave
			//factor of 256 because the data is stored as 256 left samples
			//followed by 256 right samples.
	unsigned int sampleRate;
	unsigned int nSamples;//length of data in samples
	unsigned int nBytes;//length of data in bytes. This is not necessarily
				//nSamples * bytesPerSample, because the wave may be compressed

	bool littleEndian;
	bool isSigned;

	int priority;
};

#endif // !defined(AFX_WAVEDESC_H__D7648BFA_EFA9_4BA9_8F04_CD7F129C6658__INCLUDED_)
