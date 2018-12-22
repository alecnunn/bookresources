

#include "GameCamera.h"
#include "Stream.h"
#include "AddressTranslator.h"
#include <iostream>


GameCamera::GameCamera() 
{
    SetDefaults();
}

GameCamera::GameCamera(const std::string & name) : GameEntity(name)
{
    SetDefaults();
}


void GameCamera::SetDefaults()
{
    m_FOV = 90;
    m_nearPlane = 5;
    m_farPlane  = 100;
    m_pTarget   = NULL;
}


bool GameCamera::Write(IStream & stream) const
{
    bool bSuccess = GameEntity::Write(stream);
    bSuccess &= WriteFloat(stream, m_FOV);
    bSuccess &= WriteFloat(stream, m_nearPlane);
    bSuccess &= WriteFloat(stream, m_farPlane);
    bSuccess &= WriteInt(stream, (int)(m_pTarget));
    return bSuccess;
}


bool GameCamera::Read(IStream & stream)
{
    bool bSuccess = GameEntity::Read(stream);
    m_FOV = ReadFloat(stream);
    m_nearPlane = ReadFloat(stream);
    m_farPlane  = ReadFloat(stream);
    m_pTarget = (GameEntity *)ReadInt(stream);
    return bSuccess;
}


void GameCamera::Fixup()
{
    GameEntity::Fixup();
    m_pTarget = (GameEntity *)AddressTranslator::TranslateAddress(m_pTarget);
}


const std::string GameCamera::GetClass() const
{
    return std::string("GameCamera");
}


void GameCamera::Print(int indent) const
{
    GameEntity::Print(indent);
    for (int col=0; col<indent; ++col)
        std::cout << "  ";

    std::cout << "    FOV: " << m_FOV << " Near: " << m_nearPlane << 
                 " Far: " << m_farPlane;
    std::cout << " Target: ";
    if (m_pTarget != NULL)
        std::cout << m_pTarget->GetName();
    else
        std::cout << "NULL";

    std::cout << std::endl;
}

