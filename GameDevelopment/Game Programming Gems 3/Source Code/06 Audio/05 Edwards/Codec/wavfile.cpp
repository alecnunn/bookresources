// wavfile.cpp
//
// WAV file object
//
// see wavfile.h for details

#include "stdafx.h"
#include <MMsystem.h>
#include "wavfile.h"
#include <cmath>

// WavFile
//
// new object

WavFile::WavFile()
{
	valid = 0;
}

// WavFile
//
// copy object

WavFile::WavFile(WavFile* cp)
{
	int	ii;

	valid = 0;

	if (!cp->valid)
	{
		return;
	}

	WaveInfo = cp->WaveInfo;
	dlen = cp->dlen;
	valid = 1;
	offset = 0;
	loopflag = 0;
	lptr = new float[dlen];
	rptr = cp->rptr ? new float[dlen] : NULL;

	for (ii = 0; ii < dlen; ii++)
	{
		lptr[ii] = cp->lptr[ii];
	}
	if (rptr)
	{
		for (ii = 0; ii < dlen; ii++)
		{
			rptr[ii] = cp->rptr[ii];
		}
	}
}

// ~WavFile
//
// delete object

WavFile::~WavFile()
{
	Destroy();
}

// Create
//
// create a WAV file

void WavFile::Create(int length, int rate, int depth, float* lbuf, float* rbuf)
{
	int	ii;
	int	channels;

	if (valid) Destroy();

	channels = rbuf ? 2 : 1;

	if (channels == 1)
	{
		lptr = new float[length];
		rptr = NULL;
		for (ii = 0; ii < length; ii++)
		{
			lptr[ii] = lbuf[ii];
		}
	}
	else
	{
		lptr = new float[length];
		rptr = new float[length];
		for (ii = 0; ii < length; ii++)
		{
			lptr[ii] = lbuf[ii];
			rptr[ii] = rbuf[ii];
		}
	}

	WaveInfo.wf.wFormatTag = WAVE_FORMAT_PCM;
	WaveInfo.wf.nChannels = channels;
	WaveInfo.wf.nSamplesPerSec = rate;
	WaveInfo.wf.nAvgBytesPerSec = rate * channels * depth / 8;
	WaveInfo.wf.nBlockAlign = depth * channels / 8;
	WaveInfo.wBitsPerSample = depth;

	valid = 1;
}

// Destroy
//
// destroy a WAV file

void WavFile::Destroy()
{
	if (valid)
	{
		if (lptr)	delete lptr;
		if (rptr)	delete rptr;
	}
	valid = 0;
}

// Load
//
// load a WAV file

void WavFile::Load(char* filename)
{
	HANDLE	hFile;		// WAV file
	HANDLE	hFilemap;	// mapped WAV file
	LPVOID	lpFileview;	// view of mapped WAV file

	if (valid)	Destroy();

	// initialise
	lptr = NULL;
	rptr = NULL;
	dlen = 0;
	offset = 0;
	loopflag = 0;

	// open file
    hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
	{
		TRACE("File %s not found\n", filename);
		return;
	}

	hFilemap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	CloseHandle(hFile);
	if (hFilemap == INVALID_HANDLE_VALUE)
	{
		TRACE("Couldn't map file %s\n", filename);
		return;
	}

    lpFileview = MapViewOfFile(hFilemap, FILE_MAP_READ, 0, 0, 0);
	CloseHandle(hFilemap);
	if (lpFileview)
	{
		TRACE("File %s loaded OK ... checking format\n", filename);

		ReadFile(lpFileview);

		UnmapViewOfFile(lpFileview);
	}
}

// Save
//
// save a WAV file

int WavFile::Save(char* filename)
{
	HANDLE	hFile;

	if (!valid)	return 1;

	hFile = CreateFile(filename, GENERIC_READ + GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		TRACE("Can't open file %s\n", filename);
		return 1;
	}

	WriteFile(hFile);
	CloseHandle(hFile);
	return 0;
}

