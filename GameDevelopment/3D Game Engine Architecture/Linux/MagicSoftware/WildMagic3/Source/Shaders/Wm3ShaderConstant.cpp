// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ShaderConstant.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,ShaderConstant,Object);
WM3_IMPLEMENT_STREAM(ShaderConstant);
WM3_IMPLEMENT_DEFAULT_NAME_ID(ShaderConstant,Object);

//----------------------------------------------------------------------------
ShaderConstant::ShaderConstant ()
{
    m_iSCType = SC_QUANTITY;
    m_iSCOption = SCO_NONE;
    m_iRegister = 0;
    m_iRegisterQuantity = 0;
    m_afData = 0;
}
//----------------------------------------------------------------------------
ShaderConstant::ShaderConstant (int iSCType, int iSCOption, int iRegister,
    int iRegisterQuantity)
{
    m_iSCType = iSCType;
    m_iSCOption = iSCOption;
    m_iRegister = iRegister;
    m_iRegisterQuantity = iRegisterQuantity;
    m_afData = new float[4*m_iRegisterQuantity];
    memset(m_afData,0,4*m_iRegisterQuantity*sizeof(float));
}
//----------------------------------------------------------------------------
ShaderConstant::~ShaderConstant ()
{
    delete[] m_afData;
}
//----------------------------------------------------------------------------
void ShaderConstant::SetRegisterData (int i, float fData0, float fData1,
    float fData2, float fData3)
{
    assert( 0 <= i && i < m_iRegisterQuantity );
    float* pfData = &m_afData[4*i];
    *pfData++ = fData0;
    *pfData++ = fData1;
    *pfData++ = fData2;
    *pfData   = fData3;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void ShaderConstant::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Object::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iSCType);
    rkStream.Read(m_iRegister);
    rkStream.Read(m_iRegisterQuantity);
    m_afData = new float[4*m_iRegisterQuantity];
    rkStream.Read(4*m_iRegisterQuantity,m_afData);

    WM3_END_DEBUG_STREAM_LOAD(ShaderConstant);
}
//----------------------------------------------------------------------------
void ShaderConstant::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool ShaderConstant::Register (Stream& rkStream) const
{
    return Object::Register(rkStream);
}
//----------------------------------------------------------------------------
void ShaderConstant::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Object::Save(rkStream);

    // native data
    rkStream.Write(m_iSCType);
    rkStream.Write(m_iRegister);
    rkStream.Write(m_iRegisterQuantity);
    rkStream.Write(4*m_iRegisterQuantity,m_afData);

    WM3_END_DEBUG_STREAM_SAVE(ShaderConstant);
}
//----------------------------------------------------------------------------
StringTree* ShaderConstant::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(5,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("type =",m_iSCType));
    pkTree->SetString(2,StringTree::Format("option =",m_iSCOption));
    pkTree->SetString(3,StringTree::Format("register =",m_iRegister));
    pkTree->SetString(4,StringTree::Format("register quantity=",
        m_iRegisterQuantity));

    // children
    pkTree->SetChild(0,Object::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int ShaderConstant::GetMemoryUsed () const
{
    return sizeof(ShaderConstant) - sizeof(Object) +
        Object::GetMemoryUsed() +
        4*m_iRegisterQuantity*sizeof(m_afData[0]);
}
//----------------------------------------------------------------------------
int ShaderConstant::GetDiskUsed () const
{
    return Object::GetDiskUsed() +
        sizeof(m_iSCType) +
        sizeof(m_iRegister) +
        sizeof(m_iRegisterQuantity) +
        4*m_iRegisterQuantity*sizeof(m_afData[0]);
}
//----------------------------------------------------------------------------
