
/******************************************************************************
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.

By downloading, copying, installing or using the software you agree to this
license. If you do not agree to this license, do not download, install,
copy or use the software.

Intel Open Source License for "Improved Bones Deformation" and
"Constrained Inverse Kinematics" sample code

Copyright (c) 2002, Intel Corporation.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
	be used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR
JURISDICTION. It is each licensee's responsibility to comply with any export
regulations applicable in each licensee's jurisdiction.
******************************************************************************/

#ifndef __GPGInfoRenderOGL_h__
#define __GPGInfoRenderOGL_h__


#include "Base/GPGInfoRender.h"
#include "Base/GPGResult.h"

#ifdef MACINTOSH
#include "gl.h"
#define WINAPI 
#elif GPG_OSTYPE==GPG_WIN32
#include "windows.h"
#include "gl/gl.h"
#elif GPG_OSTYPE==GPG_UNIX
#include <GL/glx.h>
#include <GL/gl.h>
#define WINAPI
#else
#error "platform not recognized"
#endif

typedef void (WINAPI * GLBegin)(GLenum);
typedef void (WINAPI * GLBlendFunc )(GLenum, GLenum );
typedef void (WINAPI * GLCallLists ) (GLsizei , GLenum , const GLvoid *);
typedef void (WINAPI * GLColor3fv )(const GLfloat *);
typedef void (WINAPI * GLColor4fv )(const GLfloat *);
typedef void (WINAPI * GLDepthMask )(GLboolean );
typedef void (WINAPI * GLDisable )(GLenum );
typedef void (WINAPI * GLEnable )(GLenum );
typedef void (WINAPI * GLEnd )(void);
typedef GLenum (WINAPI * GLGetError ) (void);
typedef void (WINAPI * GLGetFloatv )(GLenum , GLfloat *);
typedef void (WINAPI * GLGetIntegerv ) (GLenum , GLint *);
typedef void (WINAPI * GLHint )(GLenum , GLenum );
typedef void (WINAPI * GLLightModeli)(GLenum, GLint);
typedef void (WINAPI * GLLineWidth )(GLfloat );
typedef void (WINAPI * GLListBase )(GLuint );
typedef void (WINAPI * GLNormal3fv )(const GLfloat *);
typedef void (WINAPI * GLPointSize )(GLfloat );
typedef void (WINAPI * GLPolygonMode )(GLenum , GLenum );
typedef void (WINAPI * GLPopAttrib )(void);
typedef void (WINAPI * GLPushAttrib )(GLbitfield );
typedef void (WINAPI * GLRasterPos3fv )(const GLfloat *);
typedef void (WINAPI * GLShadeModel )(GLenum );
typedef void (WINAPI * GLVertex3fv )(const GLfloat *);


/******************************************************************************
class GPGInfoRenderOGL: public GPGInfoRender

	See GPGInfoRender.h for argument descriptions

	don't forget to set the fontbase or text won't draw

******************************************************************************/
class GPGInfoRenderOGL: public GPGInfoRender
	{
	public:
				GPGInfoRenderOGL();
virtual			~GPGInfoRenderOGL();

virtual void	SetPointSize(float pixels)		{ glPointSize(pixels); };

virtual void	DrawPointArray(const GPGVector3 *vertex,long vertices,
								bool multicolor,const GPGVector3 *color);

virtual void	DrawLineArray(const GPGVector3 *vertex,long vertices,
						bool strip,bool multicolor,const GPGVector3 *color,
						F32 linewidth,bool antialias=false);

virtual	void	DrawTriangleArray(const GPGVector3 *vertex,
						const GPGVector3 *normal,long vertices,
						bool strip,bool multicolor,const GPGVector3 *color,
						F32 alpha);

virtual	void	DrawAlignedText(const GPGVector3 &location,const char *text,
													const GPGVector3 &color);

		void	SetFontBase(long set)	{ fontbase=set; };

	private:
		long	fontbase;

		GPGRESULT		m_iInitialized;
		GLBegin			m_glBegin;
		GLBlendFunc		m_glBlendFunc;
		GLCallLists		m_glCallLists;
		GLColor3fv		m_glColor3fv;
		GLColor4fv		m_glColor4fv;
		GLDepthMask		m_glDepthMask;
		GLDisable		m_glDisable;
		GLEnable		m_glEnable;
		GLEnd			m_glEnd;
		GLGetError		m_glGetError;
		GLGetFloatv		m_glGetFloatv;
		GLGetIntegerv	m_glGetIntegerv;
		GLHint			m_glHint;
		GLLightModeli	m_glLightModeli;
		GLLineWidth		m_glLineWidth;
		GLListBase		m_glListBase;
		GLNormal3fv		m_glNormal3fv;
		GLPointSize		m_glPointSize;
		GLPolygonMode	m_glPolygonMode;
		GLPopAttrib		m_glPopAttrib;
		GLPushAttrib	m_glPushAttrib;
		GLRasterPos3fv	m_glRasterPos3fv;
		GLShadeModel	m_glShadeModel;
		GLVertex3fv		m_glVertex3fv;
	};

#endif /* __GPGInfoRenderOGL_h__ */
