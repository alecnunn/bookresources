#ifndef _WORLDOBJECT_H_
#define _WORLDOBJECT_H_

// This class represents an object in the game world 
// (with position and velocity). 3D sounds have corresponding
// WorldObjects, so they know how the sound is moving in the game. 
class WorldObject 
{
	public:
		WorldObject();
		
		void GetPosition(float& x, float& y, float& z) const;
		const D3DXVECTOR3& GetPosition() const;
		void SetPosition(float x, float y, float z);

		const D3DXVECTOR3& GetVelocity() const;
		void SetVelocity(float x, float y, float z);
		
	protected:
		D3DXVECTOR3 m_pos;
		D3DXVECTOR3 m_vel;
};

inline WorldObject::WorldObject()
:	m_pos(0.0f, 0.0f, 0.0f),
	m_vel(0.0f, 0.0f, 0.0f)
{
}

inline void WorldObject::GetPosition(float& x, float& y, float& z) const
{
	x = m_pos.x;
	y = m_pos.y;
	z = m_pos.z;
}

inline const D3DXVECTOR3& WorldObject::GetPosition() const
{
	return m_pos;
}

inline void WorldObject::SetPosition(float x, float y, float z)
{
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;
}

inline const D3DXVECTOR3& WorldObject::GetVelocity() const
{
	return m_vel;
}

inline void WorldObject::SetVelocity(float x, float y, float z)
{
	m_vel.x = x;
	m_vel.y = y;
	m_vel.z = z;
}

#endif
