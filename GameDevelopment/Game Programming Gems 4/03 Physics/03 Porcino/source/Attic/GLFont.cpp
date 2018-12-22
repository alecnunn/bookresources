
#include <cstring>
#include "glfont.h"

extern HDC g_HDC;

GLFont::GLFont(char* pFontName, int size) : m_GLlist(0) 
{
	int charset = (stricmp(pFontName, "symbol") != 0) ? SYMBOL_CHARSET : ANSI_CHARSET;

	HFONT hFont = CreateFont(size, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		charset, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, pFontName);

	if (hFont != 0) {
		m_GLlist = glGenLists(96);		// Create space for 96 characters.
		SelectObject(g_HDC, hFont);
		wglUseFontBitmaps(g_HDC, 32, 96, m_GLlist);
	}
}

GLFont::~GLFont() 
{
	if (m_GLlist) {
		glDeleteLists(m_GLlist, 96);
	}
}

void GLFont::Print(char* pString)
{
	if (m_GLlist != 0 && pString != 0) {
		if (strlen(pString) > 0) {
			glPushAttrib(GL_LIST_BIT);											// we are going to move list base, so preserve it
			glListBase(m_GLlist - 32);
			glCallLists((GLsizei) strlen(pString), GL_UNSIGNED_BYTE, pString);
			glPopAttrib();														// restore list base
		}
	}
}
