//------------------------------------------------------------
//	input/output
//------------------------------------------------------------
struct VS_IN
{
	float4		pos		: POSITION;
	float2		uv		: TEXCOORD;
	float2		index	: BLENDINDICES;
};

struct VS_OUT
{
	float4		pos		: POSITION;
	float4		color	: COLOR;
	float2		uv		: TEXCOORD0;
};

//------------------------------------------------------------
//	widget vertex shader
//------------------------------------------------------------
#if VSHADER

float4x4	mtxWorldViewProj : register(c0);
float4		fadePlane		 : register(c15);
float4		mtxInstances[80] : register(c16);

VS_OUT VS_Main( const VS_IN vtxin )
{
	VS_OUT	vtxout = (VS_OUT)0;
	float4	pos;
	int		index = vtxin.index.x;

	// rotate incoming vertex position and normal
	pos.x    = dot(vtxin.pos,mtxInstances[index+1].xwy);
	pos.y    = vtxin.pos.y;
	pos.z    = dot(vtxin.pos,mtxInstances[index+1].ywz);

	// scale and add origin
	pos.xyz  = pos.xyz*mtxInstances[index].w + mtxInstances[index].xyz;
	pos.w    = 1;

	// transform position & copy UV
	vtxout.pos		=	mul(mtxWorldViewProj,pos);
	vtxout.uv		=	vtxin.uv;

	// apply alpha from fade plane
	vtxout.color.xyz = float3(1,1,1);
	vtxout.color.w	 =	clamp(dot(fadePlane,pos),0,1);

	return vtxout;
}
#endif

//------------------------------------------------------------
//	widget pixel shader
//------------------------------------------------------------
#if PSHADER

sampler texture0;

float4 PS_Main( VS_OUT pixin ) : COLOR
{
	float4	color;
	
	color = tex2D(texture0,pixin.uv);
	color *= pixin.color;

	return color;
}

#endif
