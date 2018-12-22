// wavfile.h

#ifndef WAVFILE_H
#define WAVFILE_H

#include "stdafx.h"
#include <MMsystem.h>

// exposes a WAV file as a float array
// permits conversion from mono to stereo, 8 to 16 bits and
// also rate conversion (anti-aliased)

class WavFile
{
public:

	// creators
	WavFile();
	WavFile(WavFile *cp);
	~WavFile();

	// create/destroy/load/save WAV file
	void			Create(int length, int rate, int depth, float *lbuf, float *rbuf = NULL);
	void			Destroy();
	void			Load(char *filename);
	int				Save(char *filename);

	// see if a valid WAV file is loaded
	int				IsValid() { return !!valid; }

	// get/set sample rate (SetRate() takes some time)
	DWORD			GetRate() { return valid ? WaveInfo.wf.nSamplesPerSec : 0; }
	void			SetRate(DWORD rate, int window = 128);

	// get/set bit depth (only used when saving)
	WORD			GetDepth() { return valid ? WaveInfo.wBitsPerSample : 0; }
	void			SetDepth(WORD bits);

	// get/set mono or stereo
	WORD			GetChannels() { return valid ? WaveInfo.wf.nChannels : 0; }
	void			SetChannels(WORD ch);

	// spool interface:
	// get mono frame (stereo is mixed down)
	int				GetFrameMono(float *buffer, int length);

	// get stereo frame (mono is duplicated)
	int				GetFrameStereo(float *lbuf, float *rbuf, int length);

	// get number of frames in file
	int				NumFrames(int length);

	// retrigger
	void			Retrigger();

	// get/set loop flag
	int				GetLoopFlag() { return valid ? loopflag : 0; }
	void			SetLoopFlag(int looping);

private:

	PCMWAVEFORMAT	WaveInfo;	// wave format
	float			*lptr;		// left-channel data ptr
	float			*rptr;		// right-channel data ptr
	int				dlen;		// data length in samples
	int				valid;		// valid flag
	int				offset;		// read offset
	int				loopflag;	// looping?

	void			ReadFile(LPVOID fileview);
	void			WriteFile(LPVOID fileview);

	float			noise() { float val; val = (float)rand() / 32767; return val*2 - 1; }

	int				Sample16(float val)
	{
		int smp = (int)(val * 32768 + noise() / 2); 
		if (smp < -32768) smp = -32768; else if (smp > 32767) smp = 32767; return smp;
	}
	int				Sample8(float val)
	{
		int smp = (int)(val * 400 + noise() / 2 + 128);
		if (smp < 0) smp = 0; else if (smp > 255) smp = 255; return smp;
	}

};

#endif // WAVFILE_H
