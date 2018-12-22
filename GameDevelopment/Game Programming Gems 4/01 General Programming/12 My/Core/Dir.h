//--------------------------------------------------------------------------------------------------------------------//
//                                                DIRECTORY ACCESS INTERFACE                                          //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_DIR_H_
#define   _GPG4_DIR_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "BasicTypes.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                IDir                                                                //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class IDir
  {
  // public methods

  public:

    // constructors & destructor

                    IDir                (void) {}
    virtual        ~IDir                (void) {}

    // operations

    virtual bool    Exist               (const char* pszName) = 0;                        // file or dir exists ?
    virtual bool    MakeDir             (const char* pszName) = 0;                        // (recursively) create dir & subdirs
    virtual bool    Rename              (const char* pszOld, const char* pszNew) = 0;     // rename file/dir
    virtual bool    SetReadOnly         (const char* pszName,const bool  boRead) = 0;     // set/remove read only flag
    virtual bool    IsReadOnly          (const char* pszName) = 0;                        // is the file read only ?

    virtual bool    RemoveOneFile       (const char* pszName) = 0;                        // remove given file
    virtual u32     RemoveFiles         (const char* pszName) = 0;                        // remove all files in given dir
    virtual bool    RemoveEmptyDir      (const char* pszName) = 0;                        // remove given dir
    virtual bool    RemoveOneDir        (const char* pszName) = 0;                        // remove dir & its subtree
    virtual u32     RemoveDirs          (const char* pszName) = 0;                        // remove subdirs of given dir

    virtual bool    CopyOneFile         (const char* pszDest,const char* pszSrc) = 0;     // copy given file
    virtual u32     CopyFiles           (const char* pszDest,const char* pszSrc) = 0;     // copy files from 1 dir to another
    virtual bool    CopyOneDir          (const char* pszDest,const char* pszSrc) = 0;     // copy dir & subtree
    virtual u32     CopyDirs            (const char* pszDest,const char* pszSrc) = 0;     // copy subdirs

    virtual bool    MoveOneFile         (const char* pszDest,const char* pszSrc);         // move given file
    virtual u32     MoveFiles           (const char* pszDest,const char* pszSrc);         // move files from 1 dir to another
    virtual bool    MoveOneDir          (const char* pszDest,const char* pszSrc);         // move dir & subtree
    virtual u32     MoveDirs            (const char* pszDest,const char* pszSrc);         // move subdirs

    virtual bool    ChangeDir           (const char* pszName) = 0;                        // set current dir
    virtual bool    GetDir              (char* pszBuff,u32 u32Size) = 0;                  // get current dir
    virtual char*   FullPath            (char* pszAbs,const char* pszRel,u32 u32Size) = 0;
  };
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#include  "Dir.inl"

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_DIR_H_
