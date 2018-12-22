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

#include "DXUtil.h"
#include "Texture.h"
#include "Font.h"
#include "ROAMSphere.h"
#include "StarSystem.h"


/****************************************************************************
* Class: CGameEngine
*****************************************************************************
* This class implements the main engine. For now it just renders each frame. 
* It should eventually handle all logic for determining what happens each
* frame. This includes handling input, updating game objects, playing sounds,
* and rendering the frames.
****************************************************************************/
class CGameEngine : public IKeyboardListener, IMouseListener
{
protected:
	CInputEngine m_engInput;
	CFont m_fFont;
	C3DObject m_3DCamera;
	CStarSystem m_starSystem;
	GLenum m_nPolygonMode;
	bool m_bTexture;
	bool m_bUpdate;
	bool m_bInfo;
	float m_fMaxError;

public:
	CGameEngine();
	~CGameEngine();
	void RenderFrame(int nMilliseconds);
	void Pause()	{ m_engInput.Pause(); }
	void Restore()	{ m_engInput.Restore(); }
	void HandleInput(float fSeconds);
	void TogglePolygonMode()
	{
		m_nPolygonMode = (m_nPolygonMode == GL_FILL) ? GL_LINE : GL_FILL;
		//glPolygonMode(GL_FRONT, m_nPolygonMode);
	}
	void ToggleTextures()		{ m_bTexture = !m_bTexture; }
	void ToggleInfo()			{ m_bInfo = !m_bInfo; }
	void ToggleUpdate()			{ m_bUpdate = !m_bUpdate; }

	virtual void OnKeyDown(int nKey);
	virtual void OnKeyUp(int nKey);
	virtual void OnMouseButtonDown(int nButton);
	virtual void OnMouseButtonUp(int nButton);
};

#endif // __GameEngine_h__
