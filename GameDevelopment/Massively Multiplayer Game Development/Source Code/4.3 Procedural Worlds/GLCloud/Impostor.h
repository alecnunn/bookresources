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

#ifndef __Impostor_h__
#define __Impostor_h__

#include "Texture.h"


class CImpostor : public C3DObject
{
protected:
	CVector m_vDimension;		// The dimensions of the 3D bounding box for the object being represented
	CDoubleVector m_vCamera;	// The camera position when the impostor was last generated

	CTexture m_tImpostor;		// The impostor texture map
	int m_nResolution;			// The chosen resolution for the impostor texture map
	CVector m_vUp;				// The up vector used to generate the impostor texture
	CVector m_vOffset;			// The offset from the original position to draw the billboard
	float m_fMinx, m_fMiny, m_fMaxx, m_fMaxy;	// The dimensions of the 2D bounding box for the impostor billboard

public:
	CImpostor() : C3DObject() {}

	float CalculateError(C3DObject *pCamera);
	bool IsInView(C3DObject *pCamera);
	void InitImpostorRender(C3DObject *pCamera);
	void FinishImpostorRender();
	void Draw(C3DObject *pCamera);
};

#endif // __Impostor_h__
