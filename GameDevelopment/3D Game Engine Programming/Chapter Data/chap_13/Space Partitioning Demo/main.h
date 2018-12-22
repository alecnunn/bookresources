// FILE: main.h 

#ifndef MAIN_H
#define MAIN_H

// D E F I N E S ///////////////////////////////////////////////////




// P R O T O T Y P E S /////////////////////////////////////////////

LRESULT WINAPI MsgProc(HWND, UINT, WPARAM, LPARAM);
HRESULT ProgramStartup(char *chAPI);
HRESULT ProgramCleanup(void);
HRESULT ProgramTick(void);
bool    LoadLevel(const char*);
void    LoadPolygon(FILE*, ZFXPolygon*);
void    RenderPolylist(ZFXPolylist&,bool,const ZFXCOLOR*);
TCHAR*  HrToStr(HRESULT hr);

#endif