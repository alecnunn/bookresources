#include "sgx/core/core.hpp"
#include "sgx/system/system.hpp"
#include "drivers/null/nullcore.hpp"

void sgxSleep(const tREAL32 t)
{
	// NULLSTUB : Wait for t seconds
}

void sgxYield(const tREAL32 t)
{
	// NULLSTUB : Allow the CPU to breath
}

void sgxYieldWait(const tREAL32 t)
{
	// NULLSTUB : Allow the CPU to breath for t seconds
}

tINT32 sgxSystemExecute(const ExecuteProperties &props)
{
	// NULLSTUB : Call an external process
	return -1;	// usually a system failure code, e.g. no such command
}

tBOOL sgxGetEnvironment(sgxString &value, const sgxString &env_var)
{
	// NULLSTUB : Handle env vars
	value = "";
	return FALSE;
}
