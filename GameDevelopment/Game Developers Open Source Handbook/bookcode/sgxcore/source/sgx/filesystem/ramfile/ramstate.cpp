#include "core.h"
#include "ramfile.h"

//
// Block State
//
tMEMSIZE CSGXDeviceBlockStateMemory::Read(tBYTE *pDest, tMEMSIZE size)
{
    tMEMSIZE readbytes;

    // Check if file is open? Done by filemanager? Yes!
    if (m_CurrOffset + size > m_pMemDevice->m_Size) {
        readbytes = m_pMemDevice->m_Size - m_CurrOffset;
    } else {
        readbytes = size;
    }

    sgxMemmove(pDest, &m_pMemDevice->m_Ptr[m_CurrOffset], readbytes);
    m_CurrOffset += readbytes;
    return readbytes;
}

tMEMSIZE CSGXDeviceBlockStateMemory::Write(tBYTE *pSource, tMEMSIZE size)
{
    tMEMSIZE written;

    if (m_CurrOffset + size > m_pMemDevice->m_Size) {
        written = m_pMemDevice->m_Size - m_CurrOffset;
    } else {
        written = size;
    }

    sgxMemmove(&m_pMemDevice->m_Ptr[m_CurrOffset], pSource, written);
    m_CurrOffset += written;
    return written;
}

tBOOL CSGXDeviceBlockStateMemory::Seek(tINT32 offset, CSGXDeviceBlockState::tSeekFrom from)
{
    switch(from)
    {
    case    eBegin:   m_CurrOffset = offset; break;
    case    eCurrent: m_CurrOffset += offset; break;
    case    eEnd:     m_CurrOffset = m_pMemDevice->m_Size+offset; break;
    }

    return true;
}

tDISC_SIZE CSGXDeviceBlockStateMemory::GetSeekPos()
{
    return m_CurrOffset;
}
