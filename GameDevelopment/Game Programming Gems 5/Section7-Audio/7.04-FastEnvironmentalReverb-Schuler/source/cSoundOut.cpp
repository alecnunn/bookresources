
#include "stdafx.h"
// #include "DSPdemo.h"

#include <mmsystem.h>
#include <dsound.h>
#include <assert.h>
#include <math.h>

#include "cSoundOut.h"


#define SAMPLERATE 32000


cSoundOut::cSoundOut()
{
	m_pDS = 0;
	m_pDSB = 0;
	m_UpdateCursor = 0;
	m_BufferBytes = 0;
	memset( &m_BufferCaps, 0, sizeof( m_BufferCaps ) );
	m_BufferCaps.dwSize = sizeof( m_BufferCaps );
	m_bCanWrite = false;
}

cSoundOut::~cSoundOut()
{
	Stop();
	Release();
}

//////

void cSoundOut::Init( HWND hWnd, bool stereo )
{
	// Initialize the Direct Sound Interface

	if( DirectSoundCreate( 0, &m_pDS, 0 ) != DS_OK ) {
		AfxMessageBox( "Unable to open the sound device." );
		return;
	}

	if( m_pDS->SetCooperativeLevel( hWnd, DSSCL_NORMAL ) != DS_OK ) {
		AfxMessageBox( "Unable to set the cooperative level." );
		return;
	}

	WAVEFORMATEX wformat;
	memset( &wformat, 0, sizeof( wformat ) );
	wformat.wFormatTag = WAVE_FORMAT_PCM;
	wformat.nChannels = stereo ? 2 : 1;
	wformat.wBitsPerSample = 16;
	wformat.nBlockAlign = stereo ? 4 : 2;
	wformat.nSamplesPerSec = m_Samplerate;
	wformat.nAvgBytesPerSec = wformat.nBlockAlign * m_Samplerate;

	DSBUFFERDESC desc;
	memset( &desc, 0, sizeof( desc ) );
	desc.dwSize = sizeof( desc );
	desc.dwFlags = DSBCAPS_STICKYFOCUS;
	// size the buffer so that on average there are 5 cycles per second
	// also make it a multiple of 256 bytes
	desc.dwBufferBytes = ( 256 + wformat.nAvgBytesPerSec / 5 ) & ( 0xffffff00 );
	desc.lpwfxFormat = &wformat;

	if( m_pDS->CreateSoundBuffer( &desc, &m_pDSB, 0 ) != DS_OK ) {
		AfxMessageBox( "Failure to create an appropriate sound buffer." );
		return;
	}

	if( m_pDSB->GetCaps( &m_BufferCaps ) != DS_OK ) {
		AfxMessageBox( "Could not obtain buffer caps!" );
		return;
	}

	m_BlockAlign = wformat.nBlockAlign;
	m_AvgBytesPerSec = wformat.nAvgBytesPerSec;
	m_BufferBytes = m_BufferCaps.dwBufferBytes;
	m_UpdateCursor = 0;

	
	m_OutputWaveFile.SetChannels( wformat.nChannels );
	m_OutputWaveFile.SetSampleRate( wformat.nSamplesPerSec );
	m_OutputWaveFile.SetBitsPerSample( wformat.wBitsPerSample );

	if( m_OutputWaveFile.Open( 
			"record.wav", 
			CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone | CFile::typeBinary, 
			0 ) )
	{
		m_OutputWaveFile.WriteWaveHeader();
		m_bCanWrite = true;
	}
	else
		m_bCanWrite = false;
}

void cSoundOut::Release()
{
	if( m_bCanWrite )
	{
		m_OutputWaveFile.FinalizeWaveHeader();
		m_OutputWaveFile.Close();
		m_bCanWrite = false;
	}

	if( m_pDSB ) { m_pDSB->Release(); m_pDSB = 0; }
	if( m_pDS ) { m_pDS->Release(); m_pDS = 0; }
}

