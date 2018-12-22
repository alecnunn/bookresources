#ifndef _TERRAIN_VSHADER_H
#define _TERRAIN_VSHADER_H



#include "VertexShader.h"
#include "TerrainVShaderConstants.h"


class TerrainVShader : public VertexShader
{
public:
	TerrainVShader();
	virtual ~TerrainVShader();
	virtual int GetVertexStride() const { return sizeof(float)*3; }
	virtual DWORD GetVertexFormat() const { return (D3DFVF_XYZ); }

	virtual void Apply();

private:
	void SetupVertexShaderConstants();

	D3DXVECTOR4					BaseTextureProjection;
	D3DXVECTOR4					DetailTextureProjection;
	D3DXVECTOR4					CloudMapProjection0;
	D3DXVECTOR4					CloudMapProjection1;


	// textures
};

TerrainVShader *GetTerrainVShader();

#endif

