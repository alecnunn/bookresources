// FILE: CFile.cpp

#include "include/CFile.h"



void CFile::Init(HWND hwnd) {
   static TCHAR szFilter[] = TEXT ("Pandorras Box Level (*.PBL)\0*.pbl\0")  \
                             TEXT ("Alle Dateien (*.*)\0*.*\0\0") ;
     
   m_ofn.lStructSize       = sizeof (OPENFILENAME);
   m_ofn.hwndOwner         = hwnd;
   m_ofn.hInstance         = NULL;
   m_ofn.lpstrCustomFilter = NULL;
   m_ofn.nMaxCustFilter    = 0;
   m_ofn.nFilterIndex      = 0;
   m_ofn.lpstrFile         = NULL;
   m_ofn.nMaxFile          = MAX_PATH;
   m_ofn.lpstrFileTitle    = NULL;
   m_ofn.nMaxFileTitle     = MAX_PATH;
   m_ofn.lpstrInitialDir   = NULL;
   m_ofn.lpstrTitle        = NULL;
   m_ofn.Flags             = 0;
   m_ofn.nFileOffset       = 0;
   m_ofn.nFileExtension    = 0;
   m_ofn.lCustData         = 0L;
   m_ofn.lpfnHook          = NULL;
   m_ofn.lpTemplateName    = NULL;

   SetMode(FMD_LEVEL);
   }
/*----------------------------------------------------------------*/

BOOL CFile::DlgFileOpen(HWND hwnd, PTSTR pNameCmplt, PTSTR pName) {
   m_ofn.hwndOwner      = hwnd;
   m_ofn.lpstrFile      = pNameCmplt;
   m_ofn.lpstrFileTitle = pName;
   m_ofn.Flags          = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
   return GetOpenFileName(&m_ofn) ;
   }
/*----------------------------------------------------------------*/


BOOL CFile::DlgFileSave(HWND hwnd, PTSTR pNameCmplt, PTSTR pName) {
   m_ofn.hwndOwner      = hwnd;
   m_ofn.lpstrFile      = pNameCmplt;
   m_ofn.lpstrFileTitle = pName;
   m_ofn.Flags          = OFN_OVERWRITEPROMPT;
   return GetSaveFileName(&m_ofn) ;
   }
/*----------------------------------------------------------------*/


void CFile::SetMode(FILEMODE fmd) {
   static TCHAR szFilterPBL[] = TEXT ("Pandorras Box Level (*.PBL)\0*.pbl\0");
   static TCHAR szFilterPBP[] = TEXT ("Pandorras Box Prefab (*.PBP)\0*.pbp\0");
   static TCHAR szFilterX[]   = TEXT ("X File Modell (*.X)\0*.x\0");

   switch (fmd) {
      case FMD_LEVEL:
         m_ofn.lpstrFilter = szFilterPBL;
         m_ofn.lpstrDefExt = "pbl";
         break;

      case FMD_PREFAB:
         m_ofn.lpstrFilter = szFilterPBP;
         m_ofn.lpstrDefExt = "pbp";
         break;

      case FMD_XFILE:
         m_ofn.lpstrFilter = szFilterX;
         m_ofn.lpstrDefExt = "x";
         break;
      }
   } // SetMode
/*----------------------------------------------------------------*/
