// FILE: main.h 

#ifndef MAIN_H
#define MAIN_H

// D E F I N E S ///////////////////////////////////////////////////


// P R O T O T Y P E S /////////////////////////////////////////////

LRESULT   WINAPI MsgProc(HWND, UINT, WPARAM, LPARAM);
HRESULT   ProgramStartup(char *chAPI);
HRESULT   ProgramCleanup(void);
HRESULT   ProgramTick(void);
HRESULT   RenderLight(float,float,float);
HRESULT   Render(int);
HRESULT   BuildAndSetShader(void);
HRESULT   BuildGeometry(void);
void      CreateCube(ZFXVector,float,float,float,TVERTEX*,WORD*,bool);
ZFXMatrix CalcTransAttenNoRot(const ZFXVector&,float); 
TCHAR*    HrToStr(HRESULT hr);

#endif