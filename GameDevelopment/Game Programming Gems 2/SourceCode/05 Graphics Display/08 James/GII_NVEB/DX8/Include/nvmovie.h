/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
	  Movie making class.  Nice for the ability to make movies of your work.
	  Doesn't currently support 16 bits, only 32 bits.

		Call:
		Movie->BeginRecord("aviname.avi", pbackbuffer)
	
		Then, for each frame:
		Movie->AddFrame();

		Then end with:
		Movie->EndRecord();

  Class records movies on the fly, so you can just call AddFrame() after your EndScene call

  cmaughan@nvidia.com
      

      
        
******************************************************************************/
#ifndef __NVMOVIE_H
#define __NVMOVIE_H

#include <vfw.h>
#pragma comment(lib, "vfw32.lib")

namespace nv_objects
{


#define AVIIF_KEYFRAME      0x00000010L // this frame is a key frame.

class NVMovie
{

public:
	NVMovie()
	: m_bRecording(false),
		m_pFile(NULL),
		m_pBackBuffer(NULL),
		m_pBuffer(NULL),
		m_pCompressedStream(NULL),
		m_pStream(NULL)
	{
	}


	virtual ~NVMovie()
	{
		Free();
	}


	virtual bool BeginRecord(std::string strFileName, LPDIRECT3DSURFACE8 pBackBuffer)
	{
		HRESULT hr;
		D3DSURFACE_DESC SurfaceDesc;
		AVISTREAMINFO StreamHeader;
		AVICOMPRESSOPTIONS CompressOptions;
		DWORD dwPitch;
		DWORD BitCount;
		DWORD dwVersion;

		if (m_bRecording)
		{
			if (!EndRecord())
				return false;
		}
    
		// Check VFW
		dwVersion = HIWORD(VideoForWindowsVersion());
		if (dwVersion < 0x010a)
		{
			return false;
		}
    
		// Create the movie
		AVIFileInit();
		hr = AVIFileOpen(&m_pFile, strFileName.c_str(), OF_CREATE | OF_WRITE, NULL);
		if (hr != AVIERR_OK)
		{
			MessageBox(NULL, "Could not open file!", "ERROR", MB_ICONEXCLAMATION);
			Free();
			return false;
		}

		pBackBuffer->GetDesc(&SurfaceDesc);

		// Get the pitch of the backbuffer
		switch(SurfaceDesc.Format)
		{
			case D3DFMT_A8R8G8B8:
			case D3DFMT_X8R8G8B8:
				dwPitch = SurfaceDesc.Width * 4;
				BitCount = 32;
				break;
			default:
				MessageBox(NULL, "Require 32 bit desktop!", "ERROR", MB_ICONEXCLAMATION);
				return false;
		}

		ZeroMemory(&StreamHeader, sizeof(StreamHeader));
		StreamHeader.fccType                = streamtypeVIDEO;// stream type
		StreamHeader.fccHandler             = 0;
		StreamHeader.dwSuggestedBufferSize  = dwPitch * SurfaceDesc.Height;
		StreamHeader.dwScale                = 1;
		StreamHeader.dwRate                 = 15;

		SetRect(&StreamHeader.rcFrame, 0, 0,		    // rectangle for stream
			(int) SurfaceDesc.Width,
			(int) SurfaceDesc.Height);

		// Create stream
		hr = AVIFileCreateStream(m_pFile, &m_pStream, &StreamHeader);	    
		if (hr != AVIERR_OK) 
		{
			Free();
			return false;
		}

		// Create a compressed version of the source stream
		ZeroMemory(&CompressOptions, sizeof(CompressOptions));
		CompressOptions.dwFlags = AVICOMPRESSF_VALID;
		CompressOptions.fccType = streamtypeVIDEO;
		CompressOptions.fccHandler = comptypeDIB;
		CompressOptions.dwQuality = 7400;
		CompressOptions.dwKeyFrameEvery = 1;
		hr = AVIMakeCompressedStream(&m_pCompressedStream, m_pStream, &CompressOptions, NULL); 
		if (hr != AVIERR_OK) 
		{
			Free();
			return false;
		}

		// Generate bitmap data    
		ZeroMemory(&m_BitmapInfoHeader, sizeof(BITMAPINFOHEADER));
		m_BitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
		m_BitmapInfoHeader.biWidth = SurfaceDesc.Width;
		m_BitmapInfoHeader.biHeight = SurfaceDesc.Height;
		m_BitmapInfoHeader.biPlanes = 1;
		m_BitmapInfoHeader.biBitCount = BitCount;
		m_BitmapInfoHeader.biCompression = BI_RGB;
		m_BitmapInfoHeader.biSizeImage = dwPitch * SurfaceDesc.Height;
		m_BitmapInfoHeader.biXPelsPerMeter = 1000;
		m_BitmapInfoHeader.biYPelsPerMeter = 1000;
		m_BitmapInfoHeader.biClrUsed = 0;
		m_BitmapInfoHeader.biClrImportant = 0;
		hr = AVIStreamSetFormat(m_pCompressedStream, 0, &m_BitmapInfoHeader,	    // stream format
									m_BitmapInfoHeader.biSize);
		if (hr != AVIERR_OK) 
		{
			Free();
			return false;
		}


		m_pBackBuffer = pBackBuffer;
		m_pBackBuffer->AddRef();

		m_pBuffer = new BYTE[((dwPitch + 3) & ~3) * SurfaceDesc.Height];

		m_FrameCount = 0;
		m_bRecording = true;

		return true;
	}

