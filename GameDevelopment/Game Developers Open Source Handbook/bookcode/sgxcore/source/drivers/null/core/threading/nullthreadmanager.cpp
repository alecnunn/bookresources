#include "drivers/null/nullcore.hpp"
#include "sgx/core/core.hpp"
#include "drivers/null/core/threading/threading.hpp"

CThreadManager *CNullThreadManager::Create() 
{
    if (ms_pSingleton == NULL) {
        ms_pSingleton = SGX_STATIC_CAST(CThreadManager *, new CNullThreadManager());
    }
	return ms_pSingleton;
}

CThread *CNullThreadManager::Create(cbThreadFunction cb, void *pUserData) 
{
	// NULLSTUB : Create a new thread, starting with the function
	// specified in cb
	return NULL;
}

void CNullThreadManager::Destroy(CThread *pThread) 
{
}
