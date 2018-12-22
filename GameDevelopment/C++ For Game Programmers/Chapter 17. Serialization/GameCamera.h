
#ifndef GAMECAMERA_H_
#define GAMECAMERA_H_


#include "GameEntity.h"


class GameCamera: public GameEntity
{
public: 
    GameCamera();
    GameCamera(const std::string & name);

    virtual bool Write(IStream & stream) const;
    virtual bool Read(IStream & stream);  
    virtual void Fixup();

    virtual const std::string GetClass() const;
    virtual void  Print(int indent) const;

    // This wouldn't normally be public, but it's more convenient for this example
    float m_FOV;
    float m_nearPlane;
    float m_farPlane;
    GameEntity * m_pTarget;

private:
    void SetDefaults();

};



#endif