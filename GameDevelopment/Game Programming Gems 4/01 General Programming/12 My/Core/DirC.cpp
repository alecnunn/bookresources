//--------------------------------------------------------------------------------------------------------------------//
//                                                DIRECTORY ACCESS WITH C LIB                                         //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "DirC.h"
#ifdef    _DEBUG
#include  "DirC.inl"
#endif

#include  <direct.h>
#include  <io.h>

#include  "FileC.h"
#include  "FindFileC.h"
#include  "MemC.h"
#include  "StringC.h"

#include  "StdString.h"

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                OPERATIONS                                                          //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- Exist --------------------------------------------------------------//
// does given/file dir exist ?
// in : name
// out: exists?
// rem: another option is to use _access(pszName,00)
//--------------------------------------------------------------------------------------------------------------------//

bool CDirC::Exist(const char* pszName)
  {
  _finddata_t FileInfo;
  long lHandle = _findfirst(pszName,&FileInfo);
  if(lHandle  == -1) return false;

  _findclose(lHandle);
  return true;
  }

//----------------------------------------------- MakeDir ------------------------------------------------------------//
// create dir & subdirs (recursively)
// in : name
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CDirC::MakeDir(const char* pszName)
  {
  char szName[_MAX_PATH];
  String::StrnCpy(szName,pszName,_MAX_PATH);                // copy because pszName could be read only
  szName[_MAX_PATH-1] = 0;
  char* pszSub = szName;

  while(String::StrLen(pszSub))
    {
    char* pszSub2 = String::StrChr(pszSub,'/');
    if(!pszSub2) pszSub2 = String::StrChr(pszSub,'\\');
    if( pszSub2)
      {
      if((pszSub2-pszSub > 1) && (*(pszSub2-1) != ':'))
        {                                                   // pszSubDir != drive letter only
        *pszSub2 = 0;

        if(!Exist(szName))
          {
          if(_mkdir(szName) == -1) return false;
          }

        *pszSub2 = '/';
        }
      pszSub = pszSub2+1;
      }

    else
      {
      if(!Exist(szName))
        {
        if(_mkdir(szName) == -1) return false;
        }
      return true;
      }
    }

  return true;
  }

//----------------------------------------------- RemoveFiles --------------------------------------------------------//
// remove all files in given directory
// in : dir
// out: nb files removed
//--------------------------------------------------------------------------------------------------------------------//

u32 CDirC::RemoveFiles(const char* pszName)
  {
  char szSaveDir[_MAX_PATH];
  if(!GetDir(szSaveDir,_MAX_PATH)) return 0;
  if(!ChangeDir(pszName)) return 0;

  FindFile Finder;
  u32 u32Removed = 0;

  if(Finder.FindFirst("*.*"))
    {
    while(Finder.FindNext())
      {
      if(!Finder.IsDirectory() && !Finder.IsDots())
        {
        if(RemoveOneFile(Finder.GetFileName().c_str()))
          u32Removed++;
        }
      }
    }

  ChangeDir(szSaveDir);
  return u32Removed;
  }

//----------------------------------------------- RemoveOneDir -------------------------------------------------------//
// remove dir & its subtree
// in : dir
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CDirC::RemoveOneDir(const char* pszName)
  {
  RemoveDirs (pszName);
  RemoveFiles(pszName);
  return RemoveEmptyDir(pszName);
  }

//----------------------------------------------- RemoveDirs ---------------------------------------------------------//
// remove subdirs of given dir
// in : dir
// out: nb dirs removed (1st level)
//--------------------------------------------------------------------------------------------------------------------//

u32 CDirC::RemoveDirs(const char* pszName)
  {
  char szSaveDir[_MAX_PATH];
  if(!GetDir(szSaveDir,_MAX_PATH)) return 0;
  if(!ChangeDir(pszName)) return 0;

  FindFile Finder;
  u32 u32Removed = 0;

  if(Finder.FindFirst("*.*"))
    {
    while(Finder.FindNext())
      {
      if(Finder.IsDirectory() && !Finder.IsDots())
        {
        if(RemoveOneDir(Finder.GetFileName().c_str()))
          u32Removed++;
        }
      }
    }

  ChangeDir(szSaveDir);
  return u32Removed;
  }

