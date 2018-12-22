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
#include "PixelBuffer.h"

void CPixelBuffer::MakeGlow(float fExpose, float fSizeDisc)
{
	int i;
	int n = 0;
	unsigned char nIntensity;
	for(int y=0; y<m_nHeight; y++)
	{
		float fDy = (y+0.5f)/m_nHeight - 0.5f;
		for(int x=0; x<m_nWidth; x++)
		{
			float fDx = (x+0.5f)/m_nWidth - 0.5f;
			float fDist = sqrtf(fDx*fDx + fDy*fDy);
			//float fIntensity = expf(-Max(fDist-fSizeDisc,0)*fExpose);
			//float fIntensity = 2 - Min(2, powf(2.0f, Max(fDist-fSizeDisc,0)*fExpose));
			float fIntensity = 0.1f * (Min(2, powf(2.0f, Max(fDist-fSizeDisc,0)*fExpose)) - 1) + 0.9f;
			//float fIntensity = 1 - 0.5f*powf(2.0f, -Max(fDist-fSizeDisc,0)*fExpose);
			switch(m_nDataType)
			{
				case GL_UNSIGNED_BYTE:
					nIntensity = (unsigned char)(fIntensity*255 + 0.5f);
					for(i=0; i<m_nChannels; i++)
						((unsigned char *)m_pBuffer)[n++] = nIntensity;
					break;
				case GL_FLOAT:
					for(i=0; i<m_nChannels; i++)
						((float *)m_pBuffer)[n++] = fIntensity;
					break;
			}
		}
	}
}

void CPixelBuffer::MakeGlow2(float fExpose, float fSizeDisc)
{
	int n = 0;
	for(int y=0; y<m_nHeight; y++)
	{
		float fDy = (y+0.5f)/m_nHeight - 0.5f;
		for(int x=0; x<m_nWidth; x++)
		{
			float fDx = (x+0.5f)/m_nWidth - 0.5f;
			float fDist = sqrtf(fDx*fDx + fDy*fDy);
			//float fIntensity = expf(-Max(fDist-fSizeDisc,0)*fExpose);
			float fIntensity = 2 - Min(2, powf(2.0f, Max(fDist-fSizeDisc,0)*fExpose));
			//float fIntensity = 0.1f * (Min(2, powf(2.0f, Max(fDist-fSizeDisc,0)*fExpose)) - 1) + 0.9f;
			//float fIntensity = 1 - 0.5f*powf(2.0f, -Max(fDist-fSizeDisc,0)*fExpose);

			((unsigned char *)m_pBuffer)[n++] = (unsigned char)(fIntensity*255 + 0.5f);
			((unsigned char *)m_pBuffer)[n++] = (unsigned char)(fIntensity*255 + 0.5f);
		}
	}
}

void CPixelBuffer::Invert()
{
	for(int i=0; i<m_nHeight*m_nWidth*m_nChannels; i++)
	{
		switch(m_nDataType)
		{
			case GL_UNSIGNED_BYTE:
				((unsigned char *)m_pBuffer)[i] = 255 - ((unsigned char *)m_pBuffer)[i];
				break;
			case GL_FLOAT:
				((float *)m_pBuffer)[i] = 1.0f - ((float *)m_pBuffer)[i];
				break;
		}
	}
}
