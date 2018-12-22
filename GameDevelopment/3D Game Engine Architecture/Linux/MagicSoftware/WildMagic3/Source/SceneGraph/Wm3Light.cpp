// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Light.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,Light,Spatial);
WM3_IMPLEMENT_STREAM(Light);
WM3_IMPLEMENT_DEFAULT_NAME_ID(Light,Spatial);

//----------------------------------------------------------------------------
Light::Light (int iType)
    :
    Ambient(ColorRGBA::BLACK),
    Diffuse(ColorRGBA::BLACK),
    Specular(ColorRGBA::BLACK)
{
    SetFrame(Vector3f::ZERO,-Vector3f::UNIT_Z,Vector3f::UNIT_Y,
        Vector3f::UNIT_X);

    Type = iType;
    Intensity = 1.0f;
    Constant = 1.0f;
    Linear = 0.0f;
    Quadratic = 0.0f;
    Attenuate = false;
    On = true;

    // spot light parameters
    Exponent = 0.0f;
    Angle = Mathf::PI;
}
//----------------------------------------------------------------------------
Light::~Light ()
{
}
//----------------------------------------------------------------------------
void Light::SetFrame (const Vector3f& rkLocation, const Vector3f& rkDVector,
    const Vector3f& rkUVector, const Vector3f& rkRVector)
{
    Local.SetTranslate(rkLocation);
    Local.SetRotate(Matrix3f(rkDVector,rkUVector,rkRVector,true));
    OnFrameChange();
}
//----------------------------------------------------------------------------
void Light::SetFrame (const Vector3f& rkLocation, const Matrix3f& rkAxes)
{
    Local.SetTranslate(rkLocation);
    Local.SetRotate(rkAxes);
    OnFrameChange();
}
//----------------------------------------------------------------------------
void Light::SetLocation (const Vector3f& rkLocation)
{
    Local.SetTranslate(rkLocation);
    OnFrameChange();
}
//----------------------------------------------------------------------------
void Light::SetAxes (const Vector3f& rkDVector, const Vector3f& rkUVector,
    const Vector3f& rkRVector)
{
    Local.SetRotate(Matrix3f(rkDVector,rkUVector,rkRVector,true));
    OnFrameChange();
}
//----------------------------------------------------------------------------
void Light::SetAxes (const Matrix3f& rkAxes)
{
    Local.SetRotate(rkAxes);
    OnFrameChange();
}
//----------------------------------------------------------------------------
void Light::SetDirection (const Vector3f& rkDirection)
{
    Vector3f kDVector = rkDirection, kUVector, kRVector;
    Vector3f::GenerateOrthonormalBasis(kUVector,kRVector,kDVector,true);
    SetAxes(kDVector,kUVector,kRVector);
    OnFrameChange();
}
//----------------------------------------------------------------------------
void Light::OnFrameChange ()
{
    // Get the world coordinate direction vectors for the light.  The input
    // application time of MAX_REAL is based on knowing that OnFrameChange
    // was called via Light interface functions, not through controllers.
    // If the light frame is modified by controllers, the local transforms
    // are modified directly through the Spatial interface, so that interface
    // itself will update the world transforms.  The application time passed
    // through the Spatial interface will be different than MAX_REAL, so
    // the Update call in this function will not prevent a controller update.
    UpdateGS(-Mathd::MAX_REAL);
}
//----------------------------------------------------------------------------
void Light::UpdateWorldBound ()
{
    // The light has an implicit model bound whose center is the light's
    // position and whose radius is zero.  If you attach a light as a Spatial
    // object, set the position even if the light does not formally have one.
    // For example, if you attach a directional light to headlight geometry in
    // an automobile, set the position of the light to somewhere inside the
    // headlight.  By doing so, you will help the culling system by
    // generating a properly sized world bound for the parent node of the
    // headlight.
    WorldBound->SetCenter(World.ApplyForward(Local.GetTranslate()));
    WorldBound->SetRadius(0.0f);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Light::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Spatial::Load(rkStream,pkLink);

    // native data
    rkStream.Read(Type);
    rkStream.Read(Ambient);
    rkStream.Read(Diffuse);
    rkStream.Read(Specular);
    rkStream.Read(Intensity);
    rkStream.Read(Constant);
    rkStream.Read(Linear);
    rkStream.Read(Quadratic);
    rkStream.Read(Attenuate);
    rkStream.Read(On);
    rkStream.Read(Exponent);
    rkStream.Read(Angle);

    WM3_END_DEBUG_STREAM_LOAD(Light);
}
//----------------------------------------------------------------------------
void Light::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Spatial::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool Light::Register (Stream& rkStream) const
{
    return Spatial::Register(rkStream);
}
//----------------------------------------------------------------------------
void Light::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Spatial::Save(rkStream);

    // native data
    rkStream.Write(Type);
    rkStream.Write(Ambient);
    rkStream.Write(Diffuse);
    rkStream.Write(Specular);
    rkStream.Write(Intensity);
    rkStream.Write(Constant);
    rkStream.Write(Linear);
    rkStream.Write(Quadratic);
    rkStream.Write(Attenuate);
    rkStream.Write(On);
    rkStream.Write(Exponent);
    rkStream.Write(Angle);

    WM3_END_DEBUG_STREAM_SAVE(Light);
}
//----------------------------------------------------------------------------
StringTree* Light::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(11,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));

    char* acString = 0;
    switch ( Type )
    {
    case LT_AMBIENT:
        acString = StringTree::Format("type = AMBIENT");
        break;
    case LT_DIRECTIONAL:
        acString = StringTree::Format("type = DIRECTIONAL");
        break;
    case LT_POINT:
        acString = StringTree::Format("type = POINT");
        break;
    case LT_SPOT:
        acString = StringTree::Format("type = SPOT");
        break;
    default:  // LT_QUANTITY
        break;
    }
    pkTree->SetString(1,acString);

    pkTree->SetString(2,StringTree::Format("ambient =",Ambient));
    pkTree->SetString(3,StringTree::Format("diffuse =",Diffuse));
    pkTree->SetString(4,StringTree::Format("specular =",Specular));
    pkTree->SetString(5,StringTree::Format("intensity =",Intensity));

    char acDummy[128];
    sprintf(acDummy,"constant: %f, linear: %f, quadratic: %f",
        Constant,Linear,Quadratic);
    pkTree->SetString(6,StringTree::Format("attenuation =",acDummy));

    pkTree->SetString(7,StringTree::Format("attenuate enabled =",Attenuate));
    pkTree->SetString(8,StringTree::Format("light on =",On));
    pkTree->SetString(9,StringTree::Format("exponent =",Exponent));
    pkTree->SetString(10,StringTree::Format("angle =",Angle));

    // children
    pkTree->SetChild(0,Object::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int Light::GetMemoryUsed () const
{
    return sizeof(Light) - sizeof(Spatial) + Spatial::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int Light::GetDiskUsed () const
{
    return Spatial::GetDiskUsed() +
        sizeof(Type) +
        sizeof(Ambient) +
        sizeof(Diffuse) +
        sizeof(Specular) +
        sizeof(Intensity) +
        sizeof(Constant) +
        sizeof(Linear) +
        sizeof(Quadratic) +
        sizeof(char) + // m_bAttenuate
        sizeof(char) + // m_bOn
        sizeof(Exponent) +
        sizeof(Angle);
}
//----------------------------------------------------------------------------
