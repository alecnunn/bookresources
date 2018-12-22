/***************************************************************
* DirectXVideoTexture.cpp                                      *
*                                                              *
* This file contains the implementation of the                 *
* CDirectXVideoTexture class.  								   *
* To compile correctly, this file must be linked with:         *
* kernel32.lib                                                 *
* user32.lib                                                   *
* d3dx8dt.lib                                                  *
* d3d8.lib                                                     *
* strmbasd.lib                                                 *
* winmm.lib                                                    *
*                                                              *
***************************************************************/

#include "DirectXVideoTexture.h"

CDirectXVideoTexture::CDirectXVideoTexture() : CBaseVideoRenderer(__uuidof(CLSID_VideoTexture), 
													NAME("Video Texture"), NULL, NULL)
{
	m_pMediaControl  = NULL;
	m_pMediaPosition = NULL;
	m_pMediaEvent    = NULL;
	m_pGraphBuilder  = NULL;
	m_pTexture       = NULL;
}


CDirectXVideoTexture::~CDirectXVideoTexture()
{
    //Stop the video
	if (m_pMediaControl)
	{
		m_pMediaControl->Stop();
		m_pMediaControl.Release();
	}

	if (m_pMediaEvent)
		m_pMediaEvent.Release();

	if (m_pMediaPosition)
		m_pMediaPosition.Release();


    //Let the graph builder clean everything up
	if (m_pGraphBuilder)
		m_pGraphBuilder.Release();

	//Clean up the texture
	if (m_pTexture)
	{
		m_pTexture->Release();
		m_pTexture = NULL;
	}
}

//This is called when the graph builder is trying to make connections
HRESULT CDirectXVideoTexture::CheckMediaType(const CMediaType *pMediaType)
{    
    //Only accept video stuff
	if(*pMediaType->FormatType() != FORMAT_VideoInfo) 
        return E_INVALIDARG;
    
    //Only accept RGB24
	if(IsEqualGUID(*pMediaType->Type(),    MEDIATYPE_Video)  &&
	   IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_RGB24))
		return S_OK;
    
    return E_FAIL;
}

//Once the connection has been established, the graph builder calls SetMediaType
HRESULT CDirectXVideoTexture::SetMediaType(const CMediaType *pMediaType)
{
    //The video format is in the form of a bitmap info header.
	//Use the bitmap info to set the width and height of the texture
	VIDEOINFO *pVideoFormat = (VIDEOINFO *)pMediaType->Format();
    m_VideoWidth  = pVideoFormat->bmiHeader.biWidth;
    m_VideoHeight = abs(pVideoFormat->bmiHeader.biHeight);

	//The pitch is based on an RGB24 bitmap
    m_VideoPitch = (m_VideoWidth * 3 + 3) & ~(3);

    // Create the texture that maps to this media type
    if(FAILED(D3DXCreateTexture(m_pDevice, m_VideoWidth, m_VideoHeight, 1, 0, 
                                D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTexture)))
        return E_FAIL;

    //Make sure that we actually got the format we asked for
    D3DSURFACE_DESC SurfaceDescription;
    m_pTexture->GetLevelDesc(0, &SurfaceDescription);
    if (SurfaceDescription.Format != D3DFMT_A8R8G8B8) 
        return VFW_E_TYPE_NOT_ACCEPTED;

	m_TextureWidth  = SurfaceDescription.Width; 
	m_TextureHeight = SurfaceDescription.Height;

    return S_OK;
}


