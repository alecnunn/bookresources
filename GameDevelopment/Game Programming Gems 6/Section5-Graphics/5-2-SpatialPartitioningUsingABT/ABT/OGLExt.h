/*******************************************************************\
| OGLExt.h
|
|--------------------------------------------------------------------
| Copyright 2004/2005. Martin Fleisz.
| All rights reserved.
|
|--------------------------------------------------------------------
| CREATED:		2005/8/10
| AUTHOR:		Martin Fleisz
|
|--------------------------------------------------------------------
| DESCRIPTION:
| 
| Helper class for OpenGL extension functions
\********************************************************************/
#ifndef __OGLEXT_H
#define __OGLEXT_H

#include <Windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "glext.h"


class cOGLExt
{
	public:
		cOGLExt();
		~cOGLExt();

		bool isExtensionSupported(const char *szExt);

		void init();

		void glGenBuffersARB(GLsizei n, GLuint *buffers);
		void glBindBufferARB(GLenum target, GLuint buffer);
		void glBufferDataARB(GLenum target, GLsizeiptrARB size, const GLvoid *data, GLenum usage);
		void glDeleteBuffersARB(GLsizei n, const GLuint *buffers);
		GLvoid*		glMapBufferARB(GLenum target, GLenum access);
		GLboolean	glUnmapBufferARB(GLenum target);


	private:
		
		PFNGLGENBUFFERSARBPROC		m_glGenBuffersARB;		// VBO Name Generation Procedure
		PFNGLBINDBUFFERARBPROC		m_glBindBufferARB;		// VBO Bind Procedure
		PFNGLBUFFERDATAARBPROC		m_glBufferDataARB;		// VBO Data Loading Procedure
		PFNGLDELETEBUFFERSARBPROC	m_glDeleteBuffersARB;	// VBO Deletion Procedure
		PFNGLMAPBUFFERARBPROC		m_glMapBufferARB;
		PFNGLUNMAPBUFFERARBPROC		m_glUnmapBufferARB;

};





#endif // __OGLEXT_H