#include "core.h"
#include "ramfile.h"

//
// Memory Device
//
CSGXDeviceMemory::CSGXDeviceMemory(tBYTE *ptr, tUINT32 size) : CSGXDevice()
{
    m_Ptr = ptr;
    m_Size = size;
}

tBOOL CSGXDeviceMemory::Mount(CSGXFileEntry *pEntry, const sgxString &rootmount)
{
    pEntry->m_pBlock = new CSGXDeviceBlock;
    pEntry->m_Data.m_Filesize = m_Size;
    return true;
}

CSGXDeviceBlockState *CSGXDeviceMemory::Open(CSGXDeviceBlock *pBlk, const sgxString &access)
{
    CSGXDeviceBlockStateMemory *pState = new CSGXDeviceBlockStateMemory;

    pState->m_CurrOffset = 0;
    pState->m_pMemDevice = this;
    return pState;
}

tBOOL CSGXDeviceMemory::Close(CSGXDeviceBlockState *pState)
{
    delete pState;
    return true;
}
