#ifndef _CAMERA_H
#define _CAMERA_H

// A class representing the player's view camera, 
// with the position orientation information exposed
// for the AudioManager
class Camera
{
	public:
		Camera();
		virtual ~Camera();

		virtual D3DXVECTOR3& GetPosition();
		virtual D3DXVECTOR3& GetFront();
		virtual D3DXVECTOR3& GetUp();

	protected:
		D3DXVECTOR3 m_pos, m_up, m_front;
};

inline Camera::Camera()
:	m_pos(0.0f, 0.0f, 0.0f),
	m_front(0.0f, 0.0f, 1.0f),
	m_up(0.0f, 1.0f, 0.0f)
{
}

inline Camera::~Camera()
{
}

inline D3DXVECTOR3& Camera::GetPosition()
{
	return m_pos;
}

inline D3DXVECTOR3& Camera::GetFront()
{
	return m_front;
}

inline D3DXVECTOR3& Camera::GetUp()
{
	return m_up;
}

#endif