//----------------------------------------------- CopyOneFile --------------------------------------------------------//
// copy given file
// in : new file,source
// out: OK?
// rem: don't know a function in the C lib that does the job, so have to load & save
//      read-only flag is maintained, but I don't know how to do for archive,hidden and system flags
//--------------------------------------------------------------------------------------------------------------------//

bool CDirC::CopyOneFile(const char* pszDest,const char* pszSrc)
  {
  File  Loader;
  u32   u32Size;
  void* pvFile = Loader.Load(pszSrc,&u32Size);
  if(pvFile)
    {
    bool boSaved = Loader.Save(pszDest,pvFile,u32Size);
    Memory::Free(pvFile);

    if(boSaved)
      {
      if(IsReadOnly(pszSrc)) SetReadOnly(pszDest,true);
      }
    return boSaved;
    }

  // perhaps the file exists but has a size of 0 byte

  FindFile Finder;
  if(!Finder.FindFirst(pszSrc)) return false;
  if( Finder.IsDirectory())     return false;               // bad call
  u32Size = Finder.GetFileSize();
  Finder.FindClose();
  if(u32Size) return false;                                 // > 0 bytes

  bool boSaved = Loader.Save(pszDest,&u32Size,0);           // &u32Size is unused, but NULL won't do
  if(boSaved)
    {
    if(IsReadOnly(pszSrc)) SetReadOnly(pszDest,true);
    }
  return boSaved;
  }

//----------------------------------------------- CopyFiles ----------------------------------------------------------//
// copy files from a dir to another
// in : dest & src dirs
// out: nb files copied
//--------------------------------------------------------------------------------------------------------------------//

u32 CDirC::CopyFiles(const char* pszDest,const char* pszSrc)
  {
  if(!MakeDir(pszDest)) return 0;

  char szSaveDir[_MAX_PATH];
  if(!GetDir(szSaveDir,_MAX_PATH)) return 0;
  if(!ChangeDir(pszSrc)) return 0;

  FindFile Finder;
  u32 u32Copied = 0;

  if(Finder.FindFirst("*.*"))
    {
    while(Finder.FindNext())
      {
      if(!Finder.IsDirectory() && !Finder.IsDots())
        {
        char szDest[_MAX_PATH];
        String::StrnCpy(szDest,pszDest,_MAX_PATH);
        u32 u32Len = String::StrLen(szDest);
        if((szDest[u32Len-1] != '/') && (szDest[u32Len-1] != '\\'))
          String::StrCat(szDest,"/");
        String::StrCat(szDest,Finder.GetFileName().c_str());

        if(CopyOneFile(szDest,Finder.GetFileName().c_str()))
          u32Copied++;
        }
      }
    }

  ChangeDir(szSaveDir);
  return u32Copied;
  }

//----------------------------------------------- CopyOneDir ---------------------------------------------------------//
// copy dir & subtree
// in : dest & src dirs
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CDirC::CopyOneDir(const char* pszDest,const char* pszSrc)
  {
  if(!MakeDir(pszDest)) return false;
  CopyDirs (pszDest,pszSrc);
  CopyFiles(pszDest,pszSrc);
  return true;
  }

//----------------------------------------------- CopyDirs -----------------------------------------------------------//
// copy subdirs
// in : dest & src dirs
// out: nb dirs copied (1st level)
//--------------------------------------------------------------------------------------------------------------------//

u32 CDirC::CopyDirs(const char* pszDest,const char* pszSrc)
  {
  if(!MakeDir(pszDest)) return 0;

  char szSaveDir[_MAX_PATH];
  if(!GetDir(szSaveDir,_MAX_PATH)) return 0;
  if(!ChangeDir(pszSrc)) return 0;

  FindFile Finder;
  u32 u32Copied = 0;

  if(Finder.FindFirst("*.*"))
    {
    while(Finder.FindNext())
      {
      if(Finder.IsDirectory() && !Finder.IsDots())
        {
        char szDest[_MAX_PATH];
        String::StrnCpy(szDest,pszDest,_MAX_PATH);
        u32 u32Len = String::StrLen(szDest);
        if((szDest[u32Len-1] != '/') && (szDest[u32Len-1] != '\\'))
          String::StrCat(szDest,"/");
        String::StrCat(szDest,Finder.GetFileName().c_str());

        if(CopyOneDir(szDest,Finder.GetFileName().c_str()))
          u32Copied++;
        }
      }
    }

  ChangeDir(szSaveDir);
  return u32Copied;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
