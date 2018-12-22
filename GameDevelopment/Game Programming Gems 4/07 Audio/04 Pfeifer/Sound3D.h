#ifndef _SOUND3D_H_
#define _SOUND3D_H_

#include "sound.h"

class WorldObject;

// Sound3D is used for any positional audio
class Sound3D : public Sound
{
    public:
        Sound3D(const char* szWaveFileName);
        virtual ~Sound3D();

		void SetWorldObject(WorldObject* pObj) { m_pObj = pObj; }

		WorldObject* GetWorldObject() { return m_pObj; }

    protected:
		WorldObject* m_pObj;	// the object that tells us where the sound is
};


#endif