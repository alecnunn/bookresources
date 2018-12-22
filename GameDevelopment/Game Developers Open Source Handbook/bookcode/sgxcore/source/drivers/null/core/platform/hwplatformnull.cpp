#include "sgx/core/core.hpp"
#include "drivers/null/nullcore.hpp"
#include "drivers/null/core/platform/hwplatformnull.hpp"

// NULLSTUB : This whole file needs specific re-writes

CHardwarePlatform *CHardwarePlatformNull::ms_pSingleton = NULL;

CHardwarePlatform *CHardwarePlatformNull::Get()
{
    if (ms_pSingleton == NULL) {
        ms_pSingleton = new CHardwarePlatformNull;
    }
    return ms_pSingleton;            
}

const char *CHardwarePlatformNull::GetPlatform()
{
    return "Null driver"; 
}

const char *CHardwarePlatformNull::GetDocsURL()
{
    return "http://www.bluedust.com/sgx"; 
}

const char *CHardwarePlatformNull::GetDevelopmentURL() 
{
    return "http://www.bluedust.com/sgx"; 
}

tREAL32 CHardwarePlatformNull::GetDiscAccessSpeed() 
{
    return 0.009f; // 9 milliseconds
}

tREAL32 CHardwarePlatformNull::GetDiscReadSpeed()
{
    return 30*SGX_MEGABYTES;
}

CHardwarePlatform::tEndian CHardwarePlatformNull::GetEndian()
{
    return eLittleEndian;
}

tUINT32 CHardwarePlatformNull::GetNumCPUs() 
{
	return 1;
}

tUINT32 CHardwarePlatformNull::GetCPUSpeed(tUINT32 type)
{
    return 1000;
}

tUINT32 CHardwarePlatformNull::GetNumMemoryBanks()
{
    return 1;
}

tUINT32 CHardwarePlatformNull::GetMemorySize(tUINT32 type)
{
    return 16*SGX_MEGABYTES;
}

