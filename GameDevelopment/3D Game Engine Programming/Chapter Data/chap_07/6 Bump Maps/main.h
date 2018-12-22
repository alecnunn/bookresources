// FILE: main.h 

#ifndef MAIN_H
#define MAIN_H

// D E F I N E S ///////////////////////////////////////////////////

typedef struct TANGENTVERTEX_TYPE {
   ZFXVector  vcPos;  // vertex position
   ZFXVector  vcN;    // vertex normal
   float      fu;     // vertex u coordinate
   float      fv;     // vertex v coordinate
   ZFXVector  vcU;    // new tangent vector
   ZFXVector  vcV;    // new binormale vector
   ZFXVector  vcUxV;  // new tangent normal
   } TANGENTVERTEX;


// P R O T O T Y P E S /////////////////////////////////////////////

LRESULT   WINAPI MsgProc(HWND, UINT, WPARAM, LPARAM);
HRESULT   ProgramStartup(char *chAPI);
HRESULT   ProgramCleanup(void);
HRESULT   ProgramTick(void);
HRESULT   RenderLight(float,float,float);
HRESULT   Render(ZFXVector);
HRESULT   BuildAndSetShader(void);
HRESULT   BuildGeometry(void);
void      CreateRect(ZFXVector,float,float,float,TVERTEX*,WORD*,bool);
void      CreateCube(ZFXVector,float,float,float,TVERTEX*,WORD*,bool);
void      CalcTangentSpace(TANGENTVERTEX*,TANGENTVERTEX*,TANGENTVERTEX*);
TCHAR*    HrToStr(HRESULT hr);

#endif