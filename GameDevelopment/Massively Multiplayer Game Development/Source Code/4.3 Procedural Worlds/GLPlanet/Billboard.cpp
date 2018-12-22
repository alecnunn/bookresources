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

#include "Master.h"
#include "Billboard.h"

void CBillboard::Draw(CTexture *pTexture, CVector vPos, float fSize)
{
	pTexture->Enable();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(vPos.x-fSize, vPos.y+fSize, vPos.z);
	glTexCoord2f(0, 1);
	glVertex3f(vPos.x-fSize, vPos.y-fSize, vPos.z);
	glTexCoord2f(1, 1);
	glVertex3f(vPos.x+fSize, vPos.y-fSize, vPos.z);
	glTexCoord2f(1, 0);
	glVertex3f(vPos.x+fSize, vPos.y+fSize, vPos.z);
	glEnd();
	pTexture->Disable();
}

void CBillboard::Draw(CTexture *pTexture, C3DObject *pCamera, CVector vPos, CVector vUp, float fSize)
{
	// First determine the actual normal, up, and right vectors for the billboard
	vPos -= pCamera->GetPosition();
	float fDistance = vPos.Magnitude();

	CVector vNormal = vPos / -fDistance;
	CVector vRight = vUp ^ vNormal;
	vRight.Normalize();
	vUp = vNormal ^ vRight;
	vUp.Normalize();

	float fScale = C3DObject::GetScalingFactor(fDistance);
	vPos *= fScale;
	vRight *= fScale;
	vUp *= fScale;

	// Then draw the billboarded rectangle
	pTexture->Enable();
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3fv(vPos + vUp*fSize + vRight*(-fSize));
	glTexCoord2f(0, 0);
	glVertex3fv(vPos + vUp*(-fSize) + vRight*(-fSize));
	glTexCoord2f(1, 0);
	glVertex3fv(vPos + vUp*(-fSize) + vRight*fSize);
	glTexCoord2f(1, 1);
	glVertex3fv(vPos + vUp*fSize + vRight*fSize);
	glEnd();
	glDisable(GL_BLEND);
	pTexture->Disable();
}
