// speaker.cpp
//
// speaker class

#include "stdafx.h"
#include "speaker.h"
#include "linearpredictor.h"
#include <cmath>

int Speaker::wforms_ok = 0;
float Speaker::fwforms[WTableCnt][4096];
short Speaker::iwforms[WTableCnt][4096];

// Speaker
//
// default speaker

Speaker::Speaker()
{
	if (!wforms_ok)		InitWForms();

	deep = 0;
	shallow = 0;

	osc[0].wave       = Sine;
	osc[0].f_ratio_f  = true;
	osc[0].f_const    = 0;
	osc[0].f_voice    = 0;
	osc[0].f_ratio_m  = 1;
	osc[0].f_ratio_d  = 2;
	osc[0].f_mod      = 0;
	osc[0].a_level    = 100;
	osc[0].a_envelope = 23;

	osc[1].wave       = Pulse;
	osc[1].f_ratio_f  = false;
	osc[1].f_const    = 10;
	osc[1].f_voice    = 0;
	osc[1].f_ratio_m  = 1;
	osc[1].f_ratio_d  = 1;
	osc[1].f_mod      = 0;
	osc[1].a_level    = 0;
	osc[1].a_envelope = 25;
}

// speaker from a file

Speaker::Speaker(char* filename)
{
	if (!wforms_ok)		InitWForms();
	Load(filename);
}

// Load
//
// load from file, returns 1 on error

int Speaker::Load(char* filename)
{
	HANDLE	hFile;
	DWORD	length;

    hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
	{
		TRACE("File %s not found\n", filename);
		return 1;
	}

	::ReadFile(hFile, this, sizeof(*this), &length, NULL);
	CloseHandle(hFile);
	return 0;
}

#if ENCODE
// Save
//
// save to file, returns 1 on error

int Speaker::Save(char* filename)
{
	HANDLE	hFile;
	DWORD	length;

	hFile = CreateFile(filename, GENERIC_READ + GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		TRACE("Can't open file %s\n", filename);
		return 1;
	}

	::WriteFile(hFile, this, sizeof(*this), &length, NULL);
	SetEndOfFile(hFile);

	CloseHandle(hFile);
	return 0;
}

// Dump
//
// dump to file, returns 1 on error

int Speaker::Dump(char* filename)
{
	static char	*waves[6] = {"Sine", "Tri", "Saw", "Square", "Pulse", "Noise"};

	FILE*	fd = fopen(filename, "w");
	if (!fd)	return 1;

	fprintf(fd, "deep     = %d;\n", deep);
	fprintf(fd, "shallow  = %d;\n", shallow);

	for (int ii = 0; ii < 2; ii++)
	{
		fprintf(fd, "\nosc[%d].wave       = %s;\n", ii, waves[osc[ii].wave]);
		fprintf(fd, "osc[%d].f_ratio_f  = %s;\n", ii, osc[ii].f_ratio_f ? "true" : "false");
		fprintf(fd, "osc[%d].f_const    = %d;\n", ii, osc[ii].f_const);
		fprintf(fd, "osc[%d].f_voice    = %d;\n", ii, osc[ii].f_voice);
		fprintf(fd, "osc[%d].f_ratio_m  = %d;\n", ii, osc[ii].f_ratio_m);
		fprintf(fd, "osc[%d].f_ratio_d  = %d;\n", ii, osc[ii].f_ratio_d);
		fprintf(fd, "osc[%d].f_mod      = %d;\n", ii, osc[ii].f_mod);
		fprintf(fd, "osc[%d].a_level    = %d;\n", ii, osc[ii].a_level);
		fprintf(fd, "osc[%d].a_envelope = %d;\n", ii, osc[ii].a_envelope);
	}

	fclose(fd);

	return 0;
}
#endif	// ENCODE

// InitWForms
//
// initialize waveforms

void Speaker::InitWForms()
{
	int		ii;

	float pi = float(4.0 * atan(1.0));

	for (ii = 0; ii < 4096; ii++)
	{
		fwforms[Sine][ii] = float(sin(ii * 2 * pi / 4096));
		fwforms[Saw][ii] = float(-1.0 + ii/2048.0);
	}

	for (ii = 0; ii < 1024; ii++)
	{
		fwforms[Tri][ii] = float(ii / 1024.0);
		fwforms[Tri][ii+1024] = float(1 - ii/1024.0);
		fwforms[Tri][ii+2048] = float(-ii/1024.0);
		fwforms[Tri][ii+3072] = float(-1 + ii/1024.0);
	}

	for (ii = 0; ii < 2048; ii++)
	{
		fwforms[Square][ii] = 1;
		fwforms[Square][ii+2048] = -1;
	}

	// now create integer waveforms
	for (ii = 0; ii < WTableCnt; ii++)
	{
		for (int jj = 0; jj < 4096; jj++)
		{
			iwforms[ii][jj] = (int)(32767.0 * fwforms[ii][jj]);
		}
	}

	wforms_ok = 1;
}

