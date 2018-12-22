//--------------------------------------------------------------------------------------------------------------------//
//                                                RTTI CLASS & MACROS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_RTTI_H_
#define   _GPG4_RTTI_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "StdString.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class CExtraData;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CRTTI                                                               //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CRTTI
  {
  // public methods

  public:
                              CRTTI               (const CStdString& strClassName,const CRTTI* pBase,CExtraData* pExtra=NULL) :
                                                      m_strClassName(strClassName),m_pBaseRTTI(pBase),m_pExtraData(pExtra)
                                                                                {}
    virtual                  ~CRTTI               (void)                        {}

    const CStdString&         GetClassName        (void) const                  { return m_strClassName; }
    const CRTTI*              GetBaseRTTI         (void) const                  { return m_pBaseRTTI;    }
          CExtraData*         GetExtraData        (void) const                  { return m_pExtraData;   }

  // protected data

  protected:

    const CStdString          m_strClassName;               // name of the class containing this (static) object
    const CRTTI*              m_pBaseRTTI;                  // pointer to the (static) object of the container's base class
          CExtraData*         m_pExtraData;                 // here we can store some additional infos (eg. properties)
  };
}

//----------------------------------------------- MACROS -------------------------------------------------------------//

// in non-root class .h
// put this macro in the declaration of a DERIVED class - it defines a static CRTTI object for this class
// (be careful that after this macro, things are private)

#define   DECLARE_RTTI                                                                               \
  public:                                                                                            \
    static  const GPG4::CRTTI           m_RTTI;                                                      \
    virtual const GPG4::CRTTI*          GetRTTI   (void) const                  { return &m_RTTI; }  \
  private:

//in root class .h
// put this macro in the declaration of a BASE class
// (be careful that after this macro, things are private)

#define   DECLARE_ROOT_RTTI                                                                                                 \
    DECLARE_RTTI;                                                                                                           \
  public:                                                                                                                   \
    bool                      IsExactClass        (const GPG4::CRTTI* pRTTI)    { return(GetRTTI() == pRTTI); }             \
    bool                      IsKindOf            (const GPG4::CRTTI* pRTTI)    { const GPG4::CRTTI* pBase = GetRTTI();     \
                                                                                  while(pBase)                              \
                                                                                    {                                       \
                                                                                    if(pBase == pRTTI) return true;         \
                                                                                    pBase = pBase->GetBaseRTTI();           \
                                                                                    }                                       \
                                                                                  return false;                             \
                                                                                }                                           \
    void*                     DynamicCast         (const GPG4::CRTTI* pRTTI)    { return(IsKindOf(pRTTI) ? this : NULL); }  \
  private:

// in non-root class .cpp
// put one of these macros in the implementation file of a DERIVED class

#define IMPLEMENT_RTTI(ClassName,BaseClassName)                       \
  const GPG4::CRTTI ClassName::m_RTTI(#ClassName,&BaseClassName::m_RTTI);

#define IMPLEMENT_RTTI_PROP(ClassName,BaseClassName)                  \
  const GPG4::CRTTI ClassName::m_RTTI(#ClassName,&BaseClassName::m_RTTI,&m_Prop);

// in root class .cpp
// put one of these macros in the implementation file of a BASE class - they create a static CRTTI member

#define IMPLEMENT_ROOT_RTTI(ClassName)                                \
  const GPG4::CRTTI ClassName::m_RTTI(#ClassName,NULL);

#define IMPLEMENT_ROOT_RTTI_PROP(ClassName)                           \
  const GPG4::CRTTI ClassName::m_RTTI(#ClassName,NULL,&m_Prop);

// runtime type macros

#define IS_EXACT_CLASS(ClassName,pObj)                                \
  (pObj ? pObj->IsExactClass(&ClassName::m_RTTI) : false)

#define IS_KIND_OF(ClassName,pObj)                                    \
  (pObj ? pObj->IsKindOf(&ClassName::m_RTTI) : false)

#define STATIC_CAST(ClassName,pObj)                                   \
  ((ClassName*)pObj)

#define DYNAMIC_CAST(ClassName,pObj)                                  \
  (pObj ? STATIC_CAST(ClassName,pObj->DynamicCast(&ClassName::m_RTTI)) : NULL)

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_RTTI_H_
