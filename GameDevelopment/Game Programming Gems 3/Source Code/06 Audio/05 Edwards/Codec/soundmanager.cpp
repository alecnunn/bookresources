// soundmanager.cpp
//
// sound manager for DirectSound

#include "stdafx.h"
#include "soundmanager.h"

// current sound manager
SoundManager* SoundManager::TheManager = NULL;

// forward declaration
void CALLBACK EXPORT GlobalTimerCallback(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);

// SoundClient
//
// create a sound client

SoundClient::SoundClient()
{
	prev = next = this;
	lvol = rvol = 1.0;
	
	bufferptr = NULL;
	buffercnt = 0;
}

// ~SoundClient
//
// delete a sound client

SoundClient::~SoundClient()
{
}

// GetMoreData
//
// return silence - override this to implement
// a device with fixed block size
//
// the returned buffer *must* remain intact until
// the next call to GetMoreData

int SoundClient::GetMoreData(float** ptr, int* cnt)
{
	*ptr = NULL;
	*cnt = 0;

	return(-1);
}

// WriteIntoStereoBuffer
//
// write more data into the supplied buffer
// override this and MixIntoStereoBuffer to implement
// supplier-type devices

void SoundClient::WriteIntoStereoBuffer(float* ptr, int cnt)
{
	while (cnt)
	{
		// copy from the buffer
		while (buffercnt && cnt)
		{
			*ptr++ = *bufferptr * lvol;
			*ptr++ = *bufferptr * rvol;
			bufferptr++;
			buffercnt--;
			cnt--;
		}
		// refill the buffer
		if (cnt)
		{
			GetMoreData(&bufferptr, &buffercnt);
			if (!bufferptr)
			{
				while (cnt--)
				{
					*ptr++ = 0;
					*ptr++ = 0;
				}
			}
		}
	}
}

// MixIntoStereoBuffer
//
// mix more data into the supplied buffer
// override this and WriteIntoStereoBuffer to implement
// supplier-type devices

void SoundClient::MixIntoStereoBuffer(float* ptr, int cnt)
{
	while (cnt)
	{
		// copy from the buffer
		while (buffercnt && cnt)
		{
			*ptr++ += *bufferptr * lvol;
			*ptr++ += *bufferptr * rvol;
			bufferptr++;
			buffercnt--;
			cnt--;
		}
		// refill the buffer
		if (cnt)
		{
			GetMoreData(&bufferptr, &buffercnt);
			if (!bufferptr)	return;
		}
	}
}

// RunChain
//
// causes cnt stereo samples to be calculated and written
// tmp has room for cnt stereo samples
//
// note: this is called on the root SoundClient, which
// is *not* run

void SoundClient::RunChain(short* ptr, int cnt, float* tmp)
{
	SoundClient	*client;
	int			mixin;
	
	mixin = 0;

	for (client = next; client != this; client = client->next)
	{
		if (!mixin)
		{
			client->WriteIntoStereoBuffer(tmp, cnt);
			mixin = 1;
		}
		else
		{
			client->MixIntoStereoBuffer(tmp, cnt);
		}
	}

	if (mixin)
	{
		int	ii;
		int	smp;

		// convert to short
		for (ii = 0; ii < 2*cnt; ii++)
		{
			smp = (int)(tmp[ii] * 32767.0);
			if (smp < -32768)		smp = -32768;
			else if (smp > 32767)	smp = 32767;
			*ptr++ = smp;
		}
	}
	else
	{
		int	ii;

		for (ii = 0; ii < 2*cnt; ii++)	*ptr++ = 0;
	}
}

// LinkAfter
//
// link after a sound client

void SoundClient::LinkAfter(SoundClient* other)
{
	Unlink();
	this->next = other->next;
	this->prev = other;

	this->next->prev = this;
	this->prev->next = this;
}

// LinkBefore
//
// link before a sound client

void SoundClient::LinkBefore(SoundClient* other)
{
	Unlink();
	this->next = other;
	this->prev = other->prev;

	this->next->prev = this;
	this->prev->next = this;
}

// Unlink
//
// unlink

void SoundClient::Unlink()
{
	if (this->next != this)
	{
		this->next->prev = this->prev;
		this->prev->next = this->next;
		this->prev = this->next = this;
	}
}

// SoundManager
//
// create the sound manager

SoundManager::SoundManager(HWND hWnd) : hWnd(hWnd)
{
	if (!TheManager)
	{
		HRESULT			hr;

		hTimer = 0;
		WritePtr = 0;

		if (DirectSoundCreate(NULL, &lpDirectSound, NULL) == DS_OK)
		{
			lpDirectSound->SetCooperativeLevel(hWnd, DSSCL_NORMAL);

			// Set up wave format structure.
		    memset(&SoundBufferFmt, 0, sizeof(SoundBufferFmt));
		    SoundBufferFmt.wf.wFormatTag = WAVE_FORMAT_PCM;
			SoundBufferFmt.wf.nChannels = 2;
			SoundBufferFmt.wf.nSamplesPerSec = 8000;
		    SoundBufferFmt.wf.nBlockAlign = 4;
			SoundBufferFmt.wf.nAvgBytesPerSec = SoundBufferFmt.wf.nSamplesPerSec * SoundBufferFmt.wf.nBlockAlign;
			SoundBufferFmt.wBitsPerSample = 16;

		    // Set up DSBUFFERDESC structure.
			memset(&SoundBufferDesc, 0, sizeof(SoundBufferDesc));
			SoundBufferDesc.dwSize = sizeof(SoundBufferDesc);
			SoundBufferDesc.dwFlags = 0;//DSBCAPS_CTRLDEFAULT | DSBCAPS_CTRLPOSITIONNOTIFY;		// pan, volume, frequency
			SoundBufferDesc.dwBufferBytes = SoundBufferFmt.wf.nAvgBytesPerSec * 200 / 1000;
			SoundBufferDesc.lpwfxFormat = (LPWAVEFORMATEX)&SoundBufferFmt;

			// Create buffer.
			hr = lpDirectSound->CreateSoundBuffer(&SoundBufferDesc, &lpSoundBuffer, NULL);
			if (hr == DS_OK)
			{
				// create float buffer matching the DS buffer
				FloatBuffer = new float[SoundBufferDesc.dwBufferBytes / 2];

				// fill buffer initially
				WriteData(SoundBufferDesc.dwBufferBytes);

				// start timer proper
				hTimer = SetTimer(hWnd, 666, 5, GlobalTimerCallback);

				if (hTimer)
				{
					// success
					lpSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
					TheManager = this;
					return;
				}
			}
		}
	}
	// failed - TheManager is NULL
	MessageBox(hWnd, "Unable to initialize DirectSound\n", NULL, MB_OK);
	SendMessage(hWnd, WM_CLOSE, 0, 0);
}

