#ifndef SGX_FILESYSTEM_FILE_HPP
#define SGX_FILESYSTEM_FILE_HPP	1

class CHardwarePlatform;

class CSGXFile
{
public:
    CSGXFile(const sgxString &filename, const sgxString &access="r");
    ~CSGXFile();

    void                Close();
    tMEMSIZE            Read(tBYTE *pDest, tMEMSIZE size);
    tMEMSIZE            Write(tBYTE *pDest, tMEMSIZE size);
    tBOOL               Seek(tINT32 offset, CSGXDeviceBlockState::tSeekFrom from);
    tDISC_SIZE          GetSeekPos();

    void                SetTargetPlatform(CHardwarePlatform *pPlatform);
    CHardwarePlatform * GetTargetPlatform(void) { return m_pPlatform; }
    tBOOL               IsSaving()  { return m_bValid ? m_bSaving : FALSE; }
    tBOOL               IsLoading() { return m_bValid ? !m_bSaving : FALSE; }
    tBOOL               IsValid()   { return m_bValid; }

    tBOOL               SwapEndian()   { return m_bSwapEndian; }

    void                WriteString(const sgxString &str);
	void				WriteAsciiString(const sgxString &str);
	void				WriteAsciiStringCR(const sgxString &str);
	
    void                WriteConstant(tMEMSIZE val);
    void                ReadString(sgxString &str);
    tMEMSIZE            ReadConstant();

protected:
    CSGXFile() {}

private:
    CSGXFileEntry        *m_pFileEntry;
    CSGXDeviceBlockState *m_pDevState;
	tBOOL                 m_bValid;
    tBOOL                 m_bSwapEndian;
    tBOOL                 m_bSaving;
    CHardwarePlatform    *m_pPlatform;
};

#endif /* SGX_FILESYSTEM_FILE_HPP */
