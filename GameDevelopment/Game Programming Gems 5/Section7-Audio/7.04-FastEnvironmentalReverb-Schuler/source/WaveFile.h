
#ifndef WAVEFILE_INCLUDED
#define WAVEFILE_INCLUDED

#include <afx.h>

class CWaveFile : public CFile  
{
public:
	struct 
	{
		WORD wFormatTag;
		WORD wChannels;
		DWORD dwSamplesPerSec;
		DWORD dwAvgBytesPerSec;
		WORD wBlockAlign;
		WORD wBitsPerSample;
	} 
	m_FormatChunk;

	unsigned m_PositionOfDataBody;
	unsigned m_PositionOfChunkLength1;
	unsigned m_PositionOfChunkLength2;
	
public:
	CWaveFile();
	virtual ~CWaveFile();

	unsigned GetSamplerate() const { return m_FormatChunk.dwSamplesPerSec; }
	unsigned GetChannels() const { return m_FormatChunk.wChannels; }
	unsigned GetBitsPerSample() const { return m_FormatChunk.wBitsPerSample; }
	unsigned GetPositionOfDataBody() const { return m_PositionOfDataBody; }

	void SetSampleRate( unsigned x ) { m_FormatChunk.dwSamplesPerSec = x; }
	void SetChannels( unsigned x ) { m_FormatChunk.wChannels = x; }
	void SetBitsPerSample( unsigned x ) { m_FormatChunk.wBitsPerSample = x; }

	bool ReadWaveHeader();
	void WriteWaveHeader();
	void FinalizeWaveHeader();
};

#endif

