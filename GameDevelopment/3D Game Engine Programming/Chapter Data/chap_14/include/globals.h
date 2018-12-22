// File: Globals.h

#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include "ZFXRenderer.h"                  // our render interface
#include "ZFX.h"                          // return values and stuff
#include "CFile.h"                        // File class
#include "CSector.h"                      // sector class
#include "CLevel.h"                       // level class


// P R O T O T Y P E S /////////////////////////////////////////////

typedef struct GLOBALS_TYPE {
   HINSTANCE          hInst;
   HWND               hWnd;
   HWND               hWnd3D[4];
   HWND               hGUI[100];
   HWND               hTexDlg;
   bool               bChanged;
   bool               bNeedRedraw[4];
   FILE              *pLog;
   CFile              FileDlg;
   CLevel             Level;
   CSelectionBuffer   SelBuf;
   char               Dir[MAX_PATH];
   } GLOBALS;
/*----------------------------------------------------------------*/


#endif