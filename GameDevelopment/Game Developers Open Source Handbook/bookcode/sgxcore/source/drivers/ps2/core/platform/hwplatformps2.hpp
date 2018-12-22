#ifndef SGX_HWPLATFORM_PS2_HPP
#define SGX_HWPLATFORM_PS2_HPP

class CHardwarePlatformPS2 : public CHardwarePlatform {
public:

    typedef enum {
        eEE = 1,
        eVU0,
        eVU1,
        eGPU,
        eSPU2,
        eIOP,
    } tPS2CPU;

    typedef enum {
        eMainMem = 1,
        eGSVRAM,
        eSoundMem,
        eIOPMem,
    } tPS2Memory;

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
    CHardwarePlatformPS2() {}
    static CHardwarePlatform   *ms_pSingleton;
};

#endif /* SGX_HWPLATFORM_PS2_HPP */
