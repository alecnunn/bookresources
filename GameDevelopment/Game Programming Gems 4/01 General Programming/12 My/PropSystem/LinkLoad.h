//--------------------------------------------------------------------------------------------------------------------//
//                                                LINK CLASS                                                          //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_LINKLOAD_H_
#define   _GPG4_LINKLOAD_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "RefCount.h"                                      // CLinkLoad is smart-pointable
#include  "SmartPtr.h"
#include  "StdString.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class CPersistent;
  class CProperty;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CLinkLoad                                                           //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CLinkLoad : public CRefCount
  {
  // public methods

  public:

    // constructors & destructor

                              CLinkLoad           (const CPersistent* pObj,const CProperty* pProp,const CStdString& strID,
                                                   const CPersistent* pID);
    virtual                  ~CLinkLoad           (void);

    // get/set

    const CPersistent*        GetObj              (void) const;
    const CProperty*          GetProp             (void) const;
    const CStdString&         GetID               (void) const;
    const CPersistent*        GetParam            (void) const;

  // protected data

  protected:

    const CPersistent*        m_pObj;                       // object containing a ptr
    const CProperty*          m_pProp;                      // ptr or sp property
    CStdString                m_strID;                      // ID of pointed to object (text)
    const CPersistent*        m_pID;                        // ID of pointed to object (bianry)
  };

  // smart ptr

  SMART_POINTER(CLinkLoad);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#include  "LinkLoad.inl"

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_LINKLOAD_H_
