#include "sgx/core/core.hpp"

#include "drivers/wintel32/core/platform/hwplatformwintel32.hpp"
#include "drivers/gamecube/core/platform/hwplatformgamecube.hpp"
#include "drivers/ps2/core/platform/hwplatformps2.hpp"
#include "drivers/xbox/core/platform/hwplatformxbox.hpp"


CHardwarePlatform::CHardwarePlatform()
{
}

CHardwarePlatform::tPlatform CHardwarePlatform::GetCurrentPlatform()
{
#if SGX_PLATFORM_WINTEL32
    return eWinTel32;
#elif SGX_PLATFORM_PS2
    return ePS2;
#elif SGX_PLATFORM_GAMECUBE
    return eGameCube;
#elif SGX_PLATFORM_XBOX
    return eXbox;
#endif
    sgxError("No platform defined...");
    return eWinTel32;
}

CHardwarePlatform *CHardwarePlatform::Get(tPlatform Platform)
{
    if (Platform == eCurrentPlatform) {
        Platform = GetCurrentPlatform();
    }
    
    switch(Platform) {
        case eWinTel32:
            return CHardwarePlatformWinTel32::Get();

        case ePS2:
            return CHardwarePlatformPS2::Get();

        case eXbox:
            return CHardwarePlatformXbox::Get();

        case eGameCube:
            return CHardwarePlatformGameCube::Get();
        }
  
    return NULL;
}


void CHardwarePlatform::DumpStats(void)
{
    sgxTrace("SGX Engine - %s", GetPlatform());
    sgxTrace(" Local docs : %s", GetDocsURL());
    sgxTrace(" Remote site : %s", GetDevelopmentURL());

    sgxTrace("Disc Response:"); 
    sgxTrace(" Seek time (in ms) : %.2f", SGX_KILOBYTES*GetDiscAccessSpeed());
    sgxTrace(" Read time (in Mps) : %2.2f", GetDiscReadSpeed()/SGX_MEGABYTES);

    sgxTrace("CPUs:"); 
    sgxTrace(" Number : %d", GetNumCPUs());
    tUINT32 i;
    for(i=0; i<GetNumCPUs(); ++i) {
        sgxTrace("  CPU %d speed : %d", i+1, GetCPUSpeed(i));
    }

    sgxTrace("Memory banks:"); 
    sgxTrace(" Number : %d", GetNumMemoryBanks());
    for(i=0; i<GetNumMemoryBanks(); ++i) {
        sgxTrace("  Bank %d size : %d", i+1, GetMemorySize(i));
    }

    // We can retrieve platform-specific data like this:
    //     GetCPUSpeed(CHardwarePlatformPS2::tPS2CPU::eEE);
}

void CHardwarePlatform::GetCRLF(tBYTE *pDest)
{
	*(pDest+0) = '\r';
	*(pDest+1) = '\n';
	*(pDest+2) = '\0';
}

