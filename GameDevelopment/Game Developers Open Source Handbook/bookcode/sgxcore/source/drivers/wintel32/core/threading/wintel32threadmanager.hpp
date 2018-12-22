#ifndef SGXDRV_WINTEL32_CORE_THREADING_THREADMANAGER_HPP
#define SGXDRV_WINTEL32_CORE_THREADING_THREADMANAGER_HPP	1

class CWinTel32ThreadManager : public CThreadManager
{ 
public:
    static  CThreadManager    *Create();
    
	
	virtual CThread    *Create(cbThreadFunction cb, void *pUserData);
	virtual void		Destroy(CThread *pThread);
};

#endif /* SGXDRV_WINTEL32_CORE_THREADING_THREADMANAGER_HPP */

