// GPM - Pixel Shader Optimizations for Terrain Rendering Demo
// Kenny Mitchell
// Copyright Electronic Arts 2002, 2003

#ifndef ANALYTIC_SHADER_H
#define ANALYTIC_SHADER_H

#ifndef PIXEL_SHADER_H
#include "pixel_shader.h"
#endif

#ifndef ANALYTIC_SHADER_CONSTANTS_H
#include "analytic_shader_constants.h"
#endif

#define ANALYTIC_FVF (D3DFVF_XYZ)

class Analytic_Shader : public Pixel_Shader_Class
{
public:
	Analytic_Shader();
	virtual ~Analytic_Shader();
	virtual int Get_Vertex_Stride() const { return sizeof(float)*(3); }
	virtual DWORD Get_Vertex_Format() const { return ANALYTIC_FVF; }

	virtual void Apply();
	virtual void Update_Instance();

private:

	void Create_Analytic_Vertex_Shader();

	D3DXVECTOR4					Base_Texture_Projection;
	D3DXVECTOR4					Detail_Texture_Projection;
	D3DXVECTOR4					Cloud_Map_Projection;

	D3DXVECTOR4					Fog_Parameters;

	// Properties of sky light
	D3DXVECTOR4					Average_Sky_Light;

	// Properties of sun light
	D3DXVECTOR4					Sun_Light_Direction;
	D3DXVECTOR4					Sun_Light_Color;

	float						Sky_Light_Factor;
	float						Sun_Light_Factor;

	// Are shadows enabled?
	bool						Shadows_Enabled;

	// Is there an active primary light?
	bool						Active_Light;		

	bool						Enable_Terrain_Texture;
	bool						Enable_NH_Texture;
	bool						Enable_Detail_Texture;
	bool						Enable_Clouds_Texture;

	// textures
	LPDIRECT3DTEXTURE9		Terrain_Texture;
	LPDIRECT3DTEXTURE9		Positive_Normal_Horizon_Map_Texture;
	LPDIRECT3DTEXTURE9		Negative_Normal_Horizon_Map_Texture;
	LPDIRECT3DTEXTURE9		Detail_Texture;
	LPDIRECT3DTEXTURE9		White_Texture;
	LPDIRECT3DTEXTURE9		Clouds_Texture;
};

#endif