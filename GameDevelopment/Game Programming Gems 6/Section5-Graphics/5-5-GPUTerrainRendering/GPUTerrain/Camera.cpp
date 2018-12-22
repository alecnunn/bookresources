//--------------------------------------------------------------------------------------
// File: Camera.cpp
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

#include "dxstdafx.h"
#include "Camera.h"

Camera g_Camera;

//----------------------------------------------------------------------------------

Camera::Camera()
	:
	m_vEye(1000,100,1000),
	m_vLookAt(1000,100,1005),
	m_vUp(0,1,0),
	m_bViewDirty(true),
	m_bWorldViewProjDirty(true)
{
}

//----------------------------------------------------------------------------------

Camera::~Camera()
{
}

//----------------------------------------------------------------------------------

D3DXVECTOR3 Camera::GetDirectionVector() const
{ 
	D3DXVECTOR3 vDir = m_vLookAt - m_vEye; 
	D3DXVec3Normalize(&vDir, &vDir); 
	return vDir; 
}

//----------------------------------------------------------------------------------

void Camera::MoveForward(float fSpeed)
{
	D3DXVECTOR3 vDir = m_vLookAt - m_vEye;
	D3DXVec3Normalize(&vDir, &vDir);

	m_vEye.x += vDir.x * fSpeed;
	m_vEye.z += vDir.z * fSpeed;
	m_vLookAt.x += vDir.x * fSpeed;
	m_vLookAt.z += vDir.z * fSpeed;

	m_bViewDirty = true;
	m_bWorldViewProjDirty = true;
}

//----------------------------------------------------------------------------------

void Camera::MoveBackward(float fSpeed)
{
	D3DXVECTOR3 vDir = m_vLookAt - m_vEye;
	D3DXVec3Normalize(&vDir, &vDir);

	m_vEye.x -= vDir.x * fSpeed;
	m_vEye.z -= vDir.z * fSpeed;
	m_vLookAt.x -= vDir.x * fSpeed;
	m_vLookAt.z -= vDir.z * fSpeed;

	m_bViewDirty = true;
	m_bWorldViewProjDirty = true;
}

//----------------------------------------------------------------------------------

void Camera::StrafeLeft(float fSpeed)
{
	D3DXVECTOR3 vDir = m_vLookAt - m_vEye;
	D3DXVec3Normalize(&vDir, &vDir);

	D3DXVECTOR3 vLeft;
	D3DXVec3Cross(&vLeft, &vDir, &m_vUp);
	m_vEye.x += vLeft.x * fSpeed;
	m_vEye.z += vLeft.z * fSpeed;
	m_vLookAt.x += vLeft.x * fSpeed;
	m_vLookAt.z += vLeft.z * fSpeed;

	m_bViewDirty = true;
	m_bWorldViewProjDirty = true;
}

//----------------------------------------------------------------------------------

void Camera::StrafeRight(float fSpeed)
{
	D3DXVECTOR3 vDir = m_vLookAt - m_vEye;
	D3DXVec3Normalize(&vDir, &vDir);

	D3DXVECTOR3 vLeft;
	D3DXVec3Cross(&vLeft, &vDir, &m_vUp);
	m_vEye.x -= vLeft.x * fSpeed;
	m_vEye.z -= vLeft.z * fSpeed;
	m_vLookAt.x -= vLeft.x * fSpeed;
	m_vLookAt.z -= vLeft.z * fSpeed;

	m_bViewDirty = true;
	m_bWorldViewProjDirty = true;
}

//----------------------------------------------------------------------------------

void Camera::MoveUp(float fSpeed)
{
	m_vEye.x  += m_vUp.x * fSpeed;
	m_vEye.y  += m_vUp.y * fSpeed;
	m_vEye.z  += m_vUp.z * fSpeed;
	m_vLookAt.x += m_vUp.x * fSpeed;
	m_vLookAt.y += m_vUp.y * fSpeed;
	m_vLookAt.z += m_vUp.z * fSpeed;

	m_bViewDirty = true;
	m_bWorldViewProjDirty = true;
}

//----------------------------------------------------------------------------------

