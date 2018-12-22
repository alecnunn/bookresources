//-----------------------------------------------------------------------------
// File: ColumetricPostProcess.fx
//-----------------------------------------------------------------------------


float4x4 g_mWorldViewProj;
float4x4 g_mTexture;

float g_fScale;

sampler2D g_RenderTarget : register(s0);
sampler2D g_BumpMap : register(s1);
sampler2D s2 : register(s2);


//-----------------------------------------------------------------------------
// Name: vsComputeDepth     
// Type: Vertex shader                                      
//-----------------------------------------------------------------------------
struct sComputeDepthOutput 
{
   float4 Pos				: POSITION;
   float3 fDepth			: TEXCOORD0; // x = u, y = v, z = depth
};

sComputeDepthOutput vsComputeDepth( float3 inPos: POSITION )
{
	sComputeDepthOutput Out;

	// Computes the new position of the vertex
	Out.Pos = mul( float4( inPos, 1.f ), g_mWorldViewProj );
	
	// We divide the position by w so that no perspective is applied when texture mapping
	// the triangles ( which would cause artifacts when sampling pixel value from our depth
	// buffer in the pixel shader.
	Out.Pos /= Out.Pos.w;

	// Computes the UV coordinates to use by the TestDepth pixel shader
	// to compare between depth it calculates and the one in depth buffer
	float4 fTmpCoords = mul( float4( inPos, 1.f ), g_mTexture );

	Out.fDepth.x = fTmpCoords.x / fTmpCoords.w;
	Out.fDepth.y = fTmpCoords.y / fTmpCoords.w;
	Out.fDepth.z = fTmpCoords.z;

	return Out;
}


//-----------------------------------------------------------------------------
// Name: WriteDepth     
// Type: pixel shader                                      
//-----------------------------------------------------------------------------
float4 psWriteDepth( float3 fDepth: TEXCOORD0 ) : COLOR
{
   float4 oColor;
   
   // We store the depth of the pixel in the red channel of the texture
   // since R32F texture is used, we have 32 bits precision for our depth buffer.      
   oColor.r = saturate( fDepth.z );
   oColor.g = 0;
   oColor.b = 0;
   oColor.a = 0;
   
   return oColor;
}


//-----------------------------------------------------------------------------
// Name: TestDepth     
// Type: pixel shader                                      
//-----------------------------------------------------------------------------
float4 psTestDepth(	float3 fDepth: TEXCOORD0 ) : COLOR
{
  float4 oColor;
  
  // Output constant color:
  oColor.r = 0.0;
  oColor.g = 0.0;
  oColor.b = 0.0;
  oColor.a = ( tex2D( g_RenderTarget, saturate( fDepth.xy ) ).r < fDepth.z ) ? 1.f : 0.f;
  
  return oColor;
}


//-----------------------------------------------------------------------------
// Name: WriteDepthBuffer
// Type: Technique                                     
// Desc: Renders the RenderTarget as a depth buffer, putting the depth value
//       in the red channel.
//-----------------------------------------------------------------------------
technique WriteDepthBuffer
{
    pass P0
    {        
        VertexShader = compile vs_2_0 vsComputeDepth();
        PixelShader  = compile ps_2_0 psWriteDepth();
    }
}


//-----------------------------------------------------------------------------
// Name: TestDepthBuffer
// Type: Technique                                     
// Desc: Tests the Depth of the current pixel with the one of the Texture, and
//       then sets the alpha to 0 or 1 in consequence, so that the conventional
//		 depth buffer gets updated depending of the depth test executed by
//		 this effect.
//-----------------------------------------------------------------------------
technique TestDepthBuffer
{
    pass P0
    {        
        VertexShader = compile vs_2_0 vsComputeDepth();
        PixelShader  = compile ps_2_0 psTestDepth();
    }
}


//-----------------------------------------------------------------------------
// Name: vsPostProcess1
// Type: Vertex shader                                      
//-----------------------------------------------------------------------------
struct sPostProcess1Output 
{
   float4 Pos: POSITION;
   float2 fCoords : TEXCOORD0; 
};


sPostProcess1Output vsPostProcess1( float3 inPos: POSITION )
{
	sPostProcess1Output Out;

	Out.Pos = mul( float4( inPos, 1.f ), g_mWorldViewProj );

	// Computes the UV coordinates to use by the TestDepth pixel shader
	// to compare between depth it calculates and the one in depth buffer
	float4 fTmpCoords = mul( float4( inPos, 1.f ), g_mTexture );

	Out.fCoords.x = fTmpCoords.x / fTmpCoords.w;
	Out.fCoords.y = fTmpCoords.y / fTmpCoords.w;

	return Out;
}


