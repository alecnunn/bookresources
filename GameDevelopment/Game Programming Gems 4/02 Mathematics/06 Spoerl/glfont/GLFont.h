#ifndef		GLFONT_H_INCLUDED
#define		GLFONT_H_INCLUDED

#include	<gl\gl.h>

class GLFont
{

private:

	GLuint fTexture[1];
	GLuint fBase;
	int    fWidth;
	int    fHeight;
	int    fSpacing; 
	int    fStartPos;
	int    fxCount;
	int    fyCount;
	int    fdWidth;
	int    fdHeight;

public:

	GLFont();
	~GLFont();

	void SetTexture(GLuint Tex, int xCount, int yCount);
	void SetFontProperties(int Width, int Height, int Spacing);
	void SetDisplayMode(int dWidth, int dHeight);
	void SetBase(int Base);

	GLvoid KillFont(GLvoid);
	GLvoid BuildFont();
	GLvoid Print(GLint x, GLint y, const char *string);

};

#endif	// GLFONT_H_INCLUDED
