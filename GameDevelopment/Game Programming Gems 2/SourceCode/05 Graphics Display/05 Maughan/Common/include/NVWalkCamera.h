/* Copyright (C) Chris Maughan, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Chris Maughan, 2001"
 */
#ifndef _NVWALKCAMERA_H_
#define _NVWALKCAMERA_H_

// A simple camera class, based on the MouseUI class, but with controlling a camera in a world in mind, not
// a target object.  The camera does not roll to the left or right, but can be used to view above and to the
// left/right.  Quaternions are used for smoother rotations, and to avoid Gimble Lock problems.
// Create it, and pass the starting altitude and azimuth vectors.  Optional boundary extents are allowed.
#include <d3dx8.h>

namespace nv_objects
{

static const float WALKCAM_TRANSLATIONAL_SCALE = 0.05f;
static const float WALKCAM_ROTATIONAL_SCALE = 0.01f;
class NVWalkCamera
{
	
public:
	NVWalkCamera(const D3DXMATRIX& matProjection = D3DXMATRIX(), const D3DXVECTOR3& Position = D3DXVECTOR3(0.0f, 0.0f, 0.0f), float fAzimuth = 0.0f, float fAltitude = 0.0f,
					const D3DXVECTOR3& MinBounds = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX),
					const D3DXVECTOR3& MaxBounds = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX))
		: m_PositionStart(Position),
		m_fAltitudeStart(fAltitude),
		m_fAzimuthStart(fAzimuth),
		m_MinBounds(MinBounds),
		m_MaxBounds(MaxBounds),
		m_bLButtonDown(false),
		m_fTranslationalSensitivityFactor(1.0f * WALKCAM_TRANSLATIONAL_SCALE),
		m_fRotationalSensitivityFactor(1.0f * WALKCAM_ROTATIONAL_SCALE)
	{
		m_matProjection = matProjection;
		Reset();
	}
	
	// Sets
	void SetTranslationalSensitivityFactor(float fTranslate)
	{
		m_fTranslationalSensitivityFactor = fTranslate * WALKCAM_TRANSLATIONAL_SCALE;
	}

	void SetRotationalSensitivityFactor(float fRotate)
	{
		m_fRotationalSensitivityFactor = fRotate * WALKCAM_ROTATIONAL_SCALE;
	}

	void SetPosition(const D3DXVECTOR3& Position)
	{
		m_PositionStart = Position;
		Reset();
	}

	void SetAltitude(float fAltitude)
	{
		m_fAltitudeStart = fAltitude;
		Reset();
	}
	void SetAzimuth(float fAzimuth)
	{
		m_fAzimuthStart = fAzimuth;
		Reset();
	}

	void SetBounds(const D3DXVECTOR3& MinBounds, const D3DXVECTOR3& MaxBounds)
	{
		m_MinBounds = MinBounds;
		m_MaxBounds = MaxBounds;
		Reset();
	}

	void SetProjection(const D3DXMATRIX& Projection)
	{
		m_matProjection = Projection;
		Reset();
	}

	// Gets
	const D3DXVECTOR3& GetCameraLookDirection() const
	{
		return m_LookDirection;
	}

	const D3DXVECTOR3& GetCameraPosition() const
	{
		return m_Position;
	}

	const D3DXMATRIX& GetCameraMatrix() const
	{
		return m_matView;
	}	

	const D3DXMATRIX& GetProjectionMatrix() const
	{
		return m_matProjection;
	}
		
	void Reset()
	{
		m_Position = m_PositionStart;
		m_fAzimuth = m_fAzimuthStart;
		m_fAltitude = m_fAltitudeStart;
		m_bLButtonDown = false;
		
		// Convert the rotation into a Quaternion
		D3DXQUATERNION QRotate;

		D3DXQuaternionRotationYawPitchRoll(&QRotate, m_fAzimuth, m_fAltitude, 0.0f);
		
		D3DXQuaternionIdentity(&m_QuatStart);
		m_Quat = m_QuatStart * QRotate;

		UpdatePositionState();
		
	}

	int CheckAABBVisible(D3DXVECTOR3& vecMin, D3DXVECTOR3& vecMax)
	{
		D3DXVECTOR3	vecMinPt, vecMaxPt;
		bool bIntersecting = false;
	
		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (m_WorldPlanes[i][j] >= 0.0f)
				{
					vecMinPt[j] = vecMin[j];
					vecMaxPt[j] = vecMax[j];
				}
				else
				{
					vecMinPt[j] = vecMax[j];
					vecMaxPt[j] = vecMin[j];
				}
			}
			float fMinDistance = D3DXPlaneDotCoord(&m_WorldPlanes[i], &vecMinPt);
			if (fMinDistance > 0.0f)
				return 0;

			float fMaxDistance = D3DXPlaneDotCoord(&m_WorldPlanes[i], &vecMaxPt);
			if (fMaxDistance >= 0.0f)
				bIntersecting = true;
		}
		// At this stage it's visible, but may be intersecting.  Return true in either case.
		return bIntersecting ? 1 : 2;
	}

	
	virtual void OnLButtonDown(HWND hWnd, const unsigned int& x, const unsigned int& y )
	{
		POINT pt;
		pt.x = x;
		pt.y = y;
		ClientToScreen(hWnd, &pt);
		m_MouseStart = D3DXVECTOR2(pt.x, pt.y);
		m_bLButtonDown = true;
		SetCapture(hWnd);
	}
	 
	virtual void OnLButtonUp(HWND hWnd, const unsigned int& x, const unsigned int& y )
	{
		m_bLButtonDown = false;
		ReleaseCapture();
	}
	
	virtual void OnMouseMove(HWND hWnd, const unsigned int& x, const unsigned int& y )
	{
		if (m_bLButtonDown)
		{ 
			bool bShift = ( ( GetAsyncKeyState( VK_LSHIFT )   < 0 ) || ( GetAsyncKeyState( VK_SHIFT    ) < 0 ) || ( GetAsyncKeyState( VK_RSHIFT )  < 0 ) );
			bool bAlt =   ( ( GetAsyncKeyState( VK_LMENU )    < 0 ) || ( GetAsyncKeyState( VK_RMENU    ) < 0 ) || ( GetAsyncKeyState( VK_MENU )    < 0 ) );
			bool bCtl =   ( ( GetAsyncKeyState( VK_LCONTROL ) < 0 ) || ( GetAsyncKeyState( VK_RCONTROL ) < 0 ) || ( GetAsyncKeyState( VK_CONTROL ) < 0 ) );

			POINT pt;
			pt.x = x;
			pt.y = y;
			ClientToScreen(hWnd, &pt);
	
			D3DXVECTOR2 Delta = m_MouseStart - D3DXVECTOR2(pt.x, pt.y);
				
			if ( !( ( Delta.x == 0.0f ) && ( Delta.y == 0.0f ) ) )
			{
				if (!bShift && !bCtl)
				{
					m_fAltitude += -Delta.y * m_fRotationalSensitivityFactor;
					m_fAzimuth += Delta.x * m_fRotationalSensitivityFactor;
					
					D3DXQUATERNION QRotateAzimuth;
					D3DXQUATERNION QRotateAltitude;
					D3DXQuaternionRotationYawPitchRoll(&QRotateAzimuth, m_fAzimuth, 0.0f, 0.0f);
					D3DXQuaternionRotationYawPitchRoll(&QRotateAltitude, 0.0f, m_fAltitude, 0.0f);
					
					m_Quat = m_QuatStart * QRotateAzimuth * QRotateAltitude;
					D3DXQuaternionNormalize(&m_Quat, &m_Quat);
				}
				
				D3DXMATRIX matQuat;
				D3DXMatrixRotationQuaternion(&matQuat, &m_Quat);

				D3DXVECTOR3 rightVector = D3DXVECTOR3(matQuat._11, matQuat._21, matQuat._31);
				D3DXVECTOR3 forwardVector = D3DXVECTOR3(matQuat._13, matQuat._23, matQuat._33);
				D3DXVECTOR3 upVector = D3DXVECTOR3(matQuat._12, matQuat._22, matQuat._32);

				if ( bShift )
				{
					m_Position += (-rightVector * m_fTranslationalSensitivityFactor * Delta.x) +
						(upVector * m_fTranslationalSensitivityFactor * Delta.y);
				}	
				else if ( bCtl )
				{
					m_Position += forwardVector * m_fTranslationalSensitivityFactor * Delta.y;
				}
			
				UpdatePositionState();
			}
			m_MouseStart = D3DXVECTOR2(pt.x, pt.y);
		}
	}

	virtual void CheckKeyboard(HWND hWnd)
	{
		// Don't capture keys if we aren't the focus
		if (GetForegroundWindow() != hWnd)
			return;

		D3DXVECTOR3 TravelVector = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		if (GetAsyncKeyState( VK_UP ) < 0)
		{
			TravelVector.z += 2.0f;
		}
		else if (GetAsyncKeyState( VK_DOWN ) < 0)
		{
			TravelVector.z -= 2.0f;

		}
		
		if (GetAsyncKeyState( VK_LEFT) < 0)
		{
			TravelVector.x -= 2.0f;
		}
		else if (GetAsyncKeyState( VK_RIGHT ) < 0)
		{
			TravelVector.x += 2.0f;
		}

		D3DXMATRIX matQuat;
		D3DXMatrixRotationQuaternion(&matQuat, &m_Quat);

		D3DXVECTOR3 rightVector = D3DXVECTOR3(matQuat._11, matQuat._21, matQuat._31);
		D3DXVECTOR3 forwardVector = D3DXVECTOR3(matQuat._13, matQuat._23, matQuat._33);
		m_Position += forwardVector * m_fTranslationalSensitivityFactor * TravelVector.z;
		m_Position += rightVector * m_fTranslationalSensitivityFactor * TravelVector.x;
		
		UpdatePositionState();
	}

