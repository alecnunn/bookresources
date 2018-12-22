#include "sgx/core/core.hpp"
#include "drivers/xbox/core/platform/hwplatformxbox.hpp"

CHardwarePlatform *CHardwarePlatformXbox::ms_pSingleton = NULL;

CHardwarePlatform *CHardwarePlatformXbox::Get()
{
    if (ms_pSingleton == NULL) {
        ms_pSingleton = SGX_STATIC_CAST(CHardwarePlatform *, new CHardwarePlatformXbox);
    }
    return ms_pSingleton;            
}

const char *CHardwarePlatformXbox::GetPlatform()
{
    return "Microsoft Xbox"; 
}

const char *CHardwarePlatformXbox::GetDocsURL()
{
    return "//docs_drive/sdks/xbox"; 
}

const char *CHardwarePlatformXbox::GetDevelopmentURL() 
{
    return "http://www.microsoft.com"; 
}

tREAL32 CHardwarePlatformXbox::GetDiscAccessSpeed() 
{
    return 0.009f; // 9 ms
}

tREAL32 CHardwarePlatformXbox::GetDiscReadSpeed()
{
    // 2x-5x DVD, but also contains a hard drive
    return 375*SGX_KILOBYTES; // Kbps 
}

CHardwarePlatform::tEndian CHardwarePlatformXbox::GetEndian()
{
    return eLittleEndian;
}

tUINT32 CHardwarePlatformXbox::GetNumCPUs() 
{
    return 1;
}

tUINT32 CHardwarePlatformXbox::GetCPUSpeed(tUINT32 type)
{
    sgxAssert(type==0); 
    return 733; // in MHz
}

tUINT32 CHardwarePlatformXbox::GetNumMemoryBanks()
{
    return 1;
}

tUINT32 CHardwarePlatformXbox::GetMemorySize(tUINT32 type)
{
    sgxAssert(type==0); 
    return 64;
}

