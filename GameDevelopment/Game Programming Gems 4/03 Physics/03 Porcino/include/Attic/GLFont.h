
#ifndef _GLFONT_H_
#define _GLFONT_H_

// for a tutorial on fonts in Windows under OpenGL, cf www.UltimateGameProgramming.com

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif
#if defined(__APPLE__) && defined(__MACH__)
	#include <OpenGL/gl.h>		// Header File For The OpenGL32 Library
	#include <OpenGL/glu.h>	// Header File For The GLu32 Library
#else
	#include <GL/gl.h>		// Header File For The OpenGL32 Library
	#include <GL/glu.h>	// Header File For The GLu32 Library
#endif

/** class GLFont
*/

class GLFont {
public:
	GLFont(char* pFontName, int Size);
	~GLFont();

	void Print(char* pString);

private:
	GLuint m_GLlist;
};

#endif