// ReadFile
//
// decode the mapped WAV file

void WavFile::ReadFile(LPVOID fileview)
{
	LPDWORD			lpDWData;
	LPDWORD			lpDWDataEnd;
	DWORD			dwRiff;
	DWORD			dwLength;
	DWORD			dwType;
	DWORD			dwWaveDataSize;
	short			*wp;
	BYTE			*bp;
	int				GotWaveInfo;

	lpDWData = (LPDWORD)fileview;
	bp = NULL;
	wp = NULL;
	GotWaveInfo = 0;

	// read RIFF header
	dwRiff		= *lpDWData++;
	dwLength	= *lpDWData++;
	dwType		= *lpDWData++;

	if (dwRiff != mmioFOURCC('R', 'I', 'F', 'F'))	return;
	if (dwType != mmioFOURCC('W', 'A', 'V', 'E'))	return;
	lpDWDataEnd = (LPDWORD)((LPBYTE)lpDWData + dwLength - 4);
	
	while (lpDWData < lpDWDataEnd)
	{
		// read chunk header
		dwType		= *lpDWData++;
		dwLength	= *lpDWData++;

		TRACE("%c %c %c %c  %d\n", ((LPBYTE)lpDWData)[-8], ((LPBYTE)lpDWData)[-7], 
									((LPBYTE)lpDWData)[-6], ((LPBYTE)lpDWData)[-5], dwLength);

		// decode chunk
		switch (dwType)
		{
		case mmioFOURCC('f', 'm', 't', ' '):
			if (dwLength < sizeof(PCMWAVEFORMAT))	return;
			WaveInfo = *(LPPCMWAVEFORMAT)lpDWData;
			GotWaveInfo = 1;
			break;

		case mmioFOURCC('d', 'a', 't', 'a'):
			bp = (BYTE *)lpDWData;
			wp = (short *)bp;
			dwWaveDataSize = dwLength;
			break;
		}

		lpDWData = (LPDWORD)((LPBYTE)lpDWData + ((dwLength + 1) & ~1));
	}

	if (!GotWaveInfo)	return;
	if (!bp)			return;

	if (WaveInfo.wf.wFormatTag != WAVE_FORMAT_PCM)
	{
		TRACE("Not PCM\n");
		return;
	}
	TRACE("Sample rate = %d\n", WaveInfo.wf.nSamplesPerSec);
	TRACE("Bits = %d\n", WaveInfo.wBitsPerSample);

	// set length
	dlen = dwWaveDataSize/WaveInfo.wf.nBlockAlign;

	// decode
	if (WaveInfo.wBitsPerSample == 16)
	{
		int	ii;

		if (WaveInfo.wf.nChannels == 1)
		{
			// mono 16-bit
			lptr = new float[dlen];
			rptr = NULL;

			for (ii = 0; ii < dlen; ii++)
			{
				lptr[ii] = (float)*wp++ / 32768;
			}
		}
		else
		{
			// stereo 16-bit
			lptr = new float[dlen];
			rptr = new float[dlen];

			for (ii = 0; ii < dlen; ii++)
			{
				lptr[ii] = (float)*wp++ / 32768;
				rptr[ii] = (float)*wp++ / 32768;
			}
		}				
	}
	else
	{
		int	ii;

		if (WaveInfo.wf.nChannels == 1)
		{
			// mono 8-bit
			lptr = new float[dlen];
			rptr = NULL;

			for (ii = 0; ii < dlen; ii++)
			{
				lptr[ii] = ((float)*bp++ - 128) / 400;
			}
		}
		else
		{
			// stereo 8-bit
			lptr = new float[dlen];
			rptr = new float[dlen];

			for (ii = 0; ii < dlen; ii++)
			{
				lptr[ii] = ((float)*bp++ - 128) / 400;
				rptr[ii] = ((float)*bp++ - 128) / 400;
			}
		}
	}

	WaveInfo.wf.nBlockAlign = WaveInfo.wf.nChannels * WaveInfo.wBitsPerSample / 8;
	WaveInfo.wf.nAvgBytesPerSec = WaveInfo.wf.nBlockAlign * WaveInfo.wf.nSamplesPerSec;
	valid = 1;
}