// noise
//
// noise source (amplitude 1.0)

static float noise()
{
	float	val;

	val = (float)rand() / 32767;

	return val*2 - 1;
}

// Speaker_Instance
//
// initialise instance

Speaker_Instance::Speaker_Instance(Speaker *type) : type(type)
{
	phase[0] = phase[1] = feedback = 0;
	last_am1 = 0;
}

// NextFrame
//
// generate next frame

void Speaker_Instance::NextFrame(float* frame, int framesize, char pitch, char rms, int testmode)
{
	float	fpi0,fpi1;		// float phase increments
	int		pi0,pi1;		// fixed phase increments
	int		fm0,fm1;		// FM amounts 0-255
	int		p0,p1;			// phases 12:20
	short*	w0;				// wavetables
	float*	w1;
	int		am0;			// amplitudes
	float	am1;
	float	dam1;			// delta amplitude for output oscillator
	int		last;
	int		ii;

	// calculate the phase increments
	fpi0 = type->osc[0].f_const * 5.0F * 4096.0F / 8000.0F;		// constant, 100 = 500Hz
	fpi0 += type->osc[0].f_voice * 81.92F / pitch;				// voice, 50 = x1

	fpi1 = type->osc[1].f_const * 5.0F * 4096.0F / 8000.0F;		// constant, 100 = 500Hz
	fpi1 += type->osc[1].f_voice * 81.92F / pitch;				// voice, 50 = x1

	if (type->osc[0].f_ratio_f)
	{
		fpi0 += fpi1 * type->osc[0].f_ratio_m / type->osc[0].f_ratio_d;
	}
	if (type->osc[1].f_ratio_f)
	{
		fpi1 += fpi0 * type->osc[1].f_ratio_m / type->osc[1].f_ratio_d;
	}

	// convert to integers
	pi0 = (int)fpi0;
	fpi0 -= pi0;
	pi0 = (pi0 << 20) + (int)(fpi0 * (1<<20));

	pi1 = (int)fpi1;
	fpi1 -= pi1;
	pi1 = (pi1 << 20) + (int)(fpi1 * (1<<20));

	// run the synthesis
	w0 = type->iwforms[type->osc[0].wave];
	p0 = phase[0];
	fm0 = type->osc[0].f_mod * 63L / 10;
	am0 = (type->osc[0].a_level * 100 + type->osc[0].a_envelope * rms * 4) * 31 / 10000;
	last = feedback;

	w1 = type->fwforms[type->osc[1].wave];
	p1 = phase[1];
	fm1 = type->osc[1].f_mod * 255L / 10;
	am1 = last_am1;
	last_am1 = (type->osc[1].a_level * 100 + type->osc[1].a_envelope * rms) / 2500.0F;
	dam1 = (last_am1 - am1) / framesize;

	if (type->osc[1].wave == Pulse)
	{
		for (ii = 0; ii < framesize; ii++)
		{
			unsigned long	ix;
			int				oldlast;

			oldlast = last;

			// oscillator 0
			ix = (unsigned)(p0 + fm0 * last);	// phase + FM modulation
			last = (int)w0[ix >> 20] * am0;
			p0 += pi0;

			// oscillator 1 (pulse)
			frame[ii] = (((p1 + fm1 * oldlast) ^ (p1 + fm1 * last + pi1)) < 0) ? 4*am1 : 0; 
			p1 += pi1;
			am1 += dam1;

			if (testmode == 2)
			{
				frame[ii] = (float)last / (1<<20);
			}
		}
	}
	else if (type->osc[1].wave == Noise)
	{
		for (ii = 0; ii < framesize; ii++)
		{
			unsigned long	ix;

			// update oscillator 0
			ix = (unsigned)(p0 + fm0 * last);	// phase + FM modulation
			last = (int)w0[ix >> 20] * am0;
			p0 += pi0;

			// oscillator 1 (noise)
			frame[ii] = noise() * am1;
			p1 += pi1;
			am1 += dam1;

			if (testmode == 2)
			{
				frame[ii] = (float)last / (1<<20);
			}
		}
	}
	else
	{
		for (ii = 0; ii < framesize; ii++)
		{
			unsigned long	ix;

			// oscillator 0
			ix = (unsigned)(p0 + fm0 * last);	// phase + FM modulation
			last = (int)w0[ix >> 20] * am0;
			p0 += pi0;

			// oscillator 1
			ix = (unsigned)(p1 + fm1 * last);	// phase + FM modulation
			frame[ii] = w1[ix >> 20] * am1;
			p1 += pi1;
			am1 += dam1;

			if (testmode == 2)
			{
				frame[ii] = (float)last / (1<<20);
			}
		}
	}

	phase[0] = p0;
	phase[1] = p1;
	feedback = last;
}
