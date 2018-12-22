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
#define GLH_EXT_SINGLE_FILE
#include "GLUtil.h"


CVertexArray CVertex::Array;
CGLUtil g_glUtil;
CGLUtil *CGLUtil::m_pMain = &g_glUtil;

CGLUtil::CGLUtil()
{
	// Start by clearing out all the member variables
	m_bVertexArrayRanges = false;
	m_bCompiledVertexArrays = false;
	m_bPBuffers = false;
	m_bRegisterCombiners = false;
	m_nMaxTextureUnits = false;
	m_pArray = NULL;
	m_nFence = 0;
}

CGLUtil::~CGLUtil()
{
}

void CGLUtil::Init()
{
	// Start by storing the current HDC and HGLRC
	m_hDC = wglGetCurrentDC();
	m_hGLRC = wglGetCurrentContext();
	
	// Next initialize the OpenGL extensions we want to use
#ifndef _DEBUG	// Don't use the VAR when debugging because GPF's can force a reboot
	if(glh_init_extension("GL_NV_vertex_array_range") && glh_init_extension("GL_NV_fence"))
	{
		int nSize = GetVertexArraySize();
		m_pArray = (CVertex *)wglAllocateMemoryNV(nSize, 0, 0, 1.0f);
		if(!m_pArray)
			m_pArray = (CVertex *)wglAllocateMemoryNV(nSize, 0, 0, 0.5f);
		if(m_pArray)
		{
			glGenFencesNV(1, &m_nFence);
			m_bVertexArrayRanges = true;
		}
	}
#endif
	if(!m_bVertexArrayRanges)
		m_pArray = CVertex::Array.GetBuffer();

	if(glh_init_extension("GL_EXT_compiled_vertex_array"))
		m_bCompiledVertexArrays = true;

	if(glh_init_extension("GL_NV_register_combiners"))
		m_bRegisterCombiners = true;

	if(glh_init_extension("GL_ARB_multitexture"))
		glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &m_nMaxTextureUnits);

	if(glh_init_extension("WGL_ARB_pixel_format") && glh_init_extension("WGL_ARB_pbuffer"))
	{
		if(m_PBuffer.Init(512, 512))
		{
			InitRenderContext(m_PBuffer.GetHDC(), m_PBuffer.GetHGLRC());
			m_bPBuffers = true;
		}
	}

	// Finally, initialize the default rendering context
	InitRenderContext(m_hDC, m_hGLRC);
}

void CGLUtil::Cleanup()
{
	if(m_bPBuffers)
		m_PBuffer.Cleanup();
	if(m_bVertexArrayRanges)
	{
		glDeleteFencesNV(1, &m_nFence);
		wglFreeMemoryNV(m_pArray);
	}
}

void CGLUtil::InitRenderContext(HDC hDC, HGLRC hGLRC)
{
	wglMakeCurrent(hDC, hGLRC);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, CVector4(0.0f));

	if(m_bVertexArrayRanges)
	{
		glVertexArrayRangeNV(GetVertexArraySize(), m_pArray);
		glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}
	EnableVertexArray();
	EnableNormalArray();

	wglMakeCurrent(m_hDC, m_hGLRC);
}

void CGLUtil::BeginVertexArray()
{
	if(m_bVertexArrayRanges)
	{
		glFinishFenceNV(m_nFence);
		for(unsigned short i=0; i<VERTEX_ARRAY_SIZE; i++)
		{
			if(CVertex::Array.GetFlags(i) & CVertexArray::Dirty)
			{
				m_pArray[i] = *CVertex::Array[i];
				CVertex::Array.ClearFlags(i, CVertexArray::Dirty);
			}
		}
	}
	if(m_bCompiledVertexArrays)
		glLockArraysEXT(0, VERTEX_ARRAY_SIZE);
}

void CGLUtil::EndVertexArray()
{
	if(m_bCompiledVertexArrays)
		glUnlockArraysEXT();
	if(m_bVertexArrayRanges)
		glSetFenceNV(m_nFence, GL_ALL_COMPLETED_NV);
}

