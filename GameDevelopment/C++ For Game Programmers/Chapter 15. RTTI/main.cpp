
#include "stdafx.h"

#include "GameEntity.h"
#include "GameActor.h"
#include "GameCamera.h"
#include "GamePlayer.h"
#include "testlib/GameTrigger.h"


void PrintDerivedFrom(const RTTI & rtti1, const RTTI & rtti2)
{
    std::string relation = rtti1.DerivesFrom(rtti2) ? " does " : " does NOT ";
    std::cout << "Class " << rtti1.GetClassName() << relation << 
                 "derive from " << rtti2.GetClassName() << "\n";
}


int main(int argc, char* argv[])
{
    std::cout << "Custom RTTI test:\n";

    GameEntity * pActor  = new GameActor;
    GameEntity * pCamera = new GameCamera;
    GameEntity * pEntity = new GameEntity;
    GameEntity * pPlayer = new GamePlayer;

    PrintDerivedFrom(pActor->GetRTTI(), GameEntity::rtti);
    PrintDerivedFrom(pCamera->GetRTTI(), GameActor::rtti);
    PrintDerivedFrom(pCamera->GetRTTI(), GameEntity::rtti);
    PrintDerivedFrom(pEntity->GetRTTI(), GameActor::rtti);
    PrintDerivedFrom(pPlayer->GetRTTI(), GameEntity::rtti);
    PrintDerivedFrom(pPlayer->GetRTTI(), GameActor::rtti);
    PrintDerivedFrom(pActor->GetRTTI(), GamePlayer::rtti);

    std::cout << "\nTesting across static libraries:\n";

    GameEntity * pTrigger = new GameTrigger;
    PrintDerivedFrom(pTrigger->GetRTTI(), GameEntity::rtti);
    PrintDerivedFrom(pTrigger->GetRTTI(), GameActor::rtti);    

	return 0;
}

