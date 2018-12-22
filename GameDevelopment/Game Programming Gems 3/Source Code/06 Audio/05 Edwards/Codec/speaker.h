// speaker.h
//
// speaker class

#ifndef SPEAKER_H
#define SPEAKER_H

// WForm
//
// waveform types

enum WForm
{
	Sine = 0,
	Tri,
	Saw,
	Square,
	Pulse,
	Noise,
	WTableCnt = Noise
};

// OscFM
//
// FM oscillator

struct OscFM
{
	WForm	wave;

	char	f_ratio_f;	// add ratio * other oscillator if set
	char	f_const;	// constant frequency 0-100 (100 = 200 Hz)
	char	f_voice;	// voice freq multiplier 0 to 100 - 50 = x1
	char	f_ratio_m;	// ratio multiplier 1 to 16
	char	f_ratio_d;	// ratio divider 1 to 16
	char	f_mod;		// modulation/feedback amount 0 to 100 (OSC0 - 100 = x1; OSC1 - 100 = x4)

	char	a_level;	// amplitude level 0 to 100 (OSC0 - 100 = x1; OSC1 - 100 = x4)
	char	a_envelope;	// envelope level 0 to 100 (= 1 - y origin of envelope, so 0 = no envelope, 100 = normal envelope
};

// Speaker
//
// parameters for a single type of speaker

class Speaker
{
public:
	OscFM	osc[2];		// 0 = modulator, 1 = carrier
	char	deep;		// deep voice?
	char	shallow;	// shallow voice?

	Speaker();
	Speaker(char *filename);

	int		Load(char *filename);	// Load parameters from file
#if ENCODE
	int		Save(char *filename);	// Save parameters to file
	int		Dump(char *filename);	// Dump parameters to file
#endif

	static float	fwforms[WTableCnt][4096];
	static short	iwforms[WTableCnt][4096];

private:
	static int		wforms_ok;
	static void		InitWForms();
};

// Speaker_Instance
//
// active speaker

class Speaker_Instance
{
public:
	Speaker_Instance(Speaker *type);

	void	NextFrame(float *frame, int framesize, char pitch, char rms, int testmode = 0);

	const Speaker	*type;		// speaker parameters

private:
	int		phase[2];	// oscillator phases
	int		feedback;	// feedback sample for osc[0]
	float	last_am1;	// state for EG slope
};

#endif	// SPEAKER_H