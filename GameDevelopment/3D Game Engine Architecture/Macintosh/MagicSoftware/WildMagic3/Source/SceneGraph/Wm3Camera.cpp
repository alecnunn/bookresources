// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Camera.h"
#include "Wm3Renderer.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,Camera,Spatial);
WM3_IMPLEMENT_STREAM(Camera);
WM3_IMPLEMENT_DEFAULT_NAME_ID(Camera,Spatial);

//----------------------------------------------------------------------------
Camera::Camera ()
{
    m_pkRenderer = 0;

    SetFrustum(-0.5f,0.5f,-0.5f,0.5f,1.0f,2.0f);
    SetViewPort(0.0f,1.0f,1.0f,0.0f);
    SetFrame(Vector3f::ZERO,-Vector3f::UNIT_Z,Vector3f::UNIT_Y,
        Vector3f::UNIT_X);

    m_uiPlaneState = (unsigned int)(~0);  // all planes initially active
    m_iPlaneQuantity = 6;  // frustum planes always processed for culling
}
//----------------------------------------------------------------------------
void Camera::SetFrame (const Vector3f& rkLocation, const Vector3f& rkDVector,
    const Vector3f& rkUVector, const Vector3f& rkRVector)
{
    Local.SetTranslate(rkLocation);
    Local.SetRotate(Matrix3f(rkDVector,rkUVector,rkRVector,true));
    OnFrameChange();
}
//----------------------------------------------------------------------------
void Camera::SetFrame (const Vector3f& rkLocation, const Matrix3f& rkAxes)
{
    Local.SetTranslate(rkLocation);
    Local.SetRotate(rkAxes);
    OnFrameChange();
}
//----------------------------------------------------------------------------
void Camera::SetLocation (const Vector3f& rkLocation)
{
    Local.SetTranslate(rkLocation);
    OnFrameChange();
}
//----------------------------------------------------------------------------
void Camera::SetAxes (const Vector3f& rkDVector, const Vector3f& rkUVector,
    const Vector3f& rkRVector)
{
    Local.SetRotate(Matrix3f(rkDVector,rkUVector,rkRVector,true));
    OnFrameChange();
}
//----------------------------------------------------------------------------
void Camera::SetAxes (const Matrix3f& rkAxes)
{
    Local.SetRotate(rkAxes);
    OnFrameChange();
}
//----------------------------------------------------------------------------
void Camera::UpdateWorldBound ()
{
    // The camera has an implicit model bound whose center is the camera's
    // position and whose radius is zero.
    WorldBound->SetCenter(World.ApplyForward(Local.GetTranslate()));
    WorldBound->SetRadius(0.0f);
}
//----------------------------------------------------------------------------
void Camera::SetFrustum (float fRMin, float fRMax, float fUMin, float fUMax,
    float fDMin, float fDMax)
{
    m_fDMin = fDMin;
    m_fDMax = fDMax;
    m_fUMin = fUMin;
    m_fUMax = fUMax;
    m_fRMin = fRMin;
    m_fRMax = fRMax;
    OnFrustumChange();
}
//----------------------------------------------------------------------------
void Camera::SetFrustum (float fUpFovDegrees, float fAspectRatio, float fDMin,
    float fDMax)
{
    float fHalfAngleRadians = 0.5f*fUpFovDegrees*Mathf::DEG_TO_RAD;
    m_fUMax = fDMin*Mathf::Tan(fHalfAngleRadians);
    m_fRMax = fAspectRatio*m_fUMax;
    m_fUMin = -m_fUMax;
    m_fRMin = -m_fRMax;
    m_fDMin = fDMin;
    m_fDMax = fDMax;
    OnFrustumChange();
}
//----------------------------------------------------------------------------
void Camera::GetFrustum (float& rfRMin, float& rfRMax, float& rfUMin,
    float& rfUMax, float& rfDMin, float& rfDMax) const
{
    rfDMin = m_fDMin;
    rfDMax = m_fDMax;
    rfUMin = m_fUMin;
    rfUMax = m_fUMax;
    rfRMin = m_fRMin;
    rfRMax = m_fRMax;
}
//----------------------------------------------------------------------------
void Camera::SetViewPort (float fLeft, float fRight, float fTop,
    float fBottom)
{
    m_fPortL = fLeft;
    m_fPortR = fRight;
    m_fPortT = fTop;
    m_fPortB = fBottom;
    OnViewPortChange();
}
//----------------------------------------------------------------------------
void Camera::GetViewPort (float& rfLeft, float& rfRight, float& rfTop,
    float& rfBottom)
{
    rfLeft = m_fPortL;
    rfRight = m_fPortR;
    rfTop = m_fPortT;
    rfBottom = m_fPortB;
}
//----------------------------------------------------------------------------
void Camera::OnFrustumChange ()
{
    float fDMinSqr = m_fDMin*m_fDMin, fInvLength;

    // left plane normal vector coefficients
    fInvLength = 1.0f/Mathf::Sqrt(fDMinSqr + m_fRMin*m_fRMin);
    m_afCoeffL[0] = -m_fRMin*fInvLength;  // D component
    m_afCoeffL[1] = +m_fDMin*fInvLength;  // R component

    // right plane normal vector coefficients
    fInvLength = 1.0f/Mathf::Sqrt(fDMinSqr + m_fRMax*m_fRMax);
    m_afCoeffR[0] = +m_fRMax*fInvLength;  // D component
    m_afCoeffR[1] = -m_fDMin*fInvLength;  // R component

    // bottom plane normal vector coefficients
    fInvLength = 1.0f/Mathf::Sqrt(fDMinSqr + m_fUMin*m_fUMin);
    m_afCoeffB[0] = -m_fUMin*fInvLength;  // D component
    m_afCoeffB[1] = +m_fDMin*fInvLength;  // U component

    // top plane normal vector coefficients
    fInvLength = 1.0f/Mathf::Sqrt(fDMinSqr + m_fUMax*m_fUMax);
    m_afCoeffT[0] = +m_fUMax*fInvLength;  // D component
    m_afCoeffT[1] = -m_fDMin*fInvLength;  // U component

    if ( m_pkRenderer )
        m_pkRenderer->OnFrustumChange();
}
//----------------------------------------------------------------------------
void Camera::OnViewPortChange ()
{
    if ( m_pkRenderer )
        m_pkRenderer->OnViewPortChange();
}
//----------------------------------------------------------------------------
void Camera::OnFrameChange ()
{
    // Get the world coordinate direction vectors for the camera.  The input
    // application time of MAX_REAL is based on knowing that OnFrameChange
    // was called via Camera interface functions, not through controllers.
    // If the camera frame is modified by controllers, the local transforms
    // are modified directly through the Spatial interface, so that interface
    // itself will update the world transforms.  The application time passed
    // through the Spatial interface will be different than MAX_REAL, so
    // the Update call in this function will not prevent a controller update.
    UpdateGS(-Mathd::MAX_REAL);
    Vector3f kLoc = World.GetTranslate();
    Vector3f kDVec = World.GetRotate().GetColumn(0);
    Vector3f kUVec = World.GetRotate().GetColumn(1);
    Vector3f kRVec = World.GetRotate().GetColumn(2);

    float fDdE = kDVec.Dot(kLoc);

    // left plane
    m_akWPlane[0].Normal = m_afCoeffL[0]*kDVec + m_afCoeffL[1]*kRVec;
    m_akWPlane[0].Constant = kLoc.Dot(m_akWPlane[0].Normal);

    // right plane
    m_akWPlane[1].Normal = m_afCoeffR[0]*kDVec + m_afCoeffR[1]*kRVec;
    m_akWPlane[1].Constant = kLoc.Dot(m_akWPlane[1].Normal);

    // bottom plane
    m_akWPlane[2].Normal = m_afCoeffB[0]*kDVec + m_afCoeffB[1]*kUVec;
    m_akWPlane[2].Constant = kLoc.Dot(m_akWPlane[2].Normal);

    // top plane
    m_akWPlane[3].Normal = m_afCoeffT[0]*kDVec + m_afCoeffT[1]*kUVec;
    m_akWPlane[3].Constant = kLoc.Dot(m_akWPlane[3].Normal);

    // near plane
    m_akWPlane[4].Normal = kDVec;
    m_akWPlane[4].Constant = fDdE + m_fDMin;

    // far plane
    m_akWPlane[5].Normal = -kDVec;
    m_akWPlane[5].Constant = -(fDdE + m_fDMax);

    if ( m_pkRenderer )
        m_pkRenderer->OnFrameChange();
}
//----------------------------------------------------------------------------
void Camera::PushPlane (const Plane3f& rkPlane)
{
    if ( m_iPlaneQuantity < CAM_MAX_WORLD_PLANES )
        m_akWPlane[m_iPlaneQuantity++] = rkPlane;
}
//----------------------------------------------------------------------------
void Camera::PopPlane ()
{
    if ( m_iPlaneQuantity > CAM_FRUSTUM_PLANES )
    {
        // frustum planes may not be removed from the stack
        m_iPlaneQuantity--;
    }
}
//----------------------------------------------------------------------------
bool Camera::Culled (const BoundingVolume* pkWBound)
{
    // start with last pushed plane (potentially the most restrictive plane)
    int iP = m_iPlaneQuantity - 1;
    unsigned int uiMask = 1 << iP;

    for (int i = 0; i < m_iPlaneQuantity; i++, iP--, uiMask >>= 1)
    {
        if ( m_uiPlaneState & uiMask )
        {
            int iSide = pkWBound->WhichSide(m_akWPlane[iP]);

            if ( iSide < 0 )
            {
                // Object is on negative side.  Cull it.
                return true;
            }

            if ( iSide > 0 )
            {
                // Object is on positive side of plane.  There is no need to
                // compare subobjects against this plane, so mark it as
                // inactive.
                m_uiPlaneState &= ~uiMask;
            }
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool Camera::Culled (int iVertexQuantity, const Vector3f* akVertex,
    bool bIgnoreNearPlane)
{
    // The Boolean variable bIgnoreNearPlane should be set to 'true' when
    // the test polygon is a portal.  This avoids the situation when the
    // portal is in the view pyramid (eye+left/right/top/bottom), but is
    // between the eye and near plane.  In such a situation you do not want
    // the portal system to cull the portal.  This situation typically occurs
    // when the camera moves through the portal from current region to
    // adjacent region.

    // start with last pushed plane (potentially the most restrictive plane)
    int iP = m_iPlaneQuantity - 1;
    for (int i = 0; i < m_iPlaneQuantity; i++, iP--)
    {
        Plane3f& rkPlane = m_akWPlane[iP];
        if ( bIgnoreNearPlane && iP == 4 /* camera near plane */ )
            continue;

        int iV;
        for (iV = 0; iV < iVertexQuantity; iV++)
        {
            int iSide = rkPlane.WhichSide(akVertex[iV]);
            if ( iSide >= 0 )
            {
                // polygon is not totally outside this plane
                break;
            }
        }

        if ( iV == iVertexQuantity )
        {
            // polygon is totally outside this plane
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
int Camera::WhichSide (const Plane3f& rkPlane) const
{
    // The plane is N*(X-C) = 0 where the * indicates dot product.  The signed
    // distance from the camera location E to the plane is N*(E-C).
    float fNdEmC = rkPlane.DistanceTo(GetWorldLocation());

    float fNdD = rkPlane.Normal.Dot(GetWorldDVector());
    float fNdU = rkPlane.Normal.Dot(GetWorldUVector());
    float fNdR = rkPlane.Normal.Dot(GetWorldRVector());
    float fFdN = m_fDMax/m_fDMin;

    int iPositive = 0, iNegative = 0;
    float fSgnDist;

    // check near-plane vertices
    float fPDMin = m_fDMin*fNdD;
    float fNUMin = m_fUMin*fNdU;
    float fNUMax = m_fUMax*fNdU;
    float fNRMin = m_fRMin*fNdR;
    float fNRMax = m_fRMax*fNdR;

    // V = E + dmin*D + umin*U + rmin*R
    // N*(V-C) = N*(E-C) + dmin*(N*D) + umin*(N*U) + rmin*(N*R)
    fSgnDist = fNdEmC + fPDMin + fNUMin + fNRMin;
    if ( fSgnDist > 0.0f )
        iPositive++;
    else if ( fSgnDist < 0.0f )
        iNegative++;

    // V = E + dmin*D + umin*U + rmax*R
    // N*(V-C) = N*(E-C) + dmin*(N*D) + umin*(N*U) + rmax*(N*R)
    fSgnDist = fNdEmC + fPDMin + fNUMin + fNRMax;
    if ( fSgnDist > 0.0f )
        iPositive++;
    else if ( fSgnDist < 0.0f )
        iNegative++;

    // V = E + dmin*D + umax*U + rmin*R
    // N*(V-C) = N*(E-C) + dmin*(N*D) + umax*(N*U) + rmin*(N*R)
    fSgnDist = fNdEmC + fPDMin + fNUMax + fNRMin;
    if ( fSgnDist > 0.0f )
        iPositive++;
    else if ( fSgnDist < 0.0f )
        iNegative++;

    // V = E + dmin*D + umax*U + rmax*R
    // N*(V-C) = N*(E-C) + dmin*(N*D) + umax*(N*U) + rmax*(N*R)
    fSgnDist = fNdEmC + fPDMin + fNUMax + fNRMax;
    if ( fSgnDist > 0.0f )
        iPositive++;
    else if ( fSgnDist < 0.0f )
        iNegative++;

    // check far-plane vertices (s = dmax/dmin)
    float fPDMax = m_fDMax*fNdD;
    float fFUMin = fFdN*fNUMin;
    float fFUMax = fFdN*fNUMax;
    float fFRMin = fFdN*fNRMin;
    float fFRMax = fFdN*fNRMax;

    // V = E + dmax*D + umin*U + rmin*R
    // N*(V-C) = N*(E-C) + dmax*(N*D) + s*umin*(N*U) + s*rmin*(N*R)
    fSgnDist = fNdEmC + fPDMax + fFUMin + fFRMin;
    if ( fSgnDist > 0.0f )
        iPositive++;
    else if ( fSgnDist < 0.0f )
        iNegative++;

    // V = E + dmax*D + umin*U + rmax*R
    // N*(V-C) = N*(E-C) + dmax*(N*D) + s*umin*(N*U) + s*rmax*(N*R)
    fSgnDist = fNdEmC + fPDMax + fFUMin + fFRMax;
    if ( fSgnDist > 0.0f )
        iPositive++;
    else if ( fSgnDist < 0.0f )
        iNegative++;

    // V = E + dmax*D + umax*U + rmin*R
    // N*(V-C) = N*(E-C) + dmax*(N*D) + s*umax*(N*U) + s*rmin*(N*R)
    fSgnDist = fNdEmC + fPDMax + fFUMax + fFRMin;
    if ( fSgnDist > 0.0f )
        iPositive++;
    else if ( fSgnDist < 0.0f )
        iNegative++;

    // V = E + dmax*D + umax*U + rmax*R
    // N*(V-C) = N*(E-C) + dmax*(N*D) + s*umax*(N*U) + s*rmax*(N*R)
    fSgnDist = fNdEmC + fPDMax + fFUMax + fFRMax;
    if ( fSgnDist > 0.0f )
        iPositive++;
    else if ( fSgnDist < 0.0f )
        iNegative++;

    if ( iPositive > 0 )
    {
        if ( iNegative > 0 )
        {
            // frustum straddles the plane
            return 0;
        }

        // frustum is fully on the positive side
        return +1;
    }

    // frustum is fully on the negative side
    return -1;
}
//----------------------------------------------------------------------------
bool Camera::GetPickRay (int iX, int iY, int iWidth, int iHeight,
    Ray3f& rkRay) const
{
    float fPortX = ((float)iX)/(float)(iWidth-1);
    if ( fPortX < m_fPortL || fPortX > m_fPortR )
        return false;

    float fPortY = ((float)(iHeight-1-iY))/(float)(iHeight-1);
    if ( fPortY < m_fPortB || fPortY > m_fPortT )
        return false;

    float fXWeight = (fPortX - m_fPortL)/(m_fPortR - m_fPortL);
    float fViewX = (1.0f-fXWeight)*m_fRMin + fXWeight*m_fRMax;
    float fYWeight = (fPortY - m_fPortB)/(m_fPortT - m_fPortB);
    float fViewY = (1.0f-fYWeight)*m_fUMin + fYWeight*m_fUMax;

    rkRay.Origin = GetWorldLocation();
    rkRay.Direction = m_fDMin*GetWorldDVector() + fViewX*GetWorldRVector() +
        fViewY*GetWorldUVector();
    rkRay.Direction.Normalize();
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Camera::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Spatial::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_fDMin);
    rkStream.Read(m_fDMax);
    rkStream.Read(m_fUMin);
    rkStream.Read(m_fUMax);
    rkStream.Read(m_fRMin);
    rkStream.Read(m_fRMax);
    rkStream.Read(m_fPortL);
    rkStream.Read(m_fPortR);
    rkStream.Read(m_fPortT);
    rkStream.Read(m_fPortB);
    rkStream.Read(m_iPlaneQuantity);
    for (int i = 0; i < m_iPlaneQuantity; i++)
        rkStream.Read(m_akWPlane[i]);

    WM3_END_DEBUG_STREAM_LOAD(Camera);
}
//----------------------------------------------------------------------------
void Camera::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Spatial::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool Camera::Register (Stream& rkStream) const
{
    return Spatial::Register(rkStream);
}
//----------------------------------------------------------------------------
void Camera::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;
    
    Spatial::Save(rkStream);

    // native data
    rkStream.Write(m_fDMin);
    rkStream.Write(m_fDMax);
    rkStream.Write(m_fUMin);
    rkStream.Write(m_fUMax);
    rkStream.Write(m_fRMin);
    rkStream.Write(m_fRMax);
    rkStream.Write(m_fPortL);
    rkStream.Write(m_fPortR);
    rkStream.Write(m_fPortT);
    rkStream.Write(m_fPortB);
    rkStream.Write(m_iPlaneQuantity);
    for (int i = 0; i < m_iPlaneQuantity; i++)
        rkStream.Write(m_akWPlane[i]);

    WM3_END_DEBUG_STREAM_SAVE(Camera);
}
//----------------------------------------------------------------------------
StringTree* Camera::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(3,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));

    char acDummy[512];
    sprintf(acDummy,"dmin: %f, dmax:%f, umin:%f, umax:%f, rmin:%f, rmax:%f",
        m_fDMin,m_fDMax,m_fUMin,m_fUMax,m_fRMin,m_fRMax);
    pkTree->SetString(1,StringTree::Format("frustum =",acDummy));

    sprintf(acDummy,"l: %f, r:%f, t:%f, b:%f",m_fPortL,m_fPortR,m_fPortT,
        m_fPortB);
    pkTree->SetString(2,StringTree::Format("viewport =",acDummy));

    // children
    pkTree->SetChild(0,Spatial::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int Camera::GetMemoryUsed () const
{
    return sizeof(Camera) - sizeof(Spatial) + Spatial::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int Camera::GetDiskUsed () const
{
    return Spatial::GetDiskUsed() +
        sizeof(m_fDMin) +
        sizeof(m_fDMax) +
        sizeof(m_fUMin) +
        sizeof(m_fUMax) +
        sizeof(m_fRMin) +
        sizeof(m_fRMax) +
        sizeof(m_fPortL) +
        sizeof(m_fPortR) +
        sizeof(m_fPortT) +
        sizeof(m_fPortB) +
        sizeof(int) + m_iPlaneQuantity*sizeof(m_akWPlane[0]);
}
//----------------------------------------------------------------------------
