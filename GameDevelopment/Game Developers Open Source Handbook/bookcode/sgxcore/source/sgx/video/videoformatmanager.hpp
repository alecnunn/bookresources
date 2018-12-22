#ifndef SGX_VIDEO_FORMAT_MANAGER_HPP
#define SGX_VIDEO_FORMAT_MANAGER_HPP	1

/*
This is a base from which all other video foramts should derive
*/

class CVideoFormatManager {
public:
	virtual ~CVideoFormatManager();
	virtual CVideo *CreateVideo(const sgxString &name, const CVideo::CMovieParameters &params);

	static	CVideoFormatManager	*Get();

protected:
	CVideoFormatManager();
	sgxString				m_Format;

	static	CVideoFormatManager *m_pSingleton;
};

#endif /* SGX_VIDEO_FORMAT_MANAGER_HPP */