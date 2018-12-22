#include "stdafx.h"

#include "Sky.h"
#include "SkyShader.h"


extern LPDIRECT3DDEVICE8 g_Device;
extern int g_ScatterVS;


Sky::Sky(D3DXVECTOR3 &vCenter, float fRadiusHoriz, float fRadiusUp)
{
	m_nThetaSides = 256; // Azimuth
	m_nPhiSides =256;	// Zenith
	m_fRadiusHoriz = fRadiusHoriz;
	m_fRadiusUp = fRadiusUp;
	m_vCenter = vCenter;

	m_pShader1 = new SkyShader;

	CreateVertexBuffer();
}

Sky::~Sky()
{
	SAFE_RELEASE(m_pVB);
	SAFE_DELETE (m_pShader1);
}


HRESULT Sky::FillupVertexBuffer()
{
	// Fill in vertex data
	SkyVertex* pVertex;
	int nNumVertices = m_nThetaSides*2*(m_nPhiSides+1);

    m_pVB->Lock( 0, 0, (BYTE**)&pVertex, 0 );

	int index = 0;
	float x1,x2,y,z1,z2;
	for (int i=0; i < m_nThetaSides; i++)
	{
		float fTheta1 = (float)i/m_nThetaSides * D3DXToRadian(360);
		float fTheta2 = (float)(i+1)/m_nThetaSides * D3DXToRadian(360);
		for (int j=0; j <= m_nPhiSides; j++)
		{
			float fPhi = (float)j/m_nPhiSides * D3DXToRadian(135);
			
			x1 = (float)sin(fPhi)*cos(fTheta1)*m_fRadiusHoriz;
			z1 = (float)sin(fPhi)*sin(fTheta1)*m_fRadiusHoriz;
			x2 = (float)sin(fPhi)*cos(fTheta2)*m_fRadiusHoriz;
			z2 = (float)sin(fPhi)*sin(fTheta2)*m_fRadiusHoriz;
			y = (float)cos(fPhi)*m_fRadiusUp;

			pVertex[index].p = D3DXVECTOR3(x1,y,z1) + m_vCenter;
			pVertex[index].u = (float)i/m_nThetaSides;
			pVertex[index].v = (float)j/m_nPhiSides;
			index++;
			pVertex[index].p = D3DXVECTOR3(x2,y,z2) + m_vCenter;
			pVertex[index].u = (float)(i+1)/m_nThetaSides;
			pVertex[index].v = (float)j/m_nPhiSides;
			index++;
		}
	}
	
	m_pVB->Unlock();

	return S_OK;
}


HRESULT Sky::CreateVertexBuffer()
{

	int nNumVertices = m_nThetaSides*2*(m_nPhiSides+1);

	DWORD dwUsage = D3DUSAGE_WRITEONLY;

	// Create vertex buffer to hold the vertices.
    if( FAILED( g_Device->CreateVertexBuffer( nNumVertices*sizeof(SkyVertex),
                                                  dwUsage, D3DFVF_XYZ,
                                                  D3DPOOL_MANAGED, &m_pVB ) ) )
    {
        return E_FAIL;
    }

	// Fill in vertex data
	return FillupVertexBuffer();

}

HRESULT Sky::Render()
{
	HRESULT hr;
	g_ScatterVS = 2;

		// Choose shader and set up constants.
	m_pShader1->Apply();

	hr = g_Device->SetStreamSource( 0, m_pVB, sizeof(SkyVertex) );
	if (FAILED(hr)) return hr;

	int startVertex = 0;
	int numFacesPerStrip = m_nPhiSides*2;
	for (int i=0; i < m_nThetaSides; i++)
	{
		hr = g_Device->DrawPrimitive( D3DPT_TRIANGLESTRIP, startVertex, numFacesPerStrip);
		if (FAILED(hr)) return hr;

		startVertex += (m_nPhiSides+1)*2;
	}


	return S_OK;

}