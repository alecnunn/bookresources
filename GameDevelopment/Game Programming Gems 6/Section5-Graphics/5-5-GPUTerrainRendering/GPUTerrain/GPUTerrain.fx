//--------------------------------------------------------------------------------------
// File: GPUTerrain.fx
//
// Copyright (C) 2005 - Harald Vistnes (harald@vistnes.org)
// All rights reserved worldwide.
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code, for example:
// "Portions Copyright (C) Harald Vistnes, 2005"
//--------------------------------------------------------------------------------------

float4x4 matWorldViewProj;	
half4 vecLightDir;

half4 vTexelSize;
half fBiasU;
half fBiasV;
half fScale;
half fNormalScale;

sampler  hgtSampler : register(s257);

texture NormalMap;

sampler NormalMapSampler = sampler_state
{
   Texture = <NormalMap>;
   MinFilter = Linear;
   MagFilter = Linear;
   MipFilter = None;   
   AddressU  = Clamp;
   AddressV  = Clamp;
};

// -------------------------------------------------------------

struct VS_OUT_NORMAL
{
    half4 pos            : POSITION;
    half3 normal         : TEXCOORD0; 
};
struct VS_OUT_NONORMAL
{
    half4 pos            : POSITION;
    half2 normalCoord    : TEXCOORD0;
};

// -------------------------------------------------------------
half2 ScaleBiasPos(half2 pos)
{
    return half2(pos.x*fScale + fBiasU, pos.y*fScale + fBiasV);
}

// -------------------------------------------------------------
half4 TexFromPos(half2 pos)
{
    return half4(pos.x, pos.y, 0, 0);
}

// -------------------------------------------------------------
// Computes the vertex normal by sampling the
// vertex texture
// -------------------------------------------------------------
half3 ComputeNormal(half3 pos, half4 tex)
{
    half s = tex2Dlod(hgtSampler, tex-vTexelSize.wyww).x;
    half n = tex2Dlod(hgtSampler, tex+vTexelSize.wyww).x;
    half w = tex2Dlod(hgtSampler, tex-vTexelSize.xwww).x;
    half e = tex2Dlod(hgtSampler, tex+vTexelSize.xwww).x;

    half3 normal = normalize(half3(w-e, fNormalScale, s-n)); 
    return normal;
}

// -------------------------------------------------------------
// The vertex shader - calculating normals
// -------------------------------------------------------------
VS_OUT_NORMAL VS_NORMAL(half3 pos : POSITION)
{
    VS_OUT_NORMAL OUT;

    pos.xz = ScaleBiasPos(pos.xz);

    half4 tex = TexFromPos(pos.xz);

    pos.y *= tex2Dlod(hgtSampler, tex).x;
    
    OUT.normal = ComputeNormal(pos, tex);    
    OUT.pos = mul( half4(pos, 1.0f), matWorldViewProj );
    return OUT;
}

// -------------------------------------------------------------
// The vertex shader - passing texture coordinate for normal map.
// -------------------------------------------------------------
VS_OUT_NONORMAL VS_NONORMAL(half3 pos : POSITION)
{
    VS_OUT_NONORMAL OUT;

    pos.xz = ScaleBiasPos(pos.xz);

    half4 tex = TexFromPos(pos.xz);
    
    pos.y *= tex2Dlod(hgtSampler, tex).x;

    OUT.normalCoord = pos.xz;
    OUT.pos = mul( half4(pos, 1.0f), matWorldViewProj );
    return OUT;
}

// -------------------------------------------------------------
// The pixel shader - normal passed from vertex shader
// -------------------------------------------------------------
half4 PS_NONORMAL(half3 normal : TEXCOORD0) : COLOR
{
    // compute the intensity of light at the current pixel
    half intensity = saturate(dot(normalize(normal), -vecLightDir));
    
    // the diffuse color of the terrain
    half4 color = half4(0.61f, 0.61f, 0, 0);

    return color * intensity;
}

// -------------------------------------------------------------
// The pixel shader - samples normal map
// -------------------------------------------------------------
half4 PS_NORMAL(half2 normalCoord : TEXCOORD0) : COLOR
{
    // fetch the normal from the normal map
    half3 normal;
    normal.xz = tex2D(NormalMapSampler, normalCoord).rg;

    // only two components are stored in the texture, compute the last one from the other two
    normal.y = sqrt(1.0f - normal.x*normal.x - normal.z*normal.z);

    // compute the intensity of light at the current pixel
    half intensity = saturate(dot(normal, -vecLightDir));

    // the diffuse color of the terrain
    half4 color = half4(0.61f, 0.61f, 0, 0);
    
    return color * intensity;
}

// -------------------------------------------------------------
// technique used for calculating normals in vertex shader
// -------------------------------------------------------------
technique TShaderVSNormal
{
    pass P0
    {
        CullMode = CCW;

        // compile shaders
        VertexShader = compile vs_3_0 VS_NORMAL();
        PixelShader  = compile ps_3_0 PS_NONORMAL();
    }
}

// -------------------------------------------------------------
// technique used for sampling normals from normal map
// -------------------------------------------------------------
technique TShaderPSNormal
{
    pass P0
    {
        CullMode = CCW;
        
        // compile shaders
        VertexShader = compile vs_3_0 VS_NONORMAL();
        PixelShader  = compile ps_3_0 PS_NORMAL();
    }
}
