#include "stdafx.h"

#include "TerrainPShader.h"
#include "MyDebug.h"
#include "Defines.h"
#include "App.h"
#include "Camera.h"
#include "Sun.h"
#include "Atmosphere.h"

#include "TerrainPShaderConstants.h"

#include <D3d8types.h>
#include "dxutil.h"

extern LPDIRECT3DDEVICE8 g_Device;

static float getSlopeUp(D3DXVECTOR3 &v)
{
	float horiz = sqrt(v[0]*v[0] + v[1]*v[1]);
	float tanangle = v[2]/horiz;
	if (tanangle < 0)
		return 0;
	float angle = atan(tanangle);
	float result = angle/3.1415*2.0f;
	if (result > 1) result = 1.0f;
	if (result < 0) result = 0.0f;
	return result;
}

static TerrainPShader * s_terrainPShader = NULL;
TerrainPShader *GetTerrainPShader()
{
	if (!s_terrainPShader) 
		s_terrainPShader = new TerrainPShader;
	return s_terrainPShader;
}


char StrTerrainPS10[] = {
"ps.1.0 \n\
tex t0 \n\
tex t1 \n\
tex t2 \n\
dp3_sat	r0.rgb, c[1], t0_bx2 \n\
mul r0.rgb, r0, t1 \n\
+sub_sat r0.a, c[1].a, t0.a	\n\
mul r0, r0, 1-r0.a \n\
mul r0, r0, 1-t2 \n\
mul r0, r0, v0 \n\
add r0, r0, v1 \n\
"
};


TerrainPShader::TerrainPShader()
: TerrainTexture(NULL),
  PositiveNormalHorizonMapTexture(NULL),
  NegativeNormalHorizonMapTexture(NULL),
  CloudsTexture(NULL)
{
	// Create textures
	HRESULT result=D3DXCreateTextureFromFile
	(
		g_Device,
		"terrain3.tga",
		&TerrainTexture
	);
	FAILMSG("Failed to create terrain texture");


	result=D3DXCreateTextureFromFile
	(
		g_Device,
		"clouds.bmp",
		&CloudsTexture
	);
	FAILMSG("Failed to create clouds texture");

	result=D3DXCreateTextureFromFile
	(
		g_Device,
		"pnh3.dds",
		&PositiveNormalHorizonMapTexture
	);
	FAILMSG("Failed to create sun light texture");

	result=D3DXCreateTextureFromFile
	(
		g_Device,
		"nnh3.dds",
		&NegativeNormalHorizonMapTexture
	);
	FAILMSG("Failed to create sun light texture");


	// Create pixel shader
#ifdef _PREPROCESS
	CreatePixelShaderFromFile("TerrainPS10.psh");
#else
	CreatePixelShaderFromFile("TerrainPS10a.psh");
#endif

//	CreatePixelShaderFromBuffer(StrTerrainPS10);

	OutputDebugString("Compiled pixel shader\n");
}


TerrainPShader::~TerrainPShader()
{
	SAFE_RELEASE(TerrainTexture);
	SAFE_RELEASE(CloudsTexture);
	SAFE_RELEASE(PositiveNormalHorizonMapTexture);
	SAFE_RELEASE(NegativeNormalHorizonMapTexture);
}


void TerrainPShader::Apply()
{

	Sun *sun = GetApp()->GetSun();
	D3DXVECTOR3 sunDir = sun->GetDirection();
	bool positiveHorizon = (sunDir[2] > 0); // Sun is west (true), east (false);


	//positiveHorizon = true;
	// set textures
	g_Device->SetTexture(0, positiveHorizon ? PositiveNormalHorizonMapTexture : NegativeNormalHorizonMapTexture);
	g_Device->SetTexture(1, TerrainTexture);
	g_Device->SetTexture(2, CloudsTexture);

	// set pixel shader
	g_Device->SetPixelShader(m_PixelShader);
	SetupPixelShaderConstants();
}

void TerrainPShader::SetupPixelShaderConstants()
{
	Sun *sun = GetApp()->GetSun();
	D3DXVECTOR4 pSunDir = sun->GetDirection();
	bool positiveHorizon = (pSunDir[2] > 0); // Sun is west (true), east (false);

	// set pixel shader constants
	D3DXVECTOR3 pSunColorIntensity = sun->GetColorWithIntensity();
	g_Device->SetPixelShaderConstant(CP_SUN_COLOR, pSunColorIntensity, 1);

	// Interchange y & z because normal map uses z as up while in our world (for sun too), y direction is up.
	float temp = pSunDir[1]; pSunDir[1] = pSunDir[2]; pSunDir[2] = temp;
	pSunDir[3] = positiveHorizon ? pSunDir[1] : -pSunDir[1]; // horizon angle 

	g_Device->SetPixelShaderConstant(CP_SUN_DIRECTION, pSunDir, 1);

}


