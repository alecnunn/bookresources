// GAStream Implementation
// --------------------------------------------------------------------------------------------------------
// System:		ATOMGAUD
// Description:	...
//				...
//				...
// Location:	http://www.visiomedia.com/rooms/labor/src/sphinxmmos/index.htm
// Version:		0202
// Author:		Frank Luchs
// History:
//				2001-10-03	first draft
// --------------------------------------------------------------------------------------------------------
// This is part of Sphinx MMOS, the open source version of Sphinx Modular Media.
// Copyright © 1985-2001 Visiomedia Software Corporation, All Rights Reserved.
// --------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "GAStream.h"


#ifdef ATOMOS
namespace atomos
{
#endif // ATOMOS
// metaclass implementation
void* __stdcall CreateCGAStream() { return( new CGAStream); }
const CClass CGAStream::classCGAStream(CID_GAStream, CID_GAObject, "CGAStream", CreateCGAStream);
const IClass*	CGAStream::GetClass() { return((const IClass*)&classCGAStream); }


// CTOR
CGAStream::CGAStream()
{
	m_pStream = NULL;
	m_nDataType = paFloat32;
	m_nDevice = paNoDevice;
	m_nChannels = 2;
	m_nFlags = paClipOff|paDitherOff; // we won't output out of range samples so don't bother clipping them 

}

// DTOR
CGAStream::~CGAStream()
{

}

void CGAStream::Terminate()
{
	CGAObject::Terminate();
}


void CGAStream::SetChannelCount(UINT32 nChannels)
{
		m_nChannels = nChannels;

}




INT32 CGAStream::Open(UINT32 id)
{
	UINT32 nBlockSize = g_GASystem.GetFrameCount();
	FLOAT64 fSampleRate = (FLOAT64) g_GASystem.GetSampleRate();
	PortAudioCallback*	pCallback = g_GASystem.GetCallback();
	
	INT32 err = Pa_OpenStream(
		&m_pStream,
		paNoDevice,		// input device
		0,              // input channels
		m_nDataType,	// input sample format
		NULL,			// input driver info
		
		id, // DEV_ID_1, // ??nDeviceOut,		 // output device
		m_nChannels,      // output channels
		m_nDataType,      // output sample format
		NULL,				// output driver info
		fSampleRate,
		nBlockSize,
		0,					// number of buffers, if zero then use default minimum
		m_nFlags, 
		pCallback,
		this );		// set this stream as user data

	if( err != paNoError )
		g_GASystem.HandleError(err);


	return(err);
}


void CGAStream::Close()
{
	INT32 err = paNoError;
	if(m_pStream != NULL) 
		err = Pa_CloseStream( m_pStream );

	if( err != paNoError )
		g_GASystem.HandleError(err);

	
}


void CGAStream::Start()
{
	PaError err = paNoError;

	if(m_pStream != NULL) 
		err = Pa_StartStream( m_pStream );

	
}

void CGAStream::Stop()
{
	PaError err = paNoError;

	if(m_pStream != NULL) 
	//	m_Error = Pa_StopStream( m_pStream );
		err = Pa_AbortStream( m_pStream );

}

void CGAStream::Flip()
{
	PaError err = Pa_StreamActive( m_pStream);
	if(err == 1) // active
		Stop();
	else
		Start();
	

}




#ifdef ATOMOS
} // namespace atomos
#endif // ATOMOS
// --------------------------------------------------------------------------------------------------------
