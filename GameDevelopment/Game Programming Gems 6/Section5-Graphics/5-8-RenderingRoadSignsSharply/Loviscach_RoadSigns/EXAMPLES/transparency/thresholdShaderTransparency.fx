struct VertexOutput
{
	float4 position  : POSITION;
	float2 uv : TEXCOORD0;
};

struct PixelOutput
{
	float4 color : COLOR0;
};

texture myTexture
<
	string ResourceName = "bicycleAndPavement.dds";
>;

sampler mySampler = sampler_state 
{
    texture = <myTexture>;
    AddressU  = CLAMP;        
    AddressV  = CLAMP;
    MIPFILTER = LINEAR;
    MINFILTER = LINEAR;
    MAGFILTER = LINEAR;
};

PixelOutput ThresholdShader(VertexOutput IN)
{
	PixelOutput OUT;
	half4 t = tex2D(mySampler, IN.uv);
	float2 uvScaled = IN.uv*float2(64, 128); // width & height of texture
	float2 ef = ddx(uvScaled);
	float2 gh = ddy(uvScaled);
	half sInv = length(float4(ef, gh));
    half a = max(1.0, (3*1.4)/sInv);
	half c = clamp(0.5 + a*(t.r-0.5), 0.0, 1.0);
	half d = t.a*(0.8+0.2*c) + 0.8 - 0.8*c;
	OUT.color = half4(d, d, d, 1.0);
	return OUT;
}

technique threshold
{
	pass p0 
	{
		VertexShader = NULL;
		PixelShader = compile ps_2_a ThresholdShader();
	}
}