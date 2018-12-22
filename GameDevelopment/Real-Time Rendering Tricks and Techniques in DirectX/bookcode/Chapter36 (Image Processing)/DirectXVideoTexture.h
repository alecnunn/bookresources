/***************************************************************
* DirectXVideoTexture.h                                        *
*                                                              *
* This file contains the header information for the class      *
* CDirectXVideoTexture.  For this file to compile correctly,   *
* the path for the DX include files must be added to your path.*
*                                                              *
***************************************************************/
#include <d3d8.h>
#include <d3dx8.h>

//include COM and stream classes needed for the DirectShow stuff
#include <atlbase.h>
#include <streams.h>

// Define GUID for Video Texture
struct __declspec(uuid("{71771540-2017-11cf-ae26-0020afd79769}")) CLSID_VideoTexture;

class CDirectXVideoTexture : public CBaseVideoRenderer
{
public:
	CDirectXVideoTexture();
	virtual ~CDirectXVideoTexture();

	HRESULT CreateFromAVIFile(LPDIRECT3DDEVICE8 pDevice, TCHAR *pFileName);

	void CheckForLoop();
    HRESULT CheckMediaType(const CMediaType *pmt);
    HRESULT SetMediaType(const CMediaType *pmt);
    HRESULT DoRenderSample(IMediaSample *pMediaSample);
    
	long      m_VideoWidth;
	long      m_VideoHeight;
	long      m_TextureWidth;
	long      m_TextureHeight;
	long      m_VideoPitch;
	D3DFORMAT m_VideoFormat;

	CComPtr<IGraphBuilder>          m_pGraphBuilder; 
	CComPtr<IMediaControl>		    m_pMediaControl;  
	CComPtr<IMediaEvent>		    m_pMediaEvent;  
	CComPtr<IMediaPosition>		    m_pMediaPosition;      
	
	LPDIRECT3DTEXTURE8 m_pTexture;
	LPDIRECT3DDEVICE8  m_pDevice;
};