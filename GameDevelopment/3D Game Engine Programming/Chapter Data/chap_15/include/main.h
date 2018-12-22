// FILE: main.h 

#ifndef MAIN_H
#define MAIN_H

// D E F I N E S ///////////////////////////////////////////////////

#include "CGame.h"                   // application class

// P R O T O T Y P E S /////////////////////////////////////////////

LRESULT WINAPI MsgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
void CheckInputLength(char*, UINT, HWND);

#endif