#include "sgx/core/core.hpp"
#include "sgx/utils/imagelib/imagedata.hpp"
#include "sgx/video/video.hpp"

// Generic parameters
CVideo::CMovieParameters::CMovieParameters() :
	bBilearFiltering(FALSE),
	bLooping(TRUE),
	fInitialVolume(1.0f),
	bUseImageData(FALSE)
{
}

// The null driver class
tBOOL 
CVideo::Create(const sgxString &filename, const CMovieParameters &params)
{
	m_fTimecum= 0;
	m_bPaused= FALSE;
	return FALSE;
}


tBOOL  
CVideo::Play()
{
	return FALSE;
}

void   
CVideo::Pause()
{
	m_bPaused = TRUE;
}

void   
CVideo::Unpause()
{
	m_bPaused = FALSE;
}

tBOOL  
CVideo::IsPlaying()
{
	return FALSE;
}

tBOOL  
CVideo::IsPaused()
{
	return m_bPaused;
}

void   
CVideo::Rewind()
{
	Seek(0);
}

void   
CVideo::Seek(const tREAL32 time)
{
	m_fTimecum = time;
}

void   
CVideo::Update(const tREAL32 telaps)
{
	m_fTimecum += telaps;
}

void   
CVideo::BuildImage(CImageData &data)
{
}

