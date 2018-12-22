#include "sgx/core/core.hpp"
#include "sgx/filesystem/filesystem.hpp"

CSGXFileSysData::CSGXFileSysData()
{
    m_Name = "";
    m_Filesize = 0;
}

CSGXFileSysData::CSGXFileSysData(const sgxString &Name)
{
    m_Name = Name;
    m_Filesize = 0;
}

