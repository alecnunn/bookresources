//--------------------------------------------------------------------------------------------------------------------//
//                                                PROPERTIES CLASSES                                                  //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_PROPERTIES_H_
#define   _GPG4_PROPERTIES_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "Property.h"
#include  "VectorMatrix.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropString                                                         //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CProperties
  {
  // public enums

  public:

    typedef enum
      {
      String        = 1 << 0,
      Float         = 1 << 1,
      Bool          = 1 << 2,
      U32           = 1 << 3,
      Ptr           = 1 << 4,
      SP            = 1 << 5,
      Fct           = 1 << 6,
      Vect2D        = 1 << 7,
      Vect3D        = 1 << 8,
      Vect4D        = 1 << 9,

      Unknown       = 0xffffffff
      }
    PROPERTY_TYPE;

  //

  public:

    static CStdString         GetTypeName         (u32 u32Type);
  };
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropString                                                         //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CPropString : public CProperty
  {
  DECLARE_RTTI;

  // public methods

  public:

    // constructors & destructor

                              CPropString         (const CStdString& strName,const u32 u32Offset,const u32 u32Size);
    virtual CProperty*        CreateCopy          (CProperty* pDest=NULL);

    // get/set params

    virtual const CStdString  GetType             (void) const;
    virtual u32               GetEnumType         (void) const;

    // value

    virtual bool              SetValue            (const CPersistent* pObj, const CStdString& strVal) const;
    virtual const CStdString  GetRealValue        (const CPersistent* pObj) const;

    // streaming

    virtual bool              WriteBin            (const CPersistent* pObj,CLevelFiles& Level) const;
    virtual bool              ReadBin             (const CPersistent* pObj,CLevelFiles& Level) const;

    // direct access

    CStdString                Get                 (const CPersistent* pObj) const;
  };
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropFloat                                                          //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CPropFloat : public CProperty
  {
  DECLARE_RTTI;

  // public methods

  public:

    // constructors & destructor

                              CPropFloat          (const CStdString& strName,const u32 u32Offset,const u32 u32Size);
    virtual CProperty*        CreateCopy          (CProperty* pDest=NULL);

    // get/set params

    virtual const CStdString  GetType             (void) const;
    virtual u32               GetEnumType         (void) const;

    // value

    static  const CStdString  Float2String        (const void* pAddr);
    virtual bool              String2Float        (const CStdString& strVal,float* pfVal) const;

    virtual const CStdString  GetValue            (const CPersistent* pObj) const;
    virtual bool              SetValue            (const CPersistent* pObj, const CStdString& strVal) const;
    virtual const CStdString  GetRealValue        (const CPersistent* pObj) const;

    // streaming

    virtual bool              WriteBin            (const CPersistent* pObj,CLevelFiles& Level) const;
    virtual bool              ReadBin             (const CPersistent* pObj,CLevelFiles& Level) const;

    // direct access

    float                     Get                 (const CPersistent* pObj) const;
    bool                      Set                 (const CPersistent* pObj, const float fVal) const;
  };
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropBool                                                           //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CPropBool : public CProperty
  {
  DECLARE_RTTI;

  // public methods

  public:

    // constructors & destructor

                              CPropBool           (const CStdString& strName,const u32 u32Offset,const u32 u32Size);
    virtual CProperty*        CreateCopy          (CProperty* pDest=NULL);

    // get/set params

    virtual const CStdString  GetType             (void) const;
    virtual u32               GetEnumType         (void) const;

    // value

    static  const CStdString  Bool2String         (const void* pAddr);

    virtual const CStdString  GetValue            (const CPersistent* pObj) const;
    virtual bool              SetValue            (const CPersistent* pObj, const CStdString& strVal) const;
    virtual const CStdString  GetRealValue        (const CPersistent* pObj) const;

    // streaming

    virtual bool              WriteBin            (const CPersistent* pObj,CLevelFiles& Level) const;
    virtual bool              ReadBin             (const CPersistent* pObj,CLevelFiles& Level) const;

    // direct access

    bool                      Get                 (const CPersistent* pObj) const;
    bool                      Set                 (const CPersistent* pObj, const bool boVal) const;
  };
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropU32                                                            //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CPropU32 : public CProperty
  {
  DECLARE_RTTI;

  // public methods

  public:

    // constructors & destructor

                              CPropU32            (const CStdString& strName,const u32 u32Offset,const u32 u32Size);
    virtual CProperty*        CreateCopy          (CProperty* pDest=NULL);

    // get/set params

    virtual const CStdString  GetType             (void) const;
    virtual u32               GetEnumType         (void) const;

    // value

    static  const CStdString  U32ToString         (const void* pAddr);
    virtual bool              String2U32          (const CStdString& strVal,u32* pu32Val) const;

    virtual const CStdString  GetValue            (const CPersistent* pObj) const;
    virtual bool              SetValue            (const CPersistent* pObj, const CStdString& strVal) const;
    virtual const CStdString  GetRealValue        (const CPersistent* pObj) const;

    // streaming

    virtual bool              WriteBin            (const CPersistent* pObj,CLevelFiles& Level) const;
    virtual bool              ReadBin             (const CPersistent* pObj,CLevelFiles& Level) const;

    // direct access

    u32                       Get                 (const CPersistent* pObj) const;
    bool                      Set                 (const CPersistent* pObj, const u32 u32Val) const;
  };
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropVect2D                                                         //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CPropVect2D : public CProperty
  {
  DECLARE_RTTI;

  // public methods

  public:

    // constructors & destructor

                              CPropVect2D         (const CStdString& strName,const u32 u32Offset,const u32 u32Size);
    virtual CProperty*        CreateCopy          (CProperty* pDest=NULL);

    // get/set params

    virtual const CStdString  GetType             (void) const;
    virtual u32               GetEnumType         (void) const;

    // value

    virtual bool              String2Floats       (const CStdString& strVal,float* pfVect) const;

    virtual const CStdString  GetValue            (const CPersistent* pObj) const;
    virtual bool              SetValue            (const CPersistent* pObj, const CStdString& strVal) const;
    virtual const CStdString  GetRealValue        (const CPersistent* pObj) const;

    // streaming

    virtual bool              WriteBin            (const CPersistent* pObj,CLevelFiles& Level) const;
    virtual bool              ReadBin             (const CPersistent* pObj,CLevelFiles& Level) const;

    // direct access

    CVect2D                   Get                 (const CPersistent* pObj) const;
    bool                      Set                 (const CPersistent* pObj, const CVect2D& v2Val) const;
  };
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropVect3D                                                         //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CPropVect3D : public CProperty
  {
  DECLARE_RTTI;

  // public methods

  public:

    // constructors & destructor

                              CPropVect3D         (const CStdString& strName,const u32 u32Offset,const u32 u32Size);
    virtual CProperty*        CreateCopy          (CProperty* pDest=NULL);

    // get/set params

    virtual const CStdString  GetType             (void) const;
    virtual u32               GetEnumType         (void) const;

    // value

    virtual bool              String2Floats       (const CStdString& strVal,float* pfVect) const;

    virtual const CStdString  GetValue            (const CPersistent* pObj) const;
    virtual bool              SetValue            (const CPersistent* pObj, const CStdString& strVal) const;
    virtual const CStdString  GetRealValue        (const CPersistent* pObj) const;

    // streaming

    virtual bool              WriteBin            (const CPersistent* pObj,CLevelFiles& Level) const;
    virtual bool              ReadBin             (const CPersistent* pObj,CLevelFiles& Level) const;

    // direct access

    CVect3D                   Get                 (const CPersistent* pObj) const;
    bool                      Set                 (const CPersistent* pObj, const CVect3D& v3Val) const;
  };
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropVect4D                                                         //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CPropVect4D : public CProperty
  {
  DECLARE_RTTI;

  // public methods

  public:

    // constructors & destructor

                              CPropVect4D         (const CStdString& strName,const u32 u32Offset,const u32 u32Size);
    virtual CProperty*        CreateCopy          (CProperty* pDest=NULL);

    // get/set params

    virtual const CStdString  GetType             (void) const;
    virtual u32               GetEnumType         (void) const;

    // value

    virtual bool              String2Floats       (const CStdString& strVal,float* pfVect) const;

    virtual const CStdString  GetValue            (const CPersistent* pObj) const;
    virtual bool              SetValue            (const CPersistent* pObj, const CStdString& strVal) const;
    virtual const CStdString  GetRealValue        (const CPersistent* pObj) const;

    // streaming

    virtual bool              WriteBin            (const CPersistent* pObj,CLevelFiles& Level) const;
    virtual bool              ReadBin             (const CPersistent* pObj,CLevelFiles& Level) const;

    // direct access

    CVect4D                   Get                 (const CPersistent* pObj) const;
    bool                      Set                 (const CPersistent* pObj, const CVect4D& v4Val) const;
  };
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropPtr                                                            //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CPropPtr : public CProperty
  {
  DECLARE_RTTI;

  // public methods

  public:

    // constructors & destructor

                              CPropPtr            (const CStdString& strName,const u32 u32Offset,const u32 u32Size);
    virtual CProperty*        CreateCopy          (CProperty* pDest=NULL);

    // get/set params

    virtual const CStdString  GetType             (void) const;
    virtual u32               GetEnumType         (void) const;

    // value

    static  const CStdString  Ptr2String          (const void* pAddr);
    virtual bool              String2Ptr          (const CStdString& strVal,void** ppVoid) const;

    virtual const CStdString  GetValue            (const CPersistent* pObj) const;
    virtual bool              SetValue            (const CPersistent* pObj, const CStdString& strVal) const;
    virtual const CStdString  GetRealValue        (const CPersistent* pObj) const;

    // streaming

    virtual bool              Write               (const CPersistent* pObj,CLevelFiles& Level) const;
    virtual bool              WriteBin            (const CPersistent* pObj,CLevelFiles& Level) const;
    virtual bool              Read                (const CPersistent* pObj,const CStdString& strVal,CLevelFiles& Level) const;
    virtual bool              ReadBin             (const CPersistent* pObj,CLevelFiles& Level) const;
    virtual bool              Link                (const CPersistent* pObj,const CStdString& strVal,CPersistent* pPointed) const;

    // direct access

    void*                     Get                 (const CPersistent* pObj) const;
    bool                      Set                 (const CPersistent* pObj, const void* pPtr) const;
  };
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropSP                                                             //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CPropSP : public CPropPtr
  {
  DECLARE_RTTI;

  // public methods

  public:

    // constructors & destructor

                              CPropSP             (const CStdString& strName,const u32 u32Offset,const u32 u32Size);
    virtual CProperty*        CreateCopy          (CProperty* pDest=NULL);

    // get/set params

    virtual const CStdString  GetType             (void) const;
    virtual u32               GetEnumType         (void) const;

    // streaming

    virtual bool              Link                (const CPersistent* pObj,const CStdString& strVal,CPersistent* pPointed) const;
  };
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CPropFct                                                            //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CPropFct : public CProperty
  {
  DECLARE_RTTI;

  // typedefs

  public:

    typedef const CStdString  (*pfnGet)           (const CPropFct* pProp,const CPersistent* pObj);
    typedef bool              (*pfnSet)           (const CPropFct* pProp,const CPersistent* pObj,const CStdString& strVal);
    typedef bool              (*pfnWrite)         (const CPropFct* pProp,const CPersistent* pObj,CLevelFiles& Level);
    typedef bool              (*pfnRead)          (const CPropFct* pProp,const CPersistent* pObj,const CStdString& strVal,CLevelFiles& Level);
    typedef bool              (*pfnLink)          (const CPropFct* pProp,const CPersistent* pObj,const CStdString& strVal,CPersistent* pPointed);

  // public methods

  public:

    // constructors & destructor

                              CPropFct            (const CStdString& strName,const u32 u32Offset,const u32 u32Size);
    virtual CProperty*        CreateCopy          (CProperty* pDest=NULL);

    // get/set params

    virtual const CStdString  GetType             (void) const;
    virtual u32               GetEnumType         (void) const;

    void                      SetFct              (pfnGet pGet,pfnSet pSet,pfnWrite pWrite,pfnRead pRead,pfnLink pLink);

    // value

    virtual const CStdString  GetValue            (const CPersistent* pObj) const;
    virtual bool              SetValue            (const CPersistent* pObj, const CStdString& strVal) const;
    virtual const CStdString  GetRealValue        (const CPersistent* pObj) const;

    // streaming

    virtual bool              Write               (const CPersistent* pObj,CLevelFiles& Level) const;
    virtual bool              WriteBin            (const CPersistent* pObj,CLevelFiles& Level) const;

    virtual bool              Read                (const CPersistent* pObj,const CStdString& strVal,CLevelFiles& Level) const;
    virtual bool              ReadBin             (const CPersistent* pObj,CLevelFiles& Level) const;

    virtual bool              Link                (const CPersistent* pObj,const CStdString& strVal,CPersistent* pPointed) const;

  // protected data

  protected:

    pfnGet                    m_pfnGet;
    pfnSet                    m_pfnSet;
    pfnWrite                  m_pfnWrite;
    pfnRead                   m_pfnRead;
    pfnLink                   m_pfnLink;
  };
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "Properties.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_PROPERTIES_H_
