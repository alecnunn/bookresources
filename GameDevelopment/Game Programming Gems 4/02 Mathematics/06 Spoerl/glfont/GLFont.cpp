#include	<windows.h>
#include	<gl\gl.h>
#include	"GLFont.h"

GLFont::GLFont()
{
	fWidth = 16;
	fHeight = 16;
	fSpacing = 10; 
	fxCount = 16;
	fyCount = 16;
	fdWidth = 640;
	fdHeight = 480;
	fStartPos = 0;
}

GLFont::~GLFont()
{
	 KillFont();
}

GLvoid GLFont::KillFont(GLvoid)												// Delete The Font From Memory
{
	glDeleteLists(fBase,fxCount * fyCount);									// Delete Allocated Display Lists
}


GLvoid GLFont::Print(GLint x, GLint y, const char *string)						// Where The Printing Happens
{
	glBindTexture(GL_TEXTURE_2D, fTexture[0]);								// Select Our Font Texture
	glDisable(GL_DEPTH_TEST);												// Disables Depth Testing
	glMatrixMode(GL_PROJECTION);											// Select The Projection Matrix
	glPushMatrix();															// Store The Projection Matrix
	glLoadIdentity();														// Reset The Projection Matrix
	glOrtho(0,fdWidth,0,fdHeight,-1,1);										// Set Up An Ortho Screen
	glMatrixMode(GL_MODELVIEW);												// Select The Modelview Matrix
	glPushMatrix();															// Store The Modelview Matrix
	glLoadIdentity();														// Reset The Modelview Matrix
	glTranslated(x,fdHeight - y,0);											// Position The Text (0,0 - Bottom Left)
	glListBase(fBase - fStartPos);											// Choose The Font Set (0 or 1)
	glCallLists(strlen(string),GL_BYTE,string);								// Write The Text To The Screen
	glMatrixMode(GL_PROJECTION);											// Select The Projection Matrix
	glPopMatrix();															// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);												// Select The Modelview Matrix
	glPopMatrix();															// Restore The Old Projection Matrix
	glEnable(GL_DEPTH_TEST);												// Enables Depth Testing
}

void GLFont::SetBase(int Base)
{
	 fStartPos = Base;
}

void GLFont::SetTexture(GLuint Tex, int xCount, int yCount)
{
	fTexture[0] = Tex;
	fxCount     = xCount;
	fyCount     = yCount;
}

void GLFont::SetFontProperties(int Width, int Height, int Spacing)
{
	fWidth      = Width;
	fHeight     = Height;
	fSpacing    = Spacing;
}

void GLFont::SetDisplayMode(int dWidth, int dHeight)
{
	fdWidth  = dWidth;
	fdHeight = dHeight;
}


GLvoid GLFont::BuildFont()													// Build Our Font Display List
{
	int     loop;
	float	cx;																// Holds Our X Character Coord
	float	cy;																// Holds Our Y Character Coord
	float   cwx;															// CharWidth in texture units
	float   cwy;															// CharHeight in texture units

	cwx         = (1.0f / 256.0f) * fWidth;
	cwy         = (1.0f / 256.0f) * fHeight;
	fBase=glGenLists(fxCount * fyCount);									// Creating Display Lists
	glBindTexture(GL_TEXTURE_2D, fTexture[0]);								// Select Our Font Texture
	for (loop=0; loop<(fxCount * fyCount); loop++)							// Loop Through All Lists
	{
		cx=float(loop%fxCount) * cwx;										// X Position Of Current Character
		cy=float(loop/fyCount) * cwy;										// Y Position Of Current Character

		glNewList(fBase + loop,GL_COMPILE);									// Start Building A List
			glBegin(GL_QUADS);												// Use A Quad For Each Character
				glTexCoord2f(cx,1-cy-cwy);									// Texture Coord (Bottom Left)
				glVertex2i(0,0);											// Vertex Coord (Bottom Left)
				glTexCoord2f(cx+cwx,1-cy-cwy);								// Texture Coord (Bottom Right)
				glVertex2i(fWidth - 1,0);									// Vertex Coord (Bottom Right)
				glTexCoord2f(cx+cwx,1-cy);									// Texture Coord (Top Right)
				glVertex2i(fWidth - 1,fHeight -1);							// Vertex Coord (Top Right)
				glTexCoord2f(cx,1-cy);										// Texture Coord (Top Left)
				glVertex2i(0,fHeight -1);									// Vertex Coord (Top Left)
			glEnd();														// Done Building Our Quad (Character)
			glTranslated(fSpacing,0,0);										// Move To The Right Of The Character
		glEndList();														// Done Building The Display List
	}																		// Loop Until All Are Built
}
