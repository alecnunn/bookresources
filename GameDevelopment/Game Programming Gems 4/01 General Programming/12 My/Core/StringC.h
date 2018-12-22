//--------------------------------------------------------------------------------------------------------------------//
//                                                STRING FUNCTIONS                                                    //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_STRINGC_H_
#define   _GPG4_STRINGC_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "BasicTypes.h"
#include  "Defines.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CStringC                                                            //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CStringC
  {
  // public methods

  public:

    static u32      StrLen              (const char* pszString);
    static char*    StrnCpy             (char* pszDest,const char* pszSrc,u32 u32Count);
    static char*    StrCat              (char* pszDest,const char* pszSrc);
    static char*    StrChr              (const char* pszString,u32 u32Chr);
    static char*    StrCpy              (char* pszDest,const char* pszSrc);
    static si       StriCmp             (const char* pszStr1,const char* pszStr2);
  };

  typedef CStringC  String;
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "StringC.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_STRINGC_H_
