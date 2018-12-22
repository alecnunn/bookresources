//------------------------------------------------------------
//	input/output
//------------------------------------------------------------
struct VS_IN
{
	float4		pos		: POSITION;
	float3		normal	: NORMAL;
	float2		uv		: TEXCOORD;
	float4		color	: COLOR;
};

struct VS_OUT
{
	float4		pos		: POSITION;
	float4		color	: COLOR0;
	float4		lighting: COLOR1;
	float2		uv0		: TEXCOORD0;
	float2		uv1		: TEXCOORD1;
	float2		uv2		: TEXCOORD2;
};

//------------------------------------------------------------
//	landscape vertex shader
//------------------------------------------------------------
#if VSHADER

float4x4	mtxWorldViewProj : register(c0);
float4		lightDir		 : register(c4);
float4		lightColor		 : register(c5);
float4		ambientColor	 : register(c6);

VS_OUT VS_Main( const VS_IN vtxin )
{
	VS_OUT	vtxout = (VS_OUT)0;
	float	flight;
	float3	color;

	// lighting
	flight = max(dot(lightDir,vtxin.normal),0);
	color.xyz = lightColor.xyz*flight + ambientColor.xyz;
	vtxout.lighting.xyz = min(color.xyz,1);
	vtxout.lighting.w = 1.0;

	vtxout.pos		=	mul(mtxWorldViewProj,vtxin.pos);
	vtxout.color	=	vtxin.color;
	vtxout.uv0		=	vtxin.uv;
	vtxout.uv1		=	vtxin.uv;
	vtxout.uv2		=	vtxin.uv;

	return vtxout;
}

#endif

//------------------------------------------------------------
//	landscape pixel shader
//------------------------------------------------------------
#if PSHADER

sampler texture0;
sampler texture1;
sampler texture2;

float4 PS_Main( VS_OUT pixin ) : COLOR
{
	float4	color;
	float4	vRed	= {1,0,0,0};
	float4	vGreen	= {0,1,0,0};
	float4	vBlue	= {0,0,1,0};

	color  = dot(vRed,	pixin.color) * tex2D(texture0,pixin.uv0);
	color += dot(vGreen,pixin.color) * tex2D(texture1,pixin.uv1);
	color += dot(vBlue,	pixin.color) * tex2D(texture2,pixin.uv2);

	return color*pixin.lighting;
}

#endif
