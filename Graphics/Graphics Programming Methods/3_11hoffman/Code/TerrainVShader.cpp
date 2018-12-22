// (c) Preetham - ATI Research, 2003.
// (c) Naty Hoffman - WestWood Studios, 2003


#include "stdafx.h"

#include "TerrainVShader.h"
#include "MyDebug.h"
#include "Defines.h"
#include "App.h"
#include "Camera.h"
#include "Sun.h"
#include "Atmosphere.h"

#include "TerrainVShaderConstants.h"


extern LPDIRECT3DDEVICE8 g_Device;
extern int g_ScatterVS;

static TerrainVShader * s_terrainVShader = NULL;
TerrainVShader *GetTerrainVShader()
{
	if (!s_terrainVShader) 
		s_terrainVShader = new TerrainVShader;
	return s_terrainVShader;
}


TerrainVShader::TerrainVShader()
: BaseTextureProjection(INV_PATCH_WORLD_WIDTH,INV_PATCH_WORLD_WIDTH,0.0f,0.0f),
  DetailTextureProjection(1.0f/8.0f,1.0f/8.0f,0.0f,0.0f),
  CloudMapProjection0 (INV_PATCH_WORLD_WIDTH,INV_PATCH_WORLD_WIDTH,0.5f,0.0f),
  CloudMapProjection1 (INV_PATCH_WORLD_WIDTH,INV_PATCH_WORLD_WIDTH,0.0f,0.0f)
{
	// Create textures

	// Create vertex shader
	DWORD vertexshaderdeclaration[]=
	{
		D3DVSD_STREAM(0),
		(D3DVSD_REG(0, D3DVSDT_FLOAT3)), // vertex position
		D3DVSD_END()
	};

#ifdef _PREPROCESS
	CreateVertexShader("ScatterVS10.vsh",vertexshaderdeclaration);
#else
	CreateVertexShader("ScatterVS10a.vsh",vertexshaderdeclaration);
#endif
	OutputDebugString("Compiled vertex shader\n");

}

TerrainVShader::~TerrainVShader()
{
}

void TerrainVShader::Apply()
{
	// set vertex shader
	g_Device->SetVertexShader(m_VertexShader);
	SetupVertexShaderConstants();

}


