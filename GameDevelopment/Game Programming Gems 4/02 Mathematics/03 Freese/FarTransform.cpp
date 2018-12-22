#include "FarTransform.h"

void FarTransform::UpdateLocalPosition() const
{
    // store position
    m_localTransform.SetPosition( m_position.GetRelativeVector(m_basisSegment) );
    m_bPositionDirty = false;
}


FarPosition FarTransform::TransformPoint( const Vector3& vOffset ) const
{
    FarPosition worldPosition;
    worldPosition.SetSegment(GetBasisSegment());
    worldPosition.SetOffset(GetLocalTransform().TransformPoint(vOffset));
    worldPosition.Normalize();
    return worldPosition;
}


FarTransform& FarTransform::PreTransform( const matTransform& lhs )
{
    m_position = TransformPoint(lhs.GetPosition());
    ModifyQuaternion().PreMultiply(lhs.GetQuaternion());
    SetScale(lhs.GetScale() * GetScale());
    m_bPositionDirty = true;    // flag position in local transform as needing update
    return *this;
}


void FarTransform::Translate( const Vector3& vDeltaWorld )
{
    m_position.Translate(vDeltaWorld);
    m_bPositionDirty = true;
}


void FarTransform::Rotate( const matQuaternion &qRotation )
{
    m_localTransform.ModifyQuaternion().Multiply(qRotation);
}


void FarTransform::RotateEuler( float fYaw, float fPitch, float fRoll )
{
    matQuaternion q;
    q.StoreEulerAngles(fYaw, fPitch, fRoll);
    m_localTransform.ModifyQuaternion().Multiply(q);
}


void FarTransform::LocalTranslate( const Vector3 &vDeltaLocal )
{
    m_position.Translate(GetLocalTransform().TransformVector(vDeltaLocal));
    m_bPositionDirty = true;
}


void FarTransform::LocalRotate( const matQuaternion &qRotation )
{
    m_localTransform.ModifyQuaternion().PreMultiply(qRotation);
}


void FarTransform::LocalRotateEuler( float fYaw, float fPitch, float fRoll )
{
    matQuaternion q;
    q.StoreEulerAngles(fYaw, fPitch, fRoll);
    m_localTransform.ModifyQuaternion().PreMultiply(q);
}


void FarTransform::SetEulerAngles( float fYaw, float fPitch, float fRoll )
{
    m_localTransform.ModifyQuaternion().StoreEulerAngles(fYaw, fPitch, fRoll);
}
