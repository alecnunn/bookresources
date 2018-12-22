#ifndef SGX_FILESYSTEM_FILESYSDATA_HPP
#define SGX_FILESYSTEM_FILESYSDATA_HPP	1

class CSGXFileSysData
{
public:
    CSGXFileSysData();
    CSGXFileSysData(const sgxString &Name);

    sgxString          m_Name;
    tDISC_SIZE         m_Filesize;
};

#endif /* SGX_FILESYSTEM_FILESYSDATA_HPP */
