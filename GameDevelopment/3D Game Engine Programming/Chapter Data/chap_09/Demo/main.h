// FILE: main.h 

#ifndef MAIN_H
#define MAIN_H

// D E F I N E S ///////////////////////////////////////////////////

#define IDC_hMousePos    3001
#define IDC_hJoyPos      3002
#define IDC_hJoyBtn0     3003
#define IDC_hJoyBtn1     3004
#define IDC_hJoyBtn2     3005
#define IDC_hJoyBtn3     3006
#define IDC_hDummy1      3007
#define IDC_hMouseBtn0   3008
#define IDC_hMouseBtn1   3009
#define IDC_hJoytext     3010
#define IDC_hDummy2      3011
#define IDC_hText1       3012
#define IDC_hText2       3013
#define IDC_hText3       3014
#define IDC_hText4       3015
#define IDC_hText5       3016


HWND hMousePos;
HWND hJoyPos;
HWND hJoyBtn0;
HWND hJoyBtn1;
HWND hJoyBtn2;
HWND hJoyBtn3;
HWND hMouseBtn0;
HWND hMouseBtn1;
HWND hMouseBtn2;



// P R O T O T Y P E S /////////////////////////////////////////////

LRESULT WINAPI MsgProc(HWND, UINT, WPARAM, LPARAM);
HRESULT ProgramStartup(void);
HRESULT ProgramCleanup(void);
HRESULT ProgramTick(void);
void    CreateGUI(void);
TCHAR*  HrToStr(HRESULT hr);

#endif