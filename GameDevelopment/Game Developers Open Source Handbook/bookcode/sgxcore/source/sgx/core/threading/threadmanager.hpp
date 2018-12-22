#ifndef SGX_CORE_THREADING_THREADMANAGER_HPP
#define SGX_CORE_THREADING_THREADMANAGER_HPP	1

class CThreadManager
{ 
public:
    static  CThreadManager    *Get();
    
	
	virtual CThread    *Create(cbThreadFunction cb, void *pUserData) = 0;
	virtual void		Destroy(CThread *pThread) = 0;

protected:
	static CThreadManager *ms_pSingleton;
};

#endif /* SGX_CORE_THREADING_THREADMANAGER_HPP */