	virtual bool AddFrame()
	{
		D3DSURFACE_DESC SurfaceDesc;
		D3DLOCKED_RECT LockedRect;
		DWORD dwDestPitch;
		DWORD dwSourcePitch;

		if (!m_bRecording)
			return false;

		m_pBackBuffer->GetDesc(&SurfaceDesc);

		// Get the pitch of the backbuffer
		switch(SurfaceDesc.Format)
		{
			case D3DFMT_A8R8G8B8:
			case D3DFMT_X8R8G8B8:
				dwDestPitch = SurfaceDesc.Width * 4;
				break;
			case D3DFMT_R5G6B5:
			case D3DFMT_A1R5G5B5:
				dwDestPitch = SurfaceDesc.Width * 2;
				break;
		}

		m_pBackBuffer->LockRect(&LockedRect, NULL, D3DLOCK_READONLY);
		dwSourcePitch = dwDestPitch;
		if (LockedRect.Pitch != 0)
			dwSourcePitch = LockedRect.Pitch;

		LPBYTE pDest = m_pBuffer;
		for (int i = SurfaceDesc.Height - 1; i >= 0; i--)
		{
			LPBYTE pRow = (LPBYTE)LockedRect.pBits + (i * dwSourcePitch);
			memcpy(pDest, pRow, dwDestPitch);
			pDest += ((dwDestPitch + 3) & ~3);
		}

		m_pBackBuffer->UnlockRect();

		HRESULT hr = AVIStreamWrite(m_pCompressedStream,	    
						  m_FrameCount++,
						  1, 
						  m_pBuffer,
						  ((dwDestPitch + 3) & ~3) * SurfaceDesc.Height,
						  AVIIF_KEYFRAME,
						  NULL, NULL);
		


		if (hr != AVIERR_OK)
		{
			return false;	
		}

		return true;
	}

	virtual bool EndRecord()
	{
		if (!m_bRecording)
			return false;

		Free();

		m_bRecording = false;

		return true;
	}

protected:
	virtual void Free()
	{
		if (m_pCompressedStream)
		{
			AVIStreamClose(m_pCompressedStream);
			m_pCompressedStream = NULL;
		}

		if (m_pStream)
		{
			AVIStreamClose(m_pStream);
			m_pStream = NULL;
		}

		if (m_pFile)
		{
			AVIFileClose(m_pFile);
			AVIFileExit();
			m_pFile = NULL;
		}

		if (m_pBackBuffer)
		{
			m_pBackBuffer->Release();
			m_pBackBuffer = NULL;
		}

		delete []m_pBuffer;
		m_pBuffer = NULL;
	}

	BITMAPINFOHEADER m_BitmapInfoHeader;
	PAVISTREAM	m_pCompressedStream;
	PAVISTREAM	m_pStream;
    PAVIFILE	m_pFile;

	BYTE* m_pBuffer;
    
	LPDIRECT3DSURFACE8 m_pBackBuffer;
	bool m_bRecording;
	int m_FrameCount;

};

}; // nv_objects
#endif __NVMOVIE_H