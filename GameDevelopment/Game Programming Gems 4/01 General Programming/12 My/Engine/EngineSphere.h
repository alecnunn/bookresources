//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE SPHERE CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_ENGINESPHERE_H_
#define   _GPG4_ENGINESPHERE_H_

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
//                                                CEngineSphere                                                       //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CEngineSphere : public CEngineNode
  {
  DECLARE_RTTI;
  DECLARE_PROPERTIES(CEngineSphere,CExtraProp);

  // public methods

  public:

    // constructors & destructor

                              CEngineSphere       (void);
    virtual                  ~CEngineSphere       (void);

    // management

    virtual bool              Create              (IRenderer* pRenderer,const float fRadius,const u32 u32SectionPts);
    virtual CEngineSphere*    Clone               (IRenderer* pRenderer);

    // get/set

    virtual CStdString        GetEditName         (void) const;
    float                     GetRadius           (void) const;
    u32                       GetSectionPts       (void) const;

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

    float                     m_fRadius;
    u32                       m_u32SectionPts;
    CEngineMeshSP             m_spMesh;
  };

  // smart ptr

  SMART_POINTER(CEngineSphere);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "EngineSphere.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_ENGINESPHERE_H_
