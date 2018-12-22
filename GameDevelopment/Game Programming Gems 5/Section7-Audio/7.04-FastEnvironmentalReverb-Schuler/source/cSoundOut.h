
#ifndef SOUNDOUT_INCLUDED
#define SOUNDOUT_INCLUDED

#include <mmsystem.h>
#include <dsound.h>

#include "cSoundBuffer.h"
#include "WaveFile.h"

/////////////////////////////////////////////////////////////////////////////
// Forward Declarations
struct IDirectSound;
struct IDirectSoundBuffer;

class cSoundOut  
{
	IDirectSound *m_pDS;
	IDirectSoundBuffer *m_pDSB;
	DSBCAPS m_BufferCaps;
	bool m_Playing;
	unsigned m_BufferBytes;
	unsigned m_UpdateCursor;
	unsigned m_AvgBytesPerSec;
	unsigned m_BlockAlign;
	unsigned m_Samplerate;
	CWaveFile m_OutputWaveFile;
	bool m_bCanWrite;
	
public:
	void Release();
	void Init( HWND Parent, bool Stereo = false );
	void Play();
	void Stop();
	void SetSamplerate( unsigned rate );
	bool IsPlaying() const { return m_Playing; }
	unsigned long QueryUpdateInterval() const;
	unsigned long QueryUpdateSize() const;

	void Update( const cSoundBuffer<float> &wavedata );
	void UpdateStereo( const cSoundBuffer<float> &wavedataL, const cSoundBuffer<float> &wavedataR );
	
	cSoundOut();
	virtual ~cSoundOut();
};



#endif