void TerrainVShader::SetupVertexShaderConstants()
{
	/////////////////////////////////////////////////////////////////////////////////////
	// set up texture projection

	g_Device->SetVertexShaderConstant(CV_BASE_TEX_PROJECTION, &BaseTextureProjection, 1);
	g_Device->SetVertexShaderConstant(CV_DETAIL_TEX_PROJECTION, &DetailTextureProjection, 1);

	// animate cloud map offset
	CloudMapProjection0.z+=0.002f;
	CloudMapProjection1.z+=0.000125f;

	// wrap to avoid possible texture coordinate range problems
	if (CloudMapProjection0.z>TEX_COORD_RANGE) CloudMapProjection0.z-=TEX_COORD_RANGE;
	if (CloudMapProjection1.z>TEX_COORD_RANGE) CloudMapProjection1.z-=TEX_COORD_RANGE;

	g_Device->SetVertexShaderConstant(CV_CLOUD_TEX_PROJECTION_0, &CloudMapProjection0, 1);
	g_Device->SetVertexShaderConstant(CV_CLOUD_TEX_PROJECTION_1, &CloudMapProjection1, 1);

	g_Device->SetVertexShaderConstant(CV_ONE, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), 1);
	g_Device->SetVertexShaderConstant(CV_ZERO, D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), 1);
	
	// World matrix is for terrain, currently always 1.

	// TODO: Optimize all matrices here so that computation does
	// not happen every time, but happens only when camera/view changes.
	Camera *camera = GetApp()->GetCamera();
	

	D3DXMATRIX tempW, tempWV, tempWVP, temp;
	D3DXMATRIX matWorld;
	if ((g_ScatterVS == 1))
	{
		D3DXMatrixIdentity( &matWorld );
		D3DXMatrixTranspose(&tempW,&matWorld);
		g_Device->SetVertexShaderConstant( CV_WORLD, &tempW, 4 );
	}
	else
	{
		D3DXVECTOR3 vEye = camera->GetEye();
		vEye[1] = -200;
		D3DXMatrixIdentity( &matWorld );
		D3DXMatrixTranslation( &matWorld, vEye[0], vEye[1], vEye[2]);
		D3DXMatrixTranspose(&tempW,&matWorld);
		g_Device->SetVertexShaderConstant( CV_WORLD, &tempW, 4 );

	}

	D3DXMATRIX matView;
	camera->GetViewMatrix(matView);
    D3DXMatrixMultiply(&tempWV, &matWorld, &matView);
    D3DXMatrixTranspose(&temp,&tempWV);
    g_Device->SetVertexShaderConstant( CV_WORLD_VIEW, &temp, 4 );
  
	D3DXMATRIX matProj;
	camera->GetProjectionMatrix(matProj);
    D3DXMatrixMultiply(&tempWVP,&tempWV,&matProj);
    D3DXMatrixTranspose(&temp,&tempWVP);
    g_Device->SetVertexShaderConstant( CV_WORLD_VIEW_PROJECTION, &temp, 4 );


	Sun *sun = GetApp()->GetSun();
	D3DXVECTOR3 vSunDir = sun->GetDirection();
	D3DXVECTOR4 vSun(vSunDir[0], vSunDir[1], vSunDir[2], 1.0);
	g_Device->SetVertexShaderConstant( CV_SUN_DIRECTION, &vSun, 1 );

	float fReflectance = 0.1f;
	D3DXVECTOR3 vDiffuse(0.138f,0.113f, 0.08f); // Taken from soil's relectance spectrum data.
	vDiffuse *= fReflectance; 
	g_Device->SetVertexShaderConstant( CV_TERRAIN_REFLECTANCE, &vDiffuse, 1 );

	D3DXVECTOR4 vSunColorIntensity = sun->GetColorAndIntensity();
	g_Device->SetVertexShaderConstant( CV_SUN_COLOR, &vSunColorIntensity, 1 );

	// Scattering multipliers.
	Atmosphere *atm = GetApp()->GetAtmosphere();
	float fRayMult = atm->GetParam(eAtmBetaRayMultiplier);
	float fMieMult = atm->GetParam(eAtmBetaMieMultiplier);

	D3DXVECTOR3 vBetaR, vBetaDashR, vBetaM, vBetaDashM, vBetaRM, vOneOverBetaRM;

	// Rayleigh
	vBetaR = atm->GetBetaRayleigh();
	vBetaR *= fRayMult;
	g_Device->SetVertexShaderConstant( CV_BETA_1, &vBetaR, 1 );
	vBetaDashR = atm->GetBetaDashRayleigh();
	vBetaDashR *= fRayMult;
	g_Device->SetVertexShaderConstant( CV_BETA_DASH_1, &vBetaDashR, 1 );

	// Mie
	vBetaM = atm->GetBetaMie();
	vBetaM *= fMieMult;
	g_Device->SetVertexShaderConstant( CV_BETA_2, &vBetaM, 1 );
	vBetaDashM = atm->GetBetaDashMie();
	vBetaDashM *= fMieMult;
	g_Device->SetVertexShaderConstant( CV_BETA_DASH_2, &vBetaDashM, 1 );

	// Rayleigh + Mie (optimization)
	vBetaRM = vBetaR + vBetaM;
	g_Device->SetVertexShaderConstant (CV_BETA_1_PLUS_2, &vBetaRM, 1);
	vOneOverBetaRM[0] = 1.0/vBetaRM[0];
	vOneOverBetaRM[1] = 1.0/vBetaRM[1];
	vOneOverBetaRM[2] = 1.0/vBetaRM[2];
	vOneOverBetaRM[3] = 0;
	g_Device->SetVertexShaderConstant (CV_ONE_OVER_BETA_1_PLUS_2, &vOneOverBetaRM, 1);


	// Henyey Greenstein's G value.
	float g = atm->GetParam(eAtmHGg);
	D3DXVECTOR3 vG(1-g*g, 1+g*g, 2*g);
	g_Device->SetVertexShaderConstant( CV_HG, &vG, 1 );

	// constants.
	float l2e = 1/log(2);
	D3DXVECTOR4 vNumbers(1.0, l2e, 0.5, 0);
	g_Device->SetVertexShaderConstant( CV_CONSTANTS, &vNumbers, 1 );

	// each term (extinction, inscattering multiplier)
	float fExt = atm->GetParam(eAtmExtinctionMultiplier);
	float fIns = atm->GetParam(eAtmInscatteringMultiplier);
	D3DXVECTOR3 vTermMultipliers(fExt,fIns,0);
	g_Device->SetVertexShaderConstant( CV_TERM_MULTIPLIERS, &vTermMultipliers, 1 );

	// Eye Position
	D3DXVECTOR3 vEye = camera->GetEye();
	g_Device->SetVertexShaderConstant( CV_EYE_POSITION, &vEye, 1 );


}

