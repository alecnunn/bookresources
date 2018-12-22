struct VertexOutput
{
	float4 position  : POSITION;
	float2 uv : TEXCOORD0;
	float2 uvScaled : TEXCOORD1;
};

struct PixelOutput
{
	float4 color : COLOR0;
};

sampler mySampler;

PixelOutput ThresholdShader(VertexOutput IN)
{
	PixelOutput OUT;
	half4 t = tex2D(mySampler, IN.uv);
	float2 ef = ddx(IN.uvScaled);
	float2 gh = ddy(IN.uvScaled);
	half sInv = length(float4(ef, gh));
    half a = max(1.0, (3*1.4)/sInv);
	half c = 0.5 + a*(t.r-0.5);
	OUT.color = half4(c, c, c, 1.0);
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