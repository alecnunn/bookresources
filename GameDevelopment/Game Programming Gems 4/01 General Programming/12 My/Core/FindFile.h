//--------------------------------------------------------------------------------------------------------------------//
//                                                FIND FILE/DIR INTERFACE                                             //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_FINDFILE_H_
#define   _GPG4_FINDFILE_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "BasicTypes.h"
#include  "StdString.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                IFindFile                                                           //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class IFindFile
  {
  // public methods

  public:

    // constructors & destructor

                              IFindFile           (void) {}
    virtual                  ~IFindFile           (void) {}

    // operations

    virtual bool              FindFirst           (const char* pszName,const bool boRecurse=false) = 0;
    virtual bool              FindNext            (void) = 0;
    virtual bool              FindClose           (void) = 0;

    // attributes

    virtual CStdString        GetFileName         (void) const = 0;
    virtual CStdString        GetFilePath         (void) const = 0;
    virtual CStdString        GetFileRoot         (void) const = 0;
    virtual u32               GetFileSize         (void) const = 0;

    virtual bool              IsDirectory         (void) const = 0;
    virtual bool              IsDots              (void) const = 0;
    virtual bool              IsReadOnly          (void) const = 0;
  };
}

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_FINDFILE_H_
