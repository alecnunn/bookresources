//--------------------------------------------------------------------------------------
// File: EfficientSky.fx
//
// The effect file for the EfficientSky sample.  
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
// The eye position (in local space).
float3 g_vLocalEyePos;

float3 g_SunLightDir;
float4 g_SunLightColor;
float3 g_MoonLightDir;
float4 g_MoonLightColor;
float4 g_LightAmbient;

texture g_DiffuseMap;
texture g_NormalMap;
texture g_GlossMap;
texture g_CubeMap;
texture g_StarCubeMap;
texture g_DiffuseCubeMap;
texture g_SkyNormalMap;
texture g_SunTexture;
texture g_MoonTexture;
texture g_FMap;

float    g_fTime;                   // App's time in seconds
float4x4 g_mWorldViewProjection;    // World * View * Projection matrix
float4x4 g_mWorldIT;
float4x4 g_mSunTextureMatrix;
float4x4 g_mMoonTextureMatrix;
float    g_fStarColorModifier;


const float PI = 3.1415926535;

sampler DifuseCubeMapSampler = 
sampler_state
{
    Texture = <g_DiffuseCubeMap>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};


sampler DiffuseMapSampler = 
sampler_state
{
    Texture = <g_DiffuseMap>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler NormalMapSampler = 
sampler_state
{
    Texture = <g_NormalMap>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler GlossMapSampler = 
sampler_state
{
    Texture = <g_GlossMap>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler CubeMapSampler = 
sampler_state
{
    Texture = <g_CubeMap>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler StarMapSampler = 
sampler_state
{
    Texture = <g_StarCubeMap>;
    MipFilter = POINT;
    MinFilter = POINT;
    MagFilter = POINT;
};

sampler SkyNormalMapSampler = 
sampler_state
{
    Texture = <g_SkyNormalMap>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler SunTextureSampler = 
sampler_state
{
    Texture = <g_SunTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    // Clamp at borders.
    AddressU = BORDER;
    AddressV = BORDER;
};

sampler MoonTextureSampler = 
sampler_state
{
    Texture = <g_MoonTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    // Clamp at borders.
    AddressU = BORDER;
    AddressV = BORDER;
};

sampler FSampler = 
sampler_state
{
    Texture = <g_FMap>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};


struct VS_INPUT
{
	float4	Position			: POSITION;
	float3	Normal				: NORMAL;
	float3	Tangent				: TANGENT;
	float3	Binormal			: BINORMAL;
	float2	TexCoord0			: TEXCOORD0;
};


struct VS_SKYINPUT
{
	float4	Position			: POSITION;
	float3	Normal				: NORMAL;
	float3	Tangent				: TANGENT;
	float3	Binormal			: BINORMAL;
	float2	TexCoord0			: TEXCOORD0;
	float	OpticalDepth		: TEXCOORD1;
};


struct VS_OUTPUT
{
    float4 Position			: POSITION;
    float4 Color			: COLOR0;
    float2 TexCoord0		: TEXCOORD0;
    float3 LightVector		: TEXCOORD1;
    float3 EyeVector		: TEXCOORD2;
    float3 HalfVector		: TEXCOORD3;
};


struct VS_TERRAIN_OUTPUT
{
    float4 Position			: POSITION;
    float4 Color			: COLOR0;
    float2 TexCoord0		: TEXCOORD0;
    float3 SunLightVector	: TEXCOORD1;
    float3 MoonLightVector	: TEXCOORD2;
    float3 EyeVector		: TEXCOORD3;
    float3 WorldNormal		: TEXCOORD5;
};


struct VS_OUTPUT_VERTEXSKY
{
    float4 Position			: POSITION;
    float4 Color			: COLOR0;
    float4 SunProjTC		: TEXCOORD0;
    float4 MoonProjTC		: TEXCOORD1;
   	float3 CubeTC			: TEXCOORD2;
};


struct VS_OUTPUT_PIXELSKY
{
    float4 Position			: POSITION;
    float4 Color			: COLOR0;
    float2 TexCoord0		: TEXCOORD0;
    float3 LightVector		: TEXCOORD1;
    float3 EyeVector		: TEXCOORD2;
    float3 HalfVector		: TEXCOORD3;
    float3 OpticalDepth		: TEXCOORD4;
    float4 SunProjTC		: TEXCOORD5;
    float4 MoonProjTC		: TEXCOORD6;
    float3 CubeTC			: TEXCOORD7;
};


struct VS_OUTPUT_FOG
{
    float4 Position			: POSITION;
    float3 LightVector		: TEXCOORD0;
    float3 EyeVector		: TEXCOORD1;
    float3 HalfVector		: TEXCOORD2;
   	float3 ViewPosition		: TEXCOORD3;
	float3 ObjPos			: TEXCOORD4;
	float3 LightPosition	: TEXCOORD5;
	float FogFactor			: TEXCOORD6;
};


struct VS_OUTPUT_CUBEPASSTHROUGH
{
    float4 Position			: POSITION;
    float3 TexCoord0		: TEXCOORD0;
};


struct PS_OUTPUT
{
    float4	Color			: COLOR0;
//    float	Depth			: DEPTH;
};


float4 RenderDepthVS( VS_INPUT IN ) : POSITION
{
    // Transform the position from object space to homogeneous projection space.
    return mul( IN.Position, g_mWorldViewProjection );
}


float4 RenderDepthPS() : COLOR
{ 
	return float4( 0.0f, 0.0f, 0.0f, 0.0f );
}


float A0( float I0, float beta, float Dsv, float gamma )
{
	return ( beta * I0 * exp( -beta * Dsv * cos( gamma ) ) ) / ( 2 * PI * Dsv * sin( gamma ) );
}


float A1( float beta, float Dsv, float gamma )
{
	return beta * Dsv * sin( gamma );
}


// Calculate the Air Light contribution (from "A Practical Analytic Single Scattering Model for Real Time Rendering").
float CalculateAirLight( float lightIntensity, float beta, float Dsv, float Dvp, 
						 float gammasv, sampler F, float f_XOffset, float f_XScale, 
						 float f_YOffset, float f_YScale )
{
	float u = A1( beta, Dsv, gammasv );
	float v1 = 0.25 * PI + 0.5 * atan( ( Dvp - Dsv * cos( gammasv ) ) / ( Dsv * sin( gammasv ) ) );
	float v2 = 0.5 * gammasv;
			
	float4 f_1 = tex2D( F, float2( ( v1 - f_XOffset ) * f_XScale, ( u - f_YOffset ) * f_YScale ) );
	float4 f_2 = tex2D( F, float2( ( v2 - f_XOffset ) * f_XScale, ( u - f_YOffset ) * f_YScale ) );
	return A0( lightIntensity, beta, Dsv, gammasv ) * ( f_1.x - f_2.x );
}


float lightIntensity = 100000.0f;
float beta = 0.001f;
	
	
VS_OUTPUT_FOG RenderFogVS( VS_INPUT IN )
{
	VS_OUTPUT_FOG OUT;
	
    // Transform the position from object space to homogeneous projection space
    OUT.Position = mul( IN.Position, g_mWorldViewProjection );
    
    
	float3x3 mTangentBasis = { IN.Tangent, IN.Binormal, IN.Normal };

	float3 vLightVector = g_SunLightDir;
	OUT.LightVector = mul( mTangentBasis, vLightVector );

	float3 vEyeVector = g_vLocalEyePos - IN.Position;
	OUT.EyeVector = mul( mTangentBasis, vEyeVector );

	float3 vHalfVector = normalize( vEyeVector ) + normalize( vLightVector );
	OUT.HalfVector = mul( mTangentBasis, vHalfVector );

   	OUT.ViewPosition = mul( mTangentBasis, g_vLocalEyePos );
	OUT.ObjPos = mul( mTangentBasis, IN.Position );
	OUT.LightPosition = mul( mTangentBasis, g_SunLightDir );	

	// FIXME: TEMP
	// Calculate the exponential fog factor.
	float fFogDensity = 0.01f;
	float fFogExponent = length( vEyeVector ) * fFogDensity;
	
	// Calculate the exponential fog factor.
	OUT.FogFactor = exp2( -abs( fFogExponent ) );	

	return OUT;
}


float4 RenderFogPS( VS_OUTPUT_FOG IN ) : COLOR
{ 
// NOTE: Disabled for now.
/*
	float f_XOffset = 0.0f;
	float f_XScale = 0.5f;
	float f_YOffset = 0.0f;
	float f_YScale = 0.5f;

	float3 V = normalize( IN.ObjPos.xyz - IN.ViewPosition );
	float3 S = normalize( IN.LightPosition - IN.ViewPosition );
	
	float Dvp = length( IN.ViewPosition - IN.ObjPos.xyz );
	float Dsv = length( IN.LightPosition - IN.ViewPosition );
	float Dsp = length( IN.LightPosition - IN.ObjPos.xyz );
	float gamma = acos( dot( S, V ) );
	
	float3 vClr = CalculateAirLight( lightIntensity, beta, Dsv, Dvp, gamma, FSampler, f_XOffset, f_XScale, f_YOffset, f_YScale );
*/
	
	// The fog color is just the light color for now.
	float3 vFogColor = float3( 0.7f, 0.5f, 0.5f );

	return float4( g_SunLightColor.rgb * IN.FogFactor * vFogColor, 1.0f );
}


VS_OUTPUT_CUBEPASSTHROUGH RenderCubeMapPassthroughVP( VS_INPUT IN )
{
    VS_OUTPUT_CUBEPASSTHROUGH OUT;
  
    // Transform the position from object space to homogeneous projection space.
    OUT.Position = mul( IN.Position, g_mWorldViewProjection );

	// Pass through the eye vector.
	OUT.TexCoord0 = IN.Position - g_vLocalEyePos;

    return OUT;    
}


PS_OUTPUT RenderCubeMapPassthroughPS( VS_OUTPUT_CUBEPASSTHROUGH IN )
{ 
	PS_OUTPUT OUT;

	// Directly sample using the given vector (normalization is implicit through the cube map).
	OUT.Color = texCUBE( CubeMapSampler, IN.TexCoord0 );

	return OUT;
}


// Calculate sky color (based on algorithms presented in "Advanced Sky Dome Rendering" by Spoerl).
float4 CalculateSkyColor( float3 vNormal, float3 vLightDir )
{
	// The color frequencies for molecules and aerosols.
	const float4 vKr = float4( 0.0005f, 0.00025f, 0.0003f, 0.0f );
	const float4 vKm = float4( 0.00001f, 0.0004f, 0.0008f, 0.0f );
	
	float fNCosL = dot( vNormal, vLightDir );
	
	// Calculate molecule scattering.
	float fFr = 0.0f;
	if ( fNCosL > 0.0f )
	{
		fFr = fNCosL < 0.0f ? 0.0f : fNCosL * fNCosL;	
	}
	
	fFr = fFr * 0.65f + 0.65f;

	float4 fRayleigh = fFr * vKr;

	// Calculate aerosol scattering.
	float fFm = max( fNCosL, -fNCosL );
	fFm = ( fFm >= 1.0f ) ? 1.65f : 0.65f;

	float4 fMie = fFm * vKm;

	// Calculate the final Sky Color.
	return ( fRayleigh + fMie );
}


VS_OUTPUT_VERTEXSKY RenderVertexSkyVS( VS_SKYINPUT IN )
{
    VS_OUTPUT_VERTEXSKY OUT;
  
    // Transform the position from object space to homogeneous projection space
    OUT.Position = mul( IN.Position, g_mWorldViewProjection );
    
	// Multiply by the inverse transpose world matrix.
	float3 vNormal = normalize( mul( IN.Normal, (float3x3)g_mWorldIT ) );

	// Calculate scaling factor (like a fake optical depth).
	//const float fSkyHeight = 1000.0f;
	//float4 fScale = pow( 0.1f, IN.Position.zzzz / fSkyHeight ).xxxx;

	float3 vClr = CalculateSkyColor( vNormal, -g_SunLightDir ) * 2300.0f * g_SunLightColor * IN.OpticalDepth;
	
	OUT.Color = float4( vClr, 1.0f );
	
	// Output the transformed projective texture coordinates.
	OUT.SunProjTC = mul( IN.Position, g_mSunTextureMatrix );
	OUT.MoonProjTC = mul( IN.Position, g_mMoonTextureMatrix );

	// Output the Cube Map passthrough texture coordinates.
	OUT.CubeTC = IN.Position - g_vLocalEyePos;

    return OUT;
}


PS_OUTPUT RenderVertexSkyPS( VS_OUTPUT_VERTEXSKY IN )
{ 
	PS_OUTPUT OUT;

	// Calculate the Sun Projection.
	float3 vSunColor = ( IN.SunProjTC.w < 0 ? 0 : tex2Dproj( SunTextureSampler, IN.SunProjTC ) ).rgb;
	
	// Fetch the moon color (maksed by the alpha channel)..
	float4 vMoonColor = ( IN.MoonProjTC.w < 0 ? 0 : tex2Dproj( MoonTextureSampler, IN.MoonProjTC ) );
	
	// Get in the stars color.
	float3 vStarsColor = texCUBE( StarMapSampler, IN.CubeTC ) * g_fStarColorModifier;

	// Calculate the final color.
	float3 v3FinalColor = lerp( ( IN.Color + vSunColor * g_SunLightColor ) + vStarsColor, vMoonColor.rgb, vMoonColor.a );

	OUT.Color = float4( v3FinalColor, 1.0f );

	return OUT;
}


VS_OUTPUT_PIXELSKY RenderPixelSkyVS( VS_SKYINPUT IN )
{
    VS_OUTPUT_PIXELSKY OUT;
  
    // Transform the position from object space to homogeneous projection space
    OUT.Position = mul( IN.Position, g_mWorldViewProjection );
    
	OUT.Color = float4( 1.0f, 1.0f, 1.0f, 1.0f );
	OUT.TexCoord0 = IN.TexCoord0; 

	float3x3 mTangentBasis = { IN.Tangent, IN.Binormal, IN.Normal };

	// Transform the vertex normal into Tangent Space.
	//OUT.VertexNormal = mul( mTangentBasis, IN.Normal );

	float3 vLightVector = g_SunLightDir;
	OUT.LightVector = mul( mTangentBasis, vLightVector );

	float3 vEyeVector = g_vLocalEyePos - IN.Position;
	OUT.EyeVector = mul( mTangentBasis, vEyeVector );

	float3 vHalfVector = normalize( vEyeVector ) + normalize( vLightVector );
	OUT.HalfVector = mul( mTangentBasis, vHalfVector );

	// Calculate scaling factor (like a fake optical depth).
	OUT.OpticalDepth = IN.OpticalDepth;
	
	// Output the transformed projective texture coordinates.
	OUT.SunProjTC = mul( IN.Position, g_mSunTextureMatrix );
	OUT.MoonProjTC = mul( IN.Position, g_mMoonTextureMatrix );
	
	// Output the Cube Map passthrough texture coordinates.
	OUT.CubeTC = IN.Position - g_vLocalEyePos;

    return OUT;
}


PS_OUTPUT RenderPixelSkyPS( VS_OUTPUT_PIXELSKY IN )
{ 
	PS_OUTPUT OUT;

	// Sample and scale/bias normal.
	float3 vNormal = normalize( tex2D( SkyNormalMapSampler, IN.TexCoord0 ).agb * 2.0 - 1.0f );
	//float3 vNormal = normalize( IN.VertexNormal );

	// Normalize light vector.
	float3 vLightVector = normalize( IN.LightVector );

	// Calculate the sky color.
	float3 vSkyColor = CalculateSkyColor( vNormal, -vLightVector ) * 2200.0f * IN.OpticalDepth;

	// Calculate the Sun Projection.
	float3 vSunColor = ( IN.SunProjTC.w < 0 ? 0 : tex2Dproj( SunTextureSampler, IN.SunProjTC ) ).rgb;
	
	// Fetch the moon color (maksed by the alpha channel)..
	float4 vMoonColor = ( IN.MoonProjTC.w < 0 ? 0 : tex2Dproj( MoonTextureSampler, IN.MoonProjTC ) );
	
	// Get in the stars color.
	float3 vStarsColor = texCUBE( StarMapSampler, IN.CubeTC ) * g_fStarColorModifier;

	// Calculate the final color.
	float3 v3FinalColor = lerp( ( vSkyColor + vSunColor ) * g_SunLightColor + vStarsColor, vMoonColor.rgb, vMoonColor.a );

	OUT.Color = float4( v3FinalColor, 1.0f );

	return OUT;
}


VS_TERRAIN_OUTPUT RenderTerrainVP( VS_INPUT IN )
{
    VS_TERRAIN_OUTPUT OUT;
  
    // Transform the position from object space to homogeneous projection space
    OUT.Position = mul( IN.Position, g_mWorldViewProjection );
   
	OUT.Color = float4( 1.0f, 1.0f, 1.0f, 1.0f );
	OUT.TexCoord0 = IN.TexCoord0; 

	float3x3 mTangentBasis = { IN.Tangent, IN.Binormal, IN.Normal };

	// Ouput the light vectors.
	OUT.SunLightVector = mul( mTangentBasis, g_SunLightDir );
	OUT.MoonLightVector = mul( mTangentBasis, g_MoonLightDir );

	float3 vEyeVector = g_vLocalEyePos - IN.Position;
	OUT.EyeVector = mul( mTangentBasis, vEyeVector );

	OUT.WorldNormal = mul( IN.Normal, (float3x3)g_mWorldIT );

    return OUT;
}


PS_OUTPUT RenderTerrainPS( VS_TERRAIN_OUTPUT IN )
{ 
	PS_OUTPUT OUT;

	// Normalize the eye, vertex interpolated vector.
	// NOTE: Calculating this in the pixel shsder leads to a slight quality at the expensse
	// of additional instructions per-fragment (as opposed to per-vertex).
	float3 vEyeVector = normalize( IN.EyeVector );

	// Sample and Scale/Bias normal.
	// NOTE: A is swizzled with R (for extra quality on compressed dds's).
	float3 vNormal = normalize( tex2D( NormalMapSampler, IN.TexCoord0 ).agb * 2.0 - 1.0f );

	// Sample the diffuse and gloss maps.
	float4 vDiffuse = tex2D( DiffuseMapSampler, IN.TexCoord0 );

	// Hack up an ambient term, using some fake lighting to accentuate the surface normals.
	const float fAmbientScale = 0.2f;
	float4 vAmbientColor = vDiffuse * fAmbientScale * dot( vNormal, float3( 0.5f, 0.6f, 0.7f ) );

	// Normalize light vector.
	float3 vSunLightVector = normalize( IN.SunLightVector );
	float3 vMoonLightVector = normalize( IN.MoonLightVector );

	// NOTE: Interpolating from ambient to direct light ensures we don't get oversaturation by always
	// adding in the ambient term but still fade out to the ambient color instead of black.
	
	// Calculate the sun light contribution.
	float fNDotL = saturate( dot( vNormal.xyz, vSunLightVector ) );
	float4 vSunLight = fNDotL * vDiffuse * g_SunLightColor;
	vSunLight = lerp( vAmbientColor, vSunLight, fNDotL );

	// Calculate the moon light contribution.
	fNDotL = saturate( dot( vNormal.xyz, vMoonLightVector ) );
	float4 vMoonLight = fNDotL * vDiffuse * g_MoonLightColor;
	vMoonLight = lerp( vAmbientColor * g_MoonLightColor, vMoonLight, fNDotL );
	
	// Sum the light contributions.
	float4 vSkyColor = vSunLight + vMoonLight;

	// The Diffuse Cube Map (indexed by the surface normal) can be used to simulate ambient lighting
	// more realistically since it captures and diffuses the local environment of the 3D object.
	//float4 vDiffuseCubeMap = texCUBE( DifuseCubeMapSampler, IN.WorldNormal );

	// Output the final color.
	OUT.Color = vSkyColor;

	return OUT;
}




technique RenderVertexSky
{
    pass P0
    {          
        VertexShader = compile vs_2_0 RenderVertexSkyVS();
        PixelShader  = compile ps_2_0 RenderVertexSkyPS();

        ZWriteEnable = FALSE;
        ZEnable = FALSE;
    }
}


technique RenderPixelSky
{
    pass P0
    {          
        VertexShader = compile vs_2_0 RenderPixelSkyVS();
        PixelShader  = compile ps_2_0 RenderPixelSkyPS();

        ZWriteEnable = FALSE;
        ZEnable = FALSE;
    }
}


technique RenderCubeMapPassthrough
{
    pass P0
    {          
        VertexShader = compile vs_2_0 RenderCubeMapPassthroughVP();
        PixelShader  = compile ps_2_0 RenderCubeMapPassthroughPS();

        ZWriteEnable = FALSE;
        ZEnable = FALSE;
    }
}


technique RenderTerrain
{
    pass P0
    {          
        VertexShader = compile vs_2_0 RenderTerrainVP();
        PixelShader  = compile ps_2_0 RenderTerrainPS();
    }
}


technique RenderDepth
{
    pass P0
    {          
        VertexShader = compile vs_1_0 RenderDepthVS();
        PixelShader  = compile ps_1_0 RenderDepthPS();
    }
}


technique RenderFog
{
    pass P0
    {          
        VertexShader = compile vs_2_0 RenderFogVS();
        PixelShader  = compile ps_2_0 RenderFogPS();
    }
}