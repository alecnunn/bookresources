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

#ifndef __GLUtil_h__
#define __GLUtil_h__

// Set up the GL_NV_fence extension
#ifndef GL_NV_fence
#define GL_NV_fence
#define GL_ALL_COMPLETED_NV               0x84F2
#define GL_FENCE_STATUS_NV                0x84F3
#define GL_FENCE_CONDITION_NV             0x84F4
typedef void (APIENTRY * PFNGLDELETEFENCESNVPROC) (GLsizei n, const GLuint *fences);
typedef void (APIENTRY * PFNGLGENFENCESNVPROC) (GLsizei n, GLuint *fences);
typedef GLboolean (APIENTRY * PFNGLISFENCENVPROC) (GLuint fence);
typedef GLboolean (APIENTRY * PFNGLTESTFENCENVPROC) (GLuint fence);
typedef void (APIENTRY * PFNGLGETFENCEIVNVPROC) (GLuint fence, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLFINISHFENCENVPROC) (GLuint fence);
typedef void (APIENTRY * PFNGLSETFENCENVPROC) (GLuint fence, GLenum condition);
#endif
#include "glh_genext.h"
extern int glh_init_extension(const char* extension);

#include "Texture.h"
#include "PBuffer.h"

// This must be defined before GL_NV_vertex_array_range
class CVertexArray;
class CVertex
{
// Attributes
public:
	static CVertexArray Array;
	CVector m_vPosition;		// The position of the vertex
	CVector m_vNormal;			// The normal of the vertex (used for smooth shading)
	float m_fTexCoord[4];		// The texture coordinates of the vertex

// Operations
public:
	CVertex()						{}
	CVertex(const CVertex &v)		{ *this = v; }
	void operator=(const CVertex &v)
	{
		m_vPosition = v.m_vPosition;
		m_vNormal = v.m_vNormal;
		m_fTexCoord[0] = v.m_fTexCoord[0];
		m_fTexCoord[1] = v.m_fTexCoord[1];
		m_fTexCoord[2] = v.m_fTexCoord[2];
		m_fTexCoord[3] = v.m_fTexCoord[3];
	}
	void Init(CVector &v)
	{
		m_vPosition = v;
		m_vNormal = v;
		m_fTexCoord[0] = m_fTexCoord[1] = 0;
	}
};

#define VERTEX_ARRAY_SIZE	65535	// Would use 65536, but unsigned short only goes up to 65535
class CVertexArray : public CStackedArray<CVertex, unsigned short>
{
public:
	// Constructor/destructor methods
	CVertexArray() : CStackedArray<CVertex, unsigned short>(VERTEX_ARRAY_SIZE) {}
	void UpdateElement(unsigned short nElement)	{ SetFlags(nElement, Dirty); }
};

class CGLUtil
{
// Attributes
protected:
	// Standard OpenGL members
	HDC m_hDC;
	HGLRC m_hGLRC;

	// Members to indicate whether certain OpenGL extensions are supported
	bool m_bVertexArrayRanges;
	bool m_bCompiledVertexArrays;
	bool m_bPBuffers;
	bool m_bRegisterCombiners;
	int m_nMaxTextureUnits;

	// Members for GL_NV_vertex_array_range and GL_NV_fence extensions
	CVertex *m_pArray;
	GLuint m_nFence;

	// Members for the WGL_ARB_pbuffer extension
	CPBuffer m_PBuffer;

public:
	static CGLUtil *m_pMain;

// Operations
public:
	CGLUtil();
	~CGLUtil();
	void Init();
	void Cleanup();
	void InitRenderContext(HDC hDC=NULL, HGLRC hGLRC=NULL);

	HDC GetHDC()					{ return m_hDC; }
	HGLRC GetHGLRC()				{ return m_hGLRC; }

	int GetVertexArraySize()		{ return VERTEX_ARRAY_SIZE * sizeof(CVertex); }
	bool HasVertexArrayRanges()		{ return m_bVertexArrayRanges; }
	bool HasCompiledVertexArrays()	{ return m_bCompiledVertexArrays; }
	bool HasPBuffers()				{ return m_bPBuffers; }
	bool HasRegisterCombiners()		{ return m_bRegisterCombiners; }
	int GetMaxTextureUnits()		{ return m_nMaxTextureUnits; }

	void EnableVertexArray()
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(CVertex), &m_pArray->m_vPosition);
	}
	void DisableVertexArray()
	{
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	void EnableNormalArray()
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, sizeof(CVertex), &m_pArray->m_vNormal);
	}
	void DisableNormalArray()
	{
		glDisableClientState(GL_NORMAL_ARRAY);
	}
	void EnableTextureCoordArray(int nTextureUnit=GL_TEXTURE0_ARB, int nOffset=0)
	{
		if(m_nMaxTextureUnits > 1)
			glClientActiveTextureARB(nTextureUnit);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(CVertex), &m_pArray->m_fTexCoord[nOffset]);
	}
	void DisableTextureCoordArray(int nTextureUnit=GL_TEXTURE0_ARB)
	{
		if(m_nMaxTextureUnits > 1)
			glClientActiveTextureARB(nTextureUnit);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	void BeginVertexArray();
	void EndVertexArray();

	void BeginPBuffer()				{ m_PBuffer.MakeCurrent(); }
	void EndPBuffer()				{ wglMakeCurrent(m_hDC, m_hGLRC); }

	void BeginOrtho2D(int nWidth=640, int nHeight=480)
	{
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, nWidth, 0, nHeight);
	}
	void EndOrtho2D()
	{
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
	}
};

inline CGLUtil *GLUtil()			{ return CGLUtil::m_pMain; }

#endif // __GLUtil_h__
