
#include "Factory.h"
#include "GameEntities.h"
#include <iostream>
#include <assert.h>



int main()
{
    // Register the maker objects
    Factory<GameEntity> factory;
    factory.Register("GameEntity" ,new Creator<GameEntity, GameEntity>);    
    factory.Register("GameCamera" ,new Creator<GameCamera, GameEntity>);

    // Create a few objects and verify they are of the correct type
    GameEntity * pEntity1 = factory.Create("GameEntity");
    assert(!stricmp(pEntity1->GetClassName(), "GameEntity"));

    GameEntity * pEntity2 = factory.Create("GameCamera");
    assert(!stricmp(pEntity2->GetClassName(), "GameCamera"));

    return 0;
}

