//--------------------------------------------------------------------------------------------------------------------//
//                                                FIND FILE/DIR WITH C LIB                                            //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_FINDFILEC_H_
#define   _GPG4_FINDFILEC_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "FindFile.h"
#include  "Defines.h"

#include  <io.h>

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CFindFileC                                                          //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
class CFindFileC : public IFindFile
  {
  // public methods

  public:

    // constructors & destructor

                              CFindFileC          (void);
    virtual                  ~CFindFileC          (void);

    // operations

    virtual bool              FindFirst           (const char* pszName,const bool boRecurse=false);
    virtual bool              FindNext            (void);
    virtual bool              FindClose           (void);

    // attributes

    virtual CStdString        GetFileName         (void) const;
    virtual CStdString        GetFilePath         (void) const;
    virtual CStdString        GetFileRoot         (void) const;
    virtual u32               GetFileSize         (void) const;

    virtual bool              IsDirectory         (void) const;
    virtual bool              IsDots              (void) const;
    virtual bool              IsReadOnly          (void) const;

  // protected data

  protected:

    CStdString                m_strFileRoot;
    CStdString                m_strFileSpec;
    bool                      m_boRecurse;
    CFindFileC*               m_pSubLevel;

    s32                       m_s32Handle;
    bool                      m_boInFind;                   // search in progress
    _finddata_t               m_FindData;
  };

  typedef CFindFileC FindFile;
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "FindFileC.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_FINDFILEC_H_
