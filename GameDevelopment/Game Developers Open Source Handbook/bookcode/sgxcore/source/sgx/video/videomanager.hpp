#ifndef SGX_VIDEO_MANAGER_HPP
#define SGX_VIDEO_MANAGER_HPP

class CVideoManager {
public:
	static	CVideoManager	*Get();
	// Video format managers should register themselves with the manager
	// with this Register function.
	tBOOL	Register(const sgxString &id, CVideoFormatManager *pManager);
	tBOOL	Unregister(const sgxString &id);
	// Clients of the video manager should create their video
	// clips using this function
	CVideo *CreateVideo(const sgxString &name, const CVideo::CMovieParameters &params=CVideo::CMovieParameters());

protected:
	static	CVideoManager	*m_pSingleton;

private:
	typedef sgxMap<sgxString, CVideoFormatManager *> ProtocolMap;
    
	ProtocolMap             m_VideoProtocols;
};

#endif /* SGX_VIDEO_MANAGER_HPP */
