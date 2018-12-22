//--------------------------------------------------------------------------------------------------------------------//
//                                                FILE ACCESS INTERFACE                                               //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_FILE_H_
#define   _GPG4_FILE_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "BasicTypes.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                IFile                                                               //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class IFile
  {
  // public enums

  public:

    typedef enum
      {
      READ                    = 1 << 0,
      WRITE                   = 1 << 1,
      APPEND                  = 1 << 2,
      BINARY                  = 1 << 3,
      AUTOSAVE                = 1 << 4,                     // MemFile: Close calls Save when writing

      READ_TEXT               = READ,
      READ_BINARY             = READ   | BINARY,
      WRITE_TEXT              = WRITE,
      WRITE_BINARY            = WRITE  | BINARY,
      READ_WRITE_TEXT         = READ   | WRITE,
      READ_WRITE_BINARY       = READ   | WRITE | BINARY,
      APPEND_TEXT             = APPEND,
      APPEND_BINARY           = APPEND | BINARY,
      } FILE_MODE;

  // public methods

  public:

    // constructors & destructor

                    IFile               (void)    {}
    virtual        ~IFile               (void)    {}

    // operations

    virtual bool    Open                (const char* pszName, const FILE_MODE Mode) = 0;
    virtual u32     Read                (      void* pvBuffer,const u32 u32Size) = 0;
    virtual u32     Write               (const void* pvBuffer,const u32 u32Size) = 0;
    virtual bool    Close               (void) = 0;

    virtual u32     Seek                (const u32 u32Pos,const u32 u32Orig) = 0;
    virtual u32     Tell                (void) = 0;
    virtual u32     GetSize             (void) = 0;
    virtual bool    Eof                 (void) const = 0;
    virtual bool    Error               (void) const = 0;
    virtual bool    Flush               (void) = 0;

    virtual u32     GetChar             (void) = 0;
    virtual bool    GetString           (char* pszString,const u32 u32Len) = 0;
    virtual bool    PutChar             (const u32 u32Char)     = 0;
    virtual bool    PutString           (const char* pszString) = 0;

    virtual void*   Load                (const char* pszName,u32* pu32Size=NULL) = 0;
    virtual bool    Save                (const char* pszName,const void* pvBuffer,const u32 u32Size) = 0;
  };
}

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_FILE_H_
