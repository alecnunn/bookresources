// FILE: main.h 

#ifndef MAIN_H
#define MAIN_H

// D E F I N E S ///////////////////////////////////////////////////

#include "resources/resource.h"             // menu header
#include "gui.h"                            // gui element id's
#include "ComboBoxHelper.h"                 // helper functions
#include "CFile.h"                          // File class
#include "CToolbar.h"                       // toolbar class
#include "CSector.h"                        // sector class
#include "CLevel.h"                         // level class
#include "Globals.h"                        // GLOBALS struct
#include "DlgCallbacks.h"                   // dialog callbacks
#include "CGeometry.h"                      // geometry stuff
#include "CApplication.h"                   // application class



// P R O T O T Y P E S /////////////////////////////////////////////

LRESULT WINAPI MsgProc(HWND, UINT, WPARAM, LPARAM);
HRESULT ProgramTick(void);
LRESULT ProgramProc(HWND, WPARAM, LPARAM);



#endif