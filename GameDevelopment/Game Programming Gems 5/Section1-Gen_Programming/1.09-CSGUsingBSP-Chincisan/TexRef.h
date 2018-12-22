/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#ifndef __TEXREF_H__
#define __TEXREF_H__

#define TEX_CAPACITY    4096

//---------------------------------------------------------------------------------------
// Reference counter for Textures
class TexRef
{
public:
    TexRef(){
        memset(_texmap,0,sizeof(_texmap));
    };
    ~TexRef(){};
public:
    static UINT	GlGenTex(int x, int y, int bpp, BYTE* pBuff, DWORD flags);
    static void	RemoveTex(UINT it);
    short   _texmap[TEX_CAPACITY];    //tex and refs
};

//---------------------------------------------------------------------------------------
extern TexRef  GTexSys;

#endif // !__TEXREF_H__
