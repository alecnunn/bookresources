//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE CAMERA CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_ENGINECAMERA_INL_
#define   _GPG4_ENGINECAMERA_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CEngineCamera::~CEngineCamera()
  {}

INLINE CStdString CEngineCamera::GetEditName() const
  {
  return "Camera";
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                TRFS GET/SET                                                        //
//--------------------------------------------------------------------------------------------------------------------//

INLINE const CVect3D& CEngineCamera::GetTarget() const
  {
  return m_v3Target;
  }

INLINE const CVect3D& CEngineCamera::GetUp() const
  {
  return m_v3Up;
  }

INLINE const CVect3D& CEngineCamera::GetInitialUp() const
  {
  return m_v3InitialUp;
  }

INLINE const CVect3D CEngineCamera::GetWorldTarget()
  {
  if(!m_pParentNode) return m_v3Target;
  return CVect3D(m_pParentNode->GetWorldTrf()*m_v3Target,W);
  }

INLINE bool CEngineCamera::SetInitialUp(const CVect3D& v3Up)
  {
  m_v3InitialUp = v3Up;
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                PROPERTIES GET/SET                                                  //
//--------------------------------------------------------------------------------------------------------------------//

INLINE float CEngineCamera::GetFov() const
  {
  return m_fFov;
  }

INLINE float CEngineCamera::GetRatio() const
  {
  return m_fRatio;
  }

INLINE float CEngineCamera::GetNear() const
  {
  return m_fNear;
  }

INLINE float CEngineCamera::GetFar() const
  {
  return m_fFar;
  }

INLINE bool CEngineCamera::IsPerspective() const
  {
  return m_boPerspective;
  }

INLINE float CEngineCamera::GetScaleOrtho() const
  {
  return m_fScaleOrtho;
  }

INLINE bool CEngineCamera::CanRotate() const
  {
  return m_boRotations;
  }

INLINE bool CEngineCamera::CanBePerspective() const
  {
  return m_boPerspAllowed;
  }

//

INLINE void CEngineCamera::SetFov(const float fFov)
  {
  m_fFov = fFov;
  }

INLINE void CEngineCamera::SetRatio(const float fRatio)
  {
  m_fRatio = fRatio;
  }

INLINE void CEngineCamera::SetNear(const float fNear)
  {
  m_fNear = fNear;
  }

INLINE void CEngineCamera::SetFar(const float fFar)
  {
  m_fFar = fFar;
  }

INLINE void CEngineCamera::SetPerspective(const bool boPersp)
  {
  m_boPerspective = boPersp;
  }

INLINE void CEngineCamera::SetScaleOrtho(const float fScale)
  {
  m_fScaleOrtho = fScale;
  }

INLINE void CEngineCamera::AllowRotations(const bool boRotate)
  {
  m_boRotations = boRotate;
  }

INLINE void CEngineCamera::AllowPerspective(const bool boPersp)
  {
  m_boPerspAllowed = boPersp;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_ENGINECAMERA_INL_
