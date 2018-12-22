#ifndef SGX_FILESYSTEM_FILEENTRY_HPP
#define SGX_FILESYSTEM_FILEENTRY_HPP	1

class CSGXFileEntry
{
public:
    CSGXFileEntry();
    CSGXFileEntry(const sgxString &Name);
    ~CSGXFileEntry();

    tBOOL       IsFile();
    tDISC_SIZE  GetFileSize() const { return m_Data.m_Filesize; }

    CSGXFileSysData     m_Data;
    CSGXDeviceBlock    *m_pBlock;      // device-specific state block for the file
    CSGXDevice         *m_pFSDevice;   // device-manager

    CSGXFileEntry      *m_pFirstChild;
    CSGXFileEntry      *m_pNextSibling;

};

#endif /* SGX_FILESYSTEM_FILEENTRY_HPP */