//This is called every time there is a new frame from the video
HRESULT CDirectXVideoTexture::DoRenderSample(IMediaSample * pSample)
{	
	BYTE  *pVideoBuffer;
	BYTE  *pTextureBuffer;
    long  TexturePitch;
    
    //The the image buffer for the video frames
    pSample->GetPointer(&pVideoBuffer);

    //Lock the Texture buffer for writing
    D3DLOCKED_RECT LockedRect;
    if (FAILED(m_pTexture->LockRect(0, &LockedRect, 0, 0)))
        return E_FAIL;
    
    // Get the texture buffer & pitch
    pTextureBuffer = static_cast<byte *>(LockedRect.pBits);
    TexturePitch = LockedRect.Pitch;
    
    //If we've gotten this far, we are dealing with 24bit frames.
	//Walk through row by row
	for(long y = 0; y < m_VideoHeight; y++) 
	{
        BYTE *pVideoRowStart = pVideoBuffer;
        BYTE *pTextureRowStart = pTextureBuffer;  
		
		//walk through each pixel of the row
        for (int x = 0; x < m_VideoWidth; x++) 
		{
            pTextureBuffer[0] = pVideoBuffer[0];
            pTextureBuffer[1] = pVideoBuffer[1];
            pTextureBuffer[2] = pVideoBuffer[2];
            pTextureBuffer[3] = 0xff;

            pTextureBuffer += 4;
            pVideoBuffer   += 3;
        }

        //Increment to the next row
		pVideoBuffer   = pVideoRowStart   + m_VideoPitch;
        pTextureBuffer = pTextureRowStart + TexturePitch;
    }

    //unlock the texture
	if (FAILED(m_pTexture->UnlockRect(0)))
        return E_FAIL;

    return S_OK;
}

HRESULT CDirectXVideoTexture::CreateFromAVIFile(LPDIRECT3DDEVICE8 pDevice, TCHAR *pFileName)
{
    //Make sure the device is set
	m_pDevice = pDevice;
	
	//Reformat the filename parameter so that it will be accepted by AddSourceFilter
	WCHAR wFileName[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, pFileName, -1, wFileName, MAX_PATH);

    //These are some working objects used to build the filter graph
	CComPtr<IPin>           pPinIn;
    CComPtr<IBaseFilter>    pSourceFilter;
    CComPtr<IPin>           pPinOut;

	//Add a reference so that the graph builder does not delete this object.
	//That causes badness because the object is destroyed too early
	AddRef();

    //initialize the COM library
	CoInitialize(NULL) ;

	// Create the filter graph
	if (SUCCEEDED(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IFilterGraph, (void **)&m_pGraphBuilder)))
	{
		//Add this filter to the bag of tricks
		if (SUCCEEDED(m_pGraphBuilder->AddFilter(this, L"TEXTURERENDERER")))
		{
			//Add a source filter for the AVI file
			if (SUCCEEDED(m_pGraphBuilder->AddSourceFilter (wFileName, L"SOURCE", &pSourceFilter)))
			{
				//Get the input pin for this filter
				if (SUCCEEDED(FindPin(L"In", &pPinIn)))
				{
					//Get the output pin of the source filter
					if (SUCCEEDED(pSourceFilter->FindPin(L"Output", &pPinOut)))
					{
						//Let the graph builder figure out how to connect them
						if (SUCCEEDED(m_pGraphBuilder->Connect(pPinOut, pPinIn)))
						{
							//If the graph was successful, retreive all the interfaces
							m_pGraphBuilder.QueryInterface(&m_pMediaControl);
							m_pGraphBuilder.QueryInterface(&m_pMediaEvent);
							m_pGraphBuilder.QueryInterface(&m_pMediaPosition);

							//This is for audio. Some files may not have audio and the following
							//code may not work for all media types. Check the graph builder tool
							CComPtr<IBaseFilter>    pSplitterFilter;
						    CComPtr<IPin>           pAudioPin;

							//Find the a/v splitter in the graph
							if (SUCCEEDED(m_pGraphBuilder->FindFilterByName(L"AVI Splitter", &pSplitterFilter)))
							{
								//Find the pin for the first audio stream and render it. If it's not there,
								//it's OK, just move on...
								if (SUCCEEDED(pSplitterFilter->FindPin(L"Stream 01", &pAudioPin)))
									m_pGraphBuilder->Render(pAudioPin);
							}

							//If everything was successful, start retrieving frames
							if (SUCCEEDED(m_pMediaControl->Run()))
								return S_OK;
						}
					}
				}
			}
		}
	}

	//If you're here, something was bad
	return E_FAIL;
}

void CDirectXVideoTexture::CheckForLoop()
{
    long lEventCode;
    long lParam1;
    long lParam2;

    //If the video is complete, loop back to the start
    m_pMediaEvent->GetEvent(&lEventCode, &lParam1, &lParam2, 0);
    if (EC_COMPLETE == lEventCode)
        m_pMediaPosition->put_CurrentPosition(0);
}

