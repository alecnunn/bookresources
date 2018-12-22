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
#include "Texture.h"

CTexture CTexture::m_tCursor;
CTexture CTexture::m_tNoise;
CTexture CTexture::m_tStar;
CTexture CTexture::m_tAtmosphereShell;
CTexture CTexture::m_tCloudCell;

void CTexture::InitStaticMembers(int nSeed, int nSize)
{
	CPixelBuffer pb;

	pb.Init(16, 16, 2, GL_LUMINANCE_ALPHA);
	pb.MakeGlow(9.0f, 0.05f);
	m_tCursor.Init(&pb, false);

	// Initialize the shared noise texture
	pb.Init(nSize, nSize, 1, GL_LUMINANCE);
	pb.MakeNoise(nSeed);
	m_tNoise.Init(&pb, false);

	// Initialize the shared star (or glow) texture
	pb.Init(nSize, nSize);
	pb.MakeGlow(20.0f, 0.01f);
	m_tStar.Init(&pb, true);

	// Initialize the shared atmosphere shell texture
	pb.Init(nSize, nSize, 2, GL_LUMINANCE_ALPHA);
	pb.MakeAtmosphere(50.0f);
	m_tAtmosphereShell.Init(&pb, true, false);

	// Initialize the shared cloud cell texture
	pb.Init(16, 16, 2, GL_LUMINANCE_ALPHA);
	pb.MakeCloudCell(2, 0);
	m_tCloudCell.Init(&pb);

}

void CTexture::Init(CPixelBuffer *pBuffer, bool bClamp, bool bMipmap)
{
	Cleanup();
	m_nType = (pBuffer->GetHeight() == 1) ? GL_TEXTURE_1D : GL_TEXTURE_2D;
	glGenTextures(1, &m_nID);
	Bind();
	glTexParameteri(m_nType, GL_TEXTURE_WRAP_S, bClamp ? GL_CLAMP : GL_REPEAT);
	glTexParameteri(m_nType, GL_TEXTURE_WRAP_T, bClamp ? GL_CLAMP : GL_REPEAT);
	glTexParameteri(m_nType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(m_nType, GL_TEXTURE_MIN_FILTER, bMipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

	if(m_nType == GL_TEXTURE_1D)
	{
		if(bMipmap)
			gluBuild1DMipmaps(GL_TEXTURE_1D, pBuffer->GetChannels(), pBuffer->GetWidth(), pBuffer->GetFormat(), pBuffer->GetDataType(), pBuffer->GetBuffer());
		else
			glTexImage1D(GL_TEXTURE_1D, 0, pBuffer->GetChannels(), pBuffer->GetWidth(), 0, pBuffer->GetFormat(), pBuffer->GetDataType(), pBuffer->GetBuffer());
	}
	else
	{
		if(bMipmap)
			gluBuild2DMipmaps(GL_TEXTURE_2D, pBuffer->GetChannels(), pBuffer->GetWidth(), pBuffer->GetHeight(), pBuffer->GetFormat(), pBuffer->GetDataType(), pBuffer->GetBuffer());
		else
			glTexImage2D(GL_TEXTURE_2D, 0, pBuffer->GetChannels(), pBuffer->GetWidth(), pBuffer->GetHeight(), 0, pBuffer->GetFormat(), pBuffer->GetDataType(), pBuffer->GetBuffer());
	}
}

void CTexture::Update(CPixelBuffer *pBuffer, int nLevel)
{
	Bind();
	if(m_nType == GL_TEXTURE_1D)
		glTexSubImage1D(GL_TEXTURE_1D, nLevel, 0, pBuffer->GetWidth(), pBuffer->GetFormat(), pBuffer->GetDataType(), pBuffer->GetBuffer());
	else
		glTexSubImage2D(GL_TEXTURE_2D, nLevel, 0, 0, pBuffer->GetWidth(), pBuffer->GetHeight(), pBuffer->GetFormat(), pBuffer->GetDataType(), pBuffer->GetBuffer());
}

void CTexture::InitCopy(int x, int y, int nWidth, int nHeight, bool bClamp)
{
	Cleanup();
	m_nType = (nHeight == 1) ? GL_TEXTURE_1D : GL_TEXTURE_2D;
	glGenTextures(1, &m_nID);
	Bind();
	glTexParameteri(m_nType, GL_TEXTURE_WRAP_S, bClamp ? GL_CLAMP : GL_REPEAT);
	glTexParameteri(m_nType, GL_TEXTURE_WRAP_T, bClamp ? GL_CLAMP : GL_REPEAT);
	glTexParameteri(m_nType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(m_nType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	if(m_nType == GL_TEXTURE_1D)
		glCopyTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, x, y, nWidth, 0);
	else
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, nWidth, nHeight, 0);
}

void CTexture::UpdateCopy(int x, int y, int nWidth, int nHeight, int nOffx, int nOffy)
{
	Bind();
	if(m_nType == GL_TEXTURE_1D)
		glCopyTexSubImage1D(GL_TEXTURE_1D, 0, nOffx, x, y, nWidth);
	else
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, nOffx, nOffy, x, y, nWidth, nHeight);
}
