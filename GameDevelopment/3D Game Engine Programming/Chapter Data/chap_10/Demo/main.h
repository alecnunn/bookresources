// FILE: main.h 

#ifndef MAIN_H
#define MAIN_H

// D E F I N E S ///////////////////////////////////////////////////

#define IDC_hLoad    3001
#define IDC_hPlay    3002
#define IDC_hText    3003

HWND hLoad;
HWND hPlay;



// P R O T O T Y P E S /////////////////////////////////////////////

LRESULT WINAPI MsgProc(HWND, UINT, WPARAM, LPARAM);
HRESULT ProgramStartup(void);
HRESULT ProgramCleanup(void);
void    CreateGUI(void);
TCHAR*  HrToStr(HRESULT hr);

#endif