// WriteFile
//
// write a WAV file

void WavFile::WriteFile(HANDLE hFile)
{
	BYTE			*file;
	LPDWORD			lpDWData;
	DWORD			length;
	short			*wp;
	BYTE			*bp;
	
	length = 8 + 4 + (8 + sizeof(PCMWAVEFORMAT)) + (8 + dlen * WaveInfo.wf.nBlockAlign);

	file = new BYTE[length];

	lpDWData = (LPDWORD)file;

	// write RIFF header
	*lpDWData++ = mmioFOURCC('R', 'I', 'F', 'F');
	*lpDWData++ = length - 8;
	*lpDWData++ = mmioFOURCC('W', 'A', 'V', 'E');

	// write fmt chunk
	*lpDWData++ = mmioFOURCC('f', 'm', 't', ' ');
	*lpDWData++ = sizeof(PCMWAVEFORMAT);
	*(LPPCMWAVEFORMAT)lpDWData = WaveInfo;
	lpDWData = (LPDWORD)((LPBYTE)lpDWData + sizeof(PCMWAVEFORMAT));

	// write data chunk
	*lpDWData++ = mmioFOURCC('d', 'a', 't', 'a');
	*lpDWData++ = dlen * WaveInfo.wf.nBlockAlign;

	bp = (BYTE *)lpDWData;
	wp = (short *)bp;

	// encode
	if (WaveInfo.wBitsPerSample == 16)
	{
		int	ii;

		if (WaveInfo.wf.nChannels == 1)
		{
			// mono 16-bit
			for (ii = 0; ii < dlen; ii++)
			{
				*wp++ = Sample16(lptr[ii]);
			}
		}
		else
		{
			// stereo 16-bit
			lptr = new float[dlen];
			rptr = new float[dlen];

			for (ii = 0; ii < dlen; ii++)
			{
				*wp++ = Sample16(lptr[ii]);
				*wp++ = Sample16(rptr[ii]);
			}
		}				
	}
	else
	{
		int	ii;

		if (WaveInfo.wf.nChannels == 1)
		{
			// mono 8-bit
			for (ii = 0; ii < dlen; ii++)
			{
				*bp++ = Sample8(lptr[ii]);
			}
		}
		else
		{
			// stereo 8-bit
			for (ii = 0; ii < dlen; ii++)
			{
				*bp++ = Sample8(lptr[ii]);
				*bp++ = Sample8(rptr[ii]);
			}
		}
	}

	::WriteFile(hFile, file, length, &length, NULL);
	SetEndOfFile(hFile);

	delete file;
}

// SetDepth
//
// set bit depth (just changes WAV header)

void WavFile::SetDepth(WORD bits)
{
	if (!valid)	return;

	if (bits == 16)
	{
		WaveInfo.wf.nBlockAlign = 2 * WaveInfo.wf.nChannels;
		WaveInfo.wBitsPerSample = 16;
	}
	else if (bits == 8)
	{
		WaveInfo.wf.nBlockAlign = WaveInfo.wf.nChannels;
		WaveInfo.wBitsPerSample = 8;
	}
	WaveInfo.wf.nAvgBytesPerSec = WaveInfo.wf.nSamplesPerSec * WaveInfo.wf.nBlockAlign;
}

// SetChannels
//
// set number of channels

