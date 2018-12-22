// qvox.h
//
// QVOX LPC/FM anaylsis/synthesis package

#ifndef QVOX_H
#define QVOX_H

#include "linearpredictor.h"	// general LPC stuff
#include "wavfile.h"			// WAV file support
#include "speaker.h"			// speakers

const int lp_order = 8;

// QFrame
//
// quantized speech frame

struct QFrame
{
	char	pitch;			// pitch estimate
	char	rms;			// RMS of residual
	char	rc[lp_order];	// reflection coefficients

	void	SetRC(float val, int ix)	{ int ival; ival = int(val * 127); rc[ix] = ival < -127 ? -127 : (ival > 127 ? 127 : ival); }
	void	SetRMS(float val)			{ int ival; ival = int(val * 100); rms = ival < 0 ? 0 : (ival > 100 ? 100 : ival); }

	float	GetRC(int ix)				{ return float(rc[ix]) / 127; }
	float	GetRMS()					{ return float(rms) / 100; }
};

// QVOX
//
// QVOX file

class QVOX
{
public:
	int		nframes;	// number of frames
	int		framesize;	// framesize
	int		samplerate;	// sample rate
	QFrame*	qframe;	// frame data
	char*	rframe;	// residual data (not saved)

	QVOX(int framesize = 160, int samplerate = 8000);
	QVOX(char* filename);
	~QVOX();

#if ENCODE
	QVOX(WavFile* wav, int framesize = 160, int samplerate = 8000);
	int		Save(char* filename);		// save to file
	int		Dump(char* filename);		// dump as text file
#endif
};

// QVOX_Instance
//
// state of a playback instance

class QVOX_Instance
{
public:
	QVOX_Instance(QVOX* qvox, Speaker* type, int rate = 50, bool looping = false);

	void	SetLoop(bool set) { looping = set; }
	int		FrameSize() { return framesize; }
	int		NextFrame(float* frame, int testmode = 0, int play_residual = 0);

	const QVOX*			qvox;		// QVOX file

private:
	int					framesize;	// framesize for resynthesis
	int					framenum;	// frame number
	bool				looping;	// looping flag
	Speaker_Instance	speaker;	// Speaker for file
	LinearPredictor		pred;		// LPC decoder
};

#endif	// QVOX_H
