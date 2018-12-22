#include "sgx/core/core.hpp"
#include "sgx/filesystem/filesystem.hpp"

CSGXFileEntry::CSGXFileEntry() : m_Data()
{
    m_pBlock = NULL;
    m_pFSDevice = NULL;
    m_pFirstChild = NULL;
    m_pNextSibling = NULL;
}

CSGXFileEntry::CSGXFileEntry(const sgxString &Name) : m_Data(Name)
{
    m_pBlock = NULL;
    m_pFSDevice = NULL;
    m_pFirstChild = NULL;
    m_pNextSibling = NULL;
}

CSGXFileEntry::~CSGXFileEntry()
{
	delete m_pFirstChild;
	delete m_pNextSibling;
}

tBOOL CSGXFileEntry::IsFile()
{
    if (m_pBlock) {
        return true;
    }
    return false;
}
