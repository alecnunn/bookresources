//--------------------------------------------------------------------------------------------------------------------//
//                                                FILE ACCESS WITH C LIB                                              //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "FileC.h"
#ifdef    _DEBUG
#include  "FileC.inl"
#endif

#include  "MemC.h"
#include  "StringC.h"

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                OPERATIONS                                                          //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- Open ---------------------------------------------------------------//
// open given file
// in : name,mode
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CFileC::Open(const char* pszName,const FILE_MODE Mode)
  {
  if(!pszName) return false;
  if(m_pFile) Close();                                      // previous file

  char szMode[8] = { 0 };
  if(Mode & READ)
    {
    String::StrCat(szMode,"r");
    if(Mode & WRITE) String::StrCat(szMode,"+");
    }
  else if(Mode & WRITE) String::StrCat(szMode,"w");

  if(Mode & APPEND) String::StrCat(szMode,"a");
  if(Mode & BINARY) String::StrCat(szMode,"b");
  else              String::StrCat(szMode,"t");

  m_pFile = fopen(pszName,szMode);
  return(m_pFile != NULL);
  }

//----------------------------------------------- GetSize ------------------------------------------------------------//
// return file size
// in :
// out: size in bytes
//--------------------------------------------------------------------------------------------------------------------//

u32 CFileC::GetSize()
  {
  if(!m_pFile) return 0;
  u32 u32Pos  = Tell();                                     // save current position
  if(u32Pos  == -1) return 0;                               // Tell failed

  if(Seek(0,SEEK_END)) return 0;
  u32 u32Size = Tell();
  Seek(u32Pos,SEEK_SET);                                    // restore position

  if(u32Size == -1) return 0;                               // Tell failed
  return u32Size;
  }

//----------------------------------------------- Load ---------------------------------------------------------------//
// load given file
// in : name,returned size
// out: memory address
//--------------------------------------------------------------------------------------------------------------------//

void* CFileC::Load(const char* pszName,u32* pu32Size)
  {
  if(!pszName) return NULL;
  if(!Open(pszName,READ_BINARY)) return NULL;

  u32 u32Size = GetSize();
  if(!u32Size) return NULL;

  void* pvFile = Memory::Malloc(u32Size);                   // get some memory
  if(!pvFile) return NULL;

  u32 u32Read = Read(pvFile,u32Size);                       // read file
  if(u32Read < u32Size)
    {
    Memory::Free(pvFile);
    return NULL;
    }

  Close();
  if(pu32Size) *pu32Size = u32Size;
  return pvFile;
  }

//----------------------------------------------- Save ---------------------------------------------------------------//
// save given block of memory
// in : name,memory buffer,buffer size
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CFileC::Save(const char* pszName,const void* pvBuffer,const u32 u32Size)
  {
  if(!pszName || !pvBuffer) return false;
  if(!Open(pszName,WRITE_BINARY)) return false;

  u32 u32Written = Write(pvBuffer,u32Size);
  if(u32Written < u32Size) return false;

  Close();
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
