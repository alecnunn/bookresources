
#include "stdafx.h"
#include "WaveFile.h"

#include <mmsystem.h>


CWaveFile::CWaveFile()
{

}

CWaveFile::~CWaveFile()
{

}

bool CWaveFile::ReadWaveHeader()
{
	bool format_chunk_read = false;
	bool data_chunk_encounter = false;

	struct 
	{     
		FOURCC id;  			// identifier string = "RIFF"
		DWORD len;    		// remaining length after this header
	} 
	riff_header;

	// validate the riff header
	Read( &riff_header, sizeof( riff_header ) );
	if( riff_header.id != MAKEFOURCC( 'R', 'I', 'F', 'F' ) ) goto error;	
	if( riff_header.len != GetLength() - 8 ) goto error;

	FOURCC filetype;
	
	// validate we have a wave-file
	Read( &filetype, sizeof( filetype ) );
	if( filetype != MAKEFOURCC( 'W', 'A', 'V', 'E' ) ) goto error;


	struct 
	{		
		FOURCC id;			// identifier, e.g. "fmt " or "data"
		DWORD len; 		// remaining chunk length after header
	} 
	chunk_header;
	
	
	// loop through all the chunks in the file
	// and pick out the information we need
	
	try 
	{
		while( !data_chunk_encounter ) 
		{
			Read( &chunk_header, sizeof( chunk_header ) );
			unsigned pos = GetPosition();
			
			switch( chunk_header.id ) 
			{
			case MAKEFOURCC( 'f', 'm', 't', ' ' ):
				if( chunk_header.len < sizeof( m_FormatChunk ) ) goto error;
				Read( &m_FormatChunk, sizeof( m_FormatChunk ) );
				if( m_FormatChunk.wFormatTag != WAVE_FORMAT_PCM ) goto error;
				format_chunk_read = true;
				break;

			case MAKEFOURCC( 'd', 'a', 't', 'a' ):
				if( !format_chunk_read ) goto error;
				m_PositionOfDataBody = GetPosition();
				data_chunk_encounter = true;
				break;
			}
			
			Seek( pos + chunk_header.len, CFile::begin );
		}
	}

	catch( CFileException ex )
	{ 
		if( ex.m_cause != CFileException::endOfFile ) {
			goto error; 
		}
	}

	return true;

error:
	Close();
	return false;
}


void CWaveFile::WriteWaveHeader()
{
	struct 
	{     
		FOURCC id; 			// identifier string
		DWORD len;    		// remaining length after this header
	} 
	header;

	// write RIFF header
	header.id = MAKEFOURCC( 'R', 'I', 'F', 'F' );
	header.len = 0;
	Write( &header, sizeof( header ) );

	// write file type
	FOURCC filetype = MAKEFOURCC( 'W', 'A', 'V', 'E' );
	Write( &filetype, sizeof( filetype ) );

	// write format chunk header
	header.id = MAKEFOURCC( 'f', 'm', 't', ' ' );
	header.len = sizeof( m_FormatChunk );
	Write( &header, sizeof( header ) );

	// write format chunk
	m_FormatChunk.wFormatTag = WAVE_FORMAT_PCM;
	m_FormatChunk.wBlockAlign = m_FormatChunk.wChannels * m_FormatChunk.wBitsPerSample / 8;
	m_FormatChunk.dwAvgBytesPerSec = m_FormatChunk.wBlockAlign * m_FormatChunk.dwSamplesPerSec;
	Write( &m_FormatChunk, sizeof( m_FormatChunk ) );

	// write data chunk header
	header.id = MAKEFOURCC( 'd', 'a', 't', 'a' );
	header.len = 0;
	Write( &header, sizeof( header ) );
}


void CWaveFile::FinalizeWaveHeader()
{
	DWORD len;

	// fix file length #1
	len = GetLength() - 8;
	Seek( 4, CFile::begin );
	Write( &len, 4 );

	// fix file length #2
	len = GetLength() - ( 28 + sizeof( m_FormatChunk ) );
	Seek( 24 + sizeof( m_FormatChunk ), CFile::begin );
	Write( &len, 4 );
}
