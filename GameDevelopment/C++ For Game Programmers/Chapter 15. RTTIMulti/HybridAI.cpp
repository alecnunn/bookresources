

#include "stdafx.h"
#include "HybridAI.h"

const RTTI HybridAI::rtti("HybridAI", 2, &LandAI::rtti,
                                         &FlyingAI::rtti);
