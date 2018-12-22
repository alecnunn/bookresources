// soundmanager.h
//
// sound manager

#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <MMsystem.h>
#include <DSound.h>
#include <cmath>
#include "wavfile.h"
#include "qvox.h"

class SoundClient
{
public:
	SoundClient();
	virtual ~SoundClient();

	void	LinkAfter(SoundClient *other);
	void	LinkBefore(SoundClient *other);
	void	Unlink();

	float	lvol;	// left volume, 0.0 to 1.0
	float	rvol;	// right volume, 0.0 to 1.0

	virtual void	WriteIntoStereoBuffer(float *ptr, int cnt);
	virtual void	MixIntoStereoBuffer(float *ptr, int cnt);

	void	RunChain(short *ptr, int cnt, float *tmp);

protected:
	virtual int	GetMoreData(float **ptr, int *len);

private:
	SoundClient	*next;
	SoundClient	*prev;

	float		*bufferptr;
	int			buffercnt;
};

class SoundManager
{
public:

	SoundManager(HWND hWnd);
	~SoundManager();

	void	AttachClient(SoundClient *other);	// attach a client

	void	TimerCallback(void);			// DO NOT CALL !!!

	static SoundManager	*TheManager;		// current sound manager

	static void Exit();

	void	Pause();
	void	Resume();

private:

	void	WriteData(DWORD bytes);			// write (bytes) bytes to the DS buffer

	SoundManager		*prev;				// previous sound manager
	const HWND			hWnd;				// window owning the sound manager (used for timer)
	LPDIRECTSOUND		lpDirectSound;		// DirectSound object
	PCMWAVEFORMAT		SoundBufferFmt;		// format of DSB
	DSBUFFERDESC		SoundBufferDesc;	// description of DSB
	LPDIRECTSOUNDBUFFER	lpSoundBuffer;		// DirectSoundBuffer object
	UINT				hTimer;				// timer handle
	DWORD				WritePtr;			// buffer write ptr
	SoundClient			RootClient;			// root SoundClient
	float				*FloatBuffer;		// mixing buffer
};

// 1KHz sinewave

class SineWaveTest : public SoundClient
{
public:
	SineWaveTest();

protected:
	virtual int GetMoreData(float **ptr, int *len);

private:
	float	sinebuffer[441];	// contains 10 cycles of a 1 KHz sinewave
};

// soundclient for wavfile

class WavClient : public SoundClient, public WavFile
{
public:

	WavClient() : WavFile() { }
	WavClient(char *filename) : WavFile() { Load(filename); }
	WavClient(char *filename, int rate) : WavFile() { Load(filename); SetRate(rate); }

private:

	float	buffer[100];
	int		GetMoreData(float **ptr, int *len) { *ptr = buffer; *len = 100; GetFrameMono(buffer, 100); return 0; }
};

// soundclient for QVOX

class QVOX_Client : public SoundClient
{
public:
	QVOX_Client();
	~QVOX_Client();

	void	Play(QVOX *qvox, Speaker *speaker, int rate = 50);
	void	Stop();

	int		testmode;
	int		play_residual;

private:
	QVOX_Instance	*qvoxi;
	float			*buffer;
	int				buflen;
	int				framesize;

	int				GetMoreData(float **ptr, int *len);
};

#endif	// SOUNDMANAGER_H

