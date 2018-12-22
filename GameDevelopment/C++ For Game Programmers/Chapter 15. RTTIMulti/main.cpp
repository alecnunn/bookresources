
#include "stdafx.h"

#include "LandAI.h"
#include "FlyingAI.h"
#include "HybridAI.h"


void PrintDerivedFrom(const RTTI & rtti1, const RTTI & rtti2)
{
    std::string relation = rtti1.DerivesFrom(rtti2) ? " does " : " does NOT ";
    std::cout << "Class " << rtti1.GetClassName() << relation << 
                 "derive from " << rtti2.GetClassName() << "\n";
}


int main(int argc, char* argv[])
{
    std::cout << "Custom RTTI with multiple inheritance test:\n";

    LandAI * pAI1 = new HybridAI;
    PrintDerivedFrom(pAI1->GetRTTI(), HybridAI::rtti);
    PrintDerivedFrom(pAI1->GetRTTI(), LandAI::rtti);
    PrintDerivedFrom(pAI1->GetRTTI(), FlyingAI::rtti);

    LandAI * pAI2 = new LandAI;
    PrintDerivedFrom(pAI2->GetRTTI(), HybridAI::rtti);
    PrintDerivedFrom(pAI2->GetRTTI(), LandAI::rtti);
    PrintDerivedFrom(pAI2->GetRTTI(), FlyingAI::rtti);

    FlyingAI * pAI3 = new FlyingAI;
    PrintDerivedFrom(pAI3->GetRTTI(), HybridAI::rtti);
    PrintDerivedFrom(pAI3->GetRTTI(), LandAI::rtti);
    PrintDerivedFrom(pAI3->GetRTTI(), FlyingAI::rtti);

    return 0;
}

