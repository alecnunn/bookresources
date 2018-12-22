#include "dxstdafx.h"
#include "Heightfield.h"

//----------------------------------------------------------------------------------

Heightfield::Heightfield()
	:
	m_pHeightfield(0),
	m_pVertexTexture(0)
{
	ZeroMemory(&m_Info, sizeof(D3DXIMAGE_INFO));
}

//----------------------------------------------------------------------------------

Heightfield::~Heightfield()
{
}

//----------------------------------------------------------------------------------

HRESULT Heightfield::Initialize(IDirect3DDevice9 *pDevice, LPCWSTR szHeightfield)
{
	if (!pDevice || !szHeightfield ) {
		return E_INVALIDARG;
	}

	// load the original heightfield from file
	if (FAILED(D3DXCreateTextureFromFileEx(pDevice, szHeightfield, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 
		1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0xFF000000, &m_Info, NULL, &m_pHeightfield))) 
	{
		return E_FAIL;
	}

	// create the R32F vertex texture
	if (FAILED(D3DXCreateTexture(pDevice, m_Info.Width, m_Info.Height, 1, 0, D3DFMT_R32F, D3DPOOL_MANAGED, &m_pVertexTexture))) {
		return E_FAIL;
	}

	// convert and load the heightfield into the R32F vertex texture
	IDirect3DSurface9 *pOrigSurface = 0;
	IDirect3DSurface9 *pR32FSurface = 0;

	if (FAILED(m_pHeightfield->GetSurfaceLevel(0, &pOrigSurface))) {
		return E_FAIL;
	}
	if (FAILED(m_pVertexTexture->GetSurfaceLevel(0, &pR32FSurface))) {
		return E_FAIL;
	}
	if (FAILED(D3DXLoadSurfaceFromSurface(pR32FSurface, NULL, NULL, pOrigSurface, NULL, NULL, D3DX_DEFAULT, 0xFF000000))) {
		return E_FAIL;
	}
	SAFE_RELEASE(pOrigSurface);
	SAFE_RELEASE(pR32FSurface);

	SAFE_RELEASE(m_pHeightfield);
	if (FAILED(D3DXCreateTextureFromFile(pDevice, szHeightfield, &m_pHeightfield))) 
	{
		return E_FAIL;
	}

	if (FAILED(pDevice->SetSamplerState(D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP))) {
		return E_FAIL;
	} 
	if (FAILED(pDevice->SetSamplerState(D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP))) {
		return E_FAIL;
	} 
	if (FAILED(pDevice->SetSamplerState(D3DVERTEXTEXTURESAMPLER0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR))) {
		return E_FAIL;
	}
	if (FAILED(pDevice->SetSamplerState(D3DVERTEXTEXTURESAMPLER0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR))) {
		return E_FAIL;
	}

	return S_OK;
}

//----------------------------------------------------------------------------------

void Heightfield::OnDestroyDevice()
{
	SAFE_RELEASE(m_pHeightfield);
	SAFE_RELEASE(m_pVertexTexture);
}
