#ifndef SGXDRV_WINTEL32_PLATFORM_HPP
#define SGXDRV_WINTEL32_PLATFORM_HPP

class CHardwarePlatformWinTel32 : public CHardwarePlatform {
public:
    virtual const char *GetPlatform();
    virtual const char *GetDocsURL();
    virtual const char *GetDevelopmentURL();
    virtual tREAL32     GetDiscAccessSpeed();
    virtual tREAL32     GetDiscReadSpeed();
    virtual tEndian     GetEndian();
    virtual tUINT32     GetNumCPUs();
    virtual tUINT32     GetCPUSpeed(tUINT32 type=0);
    virtual tUINT32     GetNumMemoryBanks();
    virtual tUINT32     GetMemorySize(tUINT32 type=0);

    static CHardwarePlatform   *Get();

protected:
    static CHardwarePlatform   *ms_pSingleton;
};

#endif /* SGXDRV_WINTEL32_PLATFORM_HPP */
