#include "sgx/core/core.hpp"
#include "drivers/ps2/core/platform/hwplatformps2.hpp"

CHardwarePlatform *CHardwarePlatformPS2::ms_pSingleton = NULL;

CHardwarePlatform *CHardwarePlatformPS2::Get()
{
    if (ms_pSingleton == NULL) {
        ms_pSingleton = SGX_STATIC_CAST(CHardwarePlatform *, new CHardwarePlatformPS2);
    }
    return ms_pSingleton;            
}

const char *CHardwarePlatformPS2::GetPlatform()
{
    return "Sony PlayStation 2"; 
}

const char *CHardwarePlatformPS2::GetDocsURL()
{
    return "//docs_drive/sdks/ps2"; 
}

const char *CHardwarePlatformPS2::GetDevelopmentURL() 
{
    return "http://www.ps2pro.com"; 
}

tREAL32 CHardwarePlatformPS2::GetDiscAccessSpeed() 
{
    // This is based on the spec of an equivalent drive
    // for the PC! The real number would be under NDA.
    return 0.110f; // 110 ms
}

tREAL32 CHardwarePlatformPS2::GetDiscReadSpeed()
{
    // x24 speed CD-ROM, x4 DVD
    // x1 speed = 150 Kbps
    return 24*150*SGX_KILOBYTES;
}

CHardwarePlatform::tEndian CHardwarePlatformPS2::GetEndian()
{
    return eLittleEndian;
}

tUINT32 CHardwarePlatformPS2::GetNumCPUs() 
{
    return 6;
}

tUINT32 CHardwarePlatformPS2::GetCPUSpeed(tUINT32 type)
{
    sgxAssert(type==0); 

    // From http://www.ps2home.co.uk/ps2info.htm
    switch(type) {
        case    eEE:     return 295;    // actually 294.912 MHz
        case    eGPU:    return 147;    // actually 147.456 MHz
        case    eVU0:    return 0;      // unknown
        case    eVU1:    return 0;      // unknown
        case    eSPU2:   return 0;      // unknown
        case    eIOP:    return 34;     /// 33.8 MHz or 37.5 MHz (selectable)
    }

    return 0; // in MHz
}

tUINT32 CHardwarePlatformPS2::GetNumMemoryBanks()
{
    return 3;
}

tUINT32 CHardwarePlatformPS2::GetMemorySize(tUINT32 type)
{
    switch(type) {
        case    eMainMem:   return 32 * SGX_MEGABYTES;
        case    eGSVRAM:    return 4 * SGX_MEGABYTES;            
        case    eSoundMem:  return 2 * SGX_MEGABYTES;
        case    eIOPMem:    return 2 * SGX_MEGABYTES;
    }

	sgxError("PS2 doesn't have that many memory banks");
    return 0;
}

