#ifndef SGX_HWPLATFORM_XBOX_HPP
#define SGX_HWPLATFORM_XBOX_HPP

class CHardwarePlatformXbox : public CHardwarePlatform {
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

#endif /* SGX_HWPLATFORM_XBOX_HPP */