void cSoundOut::Play()
{
	short *samples;
	unsigned long len;
	
	if( m_pDSB->Lock( 0, m_BufferBytes, (void**) &samples, &len, 0, 0, 0 ) != DS_OK ) {
		AfxMessageBox( "Could not Lock buffer!" );
		return;
	}

	// clear the buffer with silence
	for( unsigned i = 0; i < len / m_BlockAlign; ++i ) samples[i] = (short) 0;

	if( m_pDSB->Unlock( samples, len, 0, 0 ) != DS_OK ) {
		AfxMessageBox( "Could not Unlock buffer!" );
		return;
	}

	if( m_pDSB->Play( 0, 0, DSBPLAY_LOOPING ) != DS_OK ) {
		AfxMessageBox( "Could not issue play command" );
		return;
	}

	m_Playing = true;
}

void cSoundOut::Stop()
{
	m_Playing = false;
	if( m_pDSB && m_Playing ) m_pDSB->Stop();
}

unsigned long cSoundOut::QueryUpdateSize() const
{
	// return how many samples the play cursor has 
	// already run ahead of the update cursor
	assert( m_pDSB );
	unsigned long result = 0;
	unsigned long playcursor = 0;
	unsigned long status = 0;

	if( m_Playing &&
		m_pDSB->GetCurrentPosition( &playcursor, 0 ) == DS_OK ) {
		while( playcursor < m_UpdateCursor ) playcursor += m_BufferBytes;
		result = ( playcursor - m_UpdateCursor ) / m_BlockAlign;
		result = result & -16;
	}
	assert( result * ( m_BlockAlign ) <= m_BufferBytes );
	assert( ( result & 15 ) == 0 );
	return result;
}

unsigned long cSoundOut::QueryUpdateInterval() const
{
	// return the recommended average update interval in milliseconds
	// which is half the time of one buffer cycle
	assert( m_pDSB );
	unsigned long result = 0;
	result = 500 * m_BufferBytes / m_AvgBytesPerSec / m_Samplerate;
	assert( result < 10000 );
	return result;
}

void cSoundOut::Update( const cSoundBuffer<float> &wavedata )
{
	float *readpointer = wavedata.GetData();
	unsigned numsamples = wavedata.GetSize();

	assert( m_pDSB );
	assert( numsamples * m_BlockAlign <= m_BufferBytes );

	// check to see if the DirectSound buffer was lost
	unsigned long status = 0;
	if( m_pDSB->GetStatus( &status ) != DS_OK ) assert( false );
	if( status & DSBSTATUS_BUFFERLOST ) {
		// try to recover
		if( m_pDSB->Restore() != DS_OK ) {
			m_UpdateCursor = 0;
			AfxMessageBox( "Direct Sound buffer was lost and could not be recovered.", MB_APPLMODAL );
			Stop();
			return;
		}
	}

	// Obtain the write pointers from MicroSoft (TM) DirectSound (TM)
	// gmblmpfsxx (TM)
	short *(writepointer[2]) = { 0, 0 };
	unsigned long numbytes[2] = { 0, 0 };
	
	int result;
	if( ( result = m_pDSB->Lock( m_UpdateCursor, numsamples * m_BlockAlign, (void**) &writepointer[0], 
		&numbytes[0], (void**) &writepointer[1], &numbytes[1], 0 ) ) != DS_OK ) {
		Stop();
		CString message;
		message.Format( "Could not Lock Buffer - Error Code %x", result );
		AfxMessageBox( message, MB_APPLMODAL );
		return;
	}

	unsigned i = 0, k = 0;

	for( k = 0; k < 2; ++k ) 
	{
		
		short *dest = writepointer[k];
		unsigned numsamples = numbytes[k] / m_BlockAlign;
			
		for( i = 0; i < numsamples; ++i ) 
		{
			float y = *readpointer++;
			if( y > 1 ) y = 1;
			if( y < -1 ) y = -1;
			dest[i] = (short) ( y * 32767.5f - 0.5f );
		}		
	}

	if( m_bCanWrite )
	{
		m_OutputWaveFile.Write( writepointer[0], numbytes[0] );	
		m_OutputWaveFile.Write( writepointer[1], numbytes[1] );	
	}
		
	// Unlock the buffers 'cause we're done
	if( m_pDSB->Unlock( writepointer[0], numbytes[0], writepointer[1], numbytes[1] ) != DS_OK ) assert( false );
	
	// Memorize the position up to which was updated
	m_UpdateCursor += numsamples * m_BlockAlign;
	while( m_UpdateCursor >= m_BufferBytes ) m_UpdateCursor -= m_BufferBytes;	

}

