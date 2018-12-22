//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITOR NODE CLASS                                                   //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_EDITORNODE_H_
#define   _GPG4_EDITORNODE_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "EditorObj.h"
#include  "Quaternion.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CEditorNode                                                         //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CEditorNode : public CEditorObj
  {
  DECLARE_RTTI;
  DECLARE_PROPERTIES(CEditorNode,CExtraProp);

  // public methods

  public:

    // constructors & destructor

                              CEditorNode         (void);
    virtual                  ~CEditorNode         (void);

    virtual bool              CreateEngineObj     (void);

    //

    virtual bool              Store               (void);
    virtual bool              Reset               (void);

    // get/set

    virtual CStdString        GetDefaultName      (void) const;
    virtual u32               GetIcon             (void) const;
    virtual u32               GetMenu             (void) const;

    // editing

    virtual bool              SpecialEditing      (CProperty* pProp,CStdString* pstrNew,CRTTIPropsDoc* pDoc);
    virtual CStdString        SpecialGetValue     (CProperty* pProp);

  // protected data

  protected:

    // stored state

    CQuaternion               m_qRot;
    CVect3D                   m_v3Pos;
    CVect3D                   m_v3Scale;
  };

  // smart ptr

  SMART_POINTER(CEditorNode);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "EditorNode.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_EDITORNODE_H_
