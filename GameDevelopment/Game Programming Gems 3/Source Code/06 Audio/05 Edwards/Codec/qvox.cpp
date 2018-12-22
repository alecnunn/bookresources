// qvox.cpp
//
// QVOX class
//
// a QVOX object stores a complete sound

#include "qvox.h"
#include <cmath>

#if ENCODE
static float	LPF_kernel[65];
static int		LPF_kernel_ok = 0;
static float	LPF_buffer[64];
static void		InitLPF();
static void		LPF(float* frame, int framesize, float* lp_frame);
static void		EncodeResidual(float* frame, int framesize, QFrame* qframe);
#endif

// QVOX
//
// QVOX encoding silence

QVOX::QVOX(int framesize, int samplerate) : framesize(framesize),
											samplerate(samplerate)
{
	nframes = 1;
	qframe = new QFrame;
	qframe->pitch = samplerate / 100;	// 100 Hz
	qframe->rms = 0;
	for (int ii = 0; ii < lp_order; ii++)
	{
		qframe->SetRC(0, ii);
	}
	rframe = 0;
}

// QVOX
//
// QVOX from a QVX file

QVOX::QVOX(char* filename)
{
	HANDLE	hFile;
	DWORD	length;

    hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
	{
		TRACE("File %s not found\n", filename);
		QVOX();
		return;
	}

	// read header
	::ReadFile(hFile, this, sizeof(*this), &length, NULL);

	// allocate & read frame data
	qframe = new QFrame[nframes];
	::ReadFile(hFile, qframe, sizeof(qframe[0])*nframes, &length, NULL);

	rframe = 0;

	CloseHandle(hFile);
}

#if ENCODE
// QVOX
//
// encode a WAV file

QVOX::QVOX(WavFile* wav, int framesize, int samplerate) :	framesize(framesize),
															samplerate(samplerate)
{
	if (!wav->IsValid())
	{
		QVOX();
		return;
	}

	// init median filter
	int	p0 = 255;
	int	p1 = 0;
	
	// set WAV file to mono and specified sample rate
	wav->SetChannels(1);
	wav->SetRate(samplerate, 32);

	// allocate frame data
	nframes = wav->NumFrames(framesize);

	// LPC data and residual data
	qframe = new QFrame[nframes];
	rframe = new char[framesize * nframes];

	// (temporary) frame stores
	float*	frame = new float[framesize];
	float*	lp_frame = new float[framesize + 64];

	// begin LPC coding
	LinearPredictor	pred(lp_order);

	char*	rframep = rframe;
	InitLPF();
	pred.InitEncoder();

	wav->Retrigger();
	for (int ii = 0; ii < nframes; ii++)
	{
		float	rc[lp_order];
		int		jj;

		// load frame from WAV file and encode it
		wav->GetFrameMono(frame, framesize);
		pred.EncodeFrame(frame, framesize, rc, frame);

		// store RCs
		for (jj = 0; jj < lp_order; jj++)
		{
			qframe[ii].SetRC(rc[jj], jj);
		}

		// store residual
		for (jj = 0; jj < framesize; jj++)
		{
			int ival = int(frame[jj] * 63);
			if (ival < -127)		*rframep++ = -127;
			else if (ival > 127)	*rframep++ = 127;
			else					*rframep++ = ival;
		}	

		// low-pass filter to 1kHz and estimate RMS and pitch
		LPF(frame, framesize, lp_frame);
		EncodeResidual(lp_frame, framesize, &qframe[ii]);

		// median filter the pitch
		int med = qframe[ii].pitch;
		if ((p0 < p1) && (p0 > qframe[ii].pitch))		med = p0;
		else if ((p0 > p1) && (p0 < qframe[ii].pitch))	med = p0;
		else if ((p1 < p0) && (p1 > qframe[ii].pitch))	med = p1;
		else if ((p1 > p0) && (p1 < qframe[ii].pitch))	med = p1;
		p0 = p1;
		p1 = qframe[ii].pitch;

		qframe[ii].pitch = med;
	}

	delete[] frame;
	delete[] lp_frame;
}

// InitLPF
//
// initialize LPF

static void InitLPF()
{
	// set up kernel
	if (!LPF_kernel_ok)
	{
		int	ii;

		float pi = float(4.0*atan(1.0));

		float sigma = 0;
		for (ii = 1; ii <= 32; ii++)
		{
			float	x = (float)ii / 8;

			LPF_kernel[32-ii] =	LPF_kernel[32+ii] = float(sin(pi * x) / (pi * x));
			sigma += LPF_kernel[32-ii] + LPF_kernel[32+ii];
		}
		LPF_kernel[32] = 1.0;
		sigma += 1.0;

		for (ii = 0; ii < 65; ii++)
		{
			LPF_kernel[ii] /= 2;
		}

		LPF_kernel_ok = 1;
	}

	// clear buffer
	for (int ii = 0; ii < 64; ii++)
	{
		LPF_buffer[ii] = 0;
	}
}

// LPF
//
// run LPF

static void LPF(float* frame, int framesize, float* lp_frame)
{
	int	ii;

	// set up lp_frame with old buffer
	for (ii = 0; ii < 64; ii++)
	{
		lp_frame[ii] = LPF_buffer[ii];
	}

	// convolve
	for (ii = 0; ii < framesize; ii++)
	{
		for (int jj = 0; jj < 64; jj++)
		{
			lp_frame[ii+jj] += frame[ii] * LPF_kernel[jj];
		}
		lp_frame[ii+64] = frame[ii] * LPF_kernel[64];
	}

	// store future 64 samples
	for (ii = 0; ii < 64; ii++)
	{
		LPF_buffer[ii] = lp_frame[framesize + ii];
	}
}

