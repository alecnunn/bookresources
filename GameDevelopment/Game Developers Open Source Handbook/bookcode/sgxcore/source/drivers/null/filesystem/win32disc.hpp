#ifndef SGX_WIN32_DISC_H
#define SGX_WIN32_DISC_H

class CSGXDeviceWin32Disc : public CSGXDevice
{
public:
    CSGXDeviceWin32Disc(const sgxString &path);
    virtual ~CSGXDeviceWin32Disc() {}

    virtual tBOOL Mount(CSGXFileEntry *pEntry, const sgxString &rootmount);

    virtual CSGXDeviceBlockState *Open(CSGXDeviceBlock *pBlk, const sgxString &access);
    virtual tBOOL Close(CSGXDeviceBlockState *pState);
    virtual CSGXFileEntry *CreateNewFile(CSGXFileEntry *pNewEntry);

private:
    sgxString   m_Root;
};


class CSGXDeviceBlockWin32Disc : public CSGXDeviceBlock
{
public:
    CSGXDeviceBlockWin32Disc(const sgxString &filename);
    tDISC_SIZE  GetFileSize(CSGXDeviceBlock *pBlock) const;

    sgxString   m_Filename;
    tDISC_SIZE  m_Filesize;
};

class CSGXDeviceBlockStateWin32Disc : public CSGXDeviceBlockState
{
public:
    CSGXDeviceBlockStateWin32Disc(HANDLE hFile);

    virtual tMEMSIZE    Read(tBYTE *pDest, tMEMSIZE size);
    virtual tMEMSIZE    Write(tBYTE *pDest, tMEMSIZE size);
    virtual tBOOL       Seek(tINT32 offset, CSGXDeviceBlockState::tSeekFrom from);
    virtual tDISC_SIZE  GetSeekPos();

    HANDLE  m_hFile;

private:
    CSGXDeviceBlockStateWin32Disc();    // no default ctor allowed
};


#endif // SGX_WIN32_DISC_H