//-----------------------------------------------------------------------------
// Name: psPostProcess1
// Type: pixel shader                                      
//-----------------------------------------------------------------------------
float4 psPostProcess1( float2 fCoords: TEXCOORD0 ) : COLOR
{
	float2 vDist = fCoords - float2( .5f, .5f );
	float fDist = sqrt( length( vDist ) );

	vDist = float2( .5f, .5f ) + vDist / fDist / 2.15f / sqrt( sqrt( fDist ) ) / sqrt( sqrt( 2.15f ) );

   float4 oColor = tex2D( g_RenderTarget, saturate( vDist.xy ) );
   
   return oColor;
}


//-----------------------------------------------------------------------------
// Name: PostProcess1
// Type: Technique                                     
// Desc: PostProcess Effect applied to the Sphere, simulating a sort of
// gravity field.
//-----------------------------------------------------------------------------
technique PostProcess1
{
    pass P0
    {        
        VertexShader = compile vs_2_0 vsPostProcess1();
        PixelShader  = compile ps_2_0 psPostProcess1();
    }
}


//-----------------------------------------------------------------------------
// Name: vsPostProcess2
// Type: Vertex shader                                      
//-----------------------------------------------------------------------------
struct sPostProcess2Output 
{
   float4 Pos: POSITION;
   float2 fCoords : TEXCOORD0; 
};


sPostProcess2Output vsPostProcess2( float3 inPos: POSITION )
{
	sPostProcess2Output Out;

	Out.Pos = mul( float4( inPos, 1.f ), g_mWorldViewProj );

	// Computes the UV coordinates to use by the TestDepth pixel shader
	// to compare between depth it calculates and the one in depth buffer
	float4 fTmpCoords = mul( float4( inPos, 1.f ), g_mTexture );

	Out.fCoords.x = fTmpCoords.x / fTmpCoords.w;
	Out.fCoords.y = fTmpCoords.y / fTmpCoords.w;

	return Out;
}


//-----------------------------------------------------------------------------
// Name: psPostProcess2
// Type: pixel shader                                      
//-----------------------------------------------------------------------------
float4 psPostProcess2( float2 fCoords: TEXCOORD0 ) : COLOR
{
	float2 fBumpMapCoords = tex2D( g_BumpMap, fCoords );
	
	float2 vDist = fCoords - float2( .5f, .5f );
	float fDist = sqrt( length( vDist ) );

	vDist = float2( .5f, .5f ) + vDist / fDist / 2.15f / sqrt( sqrt( fDist ) ) / sqrt( sqrt( 2.15f ) );

	float fLen = length( float2( .5f, .5f ) - fCoords ); 
	
	float4 oColor = tex2D(	g_RenderTarget, vDist +
							( ( fBumpMapCoords.xy - float2( .5f, .5f ) ) / g_fScale / ( .01f + fLen ) ) );   
	
	return oColor;
}


//-----------------------------------------------------------------------------
// Name: PostProcess2
// Type: Technique                                     
// Desc: PostProcess Effect applied to the Sphere, simulating a gravity field
// with an animated distortion applied through a BumpMap.
//-----------------------------------------------------------------------------
technique PostProcess2
{
    pass P0
    {        
        VertexShader = compile vs_2_0 vsPostProcess2();
        PixelShader  = compile ps_2_0 psPostProcess2();
    }
}


//-----------------------------------------------------------------------------
// Name: vsAirplaneMaterial
// Type: Vertex shader                                      
//-----------------------------------------------------------------------------
struct sAirplaneMaterialOutput 
{
   float4 Pos: POSITION;
   float2 fCoords : TEXCOORD0; 
};


sAirplaneMaterialOutput vsAirplaneMaterial( float3 inPos: POSITION, float2 fCoords : TEXCOORD0 )
{
	sAirplaneMaterialOutput Out;

	Out.Pos = mul( float4( inPos, 1.f ), g_mWorldViewProj );
	Out.fCoords = float2( 1.f, 1.f ) - fCoords;

	return Out;
}


//-----------------------------------------------------------------------------
// Name: psAirplaneMaterial
// Type: pixel shader                                      
//-----------------------------------------------------------------------------
float4 psAirplaneMaterial( float4 fCoords: TEXCOORD0 ) : COLOR
{
   float4 oColor = tex2D( g_RenderTarget, fCoords );
   
   return oColor;
}


//-----------------------------------------------------------------------------
// Name: AirplaneMaterial
// Type: Technique                                     
// Desc: Effect applied to the material of the airplanes.
//-----------------------------------------------------------------------------
technique AirplaneMaterial
{
    pass P0
    {        
        VertexShader = compile vs_2_0 vsAirplaneMaterial();
        PixelShader  = compile ps_2_0 psAirplaneMaterial();
    }
}