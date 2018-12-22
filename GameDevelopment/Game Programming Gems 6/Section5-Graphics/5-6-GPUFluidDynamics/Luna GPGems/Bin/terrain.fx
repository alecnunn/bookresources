//=============================================================================
// terrain.fx by Frank Luna (C) 2004 All Rights Reserved.
//=============================================================================


//*****************************************************************************
// Globals

uniform extern float4x4 gWorld;
uniform extern float4x4 gWorldInvTrans;
uniform extern float4x4 gWVP;
uniform extern float3   gEyePosW;
uniform extern texture  gTexMap;

static float3 gAmbientMtrl  = {1.0f, 1.0f, 1.0f};
static float3 gDiffuseMtrl  = {1.0f, 1.0f, 1.0f};

static float3 gAmbientLight = {0.5f, 0.5f, 0.5f};
static float3 gDiffuseLight = {1.0f, 1.0f, 1.0f};
static float3 gLightVecW    = {0.0f, 0.4472f, -0.8944f};


//*****************************************************************************
// Texture Samplers

sampler gTexSmplr = sampler_state
{
    Texture   = <gTexMap>;
    MinFilter = LINEAR; 
    MagFilter = LINEAR;
    MipFilter = LINEAR;
    AddressU  = WRAP;
    AddressV  = WRAP;
};

//*****************************************************************************
// Vertex Shader

struct OutVS
{
    float4 posH : POSITION0;
    float2 tex0 : TEXCOORD0;
    float  shade : TEXCOORD1;
};

OutVS TerrainVS(float3 posL : POSITION0, float3 normalL : NORMAL0, float2 tex0 : TEXCOORD0)
{
	OutVS outVS = (OutVS)0;
	
	// Scale to tile texture.
	outVS.tex0  = 4.0f*tex0;
	
	float3 normalW = normalize(mul(normalL, (float3x3)gWorldInvTrans));
	
	outVS.shade = max(dot(gLightVecW, normalW), 0.0f);
	outVS.posH  = mul(float4(posL, 1.0f), gWVP);

    return outVS;
}

float4 TerrainPS(float2 tex0 : TEXCOORD0, float shade : TEXCOORD1) : COLOR
{
	float4 color = tex2D(gTexSmplr, tex0);
	float3 ambient = color*gAmbientLight;
	float3 diffuse = shade*(color*gDiffuseLight);
	return float4(ambient + diffuse, 1.0f);
}

technique TerrainTech
{
    pass P0
    {
        vertexShader = compile vs_3_0 TerrainVS();
        pixelShader  = compile ps_3_0 TerrainPS();
    }
}