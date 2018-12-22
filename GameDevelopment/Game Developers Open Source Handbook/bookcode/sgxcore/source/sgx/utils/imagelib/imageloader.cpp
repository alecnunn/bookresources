#include "sgx/core/core.hpp"
#include "sgx/filesystem/filesystem.hpp"
#include "sgx/utils/imagelib/imagelib.hpp"

CImageLoader *CImageLoader::m_pSingleton = NULL;

CImageLoader *CImageLoader::Create()
{
	m_pSingleton = new CImageLoader;
	if (m_pSingleton) {
		m_pSingleton->Initialize();
	}
	return m_pSingleton;
}

CImageLoader *CImageLoader::Get()
{
	if (!m_pSingleton) {
		return Create();
	}
	return m_pSingleton;
}

CImageLoader::CImageLoader()
{
}

void 
CImageLoader::Initialize()
{
}

tBOOL CImageLoader::Register(const CImageParser *pCls)
{
	m_Parsers.push_back(pCls);
	return TRUE;
}

tBOOL
CImageLoader::LoadImageData(CImageData &data, CSGXFile &file)
{
	if (!file.IsValid()) {
		return FALSE;
	}

	// Find a suitable parser from the registered list
	sgxVector<const CImageParser *>::const_iterator it = m_Parsers.begin();
	tDISC_SIZE seek_pos = file.GetSeekPos();

	for(;it!=m_Parsers.end();++it) {
		if ((*it)->IsRecognised(file)) {

			// Rewind the file pointer because the recognition code
			// doesn't have to check the file extension.
			file.Seek(seek_pos, CSGXDeviceBlockState::eBegin);

			// We're ready to read the file.
			return (*it)->Open(file, data);
		}
		// Rewind pointer so every parser has a chance
		file.Seek(seek_pos, CSGXDeviceBlockState::eBegin);
	}

	return FALSE;
}
