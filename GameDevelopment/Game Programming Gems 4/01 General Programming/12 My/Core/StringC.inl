//--------------------------------------------------------------------------------------------------------------------//
//                                                STRING FUNCTIONS                                                    //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_STRINGC_INL_
#define   _GPG4_STRINGC_INL_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  <string.h>

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//

INLINE u32 CStringC::StrLen(const char* pszString)
  {
  return strlen(pszString);
  }

//

INLINE char* CStringC::StrnCpy(char* pszDest,const char* pszSrc,u32 u32Count)
  {
  return strncpy(pszDest,pszSrc,u32Count);
  }

//

INLINE char* CStringC::StrCat(char* pszDest,const char* pszSrc)
  {
  return strcat(pszDest,pszSrc);
  }

//

INLINE char* CStringC::StrChr(const char* pszString,u32 u32Chr)
  {
  return strchr(pszString,u32Chr);
  }

//

INLINE char* CStringC::StrCpy(char* pszDest,const char* pszSrc)
  {
  return strcpy(pszDest,pszSrc);
  }

//

INLINE si CStringC::StriCmp(const char* pszStr1,const char* pszStr2)
  {
  return _stricmp(pszStr1,pszStr2);
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_STRINGC_INL_
