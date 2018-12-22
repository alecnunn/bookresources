//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE OBJECT CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_ENGINEOBJ_H_
#define   _GPG4_ENGINEOBJ_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "Persistent.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class CEditorObj;
  class CRTTIMap;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CEngineObj                                                          //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CEngineObj : public CPersistent
  {
  DECLARE_RTTI;
  DECLARE_PROPERTIES(CEngineObj,CExtraProp);

  // public methods

  public:

    // constructors & destructor

                              CEngineObj          (void);
    virtual                  ~CEngineObj          (void);

    static CEngineObj*        CreateObject        (const CRTTI* pRTTI);
    static CEngineObj*        CreateObject        (const CStdString& strClassName);

    // get/set

    bool                      IsDeleted           (void) const;
    void                      SetDeleted          (const bool boDeleted);

    CEditorObj*               GetEditorObj        (void) const;
    virtual void              SetEditorObj        (CEditorObj* pEditorObj);

    // RTTI map

    static CRTTIMap*          GetRTTIMap          (void);
    static bool               FillRTTIMap         (CRTTIMap& map);

  // protected data

  protected:

    bool                      m_boDeleted;                  // don't use, but still in memory (restore or avoid crash)
    CEditorObj*               m_pEditorObj;                 // corresponding editor object - this should only be used by the editor code
  };

  // smart ptr

  SMART_POINTER(CEngineObj);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "EngineObj.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_ENGINEOBJ_H_
