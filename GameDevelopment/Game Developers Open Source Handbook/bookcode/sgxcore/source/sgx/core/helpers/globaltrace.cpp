#include "sgx/core/types/types.hpp"
#include "sgx/core/helpers/trace.hpp"

CGlobalTrace *CGlobalTrace::ms_pSingleton = NULL;

CGlobalTrace *
CGlobalTrace::Get()
{
    if (ms_pSingleton == NULL) {
        ms_pSingleton = new CGlobalTrace;
    }
    return ms_pSingleton;            
}

