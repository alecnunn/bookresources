#include "sgx/core/core.hpp"
#include "sgx/filesystem/filesystem.hpp"
#include "drivers/wintel32/wintel32core.hpp"
#include "drivers/wintel32/filesystem/win32disc.hpp"

//
// State
//
CSGXDeviceBlockStateWin32Disc::CSGXDeviceBlockStateWin32Disc(HANDLE hFile) : CSGXDeviceBlockState()
{
    m_hFile = hFile;
}

tMEMSIZE CSGXDeviceBlockStateWin32Disc::Read(tBYTE *pDest, tMEMSIZE size)
{
DWORD size2read = size;
DWORD bytesread;

    ::ReadFile(m_hFile, pDest, size2read, &bytesread, NULL);
    return bytesread;
}

tMEMSIZE CSGXDeviceBlockStateWin32Disc::Write(tBYTE *pDest, tMEMSIZE size)
{
DWORD size2write = size;
DWORD byteswritten;

    ::WriteFile(m_hFile, pDest, size2write, &byteswritten, NULL);
    return byteswritten;
}

tBOOL CSGXDeviceBlockStateWin32Disc::Seek(tINT32 offset, CSGXDeviceBlockState::tSeekFrom from)
{
DWORD win32from;

    switch(from)
    {
    case    eBegin:   win32from = FILE_BEGIN; break;
    case    eCurrent: win32from = FILE_CURRENT; break;
    case    eEnd:     win32from = FILE_END; break;
    }

    ::SetFilePointer(m_hFile, offset, NULL, win32from);
    return true;
}


tDISC_SIZE CSGXDeviceBlockStateWin32Disc::GetSeekPos()
{
    return ::SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT);
}
