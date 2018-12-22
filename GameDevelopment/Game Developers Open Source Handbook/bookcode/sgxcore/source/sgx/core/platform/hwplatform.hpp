#ifndef SGX_CORE_PLATFORM_HPP
#define SGX_CORE_PLATFORM_HPP 

#define SGX_KILOBYTES   (1024)
#define SGX_MEGABYTES   (1024*1024)

class CHardwarePlatform {
public:
    typedef enum { eWinTel32=0, eLinux32, ePS2, eXbox, eGameCube, eNumPlatforms, eCurrentPlatform } tPlatform;
    typedef enum { eBigEndian=0, eLittleEndian=1 } tEndian;
public:

    static CHardwarePlatform  *Get(tPlatform Platform = eCurrentPlatform);
    static tPlatform           GetCurrentPlatform();
    
    virtual const char *GetPlatform()=0;
    virtual const char *GetDocsURL()=0;
    virtual const char *GetDevelopmentURL()=0;
    virtual tREAL32     GetDiscAccessSpeed()=0;     // in seconds
    virtual tREAL32     GetDiscReadSpeed()=0;       // in bytes/second
    virtual tEndian     GetEndian()=0;
    virtual tUINT32     GetNumCPUs()=0;
    virtual tUINT32     GetCPUSpeed(tUINT32 type=0)=0;
    virtual tUINT32     GetNumMemoryBanks()=0;
    virtual tUINT32     GetMemorySize(tUINT32 type=0)=0;

    virtual void	    GetCRLF(tBYTE *);

    virtual void        DumpStats();

protected:
    CHardwarePlatform();
};

#endif /* SGX_CORE_PLATFORM_HPP */