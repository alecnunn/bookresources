//--------------------------------------------------------------------------------------------------------------------//
//                                                DIRECTORY ACCESS WITH C LIB                                         //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_DIRC_INL_
#define   _GPG4_DIRC_INL_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  <direct.h>
#include  <io.h>
#include  <sys/stat.h>
#include  <stdlib.h>

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CDirC::CDirC()
  {}

//

INLINE CDirC::~CDirC()
  {}

//--------------------------------------------------------------------------------------------------------------------//
//                                                OPERATIONS                                                          //
//--------------------------------------------------------------------------------------------------------------------//

INLINE bool CDirC::Rename(const char* pszOld,const char* pszNew)
  {
  return(rename(pszOld,pszNew) == 0);
  }

//

INLINE bool CDirC::SetReadOnly(const char* pszName,const bool boRead)
  {
  return(_chmod(pszName,boRead ? _S_IREAD : _S_IREAD | _S_IWRITE) == 0);
  }

//

INLINE bool CDirC::IsReadOnly(const char* pszName)
  {
  return(_access(pszName,02) != 0);                         // also return s true if file doesn't exist
  }

//

INLINE bool CDirC::RemoveOneFile(const char* pszName)
  {                                                         // can't be removed if read only or used
  return(remove(pszName) == 0);
  }

//

INLINE bool CDirC::RemoveEmptyDir(const char* pszName)
  {
  return(_rmdir(pszName) == 0);
  }

//

INLINE bool CDirC::ChangeDir(const char* pszName)
  {
  return(_chdir(pszName) == 0);
  }

//

INLINE bool CDirC::GetDir(char* pszBuff,u32 u32Size)
  {
  if(!pszBuff) return false;
  return(_getcwd(pszBuff,u32Size) != NULL);
  }

//

INLINE char* CDirC::FullPath(char* pszAbs,const char* pszRel,u32 u32Size)
  {
  return _fullpath(pszAbs,pszRel,u32Size);
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_DIRC_INL_
