#ifndef __WAVREAD_H__
#define __WAVREAD_H__

class CWavReader
{
public:
    ///  Open a wav file
    bool Open(const char *filename);

    short* GetBuffer( int index = 0 ){ return m_Buffer + index; }
    short GetValueAtIndex( int index ){ return *GetBuffer( index ); }
    int GetSize(){ return m_BufferSize; }
    
private:

    short *m_Buffer;
    int m_BufferSize;
};
#endif //__WAVEREAD_H__