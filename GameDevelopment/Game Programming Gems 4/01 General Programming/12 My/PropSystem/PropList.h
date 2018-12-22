//--------------------------------------------------------------------------------------------------------------------//
//                                                LIST OF PROPERTIES                                                  //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_PROPLIST_H_
#define   _GPG4_PROPLIST_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "BasicTypes.h"
#include  "Defines.h"
#include  "Properties.h"
#include  "StdString.h"
#include  "StlIncludes.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class IFile;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropList                                                           //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CPropList
  {
  // typedefs

  public:

    typedef std::list< CProperty* >               listProp;
    typedef listProp::iterator                    iterProp;

  // public methods

  public:

    // constructors & destructor

                              CPropList           (void);
    virtual                  ~CPropList           (void);

    // access

    CProperty*                GetFirstProp        (void);
    CProperty*                GetNextProp         (void);

    // management

    void                      AddProp             (CProperty* pProp);
    CProperty*                RegisterProp        (const CProperties::PROPERTY_TYPE Type,const CStdString& strName,const u32 u32Off,
                                                   const u32 u32Flags,const CStdString& strHelp);
    CProperty*                RegisterProp        (const CStdString& strType,const CStdString& strName);

    // streaming

    virtual bool              Write               (IFile& File);
    virtual bool              WriteBin            (IFile& File);

  // protected data

  protected:

    listProp                  m_Properties;
    iterProp                  m_CurrentProp;
  };
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "PropList.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_PROPLIST_H_
