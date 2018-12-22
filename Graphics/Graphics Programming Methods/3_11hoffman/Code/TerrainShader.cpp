// Scatter terrain shader
// (c) Kenny Mitchell - Westwood Studios EA 2002
#include "stdafx.h"

#include "TerrainShader.h"
#include "TerrainVShader.h"
#include "TerrainPShader.h"

TerrainShader::TerrainShader()
{
	m_pVertexShader = GetTerrainVShader();
	m_pPixelShader = GetTerrainPShader();
}

TerrainShader::~TerrainShader()
{
	SAFE_DELETE(m_pVertexShader);
	SAFE_DELETE(m_pPixelShader);

}

void TerrainShader::Apply()
{
	m_pVertexShader->Apply();
	m_pPixelShader->Apply();
}
