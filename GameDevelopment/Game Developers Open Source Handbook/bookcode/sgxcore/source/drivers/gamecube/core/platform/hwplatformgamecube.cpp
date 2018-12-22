#include "sgx/core/core.hpp"
#include "drivers/gamecube/core/platform/hwplatformgamecube.hpp"

CHardwarePlatform *CHardwarePlatformGameCube::ms_pSingleton = NULL;

CHardwarePlatform *CHardwarePlatformGameCube::Get()
{
    if (ms_pSingleton == NULL) {
        ms_pSingleton = SGX_STATIC_CAST(CHardwarePlatform *, new CHardwarePlatformGameCube);
    }
    return ms_pSingleton;            
}

const char *CHardwarePlatformGameCube::GetPlatform()
{
    return "Nintendo GameCube"; 
}

const char *CHardwarePlatformGameCube::GetDocsURL()
{
    return "//docs_drive/sdks/gc"; 
}

const char *CHardwarePlatformGameCube::GetDevelopmentURL() 
{
    return "http://www.warioworld.com"; 
}

tREAL32 CHardwarePlatformGameCube::GetDiscAccessSpeed() 
{
    return 0.128f; // 128 ms
}

tREAL32 CHardwarePlatformGameCube::GetDiscReadSpeed()
{
    return 20*SGX_MEGABYTES; // (actual range 16-25 Mbps)
}

CHardwarePlatform::tEndian CHardwarePlatformGameCube::GetEndian()
{
    return eBigEndian;
}

tUINT32 CHardwarePlatformGameCube::GetNumCPUs() 
{
    return 1;
}

tUINT32 CHardwarePlatformGameCube::GetCPUSpeed(tUINT32 type)
{
    sgxAssert(type==0); 
    return 485; // in MHz
}


tUINT32 CHardwarePlatformGameCube::GetNumMemoryBanks()
{
    return 2;
}

tUINT32 CHardwarePlatformGameCube::GetMemorySize(tUINT32 type)
{
	if (type == eMainMem) {
		return 24 * SGX_MEGABYTES;
	} else if (type == eARAM) {
		return 16 * SGX_MEGABYTES;
	} else {
		sgxError("GC doesn't have that many memory banks");
        return 0;
	}
}

