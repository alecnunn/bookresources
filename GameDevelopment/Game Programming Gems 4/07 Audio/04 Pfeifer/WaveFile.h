#ifndef _WAVEFILE_H_
#define _WAVEFILE_H_


// The Wavefile class implements readim PCM data from a .wav file.
// This implementation is based on the DirectSound SDK samples,
// for more info download the DirectX sdk at http://www.msdn.com/directx
class WaveFile
{
    public:
        WAVEFORMATEX* m_pwfx;        // Pointer to WAVEFORMATEX structure
        HMMIO         m_hmmio;       // MM I/O handle for the WAVE
        MMCKINFO      m_ck;          // Multimedia RIFF chunk
        MMCKINFO      m_ckRiff;      // Use in opening a WAVE file
        DWORD         m_dwSize;      // The size of the wave file
        MMIOINFO      m_mmioinfoOut;
        BOOL          m_bIsReadingFromMemory;
        BYTE*         m_pbData;
        BYTE*         m_pbDataCur;
        ULONG         m_ulDataSize;

        WaveFile();
        ~WaveFile();

        bool Open(LPCTSTR strFileName, WAVEFORMATEX* pwfx);

        void Close();

        bool Read(BYTE* pBuffer, DWORD dwOffset, DWORD dwSizeToRead, DWORD* pdwSizeRead);

        DWORD   GetSize();
        HRESULT ResetFile();
        WAVEFORMATEX* GetFormat()  {  return m_pwfx;  };


    protected:
        HRESULT ReadMMIO();
};


#endif