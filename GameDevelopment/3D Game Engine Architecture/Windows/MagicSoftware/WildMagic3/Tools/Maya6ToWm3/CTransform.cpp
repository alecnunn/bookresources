// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000-2003.  All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// 3D GAME ENGINE SOURCE CODE
// http://www.magic-software.com/License/3DGameEngine.pdf

#include "Exporter.h"
#include "Wm3TArray.h"
#include "Wm3Node.h"
#include "Wm3KeyframeController.h"
using namespace Wm3;

#include <maya/MAnimControl.h>
#include <maya/MAnimUtil.h>
#include <maya/MDagPath.h>
#include <maya/MEulerRotation.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MFnTransform.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MVector.h>
#include <maya/MItKeyframe.h>
#include <maya/MFnIkJoint.h>
#include <maya/MQuaternion.h>

//----------------------------------------------------------------------------
Matrix3f CreateRotate(int iRotateOrder, float fRotX, float fRotY, float fRotZ)
{
    // Maya rotations are given as euler angles as well as an enum for
    // which order the axes are rotated about.  This function takes those
    // four parameters and returns the correct rotation matrix.

    Matrix3f kRotation;
    switch(iRotateOrder)
    {
    default:
        // This shouldn't ever happen, but just in case...
        MGlobal::displayError("Invalid rotateOrder.");
        // Fall through
    case 0:
        // xyz
        kRotation.FromEulerAnglesXYZ((float)-fRotX,
            (float)-fRotY,(float)-fRotZ);
        break;
    case 1:
        // yzx
        kRotation.FromEulerAnglesYZX((float)-fRotX,
            (float)-fRotY,(float)-fRotZ);
        break;
    case 2:
        // zxy
        kRotation.FromEulerAnglesZXY((float)-fRotX,
            (float)-fRotY,(float)-fRotZ);
        break;
    case 3:
        // xzy
        kRotation.FromEulerAnglesXZY((float)-fRotX,
            (float)-fRotY,(float)-fRotZ);
        break;
    case 4:
        // yxz
        kRotation.FromEulerAnglesYXZ((float)-fRotX,
            (float)-fRotY,(float)-fRotZ);
        break;
    case 5:
        // zyx
        kRotation.FromEulerAnglesZYX((float)-fRotX,
            (float)-fRotY,(float)-fRotZ);
        break;
    }

    return kRotation.Transpose();
}
//----------------------------------------------------------------------------
static KeyframeController* GetBareController()
{
    // This function sets up a controller entirely except for the keyframe
    // data itself, taking the max/min time and repeat mode information
    // from global Maya variables.
    KeyframeController* pkKFCtrl = new KeyframeController;
    pkKFCtrl->MinTime = (float)MAnimControl::minTime().as(MTime::kSeconds);
    pkKFCtrl->MaxTime = (float)MAnimControl::maxTime().as(MTime::kSeconds);

    switch( MAnimControl::playbackMode() )
    {
    default:
        // This should never happen.
        MGlobal::displayError("Unknown animation playback mode.");
        // fall through
    case MAnimControl::kPlaybackOnce:
        pkKFCtrl->RepeatType = KeyframeController::RT_CLAMP;
        break;
    case MAnimControl::kPlaybackLoop:
        pkKFCtrl->RepeatType = KeyframeController::RT_WRAP;
        break;
    case MAnimControl::kPlaybackOscillate:
        pkKFCtrl->RepeatType = KeyframeController::RT_CYCLE;
        break;
    }
    return pkKFCtrl;
}
//----------------------------------------------------------------------------
static void GetCurveAndIter(const MPlug& rkPlug, MFnAnimCurve*& rpkCurve,
    MItKeyframe*& rpkIt)
{
    // Get a single curve and iter.  Takes into account indirection
    // through a character node.

    MStatus kStatus;
    bool bConnected = rkPlug.isConnected(&kStatus);
    assert(kStatus);
    // If not connected, no animation curves for this plug
    if (!bConnected)
        return;

    MPlugArray kPlugArray;
    rkPlug.connectedTo(kPlugArray,true,false,&kStatus);
    assert(kStatus);

    if (kPlugArray[0].node().hasFn(MFn::kAnimCurve))
    {
        // direct connection to an animation curve
        rpkCurve = new MFnAnimCurve;
        kStatus = rpkCurve->setObject(kPlugArray[0].node());
        assert(kStatus);
        rpkIt = new MItKeyframe(kPlugArray[0].node(),&kStatus);
        assert(kStatus);
    }
    else if (kPlugArray[0].node().hasFn(MFn::kCharacter))
    {
        // Connection from a character node
        // Ideally this will connect to an animation curve
        // directly.
        GetCurveAndIter(kPlugArray[0],rpkCurve,rpkIt);
    }
    else
    {
        // Unsupported connection
        MGlobal::displayError(kPlugArray[0].name() + " is not a character"\
            " or an animation curve.  Ignoring keyframes on this channel.");
    }
}
//----------------------------------------------------------------------------
static void GetCurvesAndIters(MItKeyframe*& rpkXIt, MItKeyframe*& rpkYIt,
    MItKeyframe*& rpkZIt, MFnAnimCurve*& rpkXCurve, MFnAnimCurve*& rpkYCurve,
    MFnAnimCurve*& rpkZCurve, const MPlug& rkPlug)
{
    // This function is just to eliminate redundant code between the
    // rotate, scale, and translate animation getter functions. 
    
    // It gets curve and iterators for each component of the plug or NULL if a
    // component isn't keyed.

    rpkXIt = rpkYIt = rpkZIt = NULL;
    rpkXCurve = rpkYCurve = rpkZCurve = NULL;

    // Get animation curves and keyframe iterators for each
    // component, if they exist.
    
    GetCurveAndIter(rkPlug.child(0),rpkXCurve,rpkXIt);
    GetCurveAndIter(rkPlug.child(1),rpkYCurve,rpkYIt);
    GetCurveAndIter(rkPlug.child(2),rpkZCurve,rpkZIt);
}
//----------------------------------------------------------------------------
static MTime GetTimeAndIncrement(MItKeyframe* pkKey1, MItKeyframe* pkKey2,
    MItKeyframe* pkKey3, MTime& rkMaxTime)
{
    // Return the minimum key time of the three keys.  If there are no more
    // frames or the kesy are null, then return MaxTime.  It also will
    // increment any iterator that has that minimal time.

    // set as max time if done or non-existant
    MTime kT1 = (!pkKey1 || pkKey1->isDone() ? rkMaxTime : pkKey1->time());
    MTime kT2 = (!pkKey2 || pkKey2->isDone() ? rkMaxTime : pkKey2->time());
    MTime kT3 = (!pkKey3 || pkKey3->isDone() ? rkMaxTime : pkKey3->time());

    MTime kT;

    // get the min time
    if ( kT1 < kT2 )
    {
        if ( kT1 < kT3 )
            kT = kT1;
        else
            kT = kT3;
    }
    else
    {
        if ( kT2 < kT3 )
            kT = kT2;
        else
            kT = kT3;
    }

    // update iterators (since multiple iterators may share the same time)
    if (pkKey1 && !pkKey1->isDone() && pkKey1->time() <= kT)
        pkKey1->next();
    if (pkKey2 && !pkKey2->isDone() && pkKey2->time() <= kT)
        pkKey2->next();
    if (pkKey3 && !pkKey3->isDone() && pkKey3->time() <= kT)
        pkKey3->next();

    return kT;
}
//----------------------------------------------------------------------------
static bool SetRotateAnim(const MFnDagNode& rkDagNode, 
    KeyframeController* pkKFCtrl, int iRotateOrder, float fStaticX, 
    float fStaticY, float fStaticZ, Matrix3f& rkLeftMultiply,
    Matrix3f& rkRightMultiply)
{
    // This function sets the rotation component of pkKFCtrl for the
    // node rkDagNode which already has the static, non-keyframed translation
    // of rkStatic.  It returns true if a translation was found (and set)
    // and false otherwise.  See SetTranslateAnim for more notes.
    // rkLeftMultiply and rkRightMultiply are multiplied by the rotate
    // value for each keyframe.
   
    // Assumption: pkKFCtrl is already instantiated
    assert( pkKFCtrl );

    // get rotation keys
    MStatus kStatus;
    MPlug kRotatePlug = rkDagNode.findPlug("rotate",&kStatus);
    assert( kStatus );

    // Components may not all be connected.  Maybe the user
    // just keyframed one principle component.  Thus, we'll use
    // pointers to reference keyframe data, with NULL meaning
    // that component is not keyed.
    MItKeyframe* pkXIt;
    MItKeyframe* pkYIt;
    MItKeyframe* pkZIt;
    MFnAnimCurve* pkXCurve;
    MFnAnimCurve* pkYCurve;
    MFnAnimCurve* pkZCurve;

    GetCurvesAndIters(pkXIt,pkYIt,pkZIt,pkXCurve,pkYCurve,pkZCurve,
        kRotatePlug);

    // Maybe there are no rotation components...
    if (!pkXIt && !pkYIt && !pkZIt)
        return false;

    // Use TArrays, because it isn't known how many keyframes will be needed
    TArray<float> kTimesAr;
    TArray<Quaternionf> kRotAr;

    MTime kMaxTime(pkKFCtrl->MaxTime,MTime::kSeconds);
    // Use a time greater than max time, so that the while loop below
    // will break when kT hits this time.  This is because it is
    // possible for kT to have a keyframe at MaxTime, so we need something
    // we know is greater than it as an "no more keyframes" value.
    MTime kPastMaxTime(pkKFCtrl->MaxTime+1,MTime::kSeconds);

    MTime kT = GetTimeAndIncrement(pkXIt,pkYIt,pkZIt,kPastMaxTime);
    while (kT <= kMaxTime)
    {
        float fX, fY, fZ;

        // Evaluate curves at keyframe time.  If there are no
        // keyframes for a component, use the static transform data.
        if (pkXCurve)
        {
            fX = (float)pkXCurve->evaluate(kT,&kStatus);
            assert(kStatus);
        }
        else
        {
            fX = fStaticX;
        }

        if (pkYCurve)
        {
            fY = (float)pkYCurve->evaluate(kT,&kStatus);
            assert(kStatus);
        }
        else
        {
            fY = fStaticY;
        }

        if (pkZCurve)
        {
            fZ = (float)pkZCurve->evaluate(kT,&kStatus);
            assert(kStatus);
        }
        else
        {
            fZ = fStaticZ;
        }

        Matrix3f kRot = CreateRotate(iRotateOrder,fX,fY,fZ);
        kRot = rkLeftMultiply*kRot*rkRightMultiply;

        // Append data into the arrays
        kTimesAr.Append((float)kT.as(MTime::kSeconds));
        
        // Since any rotation kRot can be kQ or -kQ, make sure we are
        // using the one with the shortest distance to the prior
        // quaternion.
        Quaternionf kQ(kRot);
        if (kRotAr.GetQuantity() > 0 && 
            kRotAr[kRotAr.GetQuantity()-1].Dot(kQ) < 0.0f)
        {
            kQ *= -1;
        }
        kRotAr.Append(kQ);

        kT = GetTimeAndIncrement(pkXIt,pkYIt,pkZIt,kPastMaxTime);
    }

    // We need to now copy the data out of the TArrays into shared arrays that
    // can be passed to the keyframe controller.
    int iTQuantity = kTimesAr.GetQuantity();
    if (iTQuantity <= 0)
        return false;

    Quaternionf* akRotData = new Quaternionf[iTQuantity];
    float* afTimeData = new float[iTQuantity];
    for(int i = 0; i < iTQuantity; i++)
    {
        akRotData[i] = kRotAr[i];
        afTimeData[i] = kTimesAr[i];
    }
    FloatArrayPtr spkTimes = new FloatArray(iTQuantity,afTimeData);
    QuaternionfArrayPtr spkRot = new QuaternionfArray(iTQuantity,akRotData);
    pkKFCtrl->RotationTimes = spkTimes;
    pkKFCtrl->RotationData = spkRot;

    return true;
}
//----------------------------------------------------------------------------
static bool SetScaleAnim(const MFnDagNode& rkDagNode, 
    KeyframeController* pkKFCtrl, float fStaticScale)
{
    // This function sets the scale component of pkKFCtrl for the
    // node rkDagNode which already has the static, non-keyframed translation
    // of rkStatic.  It returns true if a translation was found (and set)
    // and false otherwise.  See SetTranslateAnim for more notes.
   
    // Assumption: pkKFCtrl is already instantiated
    assert( pkKFCtrl );

    // get scale keys
    MStatus kStatus;
    MPlug kScalePlug = rkDagNode.findPlug("scale",&kStatus);
    assert( kStatus );

    // Components may not all be connected.  Maybe the user
    // just keyframed one principle component.  Thus, we'll use
    // pointers to reference keyframe data, with NULL meaning
    // that component is not keyed.
    MItKeyframe* pkXIt;
    MItKeyframe* pkYIt;
    MItKeyframe* pkZIt;
    MFnAnimCurve* pkXCurve;
    MFnAnimCurve* pkYCurve;
    MFnAnimCurve* pkZCurve;
    GetCurvesAndIters(pkXIt,pkYIt,pkZIt,pkXCurve,pkYCurve,pkZCurve,
        kScalePlug);

    // Maybe there are no scale components...
    if (!pkXIt && !pkYIt && !pkZIt)
        return false;

    // Since scales must be uniform in WM3, non-keyed components are
    // problematic.
    if (!pkXIt || !pkYIt || !pkZIt)
        MGlobal::displayWarning("Not all scale components are keyed.");

    // Use TArrays, because it isn't known how many keyframes will be needed
    TArray<float> kTimesAr;
    TArray<float> kScaleAr;

    MTime kMaxTime(pkKFCtrl->MaxTime,MTime::kSeconds);
    // Use a time greater than max time, so that the while loop below
    // will break when kT hits this time.  This is because it is
    // possible for kT to have a keyframe at MaxTime, so we need something
    // we know is greater than it as an "no more keyframes" value.
    MTime kPastMaxTime(pkKFCtrl->MaxTime+1,MTime::kSeconds);

    MTime kT = GetTimeAndIncrement(pkXIt,pkYIt,pkZIt,kPastMaxTime);
    while (kT <= kMaxTime)
    {
        Vector3f kScale(fStaticScale,fStaticScale,fStaticScale);

        // Evaluate curves at keyframe time.  If there are no
        // keyframes for a component, use the static transform data.
        if (pkXCurve)
        {
            kScale.X() = (float)pkXCurve->evaluate(kT,&kStatus);
            assert(kStatus);
        }
        if (pkYCurve)
        {
            kScale.Y() = (float)pkYCurve->evaluate(kT,&kStatus);
            assert(kStatus);
        }
        if (pkZCurve)
        {
            kScale.Z() = (float)pkZCurve->evaluate(kT,&kStatus);
            assert(kStatus);
        }

        // Check for inverted scales
        if (kScale.X() < 0 || kScale.Y() < 0 || kScale.Z() < 0)
        {
            MString kS1,kS2,kS3,kTimeStr;
            kS1.set(kScale.X());
            kS2.set(kScale.Y());
            kS3.set(kScale.Z());
            kTimeStr.set((float)kT.as(MTime::kSeconds));
            MGlobal::displayError("Inverted scale at time " +
                kTimeStr + ": (" + kS1 + "," + kS2 + "," + kS3 +
                ").  Using positive scales instead.");            

            // fix these scales
            kScale.X() = Mathf::FAbs(kScale.X());
            kScale.Y() = Mathf::FAbs(kScale.Y());
            kScale.Z() = Mathf::FAbs(kScale.Z());
        }

        // Check for non-uniform scales
        const float fTol = 1e-06f;
        if (Mathd::FAbs(kScale.X() - kScale.Y()) > fTol ||
            Mathd::FAbs(kScale.Z() - kScale.Y()) > fTol ||
            Mathd::FAbs(kScale.X() - kScale.Z()) > fTol)
        {
            // Report non-uniform scale
            MString kS1,kS2,kS3,kTimeStr;
            kS1.set(kScale.X());
            kS2.set(kScale.Y());
            kS3.set(kScale.Z());
            kTimeStr.set((float)kT.as(MTime::kSeconds));
            MGlobal::displayError("Possible non-uniform scale at time " +
                kTimeStr + ": (" + kS1 + "," + kS2 + "," + kS3 +
                ").  Using the average instead.");
        }

        // Append data into the arrays
        kTimesAr.Append((float)kT.as(MTime::kSeconds));
        kScaleAr.Append((kScale.X()+kScale.Y()+kScale.Z())/3.0f);

        kT = GetTimeAndIncrement(pkXIt,pkYIt,pkZIt,kPastMaxTime);
    }

    // We need to now copy the data out of the TArrays into shared arrays that
    // can be passed to the keyframe controller.
    int iTQuantity = kTimesAr.GetQuantity();
    if (iTQuantity <= 0)
        return false;

    float* afScaleData = new float[iTQuantity];
    float* afTimeData = new float[iTQuantity];
    for(int i = 0; i < iTQuantity; i++)
    {
        afScaleData[i] = kScaleAr[i];
        afTimeData[i] = kTimesAr[i];
    }
    FloatArrayPtr spkTimes = new FloatArray(iTQuantity,afTimeData);
    FloatArrayPtr spkScale = new FloatArray(iTQuantity,afScaleData);
    pkKFCtrl->ScaleTimes = spkTimes;
    pkKFCtrl->ScaleData = spkScale;

    return true;
}
//----------------------------------------------------------------------------
static bool SetTranslateAnim (const MFnDagNode& rkDagNode, 
    KeyframeController* pkKFCtrl, const Vector3f& rkStatic)
{
    // This function sets the translation component of pkKFCtrl for the
    // node rkDagNode which already has the static, non-keyframed translation
    // of rkStatic.  It returns true if a translation was found (and set)
    // and false otherwise.
   
    // Assumption: pkKFCtrl is already instantiated
    assert( pkKFCtrl );

    // get translation keys
    MStatus kStatus;
    MPlug kTranslatePlug = rkDagNode.findPlug("translate",&kStatus);
    assert( kStatus );

    // Components may not all be connected.  Maybe the user
    // just keyframed one principle component.  Thus, we'll use
    // pointers to reference keyframe data, with NULL meaning
    // that component is not keyed.
    MItKeyframe* pkXIt;
    MItKeyframe* pkYIt;
    MItKeyframe* pkZIt;
    MFnAnimCurve* pkXCurve;
    MFnAnimCurve* pkYCurve;
    MFnAnimCurve* pkZCurve;
    GetCurvesAndIters(pkXIt,pkYIt,pkZIt,pkXCurve,pkYCurve,pkZCurve,
        kTranslatePlug);

    // Maybe there are no translation components...
    if (!pkXIt && !pkYIt && !pkZIt)
        return false;

    // Use TArrays, because it isn't known how many keyframes will be needed
    TArray<float> kTimesAr;
    TArray<Vector3f> kTransAr;

    MTime kMaxTime(pkKFCtrl->MaxTime,MTime::kSeconds);
    // Use a time greater than max time, so that the while loop below
    // will break when kT hits this time.  This is because it is
    // possible for kT to have a keyframe at MaxTime, so we need something
    // we know is greater than it as a "no more keyframes" value.
    MTime kPastMaxTime(pkKFCtrl->MaxTime+1,MTime::kSeconds);

    MTime kT = GetTimeAndIncrement(pkXIt,pkYIt,pkZIt,kPastMaxTime);
    while (kT <= kMaxTime)
    {
        Vector3f kTrans = rkStatic;

        // Evaluate curves at keyframe time.  If there are no
        // keyframes for a component, use the static transform data.
        if (pkXCurve)
        {
            kTrans.X() = (float)pkXCurve->evaluate(kT,&kStatus);
            assert(kStatus);
        }
        if (pkYCurve)
        {
            kTrans.Y() = (float)pkYCurve->evaluate(kT,&kStatus);
            assert(kStatus);
        }
        if (pkZCurve)
        {
            kTrans.Z() = (float)pkZCurve->evaluate(kT,&kStatus);
            assert(kStatus);
        }

        // Append data into the arrays
        kTimesAr.Append((float)kT.as(MTime::kSeconds));
        kTransAr.Append(kTrans);

        kT = GetTimeAndIncrement(pkXIt,pkYIt,pkZIt,kPastMaxTime);
    }

    // We need to now copy the data out of the TArrays into shared arrays that
    // can be passed to the keyframe controller.
    int iTQuantity = kTimesAr.GetQuantity();
    if (iTQuantity <= 0)
        return false;

    Vector3f* akTransData = new Vector3f[iTQuantity];
    float* afTimeData = new float[iTQuantity];
    for(int i = 0; i < iTQuantity; i++)
    {
        akTransData[i] = kTransAr[i];
        afTimeData[i] = kTimesAr[i];
    }
    FloatArrayPtr spkTimes = new FloatArray(iTQuantity,afTimeData);
    Vector3fArrayPtr spkTrans = new Vector3fArray(iTQuantity,akTransData);
    pkKFCtrl->TranslationTimes = spkTimes;
    pkKFCtrl->TranslationData = spkTrans;

    return true;
}
//----------------------------------------------------------------------------
static KeyframeController* GetInvScaleController(
    const KeyframeController* pkSource)
{
    // Takes the scale times and data from pkSource and returns a second
    // controller, identical in all ways except the scale data is
    // inverse.

    KeyframeController* pkInv = new KeyframeController();

    // copy member variables
    pkInv->RepeatType = pkSource->RepeatType;
    pkInv->MinTime = pkSource->MinTime;
    pkInv->MaxTime = pkSource->MaxTime;
    pkInv->Phase = pkSource->Phase;
    pkInv->Frequency = pkSource->Frequency;
    pkInv->Active = pkSource->Active;

    // share time data
    pkInv->ScaleTimes = pkSource->ScaleTimes;

    // create inverse time data
    int iSize = pkSource->ScaleData->GetQuantity();
    float* afInvData = new float[iSize];
    for (int i = 0; i < iSize; i++)
    {
        afInvData[i] = 1.0f/(*pkSource->ScaleData)[i];
    }
    pkInv->ScaleData = new FloatArray(iSize,afInvData);

    return pkInv;
}
//----------------------------------------------------------------------------
void Exporter::CTransform (const MFnDagNode& rkDagNode)
{
    // This function transform both MFnTransform objects as well as
    // MFnIkJoints (which is derived from MFnTransform).  It creates a Node*
    // and inserts it into the map and attaches to the correct parent
    // in the tree.

    // Because transforms create nodes, we will always insert them into
    // the scenetree, even if they are invisible.  The reason is because
    // they could be the parent of a joint node (or a joint node itself)
    // which needs to be in the tree for skin controllers.  The user may
    // need to cull the scenetree themselves to get rid of these childless
    // nodes.

    // Flag for special joint behavior
    bool bJoint = (rkDagNode.object().apiType() == MFn::kJoint);

    // ensure dagnode is visible
    bool bVisible = true;
    MPlug kVisibility = rkDagNode.findPlug("visibility",&m_kStatus);
    if ( m_kStatus )
    {
        kVisibility.getValue(bVisible);
    }

    // ensure dagnode layer is visible
    MPlug kDrawOverride = rkDagNode.findPlug("drawOverride",&m_kStatus);
    assert( m_kStatus );

    MPlugArray kUpstreamPlugs;
    kDrawOverride.connectedTo(kUpstreamPlugs,true,false,&m_kStatus); 
    assert( m_kStatus );

    if ( kUpstreamPlugs.length() != 0 )
    {
        // not a visible layer
        // Note: Not really sure what this means
        MObject kLayer = kUpstreamPlugs[0].node();
        assert( kLayer.hasFn(MFn::kDisplayLayer) );

        MFnDependencyNode kLayerNode(kLayer,&m_kStatus);
        assert( m_kStatus);
        MPlug kVisibilityLayer = kLayerNode.findPlug("visibility",&m_kStatus);
        assert( m_kStatus );
        bool bVisibleLayer;
        kVisibilityLayer.getValue(bVisibleLayer);
        bVisible &= bVisibleLayer;
    }

    // Check DagNode overrides

    MPlug kOverrideEnabled = rkDagNode.findPlug("overrideEnabled",&m_kStatus);
    assert(m_kStatus);
    bool bOverrideEnabled;
    kOverrideEnabled.getValue(bOverrideEnabled);
    bool bPlaybackEnabled = true;

    if (bOverrideEnabled)
    {
        // visibility override
        MPlug kTempPlug;
        
        kTempPlug = rkDagNode.findPlug("overrideVisibility", &m_kStatus);
        assert(m_kStatus);
        bool bOverrideVisible;
        kTempPlug.getValue(bOverrideVisible);
        bVisible &= bOverrideVisible;

        // keyframe override
        kTempPlug = rkDagNode.findPlug("overridePlayback", &m_kStatus);
        assert(m_kStatus);
        kTempPlug.getValue(bPlaybackEnabled);
    }

    if (!bVisible)
    {
        DisplayInfo("Transform/Joint is invisible.  "\
            "Flagging subtree.");

        // flag invisible subtree
        m_bObjectInvisible = true;
    }
    
    // Get all of the transforms:

    MFnTransform kXForm(rkDagNode.object(),&m_kStatus);
    assert( m_kStatus );

    // get translation
    MVector kMTrn = kXForm.translation(MSpace::kTransform);
    Vector3f kTranslation = Vector3f((float)kMTrn.x,(float)kMTrn.y,
        (float)kMTrn.z);

    // Get rotation order so we know how to interpret the rotation
    // itself.
    int iRotateOrder;
    MPlug kPRotateOrder = rkDagNode.findPlug("rotateOrder",&m_kStatus);
    assert( m_kStatus );
    kPRotateOrder.getValue(iRotateOrder);        

    // Get the rotation itself (in radians)
    MEulerRotation kMRot;
    kXForm.getRotation(kMRot);
    Matrix3f kRotation = CreateRotate(iRotateOrder,(float)kMRot.x,
        (float)kMRot.y,(float)kMRot.z);

    // Get the rotation orientation (a static rotate)
    // Rotation orientations are applied after the normal rotation.
    Matrix3f kRotOrient;
    MFnTransform kFnTrans(rkDagNode.object(),&m_kStatus);
    assert( m_kStatus );
    MQuaternion kQRotOrient = kFnTrans.rotateOrientation(MSpace::kTransform,
        &m_kStatus);
    assert( m_kStatus );
    Quaternionf kWMQuat((float)kQRotOrient.w,(float)kQRotOrient.x,
        (float)kQRotOrient.y,(float)kQRotOrient.z);
    kWMQuat.ToRotationMatrix(kRotOrient);

    kRotOrient = Matrix3f::IDENTITY;

    Matrix3f kRotJoint = Matrix3f::IDENTITY;

    // Special case: If this is a joint, there is also a "joint rotation"
    // which needs to be applied.
    //
    // As best I can tell, a "joint orient" rotates the geometry attached
    // to the joint, the children of the joint (other joints or geometry),
    // and the joint's local reference frame itself.  A rotation of the
    // joint (or an orientation rotation) will do all of the above except
    // rotate the joint's local reference frame.  Since nodes are not
    // displayed, there is only a semantic difference.  However, joint
    // rotates are applied before the normal rotate.
    if (bJoint)
    {
        MFnIkJoint kJointFn(rkDagNode.object(),&m_kStatus);
        assert(m_kStatus);

        MQuaternion kJRot;
        kJointFn.getOrientation(kJRot);
        Quaternionf kWMRotQ((float)kJRot.w,(float)kJRot.x,(float)kJRot.y,
            (float)kJRot.z);

        kWMRotQ.ToRotationMatrix(kRotJoint);
    }

    // get scale
    double adMScale[3];
    kXForm.getScale(adMScale);

    // Check for zero scales
    if (!adMScale[0] || !adMScale[1] || !adMScale[2])
    {
        MString kS1,kS2,kS3;
        kS1.set(adMScale[0]);
        kS2.set(adMScale[1]);
        kS3.set(adMScale[2]);
        MGlobal::displayError("Zero scale found: (" + 
            kS1 + "," + kS2 + "," + kS3 +
            ").  Using (1,1,1) instead.");            

        // fix these scales
        adMScale[0] = 1.0;
        adMScale[1] = 1.0;
        adMScale[2] = 1.0;
    }

    // Set the uniform scale to be the average of all the scales
    Vector3f kScale((float)adMScale[0],(float)adMScale[1],
        (float)adMScale[2]);

    // Check for shear.  It's not supported, but we can warn the user.
    double adMShear[3];
    kXForm.getShear(adMShear);
    if (adMShear[0] || adMShear[1] || adMShear[2])
    {
        DisplayWarning("Ignoring shear parameters.");
    }

    // Before we can attach this node into the WM3 scene
    // graph and into the node map, we need to check if there
    // are pivot points (rotate and scale).
    //
    // There are two parts to a pivot in Maya.  There's the actual
    // pivot (rotatePivot and scalePivot) which is the local point
    // which the object needs to be translated to before the
    // transformation is applied and un-translated to after the
    // transformation.  There's also a "world pivot," which needs
    // to be added to the pivot translation (but not removed).
    // 
    // The full transformation sequence for a transform is:
    // Translate,RPivot,Rotate,RotOrient,RPivot^-1,SPivot,Scale,SPivot^-1
    //
    // The full transformation sequence for a joint is:
    // Translate,JointInvScale,JointOrient,Rotate,RotOrient,Scale
    //
    // Of these, JointInvScale, Rotate, Scale, and Translate may have
    // keyframes, and the rest are static.  Non-joint nodes do not
    // have either JointInvScale nor JointOrient.
    //
    // If joints, then the chain is two nodes:
    //   (parent node)->(Translate|JointInvScale)->
    //   (JointOrient|Rotate|RotOrient|Scale)->(children)
    //
    // If no pivots and no joints, then the chain is a single node:
    //   (parent node)->(Translate|Rotate|RotOrient|Scale)->(children)
    //
    // If pivots, then the chain is three nodes:
    //   (parent node)->(Translate)->
    //   (RPivot|Rotate|RotOrient)->(RPivot^-1|SPivot|Scale)->
    //   (SPivot^-1)->(children)
    //
    // In all of these cases, JointOrient and RotOrient will be
    // multiplied into the static transformation and all of
    // the rotate keyframe data (if it exists).

    MVector kRPivotM = kXForm.rotatePivot(MSpace::kTransform);
    MVector kRWorldPivotM = kXForm.rotatePivotTranslation(
        MSpace::kTransform);
    MVector kSPivotM = kXForm.scalePivot(MSpace::kTransform);
    MVector kSWorldPivotM = kXForm.scalePivotTranslation(
        MSpace::kTransform);

    // convert pivots to WM3 vectors
    Vector3f kRPivot((float)kRPivotM[0],(float)kRPivotM[1], 
        (float)kRPivotM[2]);
    Vector3f kRWorldPivot((float)kRWorldPivotM[0],(float)kRWorldPivotM[1], 
        (float)kRWorldPivotM[2]);
    Vector3f kSPivot((float)kSPivotM[0],(float)kSPivotM[1], 
        (float)kSPivotM[2]);
    Vector3f kSWorldPivot((float)kSWorldPivotM[0],(float)kSWorldPivotM[1], 
        (float)kSWorldPivotM[2]);

    bool bPivots = (kRPivot.SquaredLength() > 0 || 
        kSPivot.SquaredLength() > 0 || kRWorldPivot.SquaredLength() > 0 ||
        kSWorldPivot.SquaredLength() > 0);

    if (bPivots && bJoint)
    {
        // NOTE: I'm nearly positive that joints may not have pivots
        // but on the very slim chance that this is not true, warn the
        // user.
        DisplayWarning("Pivots not supported for joint nodes."\
            "  The pivots will be ignored.");
        bPivots = false;
    }

    // Create some handles that will make actions common to all three
    // cases easier.
    //
    // No matter what kind of pivots there are, pkFirst will be made
    // to point to the first node in the chain (to attach to the
    // parent in the scene graph) and pkLast will point to the last node
    // (to go into the node map for other nodes to attach to it).
    // pkSNode, pkTNode, and pkRNode will be set to the nodes that need
    // the static/keyframed scale, translate, and rotate.  This will
    // also make it easier to attach the keyframe controller(s) to
    // the right place.  These nodes may all be the same node.
    Node* pkFirst = NULL;
    Node* pkLast = NULL;
    Node* pkSNode = NULL;
    Node* pkTNode = NULL;
    Node* pkRNode = NULL;

    int iChildQuantity = (int)rkDagNode.childCount();

    if (iChildQuantity <= 0)
    {
        // iChildQuantity is used to pre-create the last node in the
        // chain with the correct size for the number of children.
        // Some joints or nodes without children will have a zero here
        // and so bump this up to a minimum of 1.
        iChildQuantity = 1;
    }

    // If there are any pivots...
    if (bJoint)
    {
        pkFirst = pkTNode = new Node(1);
        pkFirst->SetName((rkDagNode.name() + "_T").asChar());
        pkLast = pkRNode = pkSNode = new Node(iChildQuantity);
        pkFirst->AttachChild(pkLast);
        pkLast->SetName((rkDagNode.name() + "_RS").asChar());
    }
    else if (bPivots)
    {
        // Even if there is only one pivot or just a world pivot location,
        // S and TR still need to be split up in case there is keyframe 
        // animation.  If S,T, and R are all in the same node, then the 
        // sPivot and rPivot are dependent on the rotation and scale 
        // (respectively). Thus, putting S,T,R in the same node only works 
        // if S and R are static which can't be assumed here.
        pkFirst = new Node(1);
        pkFirst->SetName((rkDagNode.name() + "_doRPivot").asChar());
        pkFirst->Local.SetTranslate(kRPivot+kRWorldPivot);
        pkRNode = pkTNode = new Node(1);
        pkRNode->SetName((rkDagNode.name() + "_RT").asChar());
        pkFirst->AttachChild(pkRNode);
        pkSNode = new Node(1);
        pkSNode->SetName((rkDagNode.name() + "_S").asChar());
        pkRNode->AttachChild(pkSNode);
        pkSNode->Local.SetTranslate(kSPivot+kSWorldPivot-kRPivot);
        pkLast = new Node(iChildQuantity);
        pkLast->SetName((rkDagNode.name() + "_undoSPivot").asChar());
        pkSNode->AttachChild(pkLast);
        pkLast->Local.SetTranslate(-kSPivot);
    }
    else // if (no pivots and not a joint)
    {
        pkSNode = pkRNode = pkTNode = pkLast = pkFirst = 
            new Node(iChildQuantity);
        pkFirst->SetName(rkDagNode.name().asChar());
    }

    // Set static transformations
    pkTNode->Local.SetTranslate(kTranslation);
    pkRNode->Local.SetRotate(kRotJoint*kRotation*kRotOrient);

    const float fTol = 1e-06f;
    float fScale = ((kScale[0] + kScale[1] + kScale[2])/3.0f);
    bool bUniformScale;
    if (Mathf::FAbs(kScale[0] - kScale[1]) < fTol &&
        Mathf::FAbs(kScale[0] - kScale[2]) < fTol &&
        Mathf::FAbs(kScale[1] - kScale[2]) < fTol)
    {
        // Uniform scale
        pkSNode->Local.SetUniformScale(fScale);
        bUniformScale = true;
    }
    else
    {
        pkSNode->Local.SetScale(kScale);
        bUniformScale = false;
    }
    
    // Check if this node inherits the transform from its hierarchy
    // parent.  If not, then we will attach it to the world node
    // instead of its "official" parent.
    //
    // This breaks the logical Maya hierarchy, but preserves
    // the transformation hierarchy, which is more important.
    bool bInheritFromParent;
    MPlug kInherit = rkDagNode.findPlug("inheritsTransform",
        &m_kStatus);
    assert( m_kStatus );
    kInherit.getValue(bInheritFromParent); 

    // Attach this node to its parent
    AttachChainToParent(rkDagNode,pkFirst,pkLast,!bInheritFromParent);

    // If a joint not attached to the world, need inverse scales
    if (bInheritFromParent && bJoint)
    {
        Spatial* pkParent = pkFirst->GetParent();
        assert(pkParent);

        // static transform
        assert(pkParent->Local.IsRSMatrix());
        if (pkParent->Local.IsUniformScale())
        {
            float fScale = pkParent->Local.GetUniformScale();
            pkFirst->Local.SetUniformScale(1.0f/fScale);
        }
        else
        {
            Vector3f kScale = pkParent->Local.GetScale();
            pkFirst->Local.SetScale(Vector3f(1.0f/kScale[0],
                1.0f/kScale[1],1.0f/kScale[2]));
        }

        // get keyframe if it exists
        for (int i = 0; i < pkParent->GetControllerQuantity(); i++)
        {
            Controller* pkCont = pkParent->GetController(i);

            if (pkCont->IsDerived(KeyframeController::TYPE))
            {
                KeyframeController* pkKeyf = 
                    StaticCast<KeyframeController>(pkCont);

                if (pkKeyf->ScaleTimes && pkKeyf->ScaleData)
                {
                    DisplayInfo("Found controller!");
                    pkFirst->SetController(GetInvScaleController(pkKeyf));
                    break;
                }
            }
        }
    }

    // If playback has been turned off or the keyframe flag has not been set,
    // then ignore keyframe controllers
    if (!bPlaybackEnabled || !m_abFlags[FLAG_KEYS])
        return;

    // The only thing left to do is to attach keyframe controllers
    // (if they exist)
    bool bAnimated = MAnimUtil::isAnimated(rkDagNode.object(),false,
        &m_kStatus);
    assert( m_kStatus );
    if ( bAnimated )
    {
        KeyframeController* pkKFCtrl = GetBareController();
        bool bTrans = SetTranslateAnim(rkDagNode,pkKFCtrl,kTranslation);

        if (bTrans && (pkTNode != pkRNode))
        {
            pkTNode->SetController(pkKFCtrl);
            // There is no longer a translation to set
            bTrans = false;
            // make a new controller for the rotate
            pkKFCtrl = GetBareController();
        }

        bool bRotate = SetRotateAnim(rkDagNode,pkKFCtrl,iRotateOrder,
            (float)kMRot.x,(float)kMRot.y,(float)kMRot.z,
            kRotJoint,kRotOrient);

        if (bRotate && (pkRNode != pkSNode))
        {
            pkRNode->SetController(pkKFCtrl);
            // There is no longer a rotation to set
            bRotate = false;
            // make a new controller for the scale
            pkKFCtrl = GetBareController();
        }

        bool bScale = SetScaleAnim(rkDagNode,pkKFCtrl,fScale);
        if (bScale || bTrans || bRotate)
        {
            pkSNode->SetController(pkKFCtrl);
            if (!bUniformScale)
            {
                // Due to the fact that we found keyframe scale data, this
                // node will be set to using a uniform scale
                pkSNode->Local.SetUniformScale(fScale);
                DisplayWarning("Keyframe data found on a node with a "\
                    "non-uniform scale.  Using uniform scale instead.");
            }
        }
        else
        {
            // There have been no rotate, scales, or translates on
            // the current controller
            delete pkKFCtrl;
        }
    }
}
//----------------------------------------------------------------------------

