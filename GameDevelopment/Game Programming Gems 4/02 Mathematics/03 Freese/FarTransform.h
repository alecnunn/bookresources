#ifndef   INCLUDED_FarTransform
#define   INCLUDED_FarTransform
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MathTypes.h"
#include "FarPosition.h"

class Matrix4x4;
class Quaternion;
class Transform;

class FarTransform
{
public:
    FarTransform() : m_position(Zero), m_bPositionDirty(false), m_basisSegment(Zero) {};
    ~FarTransform() {};

    void SetQuaternion( const Quaternion& q ) { m_localTransform.SetQuaternion(q); }
    void SetScale( float fScale ) { m_localTransform.SetScale(fScale); }
    void SetPosition( const FarPosition &p ) { m_position = p; m_bPositionDirty = true; }
    void SetBasisSegment( FarSegment segment ) 
    {
        if ( segment != m_basisSegment )
        {
            m_bPositionDirty = true;
            m_basisSegment = segment;
        }
    }

    // accessors
    const Quaternion&   GetQuaternion() const   { return m_localTransform.GetQuaternion(); }
    const FarPosition&  GetPosition() const     { return m_position; }
    const float         GetScale() const        { return m_localTransform.GetScale(); }
    const FarSegment    GetBasisSegment() const { return m_basisSegment; }
    const Matrix4x4&    GetMatrix() const       { if (m_bPositionDirty) UpdateLocalPosition(); return m_localTransform.GetMatrix(); }
    const Matrix4x4&    GetMatrixInverse() const { if (m_bPositionDirty) UpdateLocalPosition(); return m_localTransform.GetMatrixInverse(); }
    const matTransform& GetLocalTransform() const { if (m_bPositionDirty) UpdateLocalPosition(); return m_localTransform; }

    // non-const versions allow frobbing
    Quaternion&  ModifyQuaternion()  { return m_localTransform.ModifyQuaternion(); }
    FarPosition& ModifyPosition()    { m_bPositionDirty = true; return m_position; }

    // conversion from local space to world space
    FarPosition TransformPoint( const Vector3& vOffset ) const;

    // pre-multiply a local transform
    FarTransform& PreTransform( const matTransform& lhs );

    void Translate( const Vector3& vOffsetWorld );
    void Rotate( const Quaternion &q );
    void RotateEuler( float fYaw, float fPitch, float fRoll );
    void LocalTranslate( const Vector3& vOffsetLocal );
    void LocalRotate( const Quaternion &q );
    void LocalRotateEuler( float fYaw, float fPitch, float fRoll );

    void SetEulerAngles( float fYaw, float fPitch, float fRoll );

protected:
    void UpdateLocalPosition() const;
    
private:
    FarPosition             m_position;
    mutable bool            m_bPositionDirty;
    mutable Transform       m_localTransform;
    mutable FarSegment      m_basisSegment;
};

#endif // INCLUDED_FarTransform
