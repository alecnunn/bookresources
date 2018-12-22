#include "Camera.h"
#include "Defines.h"

extern LPDIRECT3DDEVICE8 g_Device;

Camera::Camera()
{
	m_fFov = 0.78f;
	m_fAspect = 2.2;		
	m_fNear = 1.0;
	m_fFar = 50000;

	m_eye = D3DXVECTOR3(0,10,-5);
	m_look = D3DXVECTOR3(0,0,1);
	m_up = D3DXVECTOR3(0,1,0);

	// From terrain
	m_eye=D3DXVECTOR3(PATCH_WORLD_WIDTH/2, HEIGHT_SCALE/4*255,0);//-PATCH_WORLD_WIDTH/16); 

	m_eye = D3DXVECTOR3(1280, 204.753647, -81.971596);
	m_look = D3DXVECTOR3(-0.29552, -0.213142, 0.931256);
	m_up = D3DXVECTOR3(0, 0.974794, 0.223106);

	Normalize();

	m_speed = 5.0f;
	m_rotSpeed = 0.005f;

	CalculateProjectionMatrix();
	CalculateViewMatrix();
}

Camera::~Camera()
{

}





void Camera::MoveForward()
{
	D3DXVECTOR3 eye = GetEye() + GetSpeed()*GetLookDirection();
	SetEye(eye);
}

void Camera::MoveBackward()
{
	D3DXVECTOR3 eye = GetEye() - GetSpeed()*GetLookDirection();
	SetEye(eye);
}


void Camera::Pitch(float theta)
{
	D3DXVECTOR3 look = GetLookDirection();
	D3DXVECTOR3 up = GetUpDirection();

	D3DXVECTOR3 new_look = cos(theta)*look + sin(theta)*up;
	D3DXVECTOR3 new_up =  -sin(theta)*look + cos(theta)*up;
	SetLookDirection(new_look);
	SetUpDirection(new_up);
}



void Camera::MoveUp()
{
	D3DXVECTOR3 eye = GetEye() + GetSpeed()*GetUpDirection();
	SetEye(eye);
}

void Camera::MoveDown()
{
	D3DXVECTOR3 eye = GetEye() - GetSpeed()*GetUpDirection();
	SetEye(eye);
}


void Camera::MoveRight(int sign /* = 1*/) 
{
	D3DXVECTOR3 right;
	D3DXVECTOR3 look = GetLookDirection();
	D3DXVECTOR3 up  = GetUpDirection();

	D3DXVec3Cross(&right, &up, &look);		// Left handed system, I guess ?
	D3DXVECTOR3 eye = GetEye() +GetSpeed()*right*sign;
	SetEye(eye);

}

void Camera::Yaw(float theta)
// theta > 0 implies yaw right.
{
	D3DXVECTOR3 right, look, up, newlook;
	look = GetLookDirection();
	up = GetUpDirection();
	D3DXVec3Cross(&right, &look, &up);
	right = -right; // because its a left handed system.
	newlook = cos(theta)*look  + sin(theta)*right; 
	SetLookDirection(newlook);
}

void Camera::Roll(float theta)
// theta > 0 implies roll right.
{
	D3DXVECTOR3 right, look, up, newup;
	look = GetLookDirection();
	up = GetUpDirection();
	D3DXVec3Cross(&right, &look, &up);
	// right = -right; // because its a left handed system.
	newup = cos(theta)*up + sin(theta)*right;
	SetUpDirection(newup);
}


void Camera::Update(const CameraMovements &cm)
{
	Control(cm);

	CalculateViewMatrix();
	D3DXMatrixMultiply(&m_ViewProjMatrix, &m_ViewMatrix, &m_ProjMatrix);


	g_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	g_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);
}



void Camera::Control(const CameraMovements &cm)
{
	switch (cm) {
	case eCameraNone:
		return;
	case eCameraMoveForward:
		MoveForward();
		break;
	case eCameraMoveBackward:
		MoveBackward();
		break;
	case eCameraPitchUp:
		PitchUp();
		break;
	case eCameraPitchDown:
		PitchDown();
		break;
	case eCameraMoveUp:
		MoveUp();
		break;
	case eCameraMoveDown:
		MoveDown();
		break;
	case eCameraYawLeft:
		YawLeft();
		break;
	case eCameraYawRight:
		YawRight();
		break;
	case eCameraMoveLeft:
		MoveLeft();
		break;
	case eCameraMoveRight:
		MoveRight();
		break;
	case eCameraRollLeft:
		RollLeft();
		break;
	case eCameraRollRight:
		RollRight();
		break;
	default:
		break;
	}
}


void Camera::CalculateProjectionMatrix()
{
	// Set up the projection matrix
	D3DXMatrixPerspectiveFovLH
	(
		&m_ProjMatrix,
		m_fFov,			// field of view PI/2
		m_fAspect,		// aspect
		m_fNear,			// near
		m_fFar			// far
	);
}

void Camera::CalculateViewMatrix()
{
	
	D3DXMatrixIdentity(&m_ViewMatrix);

	D3DXVECTOR3 eye = GetEye();
	D3DXVECTOR3 lookat = eye + GetLookDirection();
	D3DXVECTOR3 up = GetUpDirection();

    D3DXMatrixLookAtLH( &m_ViewMatrix, 
		&eye,
        &lookat,
        &up);
}


void Camera::Interpolate(Camera *one, Camera *two, float f)
{
	*this = *one;

	m_eye = one->GetEye()*(1-f) + f*two->GetEye();
	m_look = one->GetLookDirection() * (1-f) + f*two->GetLookDirection();
	m_up = one->GetUpDirection() * (1-f) + f*two->GetUpDirection();

	Normalize();

	CalculateProjectionMatrix();
	CalculateViewMatrix();
	
}


void Camera::Dump2(FILE *f)
{
	fprintf(f, "%f %f %f ", m_eye[0], m_eye[1], m_eye[2]);
	fprintf(f, "%f %f %f ", m_look[0], m_look[1], m_look[2]);
	fprintf(f, "%f %f %f ", m_up[0], m_up[1], m_up[2]);

}

void Camera::Normalize()
{
	D3DXVec3Normalize(&m_look, &m_look);
	D3DXVec3Normalize(&m_up, &m_up);
}


void Camera::Read2(FILE *f)
{
	D3DXVECTOR3 look, up;
	fscanf(f, "%f %f %f ", &m_eye[0], &m_eye[1], &m_eye[2]);
	fscanf(f, "%f %f %f ", &m_look[0], &m_look[1], &m_look[2]);
	fscanf(f, "%f %f %f ", &m_up[0], &m_up[1], &m_up[2]);

	Finalize();
}

void Camera::Finalize()
{
	Normalize();

	CalculateProjectionMatrix();
	CalculateViewMatrix();
}


Camera Camera::operator + ( const Camera& c2) const
{
	Camera res;
	res.m_eye = m_eye + c2.m_eye;
	res.m_look = m_look + c2.m_look;
	res.m_up = m_up + c2.m_up;

	return res;
}

Camera Camera::operator * ( float f) const
{
	Camera res;
	res.m_eye = m_eye * f;
	res.m_look = m_look * f;
	res.m_up = m_up * f;
	return res;
}

Camera operator * ( float f, const Camera& c )
{
	Camera res;
	res.m_eye = c.m_eye * f;
	res.m_look = c.m_look * f;
	res.m_up = c.m_up * f;
	return res;
}



