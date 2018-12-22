//--------------------------------------------------------------------------------------------------------------------//
//                                                DIRECTORY ACCESS INTERFACE                                          //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_DIR_INL_
#define   _GPG4_DIR_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                OPERATIONS                                                          //
//--------------------------------------------------------------------------------------------------------------------//

inline bool IDir::MoveOneFile(const char* pszDest,const char* pszSrc)
  {
  if(!CopyOneFile(pszDest,pszSrc)) return false;
  return RemoveOneFile(pszSrc);
  }

//

inline u32 IDir::MoveFiles(const char* pszDest,const char* pszSrc)
  {
  u32 u32Copied  = CopyFiles(pszDest,pszSrc);
  u32 u32Removed = RemoveFiles(pszSrc);
  return(u32Removed <= u32Copied ? u32Removed : u32Copied);
  }

//

inline bool IDir::MoveOneDir(const char* pszDest,const char* pszSrc)
  {
  if(!CopyOneDir(pszDest,pszSrc)) return false;
  return RemoveOneDir(pszSrc);
  }

//

inline u32 IDir::MoveDirs(const char* pszDest,const char* pszSrc)
  {
  u32 u32Copied  = CopyDirs(pszDest,pszSrc);
  u32 u32Removed = RemoveDirs(pszSrc);
  return(u32Removed <= u32Copied ? u32Removed : u32Copied);
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_DIR_INL_
