// GPM - Pixel Shader Optimizations for Terrain Rendering Demo
// Kenny Mitchell
// Copyright Electronic Arts 2002, 2003

#include "camera.h"

Camera_Class::Camera_Class()
: FOV(0.867f),
  Aspect(1.0f),
  Near(1.0f), 
  Far(2024),
  Speed(5.0f),
  Theta(0.5f),
  Phi(-0.25f)
{
	D3DXMatrixIdentity(&View_Matrix);

	Eye=D3DXVECTOR3(64.0f,64.0f,0.0f);
	Velocity=D3DXVECTOR3(0.0f,0.0f,0.0f);

	Calculate_Projection_Matrix();
}

void Camera_Class::Update()
{
	Control();

	Eye.x+=Velocity.x;
	Eye.y+=Velocity.y;
	Eye.z+=Velocity.z;

	// damp velocity
	Velocity.x*=0.99f;
	Velocity.y*=0.99f;
	Velocity.z*=0.99f;

	// clamp vertical velocity
	if (fabsf(Velocity.y)<0.0001f) Velocity.y=0.0f;

	D3DXVECTOR3 up(0,1,0);

	SinTheta=sinf(Theta);
	CosTheta=cosf(Theta);
	//ASM_fsincos(fTheta,SinTheta,CosTheta);

	if (Phi>1.56f) Phi=1.56f;
	else if (Phi<-1.56f) Phi=-1.56f;

	SinPhi=sinf(Phi);
	CosPhi=cosf(Phi);
	//ASM_fsincos(Phi,SinPhi,CosPhi);

	D3DXVECTOR3 look_at(SinTheta*CosPhi,SinPhi,CosTheta*CosPhi);

	look_at=Eye+look_at;

	D3DXMatrixLookAtRH
	( 
		&View_Matrix, 
		&Eye, 
		&look_at, 
		&up
	);

	DEV->SetTransform(D3DTS_VIEW, &View_Matrix);

	D3DXMatrixMultiply(&View_Projection_Matrix,&View_Matrix,&Projection_Matrix);
}

void Camera_Class::Control()
{
	// keyboard input responses  
	if (Input->Get_Key(VK_LEFT))		Left();
	if (Input->Get_Key(VK_RIGHT))		Right();
	if (Input->Get_Key(VK_PRIOR))		Up();
	if (Input->Get_Key(VK_NEXT))		Down();
	if (Input->Get_Key(VK_UP))			Forward();
	if (Input->Get_Key(VK_DOWN))		Back();
	if (Input->Get_Key(VK_HOME))		Pitch_Up();
	if (Input->Get_Key(VK_END))		Pitch_Down();
	if (Input->Get_Key(VK_DELETE))	Mouse_Yaw(1);
	if (Input->Get_Key(VK_INSERT))	Mouse_Yaw(-1);

	if (Input->Get_Key('A')) Set_Speed(Speed+0.001f);
	if (Input->Get_Key('Z')) Set_Speed(Speed-0.001f);

	Mouse_Yaw(Input->Get_Mouse_Delta_X());
	Mouse_Pitch(Input->Get_Mouse_Delta_Y());

	if (Input->Get_Mouse_LButton()) Forward();
	if (Input->Get_Mouse_RButton()) Back();
}


void Camera_Class::Calculate_Projection_Matrix()
{
	// Set up the projection matrix
	D3DXMatrixPerspectiveFovRH
	(
		&Projection_Matrix,
		FOV,			// field of view PI/2
		Aspect,		// aspect
		Near,			// near
		Far			// far
	);

	DEV->SetTransform(D3DTS_PROJECTION, &Projection_Matrix);
}


unsigned int Camera_Class::Test_AAPatch_Against_Frustum(D3DXVECTOR4& corner, D3DXVECTOR3& delta)
{
	// transform box corners into homogeneous space and clip results
	// uses screen space offset method with substantially reduced cost
	// of transforming corner points and contains no conditional branches (well...1 in the clip loop)
	D3DXVECTOR4 trans_pos[8];
	D3DXVECTOR4* bp=trans_pos;
	D3DXMATRIX& cam=View_Projection_Matrix;

	// transform corner
	D3DXVec4Transform(bp, &corner, &cam);

	D3DXVECTOR4 odx(cam._11*delta.x, cam._12*delta.x, cam._13*delta.x, cam._14*delta.x);
	D3DXVECTOR4 ody(cam._21*delta.y, cam._22*delta.y, cam._23*delta.y, cam._24*delta.y);
	D3DXVECTOR4 odz(cam._31*delta.z, cam._32*delta.z, cam._33*delta.z, cam._34*delta.z);

	// transform and clip other 7 corners using offset vectors and incremental calcuations
	D3DXVECTOR4* p=&trans_pos[1];
	float x,y,z,w;

	// 1.(dx,0,0)
	x=p->x=bp->x+odx.x;
	y=p->y=bp->y+odx.y;
	z=p->z=bp->z+odx.z;
	w=p->w=bp->w+odx.w;
	p++;

	// 2.(dx,dy,0)
	x=p->x=x+ody.x;
	y=p->y=y+ody.y;
	z=p->z=z+ody.z;
	w=p->w=w+ody.w;
	p++;

	// 3.(dx,dy,dz)
	x=p->x=x+odz.x;
	y=p->y=y+odz.y;
	z=p->z=z+odz.z;
	w=p->w=w+odz.w;
	p++;

	// 4.(dx,0,dz)
	x=p->x=x-ody.x;
	y=p->y=y-ody.y;
	z=p->z=z-ody.z;
	w=p->w=w-ody.w;
	p++;

	// 5.(0,0,dz)
	x=p->x=x-odx.x;
	y=p->y=y-odx.y;
	z=p->z=z-odx.z;
	w=p->w=w-odx.w;
	p++;

	// 6.(0,dy,dz)
	x=p->x=x+ody.x;
	y=p->y=y+ody.y;
	z=p->z=z+ody.z;
	w=p->w=w+ody.w;
	p++;

	// 7.(0,dy,0)
	p->x=x-odz.x;
	p->y=y-odz.y;
	p->z=z-odz.z;
	p->w=w-odz.w;

	// clip results
	int cXL=0, cXU=0;
	int cYL=0, cYU=0;
	int cZL=0, cZU=0;

	// accumulate clip results
	p=bp;

	int i=8;
	while (i--)
	{
		p->z*=1.0007f; // adjustment tuned to fog distance
		cXL+=(p->x<-p->w);		cXU+=(p->x>p->w);
		cYL+=(p->y<-p->w);		cYU+=(p->y>p->w);
		cZL+=(p->z<-p->w);		cZU+=(p->z>p->w);
		p++;
	}

	return ((cXL|cXU|cYL|cYU|cZL|cZU)&8);
}
