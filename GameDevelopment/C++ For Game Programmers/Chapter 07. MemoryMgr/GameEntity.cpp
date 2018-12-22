
#include "GameEntity.h"


DEFINE_HEAP(GameEntity, "Game entities");


DEFINE_HIERARCHICALHEAP(GameCamera, "Cameras", "Game entities");

DEFINE_HIERARCHICALHEAP(GameActor, "Actors", "Game entities");
