/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
#ifndef	INC_GAMEOBJECTS_H
#define	INC_GAMEOBJECTS_H

// Introduce the MicroThread namespace so we can forward declare MicroThreadInfo
namespace MicroThread
{
	class MicroThreadInfo;
};

class CSBitmap;


class GameObject
{
public:
	GameObject();
	void PostConstructorInit();
	enum EClickFlags
	{
		k_leftDown,
		k_rightDown,
		k_mouseMove,
	};
	virtual bool GetDelete() {return false;}
	virtual void MicroThreadProcess();
	void RunThread();
	virtual void ProcessFrame() {}
	virtual void RenderSelf(CSBitmap* pBitmap);

	// When an object is destroyed with DestroyObject() it stays around
	// until the end of the next frame (ObjectFrameEnd() has been
	// called twice), to make sure everybody with
	// pointers to it can efficiently tell that it is dead and zero
	// their pointers.
	bool IsDead() const {return m_deadFrames != 0;}

	static void DestroyObject(GameObject* pObject);

	// In a real GameObject class these shouldn't be public, but
	// it simplifies things for demonstration purposes and
	// simpler code sharing.
	double m_x, m_y;
protected:
	// The derived class is responsible for filling these in.
	// However m_pSprite will be zeroed by GameObject.
	const CSBitmap* m_pSprite;
	MicroThread::MicroThreadInfo* m_thread;

private:
	~GameObject() {}	// Only DestroyObject can destroy objects.
	int	m_deadFrames;
	friend void ObjectFrameEnd();
	GameObject& operator=(const GameObject&);	// Not implemented.
	GameObject(const GameObject&);				// Not implemented.
};

#endif	// INC_GAMEOBJECTS_H
