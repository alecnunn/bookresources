#ifndef SGX_CORE_THREADING_THREAD_HPP
#define SGX_CORE_THREADING_THREAD_HPP	1

class CThread;
typedef size_t (*cbThreadFunction)(CThread *);

class CThread
{ 
public:
	void *pUserData;
	
	// Do not change the values below, unless you know what you're doing!
	cbThreadFunction	cb;
	size_t				SystemID;

};

#endif /* SGX_CORE_THREADING_THREAD_HPP */