void WavFile::SetChannels(WORD channels)
{
	int	ii;

	if (!valid)	return;

	if (channels == 1)
	{
		if (WaveInfo.wf.nChannels == 1)	return;
		
		for (ii = 0; ii < dlen; ii++)
		{
			lptr[ii] = (lptr[ii] + rptr[ii]) * 0.707F;
		}
		delete rptr;
		rptr = NULL;

		WaveInfo.wf.nChannels = 1;
	}
	else if (channels == 2)
	{
		if (WaveInfo.wf.nChannels == 2)	return;

		rptr = new float[dlen];
		for (ii = 0; ii < dlen; ii++)
		{
			rptr[ii] = lptr[ii];
		}

		WaveInfo.wf.nChannels = 2;
	}

	WaveInfo.wf.nBlockAlign = WaveInfo.wBitsPerSample * WaveInfo.wf.nChannels / 8;
	WaveInfo.wf.nAvgBytesPerSec = WaveInfo.wf.nSamplesPerSec * WaveInfo.wf.nBlockAlign;
}	

// SetRate
//
// set rate (resample)

void WavFile::SetRate(DWORD rate, int window)
{
	float	sintab[4096];
	double	pi = 4.0 * atan(1.0);
	int		ii;
	int		jj;
	int		newlen;
	float*	lptr2;
	float*	rptr2;
	float	omega;
	float	oldrate;

	if (!valid)	return;

	offset = 0;

	if (WaveInfo.wf.nSamplesPerSec == rate)	return;
	oldrate = (float)WaveInfo.wf.nSamplesPerSec;

	// fill sine table
	for (ii = 0; ii < 4096; ii++)
	{
		sintab[ii] = float(sin(2 * pi * ii / 4096));
	}

	// get new length
	newlen = (int)((float)dlen * (float)rate / oldrate + 0.5);

	// new buffers
	lptr2 = new float[newlen];
	rptr2 = rptr ? new float[newlen] : NULL;

	if (rate < WaveInfo.wf.nSamplesPerSec)
	{
		// filter to new Nyquist (downsample)
		omega = float(2 * pi * rate / 2);
	}
	else
	{
		// filter to old Nyquist (upsample)
		omega = float(2 * pi * WaveInfo.wf.nSamplesPerSec / 2);
	}

	for (ii = 0; ii < newlen; ii++)
	{
		float	time = (float)ii / rate;
		float	sum = 0.0;

		// convolute - do (window) samples behind and in front
		int		centre = int(ii * oldrate / rate);
		int		left = centre - window;
		int		right = centre + window;
		if (left < 0)		left = 0;
		if (right > dlen)	right = dlen;

		// left sample
		for (jj = left; jj < right; jj++)
		{
			float	t2 = float(jj / oldrate - time);
			int		ix = int(t2 * rate * 4096 / 2);

			if (t2 == 0.0)	sum += lptr[jj];
			else			sum += lptr[jj] * sintab[ix & 4095] / (t2 * omega);
//			else			sum += lptr[jj] * sin(t2 * omega) / (t2 * omega);
		}

		lptr2[ii] = sum;

		// right sample
		if (rptr)
		{
			for (jj = left; jj < right; jj++)
			{
				float	t2 = float(jj / oldrate - time);
				int		ix = int(t2 * rate * 4096 / 2);

				if (t2 == 0.0)	sum += rptr[jj];
				else			sum += rptr[jj] * sintab[ix & 4095] / (t2 * omega);
			}

			rptr2[ii] = sum;
		}
	}

	delete	lptr;
	if (rptr) delete	rptr;
	lptr = lptr2;
	rptr = rptr2;
	dlen = newlen;

	WaveInfo.wf.nSamplesPerSec = rate;
	WaveInfo.wf.nAvgBytesPerSec = WaveInfo.wf.nSamplesPerSec * WaveInfo.wf.nBlockAlign;
}

// Retrigger
//
// set offset to 0

void WavFile::Retrigger()
{
	offset = 0;
}

// SetLoopFlag
//
// set looping mode

void WavFile::SetLoopFlag(int looping)
{
	loopflag = !!looping;
}

// GetFrameMono
//
// read mono frame out, returns 0 if frame is off end of file

