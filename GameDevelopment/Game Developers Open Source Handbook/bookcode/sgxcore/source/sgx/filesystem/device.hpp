#ifndef SGX_FILESYSTEM_DEVICE_HPP
#define SGX_FILESYSTEM_DEVICE_HPP	1

class CSGXFileEntry;
class CSGXDeviceBlock;

class CSGXDeviceBlockState
{
public:
    typedef enum { eBegin=0, eCurrent, eEnd} tSeekFrom;

    virtual ~CSGXDeviceBlockState() {}

    virtual tMEMSIZE    Read(tBYTE *pDest, tMEMSIZE size)       { return 0; }
    virtual tMEMSIZE    Write(tBYTE *pDest, tMEMSIZE size)      { return 0; }
    virtual tBOOL       Seek(tINT32 offset, tSeekFrom from)     { return 0; }
    virtual tDISC_SIZE  GetSeekPos()                            { return 0; }

};

class CSGXDevice
{
public:
    virtual ~CSGXDevice() {}

    virtual tBOOL    Mount(CSGXFileEntry *pEntry, const sgxString &rootmount) { return false; }

    virtual CSGXDeviceBlockState *Open(CSGXDeviceBlock *pBlk, const sgxString &access) { return false; }
    virtual tBOOL          Close(CSGXDeviceBlockState *pBlk) { return false; }
      
    virtual CSGXFileEntry *CreateNewFile(CSGXFileEntry *pNewEntry) { return NULL; }
};

#endif /* SGX_FILESYSTEM_DEVICE_HPP */
