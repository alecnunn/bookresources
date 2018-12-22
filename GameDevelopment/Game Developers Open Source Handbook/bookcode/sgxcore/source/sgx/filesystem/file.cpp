#include "sgx/core/core.hpp"
#include "sgx/filesystem/filesystem.hpp"

CSGXFile::CSGXFile(const sgxString &filename, const sgxString &access)
{
    // assume read-only for now
    m_pFileEntry = CSGXFileSystem::Get()->GetFileEntry(filename);
    m_pPlatform = CHardwarePlatform::Get();
	m_bValid = false;
    m_bSwapEndian = FALSE;


    if (!m_pFileEntry) {
        if (access == "r") {
            sgxTrace("No such file '%s'", filename.c_str());
            return; 
        } else {
            // if writing a file that, currently, doesn't exist we
            // need to create a block for it
            CSGXFileSysData FileData;
            FileData.m_Name = filename;
            FileData.m_Filesize = 0;
            m_pFileEntry = CSGXFileSystem::Get()->CreateNewFile(FileData);

            if (!m_pFileEntry) {
                sgxTrace("No such file '%s'", filename.c_str());
                return;
            }
        }
    }

    if (access == "rw") { 
        m_bSaving = TRUE;
    } else {
        m_bSaving = FALSE;
    }

    if ((m_pDevState = m_pFileEntry->m_pFSDevice->Open(m_pFileEntry->m_pBlock, access))) {
	    m_bValid = true;
    }
}

CSGXFile::~CSGXFile()
{
	Close();
}

void CSGXFile::Close()
{
	if (m_bValid) {
		m_pFileEntry->m_pFSDevice->Close(m_pDevState);
	}
	m_bValid = false;
}

tMEMSIZE CSGXFile::Read(tBYTE *pDest, tMEMSIZE size)
{
    return m_bValid ? m_pDevState->Read(pDest, size) : 0;
}

tMEMSIZE CSGXFile::Write(tBYTE *pDest, tMEMSIZE size)
{
    return m_bValid ? m_pDevState->Write(pDest, size) : 0; 
}

tBOOL CSGXFile::Seek(tINT32 offset, CSGXDeviceBlockState::tSeekFrom from)
{
    return m_bValid ? m_pDevState->Seek(offset, from) : false; 
}

tDISC_SIZE CSGXFile::GetSeekPos()
{
    return m_bValid ? m_pDevState->GetSeekPos() : 0; 
}

void CSGXFile::WriteAsciiString(const sgxString &str)
{
    sgxAssert(IsSaving()); 
    Write((tBYTE *)str.c_str(), str.length());
}

void CSGXFile::WriteAsciiStringCR(const sgxString &str)
{
	tBYTE crlf[4];

    sgxAssert(IsSaving()); 
    Write((tBYTE *)str.c_str(), str.length());

	CHardwarePlatform::Get()->GetCRLF(crlf);
    Write((tBYTE *)crlf, sgxStrlen((char *)crlf));
}

void CSGXFile::WriteString(const sgxString &str)
{
    *this << str;
}

void CSGXFile::WriteConstant(tMEMSIZE val)
{
    *this << val;
}

void CSGXFile::ReadString(sgxString &str)
{
    *this >> str;
}

tMEMSIZE CSGXFile::ReadConstant()
{
    tMEMSIZE val;
    *this >> val;
    return val;
}

void CSGXFile::SetTargetPlatform(CHardwarePlatform *pPlatform)
{
    CHardwarePlatform *pCurrent = CHardwarePlatform::Get();

    if (pCurrent->GetEndian() != pPlatform->GetEndian()) {
        m_bSwapEndian = TRUE;
    } else {
        m_bSwapEndian = FALSE;
    }

    m_pPlatform = pPlatform;
}

