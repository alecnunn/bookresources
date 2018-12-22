#include "sgx/core/core.hpp"
#include "sgx/video/video.hpp"

CVideoManager	*CVideoManager::m_pSingleton;

CVideoManager	*
CVideoManager::Get()
{
	if (!m_pSingleton) {
		m_pSingleton = new CVideoManager;
	}
	return m_pSingleton;
}

tBOOL
CVideoManager::Register(const sgxString &id, CVideoFormatManager *pManager)
{
    return m_VideoProtocols.insert(ProtocolMap::value_type(id, pManager)).second ? TRUE : FALSE;
}

tBOOL	
CVideoManager::Unregister(const sgxString &id)
{
	m_VideoProtocols.erase(id);
	return FALSE;
}

CVideo *
CVideoManager::CreateVideo(const sgxString &name, const CVideo::CMovieParameters &params)
{
	int n = name.find_first_of(':');
	if (n == -1) {
		return NULL;
	}
	sgxString protocol = name.substr(0, n);
	sgxString subname = name.substr(n+1);

    ProtocolMap::iterator it = m_VideoProtocols.find(protocol);

    if (it == m_VideoProtocols.end()) {
        return NULL;
    }

	return (*it->second).CreateVideo(subname, params);
}
