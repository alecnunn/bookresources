#include "sgx/system/system.hpp"
#include "sgx/core/helpers/helpers.hpp"

//
// A generic implementation. Copy to your platform-specific
// version and fill in the functions as and when required.
//
void sgxSleep(const tREAL32 t) 
{ 
    sgxTraceLimit(1, sgxTrace(SID_ERROR("No sgxSleep function yet!")); );
}

void sgxYield(const tREAL32 t)
{ 
    sgxTraceLimit(1, sgxTrace(SID_ERROR("No sgxYield function yet!")); );
}

void sgxYieldWait(const tREAL32 t)
{
    sgxTraceLimit(1, sgxTrace(SID_ERROR("No sgxYieldWait function yet!")); );
}

tINT32 sgxSystemExecute(const ExecuteProperties &props)
{
	return -1;	// usually a system failure code, e.g. no such command
}

tBOOL sgxGetEnvironment(sgxString &value, const sgxString &env_var)
{
	value = "";
	return FALSE;
}
