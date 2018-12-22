#ifndef __WAV_WRITTER_H__
#define __WAV_WRITTER_H__

#include <stdio.h>

class CWavWriter
{
public:
    CWavWriter(void);
    ~CWavWriter(void);

    bool Open( const char* filename );
    void Close();

    void AddBuffer( short *buffer, int size );

private:

    FILE* m_File;
    int m_ByteWritten;
};

#endif // __WAV_WRITTER_H__