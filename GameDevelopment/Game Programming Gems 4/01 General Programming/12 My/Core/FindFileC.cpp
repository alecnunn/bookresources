//--------------------------------------------------------------------------------------------------------------------//
//                                                FIND FILE/DIR INTERFACE                                             //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//                                                                                                                    //
//           recursive mode (allows to look for files in a subtree rather than a single directory)                    //
//                                                                                                                    //
//           - in normal mode : simple findfirst/findnext/findclose                                                   //
//           - in recursive mode :                                                                                    //
//             . the starting directory is searched for files matching the spec (1st param of FindFirst)              //
//             . then it is searched for subdirs ("*.*" & IsDirectory & !IsDots)                                      //
//             . each subdir is searched for matching files and then subdirs, and so on ...                           //
//                                                                                                                    //
//  example of use :                                                                                                  //
//                                                                                                                    //
//  GPG4::FindFile Finder;                                                                                            //
//                                                                                                                    //
//  if(Finder.FindFirst("D:\\Fairy\\source\\*.inl",true))                                                             //
//    {                                                                                                               //
//    do                                                                                                              //
//      {                                                                                                             //
//      CString strFile = Finder.GetFileName();                                                                       //
//      CString strFull = Finder.GetFilePath();                                                                       //
//      u32     u32Size = Finder.GetFileSize();                                                                       //
//      }                                                                                                             //
//    while(Finder.FindNext());                                                                                       //
//                                                                                                                    //
//    Finder.FindClose();                                                                                             //
//    }                                                                                                               //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "FindFileC.h"
#ifdef    _DEBUG
#include  "FindFileC.inl"
#endif

#include  <direct.h>

#include  "StringC.h"

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                OPERATIONS                                                          //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- FindFirst ----------------------------------------------------------//
// open search
// in : file spec,recurse flag
// out: file found corresponding to spec ?
//--------------------------------------------------------------------------------------------------------------------//

bool CFindFileC::FindFirst(const char* pszName,const bool boRecurse)
  {
  if(m_boInFind) if(!FindClose()) return false;

  m_s32Handle = _findfirst(pszName,&m_FindData);
  if(m_s32Handle != -1) m_boInFind = true;

  m_boRecurse = boRecurse;
  m_pSubLevel = NULL;

  // get root path & file spec

  char szDrive[_MAX_DRIVE];
  char szDir  [_MAX_DIR];
  char szName [_MAX_FNAME];
  char szExt  [_MAX_EXT];
  _splitpath(pszName,szDrive,szDir,szName,szExt);

  char szCurr [_MAX_PATH];
  char szDriv2[_MAX_DRIVE];
  char szDir2 [_MAX_DIR];
  char szName2[_MAX_FNAME];
  char szExt2 [_MAX_EXT];
  _getcwd(szCurr,_MAX_PATH);
  _splitpath(szCurr,szDriv2,szDir2,szName2,szExt2);

  if(!szDrive[0])
    {                                                       // no drive specified: given path is relative
    String::StrCpy(szDrive,szDriv2);
    String::StrCat(szDir2, szName2);
    String::StrCat(szDir2, "\\");
    String::StrCat(szDir2, szDir);
    String::StrCpy(szDir,  szDir2);
    }

  _makepath(szCurr,szDrive,szDir,"","");
  m_strFileRoot  = szCurr;

  m_strFileSpec  = szName;
  m_strFileSpec += szExt;

  // if in recursive mode & !m_boInFind, try to find another file (in subdir)

  if(!m_boInFind && boRecurse)
    {
    m_boInFind = true;                                      // so that FindNext does its job
    m_boInFind = FindNext();
    }

  //

  return m_boInFind;
  }

//----------------------------------------------- FindNext -----------------------------------------------------------//
// continue search
// in :
// out: another file found corresponding to spec ?
// rem: this routine is very simple without the recursive mode ;)
//--------------------------------------------------------------------------------------------------------------------//

bool CFindFileC::FindNext()
  {
  if(!m_boInFind) return false;

  if(!m_pSubLevel) 
    {
    if(m_s32Handle != -1)
      {                                                     // can be == -1 when called from FindFirst in recursive mode
      int iRet = _findnext(m_s32Handle,&m_FindData);
      if(!iRet)        return true;
      if(!m_boRecurse) return false;

      FindClose();                                          // no more files in this dir & recurse => close
      }

    // recursive mode => now, look for subdirs

    m_s32Handle = _findfirst(m_strFileRoot+"*.*",&m_FindData);
    if(m_s32Handle == -1) return false;
    m_boInFind = true;

    m_pSubLevel = new CFindFileC;                           // m_boInFind = false
    if(!m_pSubLevel)
      {
      FindClose();
      return false;
      }
    }

  // recursive mode processing

  if(m_pSubLevel)
    {
    if(m_pSubLevel->FindNext()) return true;                // a good file has been found in the subtree
    delete m_pSubLevel;                                     // this subdir is no longer useful
    m_pSubLevel = NULL;

    while(true)
      {
      int iRet;
      do
        {
        iRet = _findnext(m_s32Handle,&m_FindData);
        if(iRet) return false;                              // no more subdirs to explore
        }
      while(!IsDirectory() || IsDots());                    // as long as we don't have a real dir

      // we have a new dir to explore

      m_pSubLevel = new CFindFileC;
      if(!m_pSubLevel) return false;

      if(m_pSubLevel->FindFirst(m_strFileRoot+m_FindData.name+"/"+m_strFileSpec,true)) return true;

      delete m_pSubLevel;
      m_pSubLevel = NULL;
      }
    }

  return false;
  }

//----------------------------------------------- FindClose ----------------------------------------------------------//
// close search
// in :
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CFindFileC::FindClose()
  {
  if(!m_boInFind) return false;
  _findclose(m_s32Handle);
  m_boInFind = false;

  delete m_pSubLevel;
  m_pSubLevel = NULL;
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
