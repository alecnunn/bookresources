//--------------------------------------------------------------------------------------------------------------------//
//                                                DIRECTORY ACCESS WITH C LIB                                         //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_DIRC_H_
#define   _GPG4_DIRC_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "Dir.h"
#include  "Defines.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CDirC                                                               //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
class CDirC : public IDir
  {
  // public methods

  public:

    // constructors & destructor

                    CDirC               (void);
    virtual        ~CDirC               (void);

    // operations

    virtual bool    Exist               (const char* pszName);                            // file or dir exists ?
    virtual bool    MakeDir             (const char* pszName);                            // (recursively) create dir & subdirs
    virtual bool    Rename              (const char* pszOld, const char* pszNew);         // rename file/dir
    virtual bool    SetReadOnly         (const char* pszName,const bool  boRead);         // set/remove read only flag
    virtual bool    IsReadOnly          (const char* pszName);                            // is the file read only ?

    virtual bool    RemoveOneFile       (const char* pszName);                            // remove given file
    virtual u32     RemoveFiles         (const char* pszName);                            // remove all files in given dir
    virtual bool    RemoveEmptyDir      (const char* pszName);                            // remove given dir
    virtual bool    RemoveOneDir        (const char* pszName);                            // remove dir & its subtree
    virtual u32     RemoveDirs          (const char* pszName);                            // remove subdirs of given dir

    virtual bool    CopyOneFile         (const char* pszDest,const char* pszSrc);         // copy given file
    virtual u32     CopyFiles           (const char* pszDest,const char* pszSrc);         // copy files from 1 dir to another
    virtual bool    CopyOneDir          (const char* pszDest,const char* pszSrc);         // copy dir & subtree
    virtual u32     CopyDirs            (const char* pszDest,const char* pszSrc);         // copy subdirs

    virtual bool    ChangeDir           (const char* pszName);                            // set current dir
    virtual bool    GetDir              (char* pszBuff,u32 u32Size);                      // get current dir
    virtual char*   FullPath            (char* pszAbs,const char* pszRel,u32 u32Size);    // get absolute path from relative path
  };

  typedef CDirC Dir;
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "DirC.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_DIRC_H_