// ~SoundManager
//
// delete the sound manager

SoundManager::~SoundManager()
{
	if (TheManager)
	{
		// disable callbacks
		TheManager = NULL;

		// kill timer
		if (hTimer)
		{
			KillTimer(hWnd, 666);
		}


		// destroy DirectSoundBuffer object
		lpSoundBuffer->Release();

		// destroy DirectSound object
		lpDirectSound->Release();
	}	
}

// Exit
//
// exit program

void SoundManager::Exit()
{
	if (TheManager)	delete TheManager;
}

// Pause
//
// pause the sound manager

void SoundManager::Pause()
{
	lpSoundBuffer->Stop();
}

// Resume
//
// resume after a pause

void SoundManager::Resume()
{
	WritePtr = 0;
	lpSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
}

// GlobalTimerCallback
//
// callback for use by DirectSound

void CALLBACK EXPORT GlobalTimerCallback(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	if (SoundManager::TheManager)
	{
		SoundManager::TheManager->TimerCallback();
	}
}

// TimerCallback
//
// object callback from DirectSound

void SoundManager::TimerCallback(void)
{
	DWORD		dwPlayPtr;
	DWORD		dwDontCare;
	long		bytes;

	// fill from write cursor to play cursor
	lpSoundBuffer->GetCurrentPosition(&dwPlayPtr, &dwDontCare);//NULL);

	bytes = dwPlayPtr - WritePtr;
	if (bytes < 0)	bytes += SoundBufferDesc.dwBufferBytes;

	WriteData(bytes);
}

// WriteData
//
// causes (bytes) of data to be written to the DS buffer

void SoundManager::WriteData(DWORD bytes)
{
	HRESULT		hr;
	LPVOID		lpvPtr1, lpvPtr2;
	DWORD		dwBytes1, dwBytes2;

	hr = lpSoundBuffer->Lock(WritePtr, bytes, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);
	if (hr == DSERR_BUFFERLOST)
	{
		lpSoundBuffer->Restore();
		hr = lpSoundBuffer->Lock(WritePtr, bytes, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);
	}

	if (hr == DS_OK)
	{
		if (dwBytes1)	RootClient.RunChain((short *)lpvPtr1, dwBytes1/4, FloatBuffer);
		if (dwBytes2)	RootClient.RunChain((short *)lpvPtr2, dwBytes2/4, FloatBuffer);

		lpSoundBuffer->Unlock(lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);

		WritePtr += bytes;
		if (WritePtr >= SoundBufferDesc.dwBufferBytes)
		{
			WritePtr -= SoundBufferDesc.dwBufferBytes;
		}
	}
}

// AttachClient
//
// attach a client

void SoundManager::AttachClient(SoundClient* other)
{
	other->LinkAfter(&RootClient);
}

// SineWaveTest
//
// create sine-wave test client

SineWaveTest::SineWaveTest()
{
	int		ii;
	double	pi;

	pi = 4.0 * atan(1.0);

	for (ii = 0; ii < 400; ii++)
	{
		sinebuffer[ii] = (float)sin((ii & 7) * 2 * pi / 8);
	}
}

int SineWaveTest::GetMoreData(float** ptr, int* len)
{
	*ptr = sinebuffer;
	*len = 400;

	return 0;
}

// QVOX_Client
//
// create QVOX client

QVOX_Client::QVOX_Client()
{
	buflen = 320;
	buffer = new float[buflen];
	framesize = 160;
	qvoxi = NULL;
	testmode = 0;
	play_residual = 1;
}

QVOX_Client::~QVOX_Client()
{
	Stop();
	delete buffer;
}

void QVOX_Client::Play(QVOX* qvox, Speaker* speaker, int rate)
{
	if (qvoxi)	delete qvoxi;
	qvoxi = new QVOX_Instance(qvox, speaker, rate, true);
	framesize = qvoxi->FrameSize();
	if (framesize > buflen)
	{
		delete buffer;
		buflen = framesize;
		buffer = new float[buflen];
	}
}

void QVOX_Client::Stop()
{
	if (qvoxi)	delete qvoxi;
	qvoxi = NULL;
}

int QVOX_Client::GetMoreData(float** ptr, int* len)
{
	*ptr = buffer;
	*len = framesize;

	if (qvoxi && !qvoxi->NextFrame(buffer, testmode, play_residual))
	{
		return 0;
	}

	for (int ii = 0; ii < framesize; ii++)
	{
		buffer[ii] = 0;
	}

	return 0;
}