private:
	// Collect the world clip planes from the viewProjection matrix.
	// We use these later to clip objects to the world
	void ExtractWorldPlanes()
	{
		D3DXVECTOR4	tmpVec;
		D3DXMATRIX matViewProjection;
		D3DXMatrixMultiply(&matViewProjection, &m_matView, &m_matProjection);
		D3DXMatrixTranspose(&matViewProjection, &matViewProjection);
		
		D3DXVECTOR4 Columns[4];
		Columns[0] = D3DXVECTOR4(matViewProjection(0, 0), matViewProjection(0, 1), matViewProjection(0, 2), matViewProjection(0, 3));
		Columns[1] = D3DXVECTOR4(matViewProjection(1, 0), matViewProjection(1, 1), matViewProjection(1, 2), matViewProjection(1, 3));
		Columns[2] = D3DXVECTOR4(matViewProjection(2, 0), matViewProjection(2, 1), matViewProjection(2, 2), matViewProjection(2, 3));
		Columns[3] = D3DXVECTOR4(matViewProjection(3, 0), matViewProjection(3, 1), matViewProjection(3, 2), matViewProjection(3, 3));
		
		tmpVec = Columns[3] - Columns[0];
		*((D3DXVECTOR4*)&m_WorldPlanes[0]) = -tmpVec;

		tmpVec = Columns[3] + Columns[0];
		*((D3DXVECTOR4*)&m_WorldPlanes[1]) = -tmpVec;

		tmpVec = Columns[3] - Columns[1];
		*((D3DXVECTOR4*)&m_WorldPlanes[2]) = -tmpVec;

		tmpVec = Columns[3] + Columns[1];
		*((D3DXVECTOR4*)&m_WorldPlanes[3]) = -tmpVec;

		tmpVec = Columns[3] - Columns[2];
		*((D3DXVECTOR4*)&m_WorldPlanes[4]) = -tmpVec;

		tmpVec = Columns[3] + Columns[2];
		*((D3DXVECTOR4*)&m_WorldPlanes[5]) = -tmpVec;
	}

	void UpdatePositionState()
	{
		if (m_Position.x < m_MinBounds.x)
			m_Position.x = m_MinBounds.x;
		if (m_Position.y < m_MinBounds.y)
			m_Position.y = m_MinBounds.y;
		if (m_Position.z < m_MinBounds.z)
			m_Position.z = m_MinBounds.z;

		if (m_Position.x > m_MaxBounds.x)
			m_Position.x = m_MaxBounds.x;
		if (m_Position.y > m_MaxBounds.y)
			m_Position.y = m_MaxBounds.y;
		if (m_Position.z > m_MaxBounds.z)
			m_Position.z = m_MaxBounds.z;

		// Build a camera matrix from the quaternion and the from translation
		D3DXMATRIX matTranslate;
		D3DXMATRIX matRotation;
		D3DXMatrixRotationQuaternion(&matRotation, &m_Quat);
		D3DXMatrixTranslation(&matTranslate, -m_Position.x, -m_Position.y, -m_Position.z);
		m_matView = matTranslate * matRotation;

		m_LookDirection = D3DXVECTOR3(matRotation._13, matRotation._23, matRotation._33);
		D3DXVec3Normalize(&m_LookDirection, &m_LookDirection);

		// Build the world planes.
		ExtractWorldPlanes();
	}

private:
    bool m_bLButtonDown;
	bool m_bHaveProjection;
	
	D3DXVECTOR3 m_Position;
	D3DXVECTOR3 m_PositionStart;
	D3DXVECTOR3 m_LookDirection;

	D3DXVECTOR3 m_MinBounds;
	D3DXVECTOR3 m_MaxBounds;

	D3DXVECTOR2 m_MouseStart;

	D3DXQUATERNION m_QuatStart;
	D3DXQUATERNION m_Quat;

	D3DXMATRIX m_matProjection;
	D3DXMATRIX m_matView;
	D3DXPLANE m_WorldPlanes[6];

	float m_fAltitude;
	float m_fAzimuth;

	float m_fAltitudeStart;
	float m_fAzimuthStart;

	float m_fRotationalSensitivityFactor;
	float m_fTranslationalSensitivityFactor;

};

}; // namespace nv_objects


#endif  _NVWALKCAMERA_H_


