// Scattering shader
// (c) Preetham, ATI Research, 2002

#ifndef _TerrainShader_H
#define _TerrainShader_H

#include "Shader.h"

class TerrainPShader;
class TerrainVShader;


class TerrainShader : public Shader
{
public:
	TerrainShader();
	virtual ~TerrainShader();

	virtual void Apply();

private:
	TerrainVShader *m_pVertexShader;
	TerrainPShader *m_pPixelShader;

};

#endif