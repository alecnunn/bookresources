/*
s_p_oneil@hotmail.com
Copyright (c) 2000, Sean O'Neil
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of this project nor the names of its contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __GameEngine_h__
#define __GameEngine_h__

#include "GLUtil.h"
#include "GameApp.h"
#include "Texture.h"
#include "Font.h"
#include "BitBucket.h"
#include "PBuffer.h"

class CGameEngine
{
protected:
	float m_fFPS;
	int m_nTime;
	CFont m_fFont;

	C3DObject m_3DCamera;
	CVector m_vLight;
	bool m_bRotatingLight;
	bool m_bRotatingClouds;
	bool m_bUseImpostor;
	float m_fHalfSize;
	int m_nSeed;

	CCloudBlock m_cBlock;
	CPBuffer m_PBuffer;

public:
	CGameEngine();
	~CGameEngine();
	void RenderFrame(int nMilliseconds);
	void Pause()	{}
	void Restore()	{}
	void HandleInput(float fSeconds);
};

#endif // __GameEngine_h__
