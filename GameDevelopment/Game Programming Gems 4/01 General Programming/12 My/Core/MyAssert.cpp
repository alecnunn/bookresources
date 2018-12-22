//--------------------------------------------------------------------------------------------------------------------//
//                                                ASSERT CUSTOM MACRO                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//                                                                                                                    //
// based on item 1.12 of the Game Programming Gems book                                                               //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "MyAssert.h"

#ifdef WIN32
  #include <windows.h>
#endif

#include  <stdio.h>
#include  <stdlib.h>

namespace GPG4
{

//----------------------------------------------- DisplayAssert ------------------------------------------------------//
// display message & choose to continue/abort or debug
// in : text,file,line,ignore always
// out: true means debug
// rem: I have avoided to use a custom dialog because it should be included in any project using MyAssert
//--------------------------------------------------------------------------------------------------------------------//

bool DisplayAssert(char* pszText,char* pszFile,int iLine,bool* pboIgnore)
  {
#ifdef WIN32

  char szProgName[MAX_PATH];
  char szMsg     [1024];

  if(!GetModuleFileName(NULL,szProgName,MAX_PATH)) strcpy(szProgName,"<program name unknown>");
  sprintf(szMsg,"Assertion failed !\n%s\n\nProgram: %s\nFile: %s\nLine: %u\n\n(Press Retry to debug the application)\n",
          pszText,szProgName,pszFile,iLine);

  // copy to clipboard

  if(OpenClipboard(NULL))
    { 
    HGLOBAL hMem = GlobalAlloc(GHND|GMEM_DDESHARE,strlen(szMsg)+1);

    if(hMem)
      {
      char* pMem = (char*)GlobalLock(hMem);
      strcpy(pMem,szMsg);
      GlobalUnlock(hMem);
      EmptyClipboard();
      SetClipboardData(CF_TEXT,hMem);
      }

    CloseClipboard();
    }

  // display message

  int iRes = ::MessageBox(NULL,szMsg,"MY_ASSERT",MB_ABORTRETRYIGNORE|MB_ICONHAND|MB_SETFOREGROUND|MB_TASKMODAL);
  if(iRes == IDABORT) _exit(3);
  if(iRes == IDRETRY) return true;

  iRes = ::MessageBox(NULL,"always ignore this assert ?","MY_ASSERT",MB_YESNO|MB_ICONHAND|MB_SETFOREGROUND|MB_TASKMODAL);
  if(iRes == IDYES) *pboIgnore = true;

#endif // WIN32

  return false;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
