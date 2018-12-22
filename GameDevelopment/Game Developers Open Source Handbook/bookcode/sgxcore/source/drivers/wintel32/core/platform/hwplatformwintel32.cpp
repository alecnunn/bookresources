#include "sgx/core/core.hpp"
#include "drivers/wintel32/wintel32core.hpp"
#include "drivers/wintel32/core/platform/hwplatformwintel32.hpp"

CHardwarePlatform *CHardwarePlatformWinTel32::ms_pSingleton = NULL;

CHardwarePlatform *CHardwarePlatformWinTel32::Get()
{
    if (ms_pSingleton == NULL) {
        ms_pSingleton = new CHardwarePlatformWinTel32;
    }
    return ms_pSingleton;            
}

const char *CHardwarePlatformWinTel32::GetPlatform()
{
    return "WinTel32 PC"; 
}

const char *CHardwarePlatformWinTel32::GetDocsURL()
{
    return "//docs_drive/sdks/pc"; 
}

const char *CHardwarePlatformWinTel32::GetDevelopmentURL() 
{
    return "http://www.microsoft.com"; 
}

tREAL32 CHardwarePlatformWinTel32::GetDiscAccessSpeed() 
{
    return 0.009f; // 9 milliseconds
}

tREAL32 CHardwarePlatformWinTel32::GetDiscReadSpeed()
{
    // Actual range is affected by too many parameters to make this a usable value
    return 30*SGX_MEGABYTES;
}

CHardwarePlatform::tEndian CHardwarePlatformWinTel32::GetEndian()
{
    return eLittleEndian;
}

tUINT32 CHardwarePlatformWinTel32::GetNumCPUs() 
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
}

tUINT32 CHardwarePlatformWinTel32::GetCPUSpeed(tUINT32 type)
{
    return 733; // not a real value! just to make it approximate to the Xbox
}

tUINT32 CHardwarePlatformWinTel32::GetNumMemoryBanks()
{
    return 1;
}

tUINT32 CHardwarePlatformWinTel32::GetMemorySize(tUINT32 type)
{
    // We're only concerned with system RAM. If a game
    // intentionally uses the swap we're in trouble!
    sgxAssert(type==0); 
    MEMORYSTATUS meminfo;
    GlobalMemoryStatus(&meminfo);
    return meminfo.dwAvailPhys; // as opposed to dwTotalPhys
}

