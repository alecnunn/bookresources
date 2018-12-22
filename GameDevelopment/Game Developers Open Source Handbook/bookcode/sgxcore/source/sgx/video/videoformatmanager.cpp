#include "sgx/core/core.hpp"
#include "sgx/video/video.hpp"

CVideoFormatManager *CVideoFormatManager::m_pSingleton;

CVideoFormatManager	*
CVideoFormatManager::Get()
{
	if (!m_pSingleton) {
		m_pSingleton = new CVideoFormatManager;
	}
	return m_pSingleton;
}

CVideoFormatManager::CVideoFormatManager() : m_Format("null")
{
	CVideoManager::Get()->Register(m_Format, this);
}

CVideoFormatManager::~CVideoFormatManager()
{
	CVideoManager::Get()->Unregister(m_Format);
}

CVideo *
CVideoFormatManager::CreateVideo(const sgxString &name, const CVideo::CMovieParameters &params)
{
	return NULL;
}
