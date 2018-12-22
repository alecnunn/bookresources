/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#include "stdafx.h"
#include "csg_bsp.h"
#include "TexRef.h"
#include "z_ed3View.h"

//---------------------------------------------------------------------------------------
// Texture system. 
TexRef  GTexSys;

//---------------------------------------------------------------------------------------
void	TexRef::RemoveTex(UINT it)
{
    wglMakeCurrent(z_ed3View::P3DView->_hdc, z_ed3View::P3DView->_hhRc);
    glDeleteTextures(1,&it);
}
//---------------------------------------------------------------------------------------
UINT	TexRef::GlGenTex(int x, int y, int bpp, BYTE* pBuff, DWORD flags)
{
    wglMakeCurrent(z_ed3View::P3DView->_hdc, z_ed3View::P3DView->_hhRc);

    UINT iTex=0;
    glGenTextures(1,&iTex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, iTex);
    switch(bpp)
    {
    case 1:
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_LUMINANCE, x, y, GL_LUMINANCE, GL_UNSIGNED_BYTE, pBuff);
        break;
    case 3:
        glTexImage2D(GL_TEXTURE_2D,0,3,x,y,0,GL_RGB,GL_UNSIGNED_BYTE,pBuff);
        break;
    case 4:
        glTexImage2D(GL_TEXTURE_2D,0,4,x,y,0,GL_RGBA,GL_UNSIGNED_BYTE,pBuff);
        break;
    }

    switch(bpp)
    {
    case 1:
        gluBuild2DMipmaps(GL_TEXTURE_2D,1,x,y,GL_LUMINANCE,GL_UNSIGNED_BYTE,pBuff);
        break;
    case 3:
        gluBuild2DMipmaps(GL_TEXTURE_2D,3,x,y,GL_RGB,GL_UNSIGNED_BYTE,pBuff);
        break;
    case 4:
        gluBuild2DMipmaps(GL_TEXTURE_2D,4,x,y,GL_RGBA,GL_UNSIGNED_BYTE,pBuff);
        break;
    }
    return iTex;
}

