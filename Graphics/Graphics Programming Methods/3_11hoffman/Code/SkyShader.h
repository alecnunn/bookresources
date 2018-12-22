// Scattering shader
// (c) Preetham, ATI Research, 2002

#ifndef _SkyShader_H
#define _SkyShader_H

#include "Shader.h"

class SkyPShader;
class TerrainVShader;


class SkyShader : public Shader
{
public:
	SkyShader();
	virtual ~SkyShader();

	virtual void Apply();

private:
	TerrainVShader *m_pVertexShader;
	SkyPShader *m_pPixelShader;

};

#endif