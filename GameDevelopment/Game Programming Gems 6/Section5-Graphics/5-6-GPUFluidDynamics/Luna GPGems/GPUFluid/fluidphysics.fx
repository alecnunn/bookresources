//=============================================================================
// fluidphysics.fx by Frank Luna (C) 2004 All Rights Reserved.
//
// Contains Techniques:
//
//   -CopyTexTech: Used to copy a D3DFMT_A16B16G16R16F/D3DFMT_G16R16F surface to
//      a D3DFMT_A32B32G32R32F surface.  We have to do this because we need the 
//      16-bit format for alpha blending, but we need the 32-bit format for vertex
//      texture fetch.
//
//   -DisturbTech: Explicitly adds in additional height displacements based on external disturbances.
//
//   -PhysicsTech: Performs the explicit finite difference scheme.
//
//   -CalcTangentsTech: Computes tangent vectors in x- and z- directions.  This needs to be done after
//      the physics simulation is complete (i.e., we compute the tangents of the new grid heights).     
//=============================================================================


//*****************************************************************************
// CopyTexTech

uniform extern texture gHeightMap;
uniform extern texture gTangentMap;

sampler gHeightSmplr = sampler_state
{
    Texture   = <gHeightMap>;
    MinFilter = POINT; 
    MagFilter = POINT;
    MipFilter = NONE;
    AddressU  = CLAMP;
    AddressV  = CLAMP;
};

sampler gTangentSmplr = sampler_state
{
    Texture   = <gTangentMap>;
    MinFilter = POINT; 
    MagFilter = POINT;
    MipFilter = NONE;
    AddressU  = CLAMP;
    AddressV  = CLAMP;
};

void CopyTexVS(float3 posH : POSITION0, out float4 oPosH : POSITION0, out float2 oUV : TEXCOORD0)
{
	// Vertices already specified on the projection window.
	oPosH = float4(posH, 1.0f);
	
	// Transform projection window coordinates, [-1, 1] +y up, to texture coordinates, [0, 1] +y down.
	oUV = float2(posH.x, posH.y) * float2(0.5, -0.5) + 0.5;
}

float4 CopyTexPS(float2 uv : TEXCOORD0) : COLOR
{
	// (r1, 0, 0, 0) + (r2, g, 0, 0) = (r1, r2, g, 0).
	float height   = tex2D(gHeightSmplr, uv).r;
	float2 tangents = tex2D(gTangentSmplr, uv).rg;
	return float4(height, tangents, 1.0f);
}

technique CopyTexTech
{
    pass P0
    {
        vertexShader = compile vs_3_0 CopyTexVS();
        pixelShader  = compile ps_3_0 CopyTexPS();
    }
}


//*****************************************************************************
// DisturbTech

void DisturbVS(float3 posH : POSITION0, out float4 oPosH : POSITION0, out float oHeight : TEXCOORD0)
{
	// Vertices already specified on the projection window.
	oPosH   = float4(posH.x, posH.y, 0.0f, 1.0f);
	oHeight = posH.z;
}

float4 DisturbPS(float height : TEXCOORD0) : COLOR
{
	return float4(height, 0.0f, 0.0f, 1.0f);
}

technique DisturbTech
{
    pass P0
    {
        vertexShader = compile vs_3_0 DisturbVS();
        pixelShader  = compile ps_3_0 DisturbPS();
        
        // f = src * srcBlend + dest + destBlend
        //   = src * 1 + dest * 1
        //   = src + dest (i.e., accumulate the heights)
        AlphaBlendEnable = true;
        SrcBlend = One;
        DestBlend = One;
    }
}


//*****************************************************************************
// PhysicsTech

uniform extern float gStepSizeL; // Step size in local space.
uniform extern float gTexelSize; // Texel size in texture space [0, 1] units.
uniform extern float gK1; 
uniform extern float gK2; // gK1, gK2 and gK3 are precomputed coefficents for
uniform extern float gK3; // explicit finite difference scheme.

uniform extern texture gPrevStepTex;
uniform extern texture gCurrStepTex;

sampler gPrevStepSmplr = sampler_state
{
    Texture   = <gPrevStepTex>;
    MinFilter = POINT; 
    MagFilter = POINT;
    MipFilter = NONE;
    AddressU  = CLAMP;
    AddressV  = CLAMP;
};

sampler gCurrStepSmplr = sampler_state
{
    Texture   = <gCurrStepTex>;
    MinFilter = POINT; 
    MagFilter = POINT;
    MipFilter = NONE;
    AddressU  = CLAMP;
    AddressV  = CLAMP;
};

void PhysicsVS(float3 posH : POSITION0, out float4 oPosH : POSITION0, out float2 oUV : TEXCOORD0)
{
	// Vertices already specified on the projection window.
	oPosH  = float4(posH, 1.0f);
	
	// Transform projection window coordinates, [-1, 1] +y up, to texture coordinates,
	// [0, 1] +y down.
	oUV = float2(posH.x, posH.y) * float2(0.5, -0.5) + 0.5;
}

float4 PhysicsPS(float2 uv : TEXCOORD0) : COLOR
{
	// Zero out boundaries.
	if(uv.x < 0.001f || uv.x > 0.985f || uv.y < 0.001f || uv.y > 0.985f)
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	else
	{
		// Sample grid points from previous and current time steps.
		float c0 = tex2D(gPrevStepSmplr, uv).r;
		float c1 = tex2D(gCurrStepSmplr, uv).r;
		float t  = tex2D(gCurrStepSmplr, float2(uv.x, uv.y - gTexelSize)).r;
		float b  = tex2D(gCurrStepSmplr, float2(uv.x, uv.y + gTexelSize)).r;
		float l  = tex2D(gCurrStepSmplr, float2(uv.x - gTexelSize, uv.y)).r;
		float r  = tex2D(gCurrStepSmplr, float2(uv.x + gTexelSize, uv.y)).r;
	
		// Return next solution at this grid point.
		return float4(gK1*c0 + gK2*c1 + gK3*(t+b+l+r), 0.0f, 0.0f, 0.0f);
	}
}

technique PhysicsTech
{
    pass P0
    {
        vertexShader = compile vs_3_0 PhysicsVS();
        pixelShader  = compile ps_3_0 PhysicsPS();
    }    
}


//*****************************************************************************
// CalcTangentsTech

float4 CalcTangentsPS(float2 uv : TEXCOORD0) : COLOR
{
	// Zero out boundaries.
	if(uv.x < 0.001f || uv.x > 0.985f || uv.y < 0.001f || uv.y > 0.985f)
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	else
	{
		float t  = tex2D(gCurrStepSmplr, float2(uv.x, uv.y - gTexelSize)).r;
		float b  = tex2D(gCurrStepSmplr, float2(uv.x, uv.y + gTexelSize)).r;
		float l  = tex2D(gCurrStepSmplr, float2(uv.x - gTexelSize, uv.y)).r;
		float r  = tex2D(gCurrStepSmplr, float2(uv.x + gTexelSize, uv.y)).r;
		
		// r = x-tangent y-coordinate
		// g = y-tangent y-coordinate
		float xtanY = (r - l) / (2*gStepSizeL);
		float ztanY = (t - b) / (2*gStepSizeL);
	
		return float4(xtanY, ztanY, 0.0f, 0.0f);
	}
}

technique CalcTangentsTech
{
    pass P0
    {
        vertexShader = compile vs_3_0 PhysicsVS();
        pixelShader  = compile ps_3_0 CalcTangentsPS();
    }    
}