// input
// (c) Arcot Preetham, 2002. 

#ifndef _CAMERA_H
#define _CAMERA_H

#include "Defines.h"
#include <stdio.h>

#include <D3dutil.h>

//class Input;

// Possible Camera updates
typedef enum {
	eCameraNone,
	eCameraMoveForward,
	eCameraMoveBackward,
	eCameraPitchUp,
	eCameraPitchDown,
	eCameraMoveUp,
	eCameraMoveDown,
	eCameraYawLeft,
	eCameraYawRight,
	eCameraRollLeft,
	eCameraRollRight,
	eCameraMoveLeft,
	eCameraMoveRight,
} CameraMovements;


class Camera
{
private:
	float m_fFov;			// field of view PI/2
	float m_fAspect;		// aspect
	float m_fNear;			// near
	float m_fFar;			// far

	D3DXVECTOR3 m_eye;
	D3DXVECTOR3 m_look;
	D3DXVECTOR3 m_up;

	float m_speed;
	float m_rotSpeed;

	D3DXMATRIX		m_ViewMatrix;
	D3DXMATRIX		m_ProjMatrix;
	D3DXMATRIX		m_ViewProjMatrix;

//	Input *m_pInput;


public:
	Camera();
	~Camera();
	void Update(const CameraMovements &);
	void Control(const CameraMovements &);
	void Interpolate(Camera *one, Camera *two, float f);

//	void SetInput(Input* input) { m_pInput = input; }
	void Finalize();

	void Normalize();
	inline void GetViewProjectionMatrix(D3DXMATRIX &x) const { x = m_ViewProjMatrix; }
	inline void GetProjectionMatrix(D3DXMATRIX &x) const { x = m_ProjMatrix; }
	inline void GetViewMatrix(D3DXMATRIX &x) const { x = m_ViewMatrix; }

	inline float GetSpeed() const {return m_speed;};
	inline void SetSpeed(float speed) {m_speed = speed;};
	inline float GetRotationSpeed()  const {return m_rotSpeed;}
	inline void SetRotationSpeed(float rspeed) {m_rotSpeed = rspeed;};

	inline const D3DXVECTOR3 GetEye() const {return m_eye;};
	inline void SetEye(D3DXVECTOR3 &eye) {m_eye = eye;};
	inline const D3DXVECTOR3 GetLookDirection() const {return m_look;};
	inline void SetLookDirection(D3DXVECTOR3 &look) {D3DXVec3Normalize(&m_look, &look);};
	inline const D3DXVECTOR3 GetUpDirection() const {return m_up;}
	inline void SetUpDirection(D3DXVECTOR3 &up)	{D3DXVec3Normalize(&m_up, &up);};

	inline void Faster() {m_speed *= 1.1f; m_rotSpeed *= 1.1f;};
	inline void Slower() {m_speed /= 1.1f; m_rotSpeed /= 1.1f;};


	Camera operator + ( const Camera&) const;
    Camera operator * ( float ) const;
    friend Camera operator * ( float , const Camera& );


public:
	void MoveForward();
	void MoveBackward();
	void Pitch(float theta);
	inline void PitchUp() {Pitch(GetRotationSpeed());}
	inline void PitchDown() {Pitch(-GetRotationSpeed());}
	void MoveUp();
	void MoveDown();
	void Yaw(float theta);
	inline void YawLeft() {Yaw(-GetRotationSpeed());};
	inline void YawRight() {Yaw(GetRotationSpeed());};
	void MoveRight(int sign = 1);
	inline void MoveLeft() {MoveRight(-1);};
	void Roll(float theta);
	inline void RollLeft() {Roll(-GetRotationSpeed());};
	inline void RollRight() {Roll(GetRotationSpeed());};

	void Dump2(FILE *f);
	void Read2(FILE *f);

private:
	void CalculateProjectionMatrix();
	void CalculateViewMatrix();

private:


};


#endif
