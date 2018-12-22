// Scatter terrain shader
#include "stdafx.h"

#include "SkyShader.h"
#include "TerrainVShader.h"
#include "SkyPShader.h"

SkyShader::SkyShader()
{
	m_pVertexShader = new TerrainVShader();
	m_pPixelShader = new SkyPShader();
}

SkyShader::~SkyShader()
{
	SAFE_DELETE(m_pVertexShader);
	SAFE_DELETE(m_pPixelShader);

}

void SkyShader::Apply()
{
	m_pVertexShader->Apply();
	m_pPixelShader->Apply();
}
