#ifndef SGX_FILESYSTEM_RAMFILE_HPP
#define SGX_FILESYSTEM_RAMFILE_HPP


class CSGXDeviceMemory : public CSGXDevice    // controlling manager
{
public:
    tBYTE   *m_Ptr;
    tUINT32 m_Size;

    CSGXDeviceMemory(tBYTE *ptr, tUINT32 size);
    virtual ~CSGXDeviceMemory() {}
    virtual tBOOL Mount(CSGXFileEntry *pEntry, const sgxString &rootmount);

    virtual CSGXDeviceBlockState *Open(CSGXDeviceBlock *pBlk, const sgxString &access);
    virtual tBOOL Close(CSGXDeviceBlockState *pBlk);

};

class CSGXDeviceBlockStateMemory : public CSGXDeviceBlockState
{
public:
    CSGXDeviceMemory *m_pMemDevice;
    tMEMSIZE         m_CurrOffset;

    virtual ~CSGXDeviceBlockStateMemory() {}

    virtual tMEMSIZE    Read(tBYTE *pDest, tMEMSIZE size);
    virtual tMEMSIZE    Write(tBYTE *pDest, tMEMSIZE size);
    virtual tBOOL       Seek(tINT32 offset, CSGXDeviceBlockState::tSeekFrom from);
    virtual tDISC_SIZE  GetSeekPos();

};


#endif // SGX_RAMFILE_H
