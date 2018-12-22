#ifndef SGX_VIDEO_HPP
#define SGX_VIDEO_HPP	1

#include "sgx/utils/imagelib/imagedata.hpp"

class CVideo  {
public:
	// For initialisation
	class CMovieParameters {
	public:
		CMovieParameters();
		tBOOL	bLooping;
		tBOOL	bBilearFiltering;
		tREAL32	fInitialVolume;

		tBOOL	bUseImageData;
		CImageData *pImageData;
	};

	virtual tBOOL Create(const sgxString &filename, const CMovieParameters &params=CMovieParameters());
	virtual tBOOL Play();
	virtual void  Pause();
	virtual void  Unpause();
	virtual tBOOL IsPlaying();
	virtual tBOOL IsPaused();
	virtual void  Rewind();
	virtual void  Seek(const tREAL32 time);
	virtual void  Update(const tREAL32 telaps);
	virtual void  BuildImage(CImageData &data);

protected:
	CVideo() {}

	tBOOL			m_bPaused;
	tREAL32			m_fTimecum;
};

#endif /* SGX_VIDEO_HPP */
