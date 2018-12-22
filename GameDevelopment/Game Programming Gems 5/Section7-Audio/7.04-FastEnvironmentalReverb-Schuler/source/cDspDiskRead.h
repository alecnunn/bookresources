
#ifndef DSP_DISKREAD_INCLUDED
#define DSP_DISKREAD_INCLUDED

#include <stdio.h>

template< typename > 
class cSoundBuffer;

class cDspDiskRead  
{
	FILE *m_File;

public:
	void Close();
	bool Open( const char * );
	bool QueryEOF() const;
	void Process( cSoundBuffer< float > &buffer );
	void ProcessStereo( cSoundBuffer< float > &bufferL, cSoundBuffer< float > &bufferR );
	void Attach( FILE *file ) { Close(); m_File = file; }
	void Seek( unsigned pos ) { if( m_File ) fseek( m_File, pos, SEEK_SET ); }
	cDspDiskRead();
	virtual ~cDspDiskRead();
};

#endif

