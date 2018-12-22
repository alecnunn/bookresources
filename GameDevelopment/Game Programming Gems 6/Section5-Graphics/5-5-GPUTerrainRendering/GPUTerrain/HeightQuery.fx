//--------------------------------------------------------------------------------------
// File: HeightQuery.fx
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

float4x4	g_mWorldViewProj	: WORLDVIEWPROJ;
texture		g_Heightfield		: HEIGHTFIELD;	
												
sampler Heightfield = sampler_state				
{												
	Texture = <g_Heightfield>;					
	MipFilter = LINEAR;							
	MinFilter = LINEAR;							
	MagFilter = LINEAR;							
	ADDRESSU = WRAP;							
	ADDRESSV = WRAP;							
};												
												
void VS(float3 pos : POSITION, out float4 posO : POSITION, out float2 texO : TEXCOORD0)	
{												
	posO = mul(float4(pos,1.0f), g_mWorldViewProj);
	
	// use the incoming position's x- and z-coordinates as texture coordinates
	// in the heightfield texture
	texO = pos.xz;								
}												
												
float4 PS(float2 tex : TEXCOORD0) : COLOR		
{								
    // sample the heightfield at the position's xz-coordinate to get the 
    // elevation at that location.				
	float height = tex2D(Heightfield,tex).x;		
	
	// return the sampled height as the output color of the pixel.
	return float4(height, height, height, 1.0f);
}												
												
technique THeightQuery							
{												
	pass p0										
	{											
		VertexShader = compile vs_1_1 VS();		
		PixelShader  = compile ps_1_1 PS();		
	}											
}												

