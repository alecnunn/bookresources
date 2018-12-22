//--------------------------------------------------------------------------------------------------------------------//
//                                                FILE ACCESS WITH C LIB                                              //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_FILEC_H_
#define   _GPG4_FILEC_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "File.h"
#include  "Defines.h"

#include  <stdio.h>

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CFileC                                                              //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CFileC : public IFile
  {
  // public methods

  public:

    // constructors & destructor

                    CFileC              (void);
    virtual        ~CFileC              (void);

    // operations

    virtual bool    Open                (const char* pszName, const FILE_MODE Mode);
    virtual u32     Read                (      void* pvBuffer,const u32 u32Size);
    virtual u32     Write               (const void* pvBuffer,const u32 u32Size);
    virtual bool    Close               (void);

    virtual u32     Seek                (const u32 u32Pos,const u32 u32Orig);
    virtual u32     Tell                (void);
    virtual u32     GetSize             (void);
    virtual bool    Eof                 (void) const;
    virtual bool    Error               (void) const;
    virtual bool    Flush               (void);

    virtual u32     GetChar             (void);
    virtual bool    GetString           (char* pszString,const u32 u32Len);
    virtual bool    PutChar             (const u32 u32Char);
    virtual bool    PutString           (const char* pszString);

    virtual void*   Load                (const char* pszName,u32* pu32Size=NULL);
    virtual bool    Save                (const char* pszName,const void* pvBuffer,const u32 u32Size);

  // protected data

  protected:

    FILE*           m_pFile;
  };

  typedef CFileC    File;
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "FileC.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_FILEC_H_
