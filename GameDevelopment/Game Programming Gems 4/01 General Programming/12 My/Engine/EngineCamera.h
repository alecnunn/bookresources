//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE CAMERA CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG_ENGINECAMERA_H_
#define   _GPG_ENGINECAMERA_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "EngineNode.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CEngineCamera                                                       //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CEngineCamera : public CEngineNode
  {
  DECLARE_RTTI;
  DECLARE_PROPERTIES(CEngineCamera,CExtraProp);

  // public methods

  public:

    // constructors & destructor

                              CEngineCamera       (void);
    virtual                  ~CEngineCamera       (void);

    virtual CStdString        GetEditName         (void) const;

    // trfs get/set

    virtual bool              SetPosition         (const CVect3D& v3Pos);
    virtual bool              SetRotation         (const CQuaternion& qRot);

    virtual bool              SetTarget           (const CVect3D& v3Target);
    virtual bool              SetUp               (const CVect3D& v3Up);
    virtual bool              SetRepere           (const CVect3D& v3Pos,const CVect3D& v3Target,const CVect3D& v3Up);
    virtual bool              SetInitialUp        (const CVect3D& v3Up);

    virtual const CVect3D&    GetTarget           (void) const;
    virtual const CVect3D&    GetUp               (void) const;
    virtual const CVect3D&    GetInitialUp        (void) const;
    virtual const CVect3D     GetWorldTarget      (void);

    // properties get/set

    float                     GetFov              (void) const;
    float                     GetRatio            (void) const;
    float                     GetNear             (void) const;
    float                     GetFar              (void) const;

    bool                      IsPerspective       (void) const;
    float                     GetScaleOrtho       (void) const;
    bool                      CanRotate           (void) const;
    bool                      CanBePerspective    (void) const;

    void                      SetFov              (const float fFov);
    void                      SetRatio            (const float fRatio);
    void                      SetNear             (const float fNear);
    void                      SetFar              (const float fFar);

    void                      SetPerspective      (const bool  boPersp);
    void                      SetScaleOrtho       (const float fScale);
    void                      AllowRotations      (const bool  boRotate);
    void                      AllowPerspective    (const bool  boPersp);

    // editing

    virtual bool              ModifyProp          (CProperty* pProp,const CStdString& strNew);
    virtual bool              IsPropExposed       (CProperty* pProp);
    virtual bool              IsPropReadOnly      (CProperty* pProp);

  // protected methods

  protected:

    bool                      RecalcUp            (void);
    bool                      RecalcRotation      (void);

  // protected data

  protected:

    // trf

    CVect3D                   m_v3Target;
    CVect3D                   m_v3Up;
    CVect3D                   m_v3InitialUp;                // dir can't become parallel to this vector (when rotating)

    // properties

    float                     m_fFov;
    float                     m_fRatio;
    float                     m_fNear,m_fFar;

    bool                      m_boPerspective;
    float                     m_fScaleOrtho;
    bool                      m_boRotations;
    bool                      m_boPerspAllowed;
  };

  // smart ptr

  SMART_POINTER(CEngineCamera);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "EngineCamera.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG_ENGINECAMERA_H_
