

#include "EntityFactory.h"
#include "GameEntity.h"
#include "GameCamera.h"


GameEntity * EntityFactory::Create(const std::string & className)
{
    if (className == "GameEntity")
        return new GameEntity;
    if (className == "GameCamera")
        return new GameCamera;

    return NULL;
}
