//--------------------------------------------------------------------------------------
// File: Camera.h
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

#ifndef __CAMERA_H_
#define __CAMERA_H_

#define ZNEAR		0.1f
#define ZFAR		3000.0f

class Camera 
{
public:
	Camera();
	~Camera();

			D3DXMATRIX& GetWorldMatrix()				{ return m_mWorld;	}
			D3DXMATRIX& GetProjectionMatrix()			{ return m_mProj;	}
	const	D3DXMATRIX& GetWorldMatrix() const			{ return m_mWorld; } 
	const	D3DXMATRIX& GetViewMatrix();
	const	D3DXMATRIX& GetProjectionMatrix()	const	{ return m_mProj;	}
	
	D3DXMATRIX GetWorldViewProjMatrix();

	const	D3DXVECTOR3& GetEyePt()		const			{ return m_vEye; }
	const	D3DXVECTOR3& GetLookAtPt()	const			{ return m_vLookAt; }
	const	D3DXVECTOR3& GetUpVector()	const			{ return m_vUp; } 
	D3DXVECTOR3 GetDirectionVector()	const;

	D3DXVECTOR3 GetEyeLocal();
	void SetCameraElevation(float fLocalHeight, float fHeightOverTerrain);

	void MoveForward(float fSpeed=2);
	void MoveBackward(float fSpeed=2);
	void StrafeLeft(float fSpeed=2);
	void StrafeRight(float fSpeed=2);
	void MoveUp(float fSpeed=10);
	void MoveDown(float fSpeed=10);

	void SetViewByMouse(int xrel, int yrel);

protected:
	void RotateView(float angle, float x, float y, float z);

private:
	D3DXVECTOR3			m_vEye;
	D3DXVECTOR3			m_vLookAt;
	D3DXVECTOR3			m_vUp;

	D3DXMATRIX			m_mWorld;
	D3DXMATRIX			m_mView;
	D3DXMATRIX			m_mProj;
	D3DXMATRIX			m_mWorldViewProj;

	bool				m_bViewDirty;
	bool				m_bWorldViewProjDirty;
};

extern Camera g_Camera;

#endif // __CAMERA_H_