void cSoundOut::UpdateStereo( const cSoundBuffer<float> &wavedataL, const cSoundBuffer<float> &wavedataR )
{
	float *readpointerL = wavedataL.GetData();
	float *readpointerR = wavedataR.GetData();
	unsigned numsamples = wavedataL.GetSize();

	assert( m_pDSB );
	assert( numsamples * m_BlockAlign <= m_BufferBytes );

	// check to see if the DirectSound buffer was lost
	unsigned long status = 0;
	if( m_pDSB->GetStatus( &status ) != DS_OK ) assert( false );
	if( status & DSBSTATUS_BUFFERLOST ) {
		// try to recover
		if( m_pDSB->Restore() != DS_OK ) {
			m_UpdateCursor = 0;
			AfxMessageBox( "Direct Sound buffer was lost and could not be recovered.", MB_APPLMODAL );
			Stop();
			return;
		}
	}

	// Obtain the write pointers from MicroSoft (TM) DirectSound (TM)
	// gmblmpfsxx (TM)
	short *(writepointer[2]) = { 0, 0 };
	unsigned long numbytes[2] = { 0, 0 };
	
	int result;
	if( ( result = m_pDSB->Lock( m_UpdateCursor, numsamples * m_BlockAlign, (void**) &writepointer[0], 
		&numbytes[0], (void**) &writepointer[1], &numbytes[1], 0 ) ) != DS_OK ) {
		Stop();
		CString message;
		message.Format( "Could not Lock Buffer - Error Code %x", result );
		AfxMessageBox( message, MB_APPLMODAL );
		return;
	}

	unsigned i = 0, k = 0;

	for( k = 0; k < 2; ++k ) 
	{
		
		short *dest = writepointer[k];
		unsigned numsamples = numbytes[k] / m_BlockAlign;
			
		for( i = 0; i < numsamples; ++i ) 
		{
			// hard clipping
			float y = *readpointerL++;
			if( y > 1 ) y = 1;
			if( y < -1 ) y = -1;
			dest[2*i] = (short) ( y * 32767.5f - .5f );
			y = *readpointerR++;
			if( y > 1 ) y = 1;
			if( y < -1 ) y = -1;
			dest[2*i + 1] = (short) ( y * 32767.5f - 0.5f );

			/*
			// atan clipping
			float y = *readpointerL++;
			y = atanf( 1.571f * y ) * 0.636f;
			dest[2*i] = (short) ( y * 32767.5f - .5f );
			y = *readpointerR++;
			y = atanf( 1.571f * y ) * 0.636f;
			dest[2*i + 1] = (short) ( y * 32767.5f - 0.5f );
			*/

			/*
			// hyperbolic clipping
			float y = *readpointerL++;
			if( y > 0 ) y = y / ( 1.f + y );
			else y = y / ( 1.f - y );
			dest[2*i] = (short) ( y * 32767.5f - .5f );
			y = *readpointerR++;
			if( y > 0 ) y = y / ( 1.f + y );
			else y = y / ( 1.f - y );
			dest[2*i + 1] = (short) ( y * 32767.5f - 0.5f );
			*/
		}
	}
		
	if( m_bCanWrite )
	{
		m_OutputWaveFile.Write( writepointer[0], numbytes[0] );	
		m_OutputWaveFile.Write( writepointer[1], numbytes[1] );	
	}
	
	// Unlock the buffers 'cause we're done
	if( m_pDSB->Unlock( writepointer[0], numbytes[0], writepointer[1], numbytes[1] ) != DS_OK ) assert( false );
	
	// Memorize the position up to which was updated
	m_UpdateCursor += numsamples * m_BlockAlign;
	while( m_UpdateCursor >= m_BufferBytes ) m_UpdateCursor -= m_BufferBytes;	
}

void cSoundOut::SetSamplerate( unsigned rate )
{
	m_Samplerate = rate;
	if( m_pDSB ) m_pDSB->SetFrequency( rate );
}