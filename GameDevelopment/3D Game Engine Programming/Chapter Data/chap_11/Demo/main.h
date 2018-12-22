// FILE: main.h 

#ifndef MAIN_H
#define MAIN_H

// D E F I N E S ///////////////////////////////////////////////////

#define BTN(a,x,y,xd,yd,id) CreateWindow("BUTTON",a,dwChild|BS_PUSHBUTTON,x,y,xd,yd,g_hWnd,(HMENU)id,g_hInst,NULL);
#define LBX(a,x,y,xd,yd,id) CreateWindowEx(0,"LISTBOX",a,dwChild|LBS_DISABLENOSCROLL|LBS_HASSTRINGS|LBS_STANDARD^LBS_SORT,x,y,xd,yd,g_hWnd,(HMENU)id,g_hInst,NULL);
#define EDT(a,x,y,xd,yd,id) CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT",a,dwChild|WS_BORDER,x,y,xd,yd,g_hWnd,(HMENU)id,g_hInst,NULL);
#define STC(a,x,y,xd,yd,id) CreateWindow("STATIC",a,dwChild,x,y,xd,yd,g_hWnd,(HMENU)id,g_hInst,NULL);
#define MSGBX(a) MessageBox(NULL,a,"ZFXEngine report",MB_OK|MB_ICONINFORMATION|MB_TASKMODAL)

#define IDC_hBtn        3001
#define IDC_hText       3002
#define IDC_hLabel      3003
#define IDC_hLbx        3004

HWND hBtn;
HWND hText;
HWND hLabel;
HWND hLbx;




// P R O T O T Y P E S /////////////////////////////////////////////

LRESULT WINAPI MsgProc(HWND, UINT, WPARAM, LPARAM);
HRESULT ProgramStartup(ZFXNETMODE type);
HRESULT ProgramCleanup(void);
void    CreateGUI(void);
void    CheckInputLength(char*,UINT,HWND);
BOOL CALLBACK DlgProc(HWND,UINT,WPARAM,LPARAM);
TCHAR*  HrToStr(HRESULT hr);

#endif