//=============================================================================
// fluidrendering.fx by Frank Luna (C) 2004 All Rights Reserved.
//=============================================================================


//*****************************************************************************
// Globals

uniform extern float4x4 gWorld;
uniform extern float4x4 gWorldInv;
uniform extern float4x4 gWVP;
uniform extern float3   gEyePosW;
uniform extern texture  gDispMap;
uniform extern texture  gRefractMap;
uniform extern texture  gReflectMap;
uniform extern texture  gNormMap0;
uniform extern texture  gNormMap1;
uniform extern float4   gTexOffsets;

static float3 gSpecMtrl     = {0.5f, 0.5f, 0.5f};
static float  gSpecPower    = 256.0f;
static float  gTransparency = 1.0f;

static float3 gAmbientLight = {0.4f, 0.4f, 0.4f};
static float3 gDiffuseLight = {1.0f, 1.0f, 1.0f};
static float3 gSpecLight    = {1.0f, 1.0f, 1.0f};
static float3 gLightVecW    = {0.0f, 0.4472f, -0.8944f};

//*****************************************************************************
// Texture Samplers

sampler gDispSmplr = sampler_state
{
    Texture   = <gDispMap>;
    MinFilter = POINT; 
    MagFilter = POINT;
    MipFilter = NONE;
    AddressU  = CLAMP;
    AddressV  = CLAMP;
};

sampler gRefractSmplr = sampler_state
{
    Texture   = <gRefractMap>;
    MinFilter = LINEAR; 
    MagFilter = LINEAR;
    MipFilter = LINEAR;
    AddressU  = CLAMP;
    AddressV  = CLAMP;
};

sampler gReflectSmplr = sampler_state
{
    Texture   = <gReflectMap>;
    MinFilter = LINEAR; 
    MagFilter = LINEAR;
    MipFilter = LINEAR;
    AddressU  = CLAMP;
    AddressV  = CLAMP;
};

sampler gNorm0Smplr = sampler_state
{
    Texture   = <gNormMap0>;
    MinFilter = LINEAR; 
    MagFilter = LINEAR;
    MipFilter = LINEAR;
    AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler gNorm1Smplr = sampler_state
{
    Texture   = <gNormMap1>;
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
    float4 posH     : POSITION0;
    float3 lightVecT: TEXCOORD0;
    float3 viewVecT : TEXCOORD1;
    float2 nmap0UV  : TEXCOORD2;
    float2 nmap1UV  : TEXCOORD3;
    float2 projUV   : TEXCOORD4;
    float  refractWt: TEXCOORD5;
};

OutVS GPUFluidRenderingVS(float3 posL : POSITION0, float2 uv : TEXCOORD0)
{
	OutVS outVS = (OutVS)0;
	
	// Sample heightmap for displacement.
	float4 data = tex2Dlod(gDispSmplr, float4(uv, 0.0f, 0.0f));
 
	posL.y = data.r;
	outVS.posH = mul(float4(posL, 1.0f), gWVP);
	
	// Build tangents and normal vectors.
	float3 xtan  = normalize(float3(1.0f, data.g, 0.0f));
	float3 ztan  = normalize(float3(0.0f, data.b, 1.0f));
	float3 normalL = normalize(cross(ztan, xtan));
	
	// Orthonormal basis.
	float3x3 tanBasis;
	tanBasis[0] = xtan;
	tanBasis[1] = normalL;
	tanBasis[2] = ztan;
	
	// Transforms from local space to tangent space.
	float3x3 tanBasisInv = transpose(tanBasis);
		
	// Get everything in local space.
	float3 lightVecL = mul(gLightVecW, (float3x3)gWorldInv);
	float3 eyePosL   = mul(float4(gEyePosW, 1.0f), gWorldInv).xyz;
	float3 viewVecL  = normalize(eyePosL - posL);

	// Refract based on view angle.
	float refractWt = pow(max(dot(viewVecL, normalL), 0.0f), 1.0f);
	
	// Refract based on height of camera from water plane.
	float3 posW = mul(float4(posL, 1.0f), gWorld);
	outVS.refractWt = max(0.0f, refractWt - 0.020f*abs(posW.y-gEyePosW.y));
	
	// Transform to tangent space.
	outVS.lightVecT = mul(lightVecL, tanBasisInv);
	outVS.viewVecT  = mul(viewVecL, tanBasisInv);
	
	// Scroll normal maps and scale for tiling.
	outVS.nmap0UV = (uv*2.0f) + gTexOffsets.xy;
	outVS.nmap1UV = (uv*2.0f) + gTexOffsets.zw;
	
	// Projection texture coordinates.
	outVS.projUV  = outVS.posH.xy / outVS.posH.w;

	// Transform to [0, 1].
	outVS.projUV.x = ((outVS.projUV.x * 0.49f) + 0.5f);
	outVS.projUV.y = ((outVS.projUV.y * -0.49f) + 0.5f);
	
    return outVS;
}

float4 GPUFluidRenderingPS(float3 lightVecT : TEXCOORD0,
                           float3 viewVecT : TEXCOORD1,
                           float2 nmap0UV : TEXCOORD2, 
                           float2 nmap1UV : TEXCOORD3, 
                           float2 projUV  : TEXCOORD4,
                           float refractWt : TEXCOORD5) : COLOR
{
	// Interpolated normals can become unnormal, so normalize them.
	lightVecT = normalize(lightVecT);
	viewVecT  = normalize(viewVecT);
	
 	// Swap z,y coords since normal map "up" is along z-axis.
	float3 n1 = tex2D(gNorm0Smplr, nmap0UV).xzy;
	float3 n2 = tex2D(gNorm1Smplr, nmap1UV).xzy;
	
	// Expand from [0, 1] compressed interval to true [-1, 1] interval.
    n1 = (n1 - 0.5f) * 2.0f;
    n2 = (n2 - 0.5f) * 2.0f;
	float3 normT  = normalize(0.5f*(n1+n2));
	
	// Sample texture maps.
	float4 c0 = tex2D(gReflectSmplr, projUV + (normT.xz*0.05f));
	float4 c2 = tex2D(gRefractSmplr, projUV + (normT.xz*0.05f));
	float3 color = (1.0f-refractWt)* c0 + (refractWt) * c2;
 
	// Do lighting.
	float3 ambient = color*gAmbientLight;
	float3 diffuse = max(dot(lightVecT, normT), 0.0f)*color*gDiffuseLight;
	
	float3 r = reflect(-lightVecT, normT);
	float  s = pow(max(dot(r, viewVecT), 0.0f), gSpecPower);
	float3 spec = s*gSpecMtrl*gSpecLight;
	
	return float4(ambient + diffuse + spec, gTransparency);
}

technique GPUFluidRenderingTech
{
    pass P0
    {
        vertexShader = compile vs_3_0 GPUFluidRenderingVS();
        pixelShader  = compile ps_3_0 GPUFluidRenderingPS();
    }
}