int WavFile::GetFrameMono(float* buffer, int length)
{
	int	ii;
	int	rc;

	if (!valid)
	{
		for (ii = 0; ii < length; ii++)
		{
			buffer[ii] = 0;
		}
		return 0;
	}

	rc = (offset == dlen) ? 0 : 1;

	if (!rptr)
	{
		// mono source
		while (length)
		{
			if (length > dlen - offset)
			{
				for (ii = 0; ii < dlen - offset; ii++)
				{
					*buffer++ = lptr[ii + offset];
				}
				length -= ii;
				if (loopflag)
				{
					offset = 0;
					rc = 1;
				}
				else
				{
					offset = dlen;
					for (ii = 0; ii < length; ii++)
					{
						*buffer++ = 0;
					}
					length = 0;
				}
			}
			else
			{
				for (ii = 0; ii < length; ii++)
				{
					*buffer++ = lptr[ii + offset];
				}
				length -= ii;
				offset += ii;
			}
		}
	}
	else
	{
		// stereo source
		while (length)
		{
			if (length > dlen - offset)
			{
				for (ii = 0; ii < dlen - offset; ii++)
				{
					*buffer++ = (lptr[ii + offset] + rptr[ii + offset]) * 0.707F;
				}
				length -= ii;
				if (loopflag)
				{
					offset = 0;
					rc = 1;
				}
				else
				{
					offset = dlen;
					for (ii = 0; ii < length; ii++)
					{
						*buffer++ = 0;
					}
					length = 0;
				}
			}
			else
			{
				for (ii = 0; ii < length; ii++)
				{
					*buffer++ = (lptr[ii + offset] + rptr[ii + offset]) * 0.707F;
				}
				length -= ii;
				offset += ii;
			}
		}
	}

	return rc;
}

// GetFrameStereo
//
// read stereo frame out, returns 0 if frame is off end of file

int WavFile::GetFrameStereo(float* lbuf, float* rbuf, int length)
{
	int	ii;
	int	rc;

	if (!valid)
	{
		for (ii = 0; ii < length; ii++)
		{
			lbuf[ii] = 0;
			rbuf[ii] = 0;
		}
		return 0;
	}

	rc = (offset == dlen) ? 0 : 1;

	if (!rptr)
	{
		// mono source
		while (length)
		{
			if (length > dlen - offset)
			{
				for (ii = 0; ii < dlen - offset; ii++)
				{
					*lbuf++ = lptr[ii + offset];
					*rbuf++ = lptr[ii + offset];
				}
				length -= ii;
				if (loopflag)
				{
					offset = 0;
					rc = 1;
				}
				else
				{
					offset = dlen;
					for (ii = 0; ii < length; ii++)
					{
						*lbuf++ = 0;
						*rbuf++ = 0;
					}
					length = 0;
				}
			}
			else
			{
				for (ii = 0; ii < length; ii++)
				{
					*lbuf++ = lptr[ii + offset];
					*rbuf++ = lptr[ii + offset];
				}
				length -= ii;
				offset += ii;
			}
		}
	}
	else
	{
		// stereo source
		while (length)
		{
			if (length > dlen - offset)
			{
				for (ii = 0; ii < dlen - offset; ii++)
				{
					*lbuf++ = lptr[ii + offset];
					*rbuf++ = rptr[ii + offset];
				}
				length -= ii;
				if (loopflag)
				{
					offset = 0;
					rc = 1;
				}
				else
				{
					offset = dlen;
					for (ii = 0; ii < length; ii++)
					{
						*lbuf++ = 0;
						*rbuf++ = 0;
					}
					length = 0;
				}
			}
			else
			{
				for (ii = 0; ii < length; ii++)
				{
					*lbuf++ = lptr[ii + offset];
					*rbuf++ = rptr[ii + offset];
				}
				length -= ii;
				offset += ii;
			}
		}
	}

	return rc;
}

// NumFrames
//
// number of frames in file

int WavFile::NumFrames(int length)
{
	return (dlen + length - 1) / length;
}
