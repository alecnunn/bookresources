#include "drivers/wintel32/wintel32core.hpp"
#include "sgx/core/core.hpp"
#include "drivers/wintel32/core/threading/threading.hpp"

CThreadManager *CWinTel32ThreadManager::Create() 
{
    if (ms_pSingleton == NULL) {
        ms_pSingleton = SGX_STATIC_CAST(CThreadManager *, new CWinTel32ThreadManager());
    }
	return ms_pSingleton;
}

unsigned long __stdcall startThread(LPVOID usr)
{
	CThread *pThread = SGX_STATIC_CAST(CThread *, usr);
	size_t returnValue = pThread->cb(pThread);
	return 0L;
}

CThread *CWinTel32ThreadManager::Create(cbThreadFunction cb, void *pUserData) 
{
	CThread *pThread = new CThread();
	pThread->pUserData = pUserData;
	pThread->cb = cb;
	pThread->SystemID = (size_t)CreateThread(NULL, 10000, startThread, pThread, 0L, NULL);
	return pThread;
}

void CWinTel32ThreadManager::Destroy(CThread *pThread) 
{
	TerminateThread(SGX_STATIC_CAST(HANDLE, pThread->SystemID), 0);
}

// Wintel can also support GetCurrentThread
