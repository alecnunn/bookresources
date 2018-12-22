#include "WavWriter.h"
#include "WavHeader.h"
#include <memory.h>

CWavWriter::CWavWriter(void)
{
}

CWavWriter::~CWavWriter(void)
{
}

bool CWavWriter::Open( const char* filename )
{
    m_File = fopen( filename, "wb" );

    if( m_File == NULL )
        return false;
    _header header;
    fwrite( &header, sizeof( _header ), 1, m_File );
    m_ByteWritten = 0;
    return true;
}

void CWavWriter::Close()
{
    rewind( m_File );

    //Write headers
    _header header;

    memcpy( header._chunkID, "RIFF", 4 );
    header._chunkSize = sizeof( _header ) +  m_ByteWritten - 8; 
    memcpy( header._format, "WAVE", 4 );
    memcpy( header._fmt, "fmt ", 4 );

	header._fmtSize = 16;
	header._fmtFormat = 1;
	header._numChannel = 1;
	header._sampleRate = 44100;
	header._byteRate =  44100 * 2;
	header._blockAlign = 2;
	header._bitsPerSample = 16;
	memcpy( header._data, "data", 4 );
	header._dataSize = m_ByteWritten;
    fwrite( &header, sizeof( _header ), 1, m_File );
    fclose( m_File );
}

 void  CWavWriter::AddBuffer( short *buffer, int size )
 {
    fwrite( buffer, size, 2, m_File );
    m_ByteWritten += 2 * size;
 }