#ifndef _TERRAIN_PSHADER_H
#define _TERRAIN_PSHADER_H

#include "PixelShader.h"

class TerrainPShader : public PixelShader {

public:
	TerrainPShader();
	~TerrainPShader();

	virtual void Apply();

private:
	void SetupPixelShaderConstants();

	// textures
	LPDIRECT3DTEXTURE8		TerrainTexture;
	LPDIRECT3DTEXTURE8		PositiveNormalHorizonMapTexture;
	LPDIRECT3DTEXTURE8		NegativeNormalHorizonMapTexture;
	LPDIRECT3DTEXTURE8		CellSkyLightTexture; // to be calculated
	LPDIRECT3DTEXTURE8		CloudsTexture;
};

TerrainPShader *GetTerrainPShader();

#endif