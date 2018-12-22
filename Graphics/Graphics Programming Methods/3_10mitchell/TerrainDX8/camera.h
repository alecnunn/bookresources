// GPM - Pixel Shader Optimizations for Terrain Rendering Demo
// Kenny Mitchell
// Copyright Electronic Arts 2002, 2003

#ifndef CAMERA_H
#define CAMERA_H

#ifndef DEBUG_H
#include "debug.h"
#endif

#ifndef INPUT_H
#include "input.h"
#endif

class Camera_Class
{
public:
	Camera_Class();

	void Update();

	void Set_Input(Input_Class* input) { Input=input; }

	D3DXMATRIX* Get_View_Projection_Matrix() { return &View_Projection_Matrix; }
	D3DXMATRIX* Get_Projection_Matrix() { return &Projection_Matrix; }
	D3DXMATRIX* Get_View_Matrix() { return &View_Matrix; }

	D3DXVECTOR3* Get_Eye() { return &Eye; }

	unsigned int Test_AAPatch_Against_Frustum(D3DXVECTOR4& corner, D3DXVECTOR3& delta);

private:

	void Control();

	void Mouse_Yaw(const int d) { Theta-=((float)(d))*0.01f; }
	void Mouse_Pitch(const int d) { Phi+=((float)(d))*0.01f; }

	void Forward() { Eye.z+=Speed*CosTheta; Eye.x+=Speed*SinTheta; }
	void Back()		{ Eye.z-=Speed*CosTheta; Eye.x-=Speed*SinTheta; }

	void Up()		{ Eye.y+=Speed; }
	void Down()		{ Eye.y-=Speed; }

	void Left()		{ Eye.x+=Speed*CosTheta; Eye.z-=Speed*SinTheta; }
	void Right()	{ Eye.x-=Speed*CosTheta; Eye.z+=Speed*SinTheta; }

	void Pitch_Up()	{ Phi-=0.01f; }
	void Pitch_Down()	{ Phi+=0.01f; }

	void Set_Speed(const float speed) { Speed=speed; }
	float Get_Speed() const { return Speed; }

	float Get_Planar_Sine() const { return SinPhi; }

	void Calculate_Projection_Matrix();


	Input_Class*	Input;

	D3DXVECTOR3		Eye;
	D3DXVECTOR3		Velocity;
	D3DXMATRIX		View_Matrix;
	D3DXMATRIX		Projection_Matrix;
	D3DXMATRIX		View_Projection_Matrix;

	float FOV, Aspect, Near, Far; // projection parameters
	float	Speed;
	float	Theta, Phi;

	float CosTheta, SinTheta;
	float CosPhi, SinPhi;
};

#endif