#include "sgx/core/core.hpp"
#include "sgx/system/system.hpp"

CThreadManager *CThreadManager::ms_pSingleton = NULL;

CThreadManager *CThreadManager::Get()
{
	// NOTE: We do not construct a CThreadManager obj since this is an
	// abstract class.
	sgxAssert(ms_pSingleton && "Have you called Get() on a specific CThreadManager driver?");
    return ms_pSingleton;            
}

