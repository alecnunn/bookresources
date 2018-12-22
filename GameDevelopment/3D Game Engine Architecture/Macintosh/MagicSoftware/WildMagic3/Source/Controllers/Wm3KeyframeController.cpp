// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3KeyframeController.h"
#include "Wm3Spatial.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,KeyframeController,Controller);
WM3_IMPLEMENT_STREAM(KeyframeController);

//----------------------------------------------------------------------------
KeyframeController::KeyframeController ()
{
    m_iTLastIndex = 0;
    m_iRLastIndex = 0;
    m_iSLastIndex = 0;
}
//----------------------------------------------------------------------------
KeyframeController::~KeyframeController ()
{
}
//----------------------------------------------------------------------------
void KeyframeController::GetKeyInfo (float fCtrlTime, int iQuantity,
    float* afTime, int& riLastIndex, float& rfTime, int& ri0, int& ri1)
{
    if ( fCtrlTime <= afTime[0] )
    {
        rfTime = 0.0f;
        riLastIndex = 0;
        ri0 = 0;
        ri1 = 0;
        return;
    }

    if ( fCtrlTime >= afTime[iQuantity-1] )
    {
        rfTime = 0.0f;
        riLastIndex = iQuantity - 1;
        ri0 = riLastIndex;
        ri1 = riLastIndex;
        return;
    }

    int iNextIndex;
    if ( fCtrlTime > afTime[riLastIndex] )
    {
        iNextIndex = riLastIndex + 1;
        while ( fCtrlTime >= afTime[iNextIndex] )
        {
            riLastIndex = iNextIndex;
            iNextIndex++;
        }

        ri0 = riLastIndex;
        ri1 = iNextIndex;
        rfTime = (fCtrlTime - afTime[ri0])/(afTime[ri1] - afTime[ri0]);
    }
    else if ( fCtrlTime < afTime[riLastIndex] )
    {
        iNextIndex = riLastIndex - 1;
        while ( fCtrlTime <= afTime[iNextIndex] )
        {
            riLastIndex = iNextIndex;
            iNextIndex--;
        }

        ri0 = iNextIndex;
        ri1 = riLastIndex;
        rfTime = (fCtrlTime - afTime[ri0])/(afTime[ri1] - afTime[ri0]);
    }
    else
    {
        rfTime = 0.0f;
        ri0 = riLastIndex;
        ri1 = riLastIndex;
    }
}
//----------------------------------------------------------------------------
Vector3f KeyframeController::GetTranslate (float fNormTime, int i0, int i1)
{
    Vector3f* akTData = TranslationData->GetData();
    return akTData[i0] + fNormTime*(akTData[i1] - akTData[i0]);
}
//----------------------------------------------------------------------------
Matrix3f KeyframeController::GetRotate (float fNormTime, int i0, int i1)
{
    Quaternionf* akRData = RotationData->GetData();
    Quaternionf kQ;
    kQ.Slerp(fNormTime,akRData[i0],akRData[i1]);

    Matrix3f kRot;
    kQ.ToRotationMatrix(kRot);
    return kRot;
}
//----------------------------------------------------------------------------
float KeyframeController::GetScale (float fNormTime, int i0, int i1)
{
    float* afSData = ScaleData->GetData();
    return afSData[i0] + fNormTime*(afSData[i1] - afSData[i0]);
}
//----------------------------------------------------------------------------
bool KeyframeController::Update (double dAppTime)
{
    if ( !Controller::Update(dAppTime) )
        return false;

    Spatial* pkSpatial = StaticCast<Spatial>(m_pkObject);
    float fCtrlTime = (float)GetControlTime(dAppTime);
    float fNormTime;
    int i0, i1;

    // The logic here checks for equal time arrays to minimize the number of
    // times GetKeyInfo is called.
    if ( TranslationTimes == RotationTimes )
    {
        if ( TranslationTimes )
        {
            GetKeyInfo(fCtrlTime,TranslationTimes->GetQuantity(),
                TranslationTimes->GetData(),m_iTLastIndex,fNormTime,
                i0,i1);
        }

        if ( TranslationData )
            pkSpatial->Local.SetTranslate(GetTranslate(fNormTime,i0,i1));

        if ( RotationData )
            pkSpatial->Local.SetRotate(GetRotate(fNormTime,i0,i1));

        if ( TranslationTimes == ScaleTimes )
        {
            // T == R == S
            if ( ScaleData )
                pkSpatial->Local.SetUniformScale(GetScale(fNormTime,i0,i1));
        }
        else
        {
            // T == R != S
            if ( ScaleTimes && ScaleData )
            {
                GetKeyInfo(fCtrlTime,ScaleTimes->GetQuantity(),
                    ScaleTimes->GetData(),m_iSLastIndex,fNormTime,i0,i1);
                pkSpatial->Local.SetUniformScale(GetScale(fNormTime,i0,i1));
            }
        }
    }
    else if ( TranslationTimes == ScaleTimes )
    {
        // R != T == S
        if ( TranslationTimes )
        {
            GetKeyInfo(fCtrlTime,TranslationTimes->GetQuantity(),
                TranslationTimes->GetData(),m_iTLastIndex,fNormTime,
                i0,i1);
        }

        if ( TranslationData )
            pkSpatial->Local.SetTranslate(GetTranslate(fNormTime,i0,i1));

        if ( ScaleData )
            pkSpatial->Local.SetUniformScale(GetScale(fNormTime,i0,i1));

        if ( RotationTimes && RotationData )
        {
            GetKeyInfo(fCtrlTime,RotationTimes->GetQuantity(),
                RotationTimes->GetData(),m_iRLastIndex,fNormTime,i0,i1);
            pkSpatial->Local.SetRotate(GetRotate(fNormTime,i0,i1));
        }
    }
    else if ( RotationTimes == ScaleTimes )
    {
        // S == R != T
        if ( RotationTimes  )
        {
            GetKeyInfo(fCtrlTime,RotationTimes->GetQuantity(),
                RotationTimes->GetData(),m_iRLastIndex,fNormTime,i0,i1);
        }

        if ( RotationData )
            pkSpatial->Local.SetRotate(GetRotate(fNormTime,i0,i1));

        if ( ScaleData )
            pkSpatial->Local.SetUniformScale(GetScale(fNormTime,i0,i1));

        if ( TranslationTimes && TranslationData )
        {
            GetKeyInfo(fCtrlTime,TranslationTimes->GetQuantity(),
                TranslationTimes->GetData(),m_iTLastIndex,fNormTime,i0,i1);
            pkSpatial->Local.SetTranslate(GetTranslate(fNormTime,i0,i1));
        }
    }
    else
    {
        // S != R, T != R (and R != T, something caught earlier in the logic)
        if ( TranslationTimes && TranslationData )
        {
            GetKeyInfo(fCtrlTime,TranslationTimes->GetQuantity(),
                TranslationTimes->GetData(),m_iTLastIndex,fNormTime,i0,i1);
            pkSpatial->Local.SetTranslate(GetTranslate(fNormTime,i0,i1));
        }

        if ( RotationTimes && RotationData )
        {
            GetKeyInfo(fCtrlTime,RotationTimes->GetQuantity(),
                RotationTimes->GetData(),m_iRLastIndex,fNormTime,i0,i1);
            pkSpatial->Local.SetRotate(GetRotate(fNormTime,i0,i1));
        }

        if ( ScaleTimes && ScaleData )
        {
            GetKeyInfo(fCtrlTime,ScaleTimes->GetQuantity(),
                ScaleTimes->GetData(),m_iSLastIndex,fNormTime,i0,i1);
            pkSpatial->Local.SetUniformScale(GetScale(fNormTime,i0,i1));
        }
    }

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* KeyframeController::GetObjectByName (const String& rkName)
{
    Object* pkFound = Controller::GetObjectByName(rkName);
    if ( pkFound )
        return pkFound;

    if ( TranslationTimes )
    {
        pkFound = TranslationTimes->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    if ( TranslationData )
    {
        pkFound = TranslationData->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    if ( RotationTimes )
    {
        pkFound = RotationTimes->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    if ( RotationData )
    {
        pkFound = RotationData->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    if ( ScaleTimes )
    {
        pkFound = ScaleTimes->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    if ( ScaleData )
    {
        pkFound = ScaleData->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------
void KeyframeController::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    Controller::GetAllObjectsByName(rkName,rkObjects);

    if ( TranslationTimes )
        TranslationTimes->GetAllObjectsByName(rkName,rkObjects);

    if ( TranslationData )
        TranslationData->GetAllObjectsByName(rkName,rkObjects);

    if ( RotationTimes )
        RotationTimes->GetAllObjectsByName(rkName,rkObjects);

    if ( RotationData )
        RotationData->GetAllObjectsByName(rkName,rkObjects);

    if ( ScaleTimes )
        ScaleTimes->GetAllObjectsByName(rkName,rkObjects);

    if ( ScaleData )
        ScaleData->GetAllObjectsByName(rkName,rkObjects);
}
//----------------------------------------------------------------------------
Object* KeyframeController::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Controller::GetObjectByID(uiID);
    if ( pkFound )
        return pkFound;

    if ( TranslationTimes )
    {
        pkFound = TranslationTimes->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    if ( TranslationData )
    {
        pkFound = TranslationData->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    if ( RotationTimes )
    {
        pkFound = RotationTimes->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    if ( RotationData )
    {
        pkFound = RotationData->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    if ( ScaleTimes )
    {
        pkFound = ScaleTimes->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    if ( ScaleData )
    {
        pkFound = ScaleData->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void KeyframeController::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Controller::Load(rkStream,pkLink);

    // link data
    Object* pkObject;
    rkStream.Read(pkObject);  // TranslationTimes
    pkLink->Add(pkObject);

    rkStream.Read(pkObject);  // TranslationData
    pkLink->Add(pkObject);

    rkStream.Read(pkObject);  // RotationTimes
    pkLink->Add(pkObject);

    rkStream.Read(pkObject);  // RotationData
    pkLink->Add(pkObject);

    rkStream.Read(pkObject);  // ScaleTimes
    pkLink->Add(pkObject);

    rkStream.Read(pkObject);  // ScaleData
    pkLink->Add(pkObject);

    WM3_END_DEBUG_STREAM_LOAD(KeyframeController);
}
//----------------------------------------------------------------------------
void KeyframeController::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Controller::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    TranslationTimes = (FloatArray*)rkStream.GetFromMap(pkLinkID);

    pkLinkID = pkLink->GetLinkID();
    TranslationData = (Vector3fArray*)rkStream.GetFromMap(pkLinkID);

    pkLinkID = pkLink->GetLinkID();
    RotationTimes = (FloatArray*)rkStream.GetFromMap(pkLinkID);

    pkLinkID = pkLink->GetLinkID();
    RotationData = (QuaternionfArray*)rkStream.GetFromMap(pkLinkID);

    pkLinkID = pkLink->GetLinkID();
    ScaleTimes = (FloatArray*)rkStream.GetFromMap(pkLinkID);

    pkLinkID = pkLink->GetLinkID();
    ScaleData = (FloatArray*)rkStream.GetFromMap(pkLinkID);
}
//----------------------------------------------------------------------------
bool KeyframeController::Register (Stream& rkStream) const
{
    if ( !Controller::Register(rkStream) )
        return false;

    if ( TranslationTimes )
        TranslationTimes->Register(rkStream);

    if ( TranslationData )
        TranslationData->Register(rkStream);

    if ( RotationTimes )
        RotationTimes->Register(rkStream);

    if ( RotationData )
        RotationData->Register(rkStream);

    if ( ScaleTimes )
        ScaleTimes->Register(rkStream);

    if ( ScaleData )
        ScaleData->Register(rkStream);

    return true;
}
//----------------------------------------------------------------------------
void KeyframeController::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Controller::Save(rkStream);

    // link data
    rkStream.Write(TranslationTimes);
    rkStream.Write(TranslationData);
    rkStream.Write(RotationTimes);
    rkStream.Write(RotationData);
    rkStream.Write(ScaleTimes);
    rkStream.Write(ScaleData);

    WM3_END_DEBUG_STREAM_SAVE(KeyframeController);
}
//----------------------------------------------------------------------------
StringTree* KeyframeController::SaveStrings (const char*)
{
    int iCQuantity = 1;
    if ( TranslationTimes ) iCQuantity++;
    if ( TranslationData )  iCQuantity++;
    if ( RotationTimes ) iCQuantity++;
    if ( RotationData )  iCQuantity++;
    if ( ScaleTimes ) iCQuantity++;
    if ( ScaleData )  iCQuantity++;

    StringTree* pkTree = new StringTree(1,iCQuantity);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));

    // children
    pkTree->SetChild(0,Controller::SaveStrings());
    iCQuantity = 1;
    if ( TranslationTimes )
    {
        pkTree->SetChild(iCQuantity++,
            TranslationTimes->SaveStrings("trn times"));
    }
    if ( TranslationData )
    {
        pkTree->SetChild(iCQuantity++,
            TranslationData->SaveStrings("trn data"));
    }
    if ( RotationTimes )
    {
        pkTree->SetChild(iCQuantity++,
            RotationTimes->SaveStrings("rot times"));
    }
    if ( RotationData )
        pkTree->SetChild(iCQuantity++,RotationData->SaveStrings("rot data"));
    if ( ScaleTimes )
        pkTree->SetChild(iCQuantity++,ScaleTimes->SaveStrings("sca times"));
    if ( ScaleData )
        pkTree->SetChild(iCQuantity++,ScaleData->SaveStrings("sca data"));

    return pkTree;
}
//----------------------------------------------------------------------------
int KeyframeController::GetMemoryUsed () const
{
    return sizeof(KeyframeController) - sizeof(Controller) +
        Controller::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int KeyframeController::GetDiskUsed () const
{
    return Controller::GetDiskUsed() +
        sizeof(TranslationTimes) +
        sizeof(TranslationData) +
        sizeof(RotationTimes) +
        sizeof(RotationData) +
        sizeof(ScaleTimes) +
        sizeof(ScaleData);
}
//----------------------------------------------------------------------------
