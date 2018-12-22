

#include <assert.h>

#include "cDspDiskRead.h"
#include "cSoundBuffer.h"


cDspDiskRead::cDspDiskRead()
{
	m_File = 0;
}

cDspDiskRead::~cDspDiskRead()
{
	Close();
}

bool cDspDiskRead::Open( const char *filename )
{
	Close();
	m_File = fopen( filename, "rb" );
	return m_File != 0;
}

void cDspDiskRead::Close()
{
	if( m_File ) fclose( m_File );
	m_File = 0;
}

void cDspDiskRead::Process( cSoundBuffer<float> &buffer )
{
	assert( m_File );
	
	int numsamples = buffer.GetSize();
	short *source = ( (short *) buffer.GetData() ) + numsamples;
	float *dest = (float *) buffer.GetData();
	fread( source, 2, buffer.GetSize(), m_File );

	const float invscale = 1.f / 32767.5f;
	for( int i = 0; i < numsamples; ++i ) dest[i] = ( float( source[i] ) + .5f ) * invscale;
}

void cDspDiskRead::ProcessStereo( cSoundBuffer<float> &bufferL, cSoundBuffer<float> &bufferR )
{
	assert( m_File );
	
	int numsamples = bufferL.GetSize();

	float *destL = (float *) bufferL.GetData();
	float *destR = (float *) bufferR.GetData();
	short *source = ( (short *) bufferR.GetData() );
	fread( source, 4, bufferR.GetSize(), m_File );

	const float invscale = 1.f / 32767.5f;
	for( int i = 0; i < numsamples; ++i ) 
	{
		destL[i] = ( float( source[2*i] ) + .5f ) * invscale;
		destR[i] = ( float( source[2*i + 1] ) + .5f ) * invscale;
	}
}



bool cDspDiskRead::QueryEOF() const
{ 
	assert( m_File );
	return feof( m_File ) != 0;
}

