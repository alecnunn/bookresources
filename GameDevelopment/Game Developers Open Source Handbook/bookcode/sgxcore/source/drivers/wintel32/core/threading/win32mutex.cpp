#include "drivers/wintel32/wintel32core.hpp"
#include "sgx/core/core.hpp"

class CMutexWin32 : public CMutex
{
public:
	CMutexWin32() 
	{
		
	}


	void Capture(const char *pFile, size_t line)
	{
	}


	tBOOL IsLocked()
	{
	}


	void Release()
	{
	}


	void Destroy()
	{
	}


}

CMutex *CMutex::Create() 
{
	return new CMutexWin32();
}

