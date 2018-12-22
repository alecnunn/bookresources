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
#include "PBuffer.h"


bool CPBuffer::Init(int nWidth, int nHeight, int nFlags)
{
	Cleanup();
	m_nFlags = nFlags;

	// Temporarily store the current device and rendering contexts
	HDC hDC = wglGetCurrentDC();
	HGLRC hGLRC = wglGetCurrentContext();

	// Set up floating point attributes for pbuffer
	int i;
	float fAttributes[2*MAX_ATTRIBS];
	for(i=0; i<2*MAX_ATTRIBS; i++)
		fAttributes[i] = 0;

	// Set up integer point attributes for pbuffer
	int iAttributes[2*MAX_ATTRIBS];
	for(i=0; i<2*MAX_ATTRIBS; i++)
		iAttributes[i] = 0;
	i=0;
	iAttributes[i++] = WGL_DRAW_TO_PBUFFER_ARB;
	iAttributes[i++] = true;
	iAttributes[i++] = WGL_PIXEL_TYPE_ARB;
	iAttributes[i++] = WGL_TYPE_RGBA_ARB;
	if(nFlags & DepthBuffer)
	{
		iAttributes[i++] = WGL_DEPTH_BITS_ARB;
		iAttributes[i++] = 1;
	}
	if(nFlags & StencilBuffer)
	{
		iAttributes[i++] = WGL_STENCIL_BITS_ARB;
		iAttributes[i++] = 1;
	}
	iAttributes[i++] = WGL_SUPPORT_OPENGL_ARB;
	iAttributes[i++] = true;

	int pFormat[MAX_PFORMATS];
	unsigned int nFormats;
	if(wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, MAX_PFORMATS, pFormat, &nFormats) && nFormats > 0)
	{
#ifdef _DEBUG
		int nAttributes[MAX_ATTRIBS] = {WGL_PIXEL_TYPE_ARB, WGL_COLOR_BITS_ARB, WGL_RED_BITS_ARB, WGL_GREEN_BITS_ARB, WGL_BLUE_BITS_ARB, WGL_ALPHA_BITS_ARB, WGL_DEPTH_BITS_ARB, WGL_STENCIL_BITS_ARB, WGL_ACCUM_BITS_ARB, WGL_DOUBLE_BUFFER_ARB, WGL_SUPPORT_OPENGL_ARB, WGL_ACCELERATION_ARB};
		int nValues[MAX_ATTRIBS] = {false, false, false, false, false, false, false, false, false, false, false, false};
		wglGetPixelFormatAttribivARB(hDC, pFormat[0], 0, 12, nAttributes, nValues);
		TRACE("PixelFormat %d:", pFormat[0]);
		for(i=0; i<12; i++)
		{
			if(nAttributes[i] == WGL_COLOR_BITS_ARB)
				TRACE("  Color bits =   %d", nValues[i]);
			if(nAttributes[i] == WGL_RED_BITS_ARB)
				TRACE("    red          %d", nValues[i]);
			if(nAttributes[i] == WGL_GREEN_BITS_ARB)
				TRACE("    green        %d", nValues[i]);
			if(nAttributes[i] == WGL_BLUE_BITS_ARB)
				TRACE("    blue         %d", nValues[i]);
			if(nAttributes[i] == WGL_ALPHA_BITS_ARB)
				TRACE("    alpha        %d", nValues[i]);
			if(nAttributes[i] == WGL_DEPTH_BITS_ARB)
				TRACE("  Depth bits =   %d", nValues[i]);
			if(nAttributes[i] == WGL_STENCIL_BITS_ARB)
				TRACE("  Stencil bits = %d", nValues[i]);
			if(nAttributes[i] == WGL_ACCELERATION_ARB)
				TRACE("  Acceleration = %s", nValues[i] == WGL_FULL_ACCELERATION_ARB ? "full" : "???");
		}
		TRACE("");
#endif

		iAttributes[0] = 0;
		m_hBuffer = wglCreatePbufferARB(hDC, pFormat[0], nWidth, nHeight, iAttributes);
		m_hDC = wglGetPbufferDCARB(m_hBuffer);
		m_hGLRC = wglCreateContext(m_hDC);
		wglQueryPbufferARB(m_hBuffer, WGL_PBUFFER_WIDTH_ARB, &m_nWidth);
		wglQueryPbufferARB(m_hBuffer, WGL_PBUFFER_HEIGHT_ARB, &m_nHeight);
		wglShareLists(hGLRC, m_hGLRC);
		return true;
	}
	return false;
}

void CPBuffer::Cleanup()
{
	if(m_hBuffer)
	{
		wglDeleteContext(m_hGLRC);
		wglReleasePbufferDCARB(m_hBuffer, m_hDC);
		wglDestroyPbufferARB(m_hBuffer);
		m_hBuffer = NULL;
	}
}

void CPBuffer::HandleModeSwitch()
{
	if(m_hBuffer)
	{
		int nLost = 0;
		wglQueryPbufferARB(m_hBuffer, WGL_PBUFFER_LOST_ARB, &nLost);
		if(nLost)
		{
			Cleanup();
			Init(m_nWidth, m_nHeight);
		}
	}
}
