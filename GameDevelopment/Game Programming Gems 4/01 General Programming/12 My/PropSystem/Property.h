//--------------------------------------------------------------------------------------------------------------------//
//                                                PROPERTY BASE CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_PROPERTY_H_
#define   _GPG4_PROPERTY_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "BasicTypes.h"
#include  "Defines.h"
#include  "RTTI.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class CLevelFiles;
  class CPersistent;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CProperty                                                           //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CProperty
  {
  DECLARE_ROOT_RTTI;


  //

  public:

    enum
      {
      EXPOSE        = 1 << 0,
      STREAM        = 1 << 1,
      READONLY      = 1 << 2,
      }
    FLAGS;

    enum
      {
      NONE          = 0,
      BUTTON,
      COLOR,
      }
    CONTROL;

  // public methods

  public:

    // constructors & destructor

                              CProperty           (const CStdString& strName,const u32 u32Offset,const u32 u32Size);
    virtual                  ~CProperty           (void);
    virtual CProperty*        CreateCopy          (CProperty* pDest=NULL);

    // get/set params

    virtual const CStdString  GetType             (void) const = 0;
    virtual u32               GetEnumType         (void) const = 0;
    virtual const CStdString& GetName             (void) const;
    virtual u32               GetOffset           (void) const;
    virtual u32               GetSize             (void) const;

    virtual const CStdString& GetHelp             (void) const;
    virtual void              SetHelp             (const CStdString& strHelp);

    // value

    virtual const CStdString  GetValue            (const CPersistent*  pObj) const;
    virtual bool              SetValue            (const CPersistent*  pObj, const CStdString& strVal) const = 0;
    virtual const CStdString  GetRealValue        (const CPersistent*  pObj) const = 0;

    // flags

    bool                      IsExposed           (void) const;
    bool                      IsStreamed          (void) const;
    bool                      IsReadOnly          (void) const;
    void                      SetFlags            (const u32 u32Flags);

    // control for editing

    u32                       GetCtrlType         (void) const;
    void                      SetCtrlType         (const u32 u32Type);

    // streaming

    virtual bool              Write               (const CPersistent* pObj,CLevelFiles& Level) const;
    virtual bool              WriteBin            (const CPersistent* pObj,CLevelFiles& Level) const = 0;
    virtual bool              Read                (const CPersistent* pObj,const CStdString& strVal,CLevelFiles& Level)    const;
    virtual bool              ReadBin             (const CPersistent* pObj,CLevelFiles& Level) const = 0;
    virtual bool              Link                (const CPersistent* pObj,const CStdString& strVal,CPersistent* pPointed) const;

  // protected data

  protected:

    CStdString                m_strName;
    u32                       m_u32Offset;                  // var offset in class
    u32                       m_u32Size;                    // var size
    CStdString                m_strHelp;

    struct
    {
      bool                    m_boExpose    : 1;            // prop appears in grid
      bool                    m_boStream    : 1;            // prop is loaded/saved
      bool                    m_boReadOnly  : 1;            // prop can't be edited
    };

    u32                       m_u32CtrlType;
  };
}

//----------------------------------------------- MACROS -------------------------------------------------------------//

#define PROPERTY_CREATECOPY(Name)                            \
  CProperty* CProp##Name::CreateCopy(CProperty* pDest)       \
    {                                                        \
    CProp##Name* pCopy = DYNAMIC_CAST(CProp##Name,pDest);    \
    if(!pCopy)                                               \
      {                                                      \
      pCopy = new CProp##Name("",0,0);                       \
      if(!pCopy) return NULL;                                \
      }                                                      \
                                                             \
    return CProperty::CreateCopy(pCopy);                     \
    }

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "Property.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_PROPERTY_H_
