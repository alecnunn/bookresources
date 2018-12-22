
#ifndef ENTITYFACTORY_H_
#define ENTITYFACTORY_H_


#include <string>
class GameEntity;


namespace EntityFactory
{
    GameEntity * Create(const std::string & className);
};


#endif // ENTITYFACTORY_H_