// EncodeResidual
//
// find RMS and pitch

static void EncodeResidual(float* frame, int framesize, QFrame* qframe)
{
	int		ii;

	// find RMS
	float rms = 0;
	for (ii = 0; ii < framesize; ii++)
	{
		rms += frame[ii] * frame[ii];
	}
	qframe->SetRMS(float(sqrt(rms / framesize)));

	// normalize & centre-clip the waveform
	float	max = 0;
	for (ii = 0; ii < framesize; ii++)
	{
		if (frame[ii] > max)		max = frame[ii];
		else if (-frame[ii] > max)	max = -frame[ii];
	}
	if (max > 0)
	{
		for (ii = 0; ii < framesize; ii++)
		{
			if (frame[ii] > max/3)			frame[ii] = frame[ii] / max;
			else if (frame[ii] < -max/3)	frame[ii] = frame[ii] / max;
			else							frame[ii] = 0;
		}
	}

	// find pitch
	qframe->pitch = 0;
	float	minerr = float(framesize) * 10000;

	for (ii = 2; ii < framesize/2; ii++)
	{
		float	err = 0;

		for (int jj = 0; jj < framesize; jj++)
		{
			if (jj % ii)
			{
				err += frame[jj] * frame[jj];
			}
			else
			{
				err += (4 - frame[jj]) * (4 - frame[jj]);
			}
		}

		if (err < minerr)
		{
			qframe->pitch = ii;
			minerr = err;
		}
	}
}
#endif	// ENCODE

// ~QVOX
//
// destructor

QVOX::~QVOX()
{
	delete[] qframe;
	delete[] rframe;
}

#if ENCODE
// Save
//
// save QVOX as file

int QVOX::Save(char* filename)
{
	HANDLE	hFile;
	DWORD	length;
	QFrame	*tmp;
	char	*rtmp;

	hFile = CreateFile(filename, GENERIC_READ + GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		TRACE("Can't open file %s\n", filename);
		return 1;
	}

	// write header
	tmp = qframe;
	qframe = NULL;
	rtmp = rframe;
	rframe = NULL;
	::WriteFile(hFile, this, sizeof(*this), &length, NULL);
	qframe = tmp;
	rframe = rtmp;

	// write frames
	::WriteFile(hFile, qframe, sizeof(qframe[0])*nframes, &length, NULL);
	SetEndOfFile(hFile);

	CloseHandle(hFile);
	return 0;
}

// Dump
//
// dump QVOX to file

int QVOX::Dump(char* filename)
{
	FILE	*fd;

	fd = fopen(filename, "w");
	if (fd)
	{
		QFrame	*qptr;
		int		ii;

		fprintf(fd, "Frames      %d\n", nframes);
		fprintf(fd, "Framesize   %d\n", framesize);
		fprintf(fd, "Sample Rate %dHz\n\n", samplerate);

		fprintf(fd, "Frm# rc0  rc1  rc2  rc3  rc4  rc5  rc6  rc7  Pit RMS\n");
		for (ii = 0, qptr = qframe; ii < nframes; ii++, qptr++)
		{
			fprintf(fd, "%-4d %-4d %-4d %-4d %-4d ", ii, qptr->rc[0], qptr->rc[1], qptr->rc[2], qptr->rc[3]);
			fprintf(fd, "%-4d %-4d %-4d %-4d ", qptr->rc[4], qptr->rc[5], qptr->rc[6], qptr->rc[7]);
			fprintf(fd, "%-3d %-3d\n", qptr->pitch, qptr->rms);
		}
		fclose(fd);
		return 0;
	}

	return 1;
}
#endif

// QVOX_Instance
//
// create an instance of a QVOX - i.e. someone saying it

QVOX_Instance::QVOX_Instance(QVOX* qvox, Speaker* speaker, int rate, bool looping) :	qvox(qvox), 
																						speaker(speaker), 
																						pred((lp_order << speaker->deep) >> speaker->shallow),
																						looping(looping)
{
	framenum = 0;
	framesize = (int)qvox->framesize * rate / 50;
}

// NextFrame
//
// get more data from QVOX instance - returns 1 if there is no more

int QVOX_Instance::NextFrame(float* frame, int testmode, int play_residual)
{
	float	rc[lp_order*2];
	int		ii;

	if (framenum >= qvox->nframes)
	{
		if (looping)	framenum = 0;
		else			return 1;
	}

	// decode RCs
	if (speaker.type->deep)
	{
		for (ii = 0; ii < lp_order; ii++)
		{
			rc[ii*2] = qvox->qframe[framenum].GetRC(ii);
			rc[ii*2+1] = rc[ii*2] * 0.707F;
		}
	}
	else
	{
		for (ii = 0; ii < lp_order; ii++)
		{
			rc[ii] = qvox->qframe[framenum].GetRC(ii);
		}
	}

	// run speaker
	if (!play_residual)
	{
		// use the speaker to generate the residual
		speaker.NextFrame(frame, framesize, qvox->qframe[framenum].pitch, qvox->qframe[framenum].rms, testmode);
	}
	else if (qvox->rframe)
	{
		// copy the residual over
		// at reduced speed some parts are duplicated
		// at increased speed some parts are missed
		char*	rptr = qvox->rframe + framenum * qvox->framesize;

		for (ii = 0; ii < framesize; ii++)
		{
			frame[ii] = float(*rptr++) / 127;
		}
	}
	else
	{
		// no residual - silence
		for (ii = 0; ii < framesize; ii++)
		{
			frame[ii] = 0.0;
		}
	}

	if (!testmode)
	{
		// run filter
		pred.ForwardFilter(frame, framesize, rc, frame);
	}

	framenum++;

	return 0;
}
