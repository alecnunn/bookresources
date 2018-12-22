#ifndef SGXDRV_NULL_CORE_THREADING_THREADMANAGER_HPP
#define SGXDRV_NULL_CORE_THREADING_THREADMANAGER_HPP	1

class CNullThreadManager : public CThreadManager
{ 
public:
    static  CThreadManager    *Create();
    
	
	virtual CThread    *Create(cbThreadFunction cb, void *pUserData);
	virtual void		Destroy(CThread *pThread);
};

#endif /* SGXDRV_NULL_CORE_THREADING_THREADMANAGER_HPP */