void Camera::MoveDown(float fSpeed)
{
	m_vEye.x  -= m_vUp.x * fSpeed;
	m_vEye.y  -= m_vUp.y * fSpeed;
	m_vEye.z  -= m_vUp.z * fSpeed;
	m_vLookAt.x -= m_vUp.x * fSpeed;
	m_vLookAt.y -= m_vUp.y * fSpeed;
	m_vLookAt.z -= m_vUp.z * fSpeed;

	m_bViewDirty = true;
	m_bWorldViewProjDirty = true;
}

//----------------------------------------------------------------------------------

void Camera::RotateView(float angle, float x, float y, float z)
{
	D3DXVECTOR3 vView = m_vLookAt - m_vEye;
	D3DXVECTOR3 vNew;

	float cosTheta = cosf(angle);
	float sinTheta = sinf(angle);

	vNew.x = (cosTheta + (1 - cosTheta) * x * x) * vView.x;
	vNew.x += ((1 - cosTheta) * x * y - z * sinTheta) * vView.y;
	vNew.x += ((1 - cosTheta) * x * z + y * sinTheta) * vView.z;

	vNew.y = ((1 - cosTheta) * x * y + z * sinTheta) * vView.x;
	vNew.y += (cosTheta + (1 - cosTheta) * y * y) * vView.y;
	vNew.y += ((1 - cosTheta) * y * z - x * sinTheta) * vView.z;

	vNew.z = ((1 - cosTheta) * x * z - y * sinTheta) * vView.x;
	vNew.z += ((1 - cosTheta) * y * z + x * sinTheta) * vView.y;
	vNew.z += (cosTheta + (1 - cosTheta) * z * z) * vView.z;

	m_vLookAt = m_vEye + vNew;
}

//----------------------------------------------------------------------------------

void Camera::SetViewByMouse(int xrel, int yrel)
{
	float angleY = 0.0f;
	float angleZ = 0.0f;
	static float currentRotX = 0.0f;

	// the direction of the mouse movement
	angleY = (float)-xrel / 1000.0f;
	angleZ = (float)-yrel / 1000.0f;

	// restrict the camera from looping 360 degrees
	currentRotX -= angleZ;
	if (currentRotX > 1.0f)
		currentRotX = 1.0f;
	else if (currentRotX < -1.0f)
		currentRotX = -1.0f;
	else { // rotate the view around the camera's current position
		D3DXVECTOR3 vVector = m_vLookAt - m_vEye;
		D3DXVECTOR3 vAxis;
		D3DXVec3Cross(&vAxis, &vVector, &m_vUp);
		D3DXVec3Normalize(&vAxis,&vAxis);
		RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
		RotateView(angleY, 0, 1, 0);
	}

	m_bViewDirty = true;
	m_bWorldViewProjDirty = true;
}

//----------------------------------------------------------------------------------

D3DXVECTOR3 Camera::GetEyeLocal()
{
	D3DXMATRIX mWorldInverse;
	D3DXMatrixInverse(&mWorldInverse, NULL, &m_mWorld);
	D3DXVECTOR4 ptEyeLocal;
	D3DXVec3Transform(&ptEyeLocal, &m_vEye, &mWorldInverse);
	return D3DXVECTOR3(ptEyeLocal.x, ptEyeLocal.y, ptEyeLocal.z);
}

//----------------------------------------------------------------------------------

void Camera::SetCameraElevation(float fLocalHeight, float fHeightOverTerrain)
{
	float height = fLocalHeight * m_mWorld._22 + fHeightOverTerrain;
	m_vEye.y = height;
	m_vLookAt.y = height;
}

//----------------------------------------------------------------------------------

const D3DXMATRIX& Camera::GetViewMatrix() 
{ 
	if (m_bViewDirty) {
		// set the view matrix
		D3DXMatrixLookAtLH(&m_mView, &m_vEye, &m_vLookAt, &m_vUp);
		m_bViewDirty = false;
	}
	return m_mView;	
}

//----------------------------------------------------------------------------------

D3DXMATRIX Camera::GetWorldViewProjMatrix()
{
	if (m_bWorldViewProjDirty) {
		m_mWorldViewProj = m_mWorld * m_mView * m_mProj;
		m_bWorldViewProjDirty = false;
	}
	return m_mWorldViewProj;
}
