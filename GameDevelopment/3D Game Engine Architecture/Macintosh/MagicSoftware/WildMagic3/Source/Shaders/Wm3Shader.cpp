// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Shader.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,Shader,Object);
WM3_IMPLEMENT_ABSTRACT_STREAM(Shader);

//----------------------------------------------------------------------------
Shader::Shader (int eShaderType)
    :
    BIArray(1,1)
{
    m_eShaderType = eShaderType;
    m_acProgram = 0;
}
//----------------------------------------------------------------------------
Shader::~Shader ()
{
    // Derived classes set m_acProgram to a static class member, so it should
    // not be deleted here.

    // Inform all renderers using this shader that it is being destroyed.
    // This allows the renderer to free up any associated resources.
    const TArray<BindInfo>& rkArray = BIArray.GetArray();
    for (int i = 0; i < rkArray.GetQuantity(); i++)
        rkArray[i].User->ReleaseShader(this);
}
//----------------------------------------------------------------------------
void Shader::AddConstant (ShaderConstant* pkConstant)
{
    m_kConstant.Append(pkConstant);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* Shader::GetObjectByName (const String& rkName)
{
    Object* pkFound = Object::GetObjectByName(rkName);
    if ( pkFound )
        return pkFound;

    for (int i = 0; i < m_kConstant.GetQuantity(); i++)
    {
        pkFound = m_kConstant[i]->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------
void Shader::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    Object::GetAllObjectsByName(rkName,rkObjects);

    for (int i = 0; i < m_kConstant.GetQuantity(); i++)
        m_kConstant[i]->GetAllObjectsByName(rkName,rkObjects);
}
//----------------------------------------------------------------------------
Object* Shader::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Object::GetObjectByID(uiID);
    if ( pkFound )
        return pkFound;

    for (int i = 0; i < m_kConstant.GetQuantity(); i++)
    {
        pkFound = m_kConstant[i]->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Shader::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Object::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_eShaderType);

    // link data
    int iQuantity;
    rkStream.Read(iQuantity);
    m_kConstant.SetMaxQuantity(iQuantity,false);
    for (int i = 0; i < iQuantity; i++)
    {
        Object* pkObject;
        rkStream.Read(pkObject);  // m_kConstants[i]
        pkLink->Add(pkObject);
    }

    WM3_END_DEBUG_STREAM_LOAD(Shader);
}
//----------------------------------------------------------------------------
void Shader::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);

    for (int i = 0; i < m_kConstant.GetQuantity(); i++)
    {
        Object* pkLinkID = pkLink->GetLinkID();
        m_kConstant[i] = (ShaderConstant*)rkStream.GetFromMap(pkLinkID);
    }
}
//----------------------------------------------------------------------------
bool Shader::Register (Stream& rkStream) const
{
    if ( !Object::Register(rkStream) )
        return false;

    for (int i = 0; i < m_kConstant.GetQuantity(); i++)
        m_kConstant[i]->Register(rkStream);

    return true;
}
//----------------------------------------------------------------------------
void Shader::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Object::Save(rkStream);

    // native data
    rkStream.Write(m_eShaderType);

    // link data
    int iQuantity = m_kConstant.GetQuantity();
    rkStream.Write(iQuantity);
    for (int i = 0; i < iQuantity; i++)
        rkStream.Write(m_kConstant[i]);

    WM3_END_DEBUG_STREAM_SAVE(Shader);
}
//----------------------------------------------------------------------------
StringTree* Shader::SaveStrings (const char*)
{
    int iQuantity = m_kConstant.GetQuantity();
    StringTree* pkTree = new StringTree(1,iQuantity+1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));

    // children
    pkTree->SetChild(0,Object::SaveStrings());
    for (int i = 0; i < iQuantity; i++)
        pkTree->SetChild(i+1,m_kConstant[i]->SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int Shader::GetMemoryUsed () const
{
    return sizeof(Shader) - sizeof(Object) +  Object::GetMemoryUsed() +
        m_kConstant.GetMaxQuantity()*sizeof(m_kConstant[0]);
}
//----------------------------------------------------------------------------
int Shader::GetDiskUsed () const
{
    return Object::GetDiskUsed() +
        sizeof(m_eShaderType) +
        sizeof(int) + // m_kConstant.GetQuantity()
        m_kConstant.GetQuantity()*sizeof(m_kConstant[0]);
}
//----------------------------------------------------------------------------
