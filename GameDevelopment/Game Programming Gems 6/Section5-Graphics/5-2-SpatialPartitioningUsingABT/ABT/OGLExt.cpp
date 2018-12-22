#include "OGLExt.h"


/* ***************************************************************** *\
|  CONSTRUCTION / DESTRUCTION
\* ***************************************************************** */
cOGLExt::cOGLExt()
{
	m_glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffersARB");
	m_glBindBufferARB = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress("glBindBufferARB");
	m_glBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferDataARB");
	m_glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress("glDeleteBuffersARB");
	m_glMapBufferARB =	(PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB");
	m_glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBufferARB");
}


cOGLExt::~cOGLExt()
{
	
}


/* ***************************************************************** *\
|  init
|  *****************************************************************
|  Initializes all function pointers again because the constructor
|  could be too early called and wglGetProcAddress could fail.
\* ***************************************************************** */
void cOGLExt::init()
{
	if(!m_glGenBuffersARB)
		m_glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffersARB");
	if(!m_glBindBufferARB)
		m_glBindBufferARB = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress("glBindBufferARB");
	if(!m_glBufferDataARB)
		m_glBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferDataARB");
	if(!m_glDeleteBuffersARB)
		m_glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress("glDeleteBuffersARB");
	if(!m_glMapBufferARB)
		m_glMapBufferARB =	(PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB");
	if(!m_glUnmapBufferARB)
		m_glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBufferARB");
}


/* ***************************************************************** *\
|  isExtensionSupported
|  *****************************************************************
|  Returns true if the given extension is supported. 
|  Taken from http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=45
\* ***************************************************************** */
bool cOGLExt::isExtensionSupported(const char* szExt)
{
	const unsigned char *pszExtensions = NULL;
	const unsigned char *pszStart;
	unsigned char *pszWhere, *pszTerminator;

	// Extension names should not have spaces
	pszWhere = (unsigned char *) strchr( szExt, ' ' );
	if( pszWhere || *szExt == '\0' )
		return false;

	// Get Extensions String
	pszExtensions = glGetString( GL_EXTENSIONS );

	// Search The Extensions String For An Exact Copy
	pszStart = pszExtensions;
	for(;;)
	{
		pszWhere = (unsigned char *) strstr( (const char *) pszStart, szExt );
		if( !pszWhere )
			break;
		pszTerminator = pszWhere + strlen( szExt );
		if( pszWhere == pszStart || *( pszWhere - 1 ) == ' ' )
			if( *pszTerminator == ' ' || *pszTerminator == '\0' )
				return true;
		pszStart = pszTerminator;
	}
	return false;
}





/* ***************************************************************** *\
|  OpenGL extensions function wrapper
\* ***************************************************************** */
void cOGLExt::glGenBuffersARB(GLsizei n, GLuint *buffers)
{
	if(m_glGenBuffersARB)
		m_glGenBuffersARB(n, buffers);
}

void cOGLExt::glBindBufferARB(GLenum target, GLuint buffer)
{
	if(m_glBindBufferARB)
		m_glBindBufferARB(target, buffer);
}

void cOGLExt::glBufferDataARB(GLenum target, GLsizeiptrARB size, const GLvoid *data, GLenum usage)
{
	if(m_glBufferDataARB)
		m_glBufferDataARB(target, size, data, usage);
}

void cOGLExt::glDeleteBuffersARB(GLsizei n, const GLuint *buffers)
{
	if(m_glDeleteBuffersARB)
		m_glDeleteBuffersARB(n, buffers);
}

GLvoid* cOGLExt::glMapBufferARB(GLenum target, GLenum access)
{
	if(m_glMapBufferARB)
		return m_glMapBufferARB(target, access);

	return NULL;
}

GLboolean cOGLExt::glUnmapBufferARB(GLenum target)
{
	if(m_glUnmapBufferARB)
		return m_glUnmapBufferARB(target);
	return 0;
}