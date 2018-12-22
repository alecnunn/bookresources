#include "sgx/core/core.hpp"
#include "sgx/system/system.hpp"
#include "drivers/wintel32/wintel32core.hpp"

void sgxSleep(const tREAL32 t)
{
    // WinTel version of 'Sleep' is in milliseonds
    Sleep(DWORD(t * 1000.0f));
}

void sgxYield(const tREAL32 t)
{
    Yield();
}

void sgxYieldWait(const tREAL32 t)
{
    Yield();
    sgxSleep(t);
}

tINT32 sgxSystemExecute(const ExecuteProperties &props)
{
	//WinExec(...) 
	return -1;	// usually a system failure code, e.g. no such command
}

tBOOL sgxGetEnvironment(sgxString &value, const sgxString &env_var)
{
	value = "";
	return FALSE;
}
