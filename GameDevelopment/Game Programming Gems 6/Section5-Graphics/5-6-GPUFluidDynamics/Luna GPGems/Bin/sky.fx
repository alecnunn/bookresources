//=============================================================================
// sky.fx by Frank Luna (C) 2004 All Rights Reserved.
//=============================================================================

uniform extern float4x4 gWVP;
uniform extern texture  gSkyMap;

sampler EnvMapSampler = sampler_state
{
    Texture   = <gSkyMap>;
    MinFilter = LINEAR; 
    MagFilter = LINEAR;
    MipFilter = LINEAR;
    AddressU  = WRAP;
    AddressV  = WRAP;
};


void SkyVS(float3 posL : POSITION0, out float4 oPosH : POSITION0, out float3 oEnvTex : TEXCOORD0)
{
	// Set z = w so that z/w = 1 (i.e., skydome always on far plane).
    oPosH = mul(float4(posL, 1.0f), gWVP).xyww; 
    
    // Use skymesh vertex position, in local space, as index into cubemap. 
    oEnvTex = normalize(posL);
}

float4 SkyPS(float3 envTex    : TEXCOORD0) : COLOR
{
    return texCUBE(EnvMapSampler, envTex);
}

technique SkyTech
{
    pass P0
    {
        vertexShader = compile vs_1_1 SkyVS();
        pixelShader  = compile ps_1_1 SkyPS();
		CullMode = None;
    }
}

technique SkyReflectTech
{
    pass P0
    {
        vertexShader = compile vs_1_1 SkyVS();
        pixelShader  = compile ps_1_1 SkyPS();
		CullMode = None;
    }
}
