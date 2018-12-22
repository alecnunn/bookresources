//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE NODE CLASS                                                   //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_ENGINENODE_H_
#define   _GPG4_ENGINENODE_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "EngineObj.h"
#include  "Quaternion.h"
#include  "VectorMatrix.h"
#include  "StlIncludes.h"

//----------------------------------------------- TYPEDEFS -----------------------------------------------------------//

typedef   bool (*pfnDrawCondition) (GPG4::CEngineObj* pEngineObj);

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class CEngineMesh;
  class CEngineNode;
  class CPropFct;
  class IRenderer;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CEngineNode                                                         //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  SMART_POINTER(CEngineMesh);
  SMART_POINTER(CEngineNode);

  class CEngineNode : public CEngineObj
  {
  DECLARE_RTTI;
  DECLARE_PROPERTIES(CEngineNode,CExtraProp);

  // typedefs

  public:

    typedef std::list< CEngineNodeSP >            listNode;
    typedef listNode::iterator                    iterNode;

  // public enums

  public:

    typedef enum
      {
      NONE                    = 0,
      SELECTED                = 1 << 0,
      UNSELECTED              = 1 << 1,
      } RENDERING_FLAGS;

  // public methods

  public:

    // constructors & destructor

                              CEngineNode         (void);
    virtual                  ~CEngineNode         (void);

    // trfs get/set

    virtual const CVect3D&    GetPosition         (void) const;
    virtual const CQuaternion&GetRotation         (void) const;
    virtual const CVect3D&    GetScale            (void) const;

    virtual       CVect3D     GetWorldPosition    (void);
    virtual       CQuaternion GetWorldRotation    (void);
    virtual       CVect3D     GetWorldScale       (void);

    virtual const CMat4x4&    GetLocalTrf         (void);
    virtual const CMat4x4&    GetWorldTrf         (void);
    virtual bool              IsLocalDirty        (void) const;
    virtual bool              IsWorldDirty        (void) const;

    virtual bool              SetPosition         (const CVect3D& v3Pos);
    virtual bool              SetRotation         (const CQuaternion& qRot);
    virtual bool              SetScale            (const CVect3D& v3Scale);

    virtual bool              SetWorldPosition    (const CVect3D& v3Pos);
    virtual bool              SetWorldRotation    (const CQuaternion& qRot);

    virtual const CMat4x4&    SetLocalTrf         (const CVect3D& v3Pos,const CQuaternion& qRot,const CVect3D& v3Scale);
    virtual const CMat4x4&    SetLocalTrf         (const CVect3D& v3Pos,const CQuaternion& qRot,const CVect3D& v3Scale,const CMat4x4& m4Trf);

    virtual void              SetLocalDirty       (void);
    virtual void              SetWorldDirty       (void);
    virtual const CMat4x4&    UpdateLocalTrf      (void);
    virtual const CMat4x4&    UpdateWorldTrf      (void);

    // flags get/set

    bool                      GetDrawNode         (void) const;
    void                      SetDrawNode         (const bool boDraw);
    bool                      GetDrawMesh         (void) const;
    void                      SetDrawMesh         (const bool boDraw);
    bool                      GetCollide          (void) const;
    void                      SetCollide          (const bool boColl);

    virtual CStdString        GetEditName         (void) const;

    // meshes

    CEngineMeshSP             GetMesh             (void) const;
    CEngineMeshSP             GetEditMesh         (void) const;

    // subnodes

    CEngineNode*              GetParentNode       (void) const;
    void                      SetParentNode       (CEngineNode* pParent);

    u32                       GetNbSubnodes       (void) const;
    CEngineNode*              GetFirstSubnode     (void);
    CEngineNode*              GetNextSubnode      (CEngineNode* pCurrent=NULL);

    bool                      AddSubnode          (CEngineNode* pNode);
    bool                      RemoveSubnode       (CEngineNode* pNode);

    // draw

    virtual u32               Draw                (IRenderer* pRenderer,const u32 u32Flags,pfnDrawCondition pfnCondition);

    // BVs

    bool                      HasBV               (void);

    // streaming

    static bool               WriteNodes          (const CPropFct* pProp,const CPersistent* pObj,CLevelFiles& Level);
    static bool               ReadNodes           (const CPropFct* pProp,const CPersistent* pObj,const CStdString& strVal,CLevelFiles& Level);
    static bool               LinkNodes           (const CPropFct* pProp,const CPersistent* pObj,const CStdString& strVal,CPersistent* pPointed);
    virtual bool              PostRead            (void);

    // editing

    virtual bool              ModifyProp          (CProperty* pProp,const CStdString& strNew);

  // protected data

  protected:

    // local & world trfs

    CQuaternion               m_qRot;
    CVect3D                   m_v3Pos;
    CVect3D                   m_v3Scale;

    CMat4x4                   m_m4Local;                    // updated by GetLocalTrf & UpdateLocalTrf
    CMat4x4                   m_m4World;                    // updated by GetWorldTrf & UpdateWorldTrf
    bool                      m_boLocalDirty;
    bool                      m_boWorldDirty;

    // subnodes

    CEngineNode*              m_pParentNode;
    listNode                  m_Subnodes;
    iterNode                  m_CurrSubnode;

    // flags

    bool                      m_boDrawNode;
    bool                      m_boDrawMesh;
    bool                      m_boCollide;
  };

  SMART_POINTER(CEngineNode);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "EngineNode.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_ENGINENODE_H_
