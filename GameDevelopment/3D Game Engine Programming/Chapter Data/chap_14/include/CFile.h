// FILE: CFile.h 

#ifndef CFILE_H
#define CFILE_H

#include <windows.h>


typedef enum FILEMODE_TYPE {
   FMD_LEVEL,
   FMD_PREFAB,
   FMD_XFILE
   } FILEMODE;


// P R O T O T Y P E S /////////////////////////////////////////////

class CFile {
   public:
      CFile()      { }
      ~CFile(void) { }

      void Init(HWND hwnd);

      BOOL DlgFileOpen(HWND hwnd, PTSTR pNameCmplt, PTSTR pName);
      BOOL DlgFileSave(HWND hwnd, PTSTR pNameCmplt, PTSTR pName);

      void SetMode(FILEMODE fmd);

   private:
      OPENFILENAME m_ofn;

      void LoadFile(void);
      void SaveFile(void);
   }; // class
/*----------------------------------------------------------------*/

#endif