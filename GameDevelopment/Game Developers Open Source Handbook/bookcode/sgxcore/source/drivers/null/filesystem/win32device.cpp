#include "sgx/core/core.hpp"
#include "sgx/filesystem/filesystem.hpp"
#include "drivers/wintel32/wintel32core.hpp"
#include "drivers/wintel32/filesystem/win32disc.hpp"

// Here we have:
// 1. Setup block: One per file. Contains general file information that you
//      don't want to search for. e.g. size
// 2. State block: Dyanmically created for each file that is opened. Acts like a
//      file handle. Use this to retrieve the data. Brave users can combine file handles
//      in Win32Disc class
// 3. DeviceHandler class.
//      Open & Close: Creates state block for the file requested (by setup block)
//      Mount:  Attach file 'setup blocks'[1] to heirarchy

CSGXDeviceWin32Disc::CSGXDeviceWin32Disc(const sgxString &path) : CSGXDevice()
{
    m_Root = path;
}

CSGXFileEntry *CSGXDeviceWin32Disc::CreateNewFile(CSGXFileEntry *pNewEntry)
{
    // The real Windows path can be deduced from the parents full
    // windows path very easily.
    sgxString winpath = m_Root + "\\" + pNewEntry->m_Data.m_Name;
    pNewEntry->m_pBlock = new CSGXDeviceBlockWin32Disc(winpath);
    pNewEntry->m_pFSDevice = this;

    return pNewEntry;
}

tBOOL CSGXDeviceWin32Disc::Mount(CSGXFileEntry *pEntry, const sgxString &rootmount)
{
    WIN32_FIND_DATA finddata;
    HANDLE hFind;
    sgxString search = m_Root + "/*.*";
    CSGXFileSystem *pFS = CSGXFileSystem::Get();

    hFind = ::FindFirstFile(search.c_str(), &finddata);
    if (hFind == INVALID_HANDLE_VALUE) {
        return false;
    } else {
        // Scan directory for files
        do {
            if (finddata.cFileName[0] == '.') {
                // Ignore all 'dot' files
            } else if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                // Ignore sub-directories (possible future expansion task)
            } else {
                CSGXFileSysData FileData;

                FileData.m_Name = rootmount + "/" + sgxString(finddata.cFileName);
                FileData.m_Filesize = finddata.nFileSizeLow;

                pFS->CreateNewFile(FileData);
            }
        } while (::FindNextFile(hFind, &finddata));
  
        ::FindClose(hFind);
    }

    return true;
}

CSGXDeviceBlockState *CSGXDeviceWin32Disc::Open(CSGXDeviceBlock *pBlk, const sgxString &access)
{
    CSGXDeviceBlockWin32Disc *pBlock = SGX_STATIC_CAST(CSGXDeviceBlockWin32Disc *, pBlk);
    HANDLE hFile;

    if (access == "rw") {
        hFile = ::CreateFile(pBlock->m_Filename.c_str(), FILE_WRITE_DATA, FILE_SHARE_READ, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    } else {
        hFile = ::CreateFile(pBlock->m_Filename.c_str(), FILE_READ_DATA, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    }

    if (hFile == INVALID_HANDLE_VALUE ) {
        return NULL;
    }

    CSGXDeviceBlockStateWin32Disc *pState = new CSGXDeviceBlockStateWin32Disc(hFile);
 
    return pState;
}

tBOOL CSGXDeviceWin32Disc::Close(CSGXDeviceBlockState *pState)
{
    CSGXDeviceBlockStateWin32Disc *pWinState = SGX_STATIC_CAST(CSGXDeviceBlockStateWin32Disc *, pState);
    
    ::CloseHandle(pWinState->m_hFile);
    delete pState;
    return true;
}

