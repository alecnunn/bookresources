#include "stdafx.h"
#include "sound3D.h"
#include "wavefile.h"


#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }


//-----------------------------------------------------------------------------
// Name: Sound::Sound()
// Desc: Constructs the class
//-----------------------------------------------------------------------------
Sound3D::Sound3D(const char* szWaveFile)
:	Sound(szWaveFile),
	m_pObj(0)
{
	m_type = Audio::SOUND3D;
}


Sound3D::~Sound3D()
{
}
