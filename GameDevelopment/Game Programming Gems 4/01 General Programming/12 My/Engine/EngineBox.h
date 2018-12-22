//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE BOX CLASS                                                    //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_ENGINEBOX_H_
#define   _GPG4_ENGINEBOX_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "EngineNode.h"
#include  "EngineMesh.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class CPropFct;
  class IRenderer;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CEngineBox                                                          //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CEngineBox : public CEngineNode
  {
  DECLARE_RTTI;
  DECLARE_PROPERTIES(CEngineBox,CExtraProp);

  // public methods

  public:

    // constructors & destructor

                              CEngineBox          (void);
    virtual                  ~CEngineBox          (void);

    // management

    virtual bool              Create              (IRenderer* pRenderer,const CVect3D& v3Extent);
    virtual CEngineBox*       Clone               (IRenderer* pRenderer);

    // get/set

    virtual CStdString        GetEditName         (void) const;
    const CVect3D&            GetExtent           (void) const;
    CEngineMeshSP             GetMesh             (void) const;
    CEngineMaterial*          GetMaterial         (void) const;
    void                      SetMaterial         (CEngineMaterial* pMat);

    // draw

    virtual u32               Draw                (IRenderer* pRenderer,const u32 u32Flags,pfnDrawCondition pfnCondition);

    // streaming

    static const CStdString   PropGetMat          (const CPropFct* pProp,const CPersistent* pObj);
    static bool               PropSetMat          (const CPropFct* pProp,const CPersistent* pObj,const CStdString& strVal);
    static bool               PropWriteMat        (const CPropFct* pProp,const CPersistent* pObj,CLevelFiles& Level);
    static bool               PropReadMat         (const CPropFct* pProp,const CPersistent* pObj,const CStdString& strVal,CLevelFiles& Level);
    static bool               PropBuildMesh       (const CPropFct* pProp,const CPersistent* pObj,const CStdString& strVal,CPersistent* pPointed);
    virtual bool              PostRead            (void);

    // editing

    virtual bool              ModifyProp          (CProperty* pProp,const CStdString& strNew);

  // protected data

  protected:

    CVect3D                   m_v3Extent;
    CEngineMeshSP             m_spMesh;
  };

  // smart ptr

  SMART_POINTER(CEngineBox);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "EngineBox.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_ENGINEBOX_H_
