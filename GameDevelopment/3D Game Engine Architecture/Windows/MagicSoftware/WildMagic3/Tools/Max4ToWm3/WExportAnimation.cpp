// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "WSceneBuilder.h"
#include "decomp.h"
#include <algorithm>
using namespace std;

//----------------------------------------------------------------------------
bool WSceneBuilder::AreEqual (const Point3& rkPoint1, const Point3& rkPoint2)
{   
    // Fuzzy comparison which determines whether two points have (almost) the
    // same coordinates.

    return fabsf(rkPoint1.x - rkPoint2.x) <= MIN_DIFFERENCE
        && fabsf(rkPoint1.y - rkPoint2.y) <= MIN_DIFFERENCE
        && fabsf(rkPoint1.z - rkPoint2.z) <= MIN_DIFFERENCE;
}
//----------------------------------------------------------------------------
bool WSceneBuilder::AreEqual (const Quat& rkQuat1, const Quat& rkQuat2)
{
    // Fuzzy comparison which determines whether two quaternions have (almost)
    // the same coordinates.

    return fabsf(rkQuat1.x - rkQuat2.x) <= MIN_DIFFERENCE
        && fabsf(rkQuat1.y - rkQuat2.y) <= MIN_DIFFERENCE
        && fabsf(rkQuat1.z - rkQuat2.z) <= MIN_DIFFERENCE
        && fabsf(rkQuat1.w - rkQuat2.w) <= MIN_DIFFERENCE;
}
//----------------------------------------------------------------------------
bool WSceneBuilder::CompareKeyTimes (KeyInfo* pkFirst, KeyInfo* pkSecond)
{
    // Comparison method to sort KeyInfo objects into order of  ascending
    // time.
    return pkFirst->Time < pkSecond->Time;
}
//----------------------------------------------------------------------------
void WSceneBuilder::BuildKeyFrameController (INode* pkMaxNode,
    Wm3::Spatial* pkWm3Node)
{
    // Attaches a key frame controller to a key frame animated node.  Note
    // Note that animation involving non-uniform scale is NOT supported.
    //
    // pkMaxNode:
    //     Key frame animated node in the Max node hierarchy.
    // pkWm3Node:
    //     Key frame animated node in the Wild Magic scene graph.

    assert( pkWm3Node );
    assert( pkMaxNode );

    // check that this node is key frame animated
    Control* pkTMC = pkMaxNode->GetTMController();

    assert( pkTMC );

    if ( !pkTMC->IsKeyable() )
    {
        // The node is not keyframed, but it may still be animated, e.g. if
        // it is an IK node.  In that case, export all frames of the node's
        // animation.
        BuildFrameController(pkMaxNode,pkWm3Node);
        return;
    }

    // For optimization, check whether the node's translation, rotation and
    // scaling actually change over the time interval of interest.
    AnimationTiming kTrnTiming, kRotTiming, kScaTiming;
    if ( !GetAnimationTiming(pkMaxNode,kTrnTiming,kRotTiming,kScaTiming) ) 
        return;

    vector<KeyInfo*> kKeyInfo;

    // process translation keys
    int iNumTrnKeys = 0;
    if ( kTrnTiming.Active )
    {
        Control* pkPC = pkTMC->GetPositionController();
        IKeyControl* pkKeyCon = GetKeyControlInterface(pkPC);
        if ( pkKeyCon )
        {

            iNumTrnKeys = pkKeyCon->GetNumKeys();
            GetTrnKeyInfo(iNumTrnKeys,pkPC->ClassID(),pkKeyCon,kTrnTiming,
                kKeyInfo);
        }
    }

    // process rotation keys
    int iNumRotKeys = 0;
    if ( kRotTiming.Active )
    {
        Control* pkRC = pkTMC->GetRotationController();
        IKeyControl* pkKeyCon = GetKeyControlInterface(pkRC);
        if ( pkKeyCon )
        {
            iNumRotKeys = pkKeyCon->GetNumKeys();
            GetRotKeyInfo(iNumRotKeys,pkRC->ClassID(),pkKeyCon,kRotTiming,
                kKeyInfo);
        }
    }

    // process scale keys
    int iNumScaKeys = 0;
    if ( kScaTiming.Active )
    {
        Control* pkSC = pkTMC->GetScaleController();
        IKeyControl* pkKeyCon = GetKeyControlInterface(pkSC);
        if ( pkKeyCon )
        {
            iNumScaKeys = pkKeyCon->GetNumKeys();
            GetScaleKeyInfo(iNumScaKeys,pkSC->ClassID(),pkKeyCon,kScaTiming,
                kKeyInfo);
        }
    }

    // If an object uses procedural controllers instead of plain keyframe
    // controllers, then all of the above GetKeyControlInterface(...) calls
    // will return NULL, because there are no "keys" in a procedural controller.
    // In that case, there will be no keys.
    if ( !iNumTrnKeys && !iNumRotKeys && !iNumScaKeys )
    {
        BuildFrameController(pkMaxNode,pkWm3Node);
        return;
    }

    // Instantiate the arrays for the keyframe controller

    Wm3::KeyframeController* pkWm3KFC = new Wm3::KeyframeController;

    if ( iNumTrnKeys > 0 )
    {
        pkWm3KFC->TranslationTimes = new Wm3::FloatArray(iNumTrnKeys,
            new float[iNumTrnKeys]);
        pkWm3KFC->TranslationData = new Wm3::Vector3fArray(iNumTrnKeys,
            new Wm3::Vector3f[iNumTrnKeys]);
    }
    if ( iNumRotKeys > 0 )
    {
        pkWm3KFC->RotationTimes = new Wm3::FloatArray(iNumRotKeys,
            new float[iNumRotKeys]);
        pkWm3KFC->RotationData = new Wm3::QuaternionfArray(iNumRotKeys,
            new Wm3::Quaternionf[iNumRotKeys]);
    }
    if ( iNumScaKeys > 0 )
    {
        pkWm3KFC->ScaleTimes = new Wm3::FloatArray(iNumScaKeys,
            new float[iNumScaKeys]);
        pkWm3KFC->ScaleData = new Wm3::FloatArray(iNumScaKeys,
            new float[iNumScaKeys]);
    }

    // sort the vector of KeyInfo objects in order of ascending time
    sort(kKeyInfo.begin(),kKeyInfo.end(),CompareKeyTimes);

    // get pointers to the keyframe controller arrays
    Wm3::Vector3f* pkTKey = (pkWm3KFC->TranslationData ? 
        pkWm3KFC->TranslationData->GetData() : NULL);
    float* pfTTime = (pkWm3KFC->TranslationTimes ?
        pkWm3KFC->TranslationTimes->GetData() : NULL);

    Wm3::Quaternionf* pkRKey = (pkWm3KFC->RotationData ?
        pkWm3KFC->RotationData->GetData() : NULL);
    float* pfRTime = (pkWm3KFC->RotationTimes ?
        pkWm3KFC->RotationTimes->GetData() : NULL);

    float* pfSKey = (pkWm3KFC->ScaleData ?
        pkWm3KFC->ScaleData->GetData() : NULL);
    float* pfSTime = (pkWm3KFC->ScaleTimes ?
        pkWm3KFC->ScaleTimes->GetData() : NULL);
    TimeValue iTimeNow = -1;

    Wm3::Transformation kTransform;

    for (int i = 0; i < (int)kKeyInfo.size(); i++ )
    {
        KeyInfo* pkInfo = kKeyInfo[i];

        if ( iTimeNow != pkInfo->Time )
        {
            iTimeNow = pkInfo->Time;
            kTransform = GetLocalTransform(pkMaxNode,iTimeNow);
        }

        switch ( pkInfo->Type )
        {
        case KT_TRANSLATION:
            assert( pkTKey );
            assert( pfTTime );
            *pkTKey++ = kTransform.GetTranslate();
            *pfTTime++ = TicksToSec(iTimeNow - m_iTimeOffset);
            break;
        case KT_ROTATION:
            // TO DO.  Warn about non-RS matrices.
            if ( kTransform.IsRSMatrix() )
            {
                assert( pkRKey );
                assert( pfRTime );
                pkRKey++->FromRotationMatrix(kTransform.GetRotate());
                *pfRTime++ = TicksToSec(iTimeNow - m_iTimeOffset);
            }
            break;
        case KT_SCALE:
        {
            // TO DO.  Warn about nonuniform or negative scales.
            float fMax;
            if ( kTransform.IsUniformScale() )
            {
                fMax = kTransform.GetUniformScale();
            }
            else
            {
                fMax = Wm3::Mathf::FAbs(kTransform.GetScale().X());
                float fAbs = Wm3::Mathf::FAbs(kTransform.GetScale().Y());
                if ( fAbs > fMax )
                    fMax = fAbs;
                fAbs = Wm3::Mathf::FAbs(kTransform.GetScale().Z());
                if ( fAbs > fMax )
                    fMax = fAbs;
            }

            assert( pfSKey );
            assert( pfSTime );
            *pfSKey++ = fMax;
            *pfSTime++ = TicksToSec(iTimeNow - m_iTimeOffset);
            break;
        }
        }

        delete pkInfo;
    }

    pkWm3KFC->MinTime = TicksToSec(m_iTimeStart - m_iTimeOffset);
    pkWm3KFC->MaxTime = TicksToSec(m_iTimeEnd - m_iTimeOffset);
    pkWm3Node->SetController(pkWm3KFC);
}
//----------------------------------------------------------------------------
void WSceneBuilder::BuildFrameController (INode* pkNode,
    Wm3::Spatial* pkWm3Node)
{
    // Attaches a "frame controller" to a key frame animated node.  This
    // method exports ALL frames, rather than just key frames, by stuffing
    // the node's transform for each frame into a Wm3::KeyframeController.
    // This isn't optimal, in the sense that Wild Magic will still
    // interpolate between the individual frames, however, it is useful in
    // the case where the animator uses IK animation, which the exporter
    // currently doesn't support.  Note that animation involving nonuniform
    // scale is NOT supported.
    //
    // pkNode:
    //     Animated node in the Max node hierarchy.
    // pkWm3Node:
    //     Animated node in the Wild Magic scene graph.

    const char* acName = (const char*)pkNode->GetName();
    if ( strcmp(acName,"Bip01 L Finger0") == 0 )
    {
        int iStopHere = 0;
    }

    // store the unique keys (using fuzzy comparisons)
    vector<int> kTTime, kRTime, kSTime;
    vector<Point3> kTData, kSData;
    vector<Quat> kRData;

    // Get the components of the initial transformation.  Each successive
    // component is compared to the previous one to see if a change has
    // occurred.  If so, the new component is added to the list.  If not,
    // it is ignored.  At the end of the process, if an array has one
    // element, no keyframe data is generated.
    int iT = m_iTimeStart;
    Matrix3 kLocal = pkNode->GetNodeTM(iT)*Inverse(pkNode->GetParentTM(iT));
    AffineParts kAffParts;
    decomp_affine(kLocal,&kAffParts);
    kTTime.push_back(iT);
    kRTime.push_back(iT);
    kSTime.push_back(iT);
    kTData.push_back(kAffParts.t);
    kRData.push_back(kAffParts.q);
    kSData.push_back(kAffParts.k);

    // TO DO.  Test kAffParts.f to see if it is negative.  This represents
    // either negatives scales or a reflection rather than a rotation.  Warn
    // about this when it happens.

    // Generate samples for the duration of the animation period.  Unlike the
    // ASCII exporter sample application, I use an exact comparison of the
    // components.  Postprocessing tools may then be used to reduce the
    // number of keyframes using algorithms more sophisticated than pairwise
    // comparisons.
    for (iT += m_iTicksPerFrame; iT <= m_iTimeEnd; iT += m_iTicksPerFrame)
    {
        kLocal = pkNode->GetNodeTM(iT)*Inverse(pkNode->GetParentTM(iT));
        decomp_affine(kLocal,&kAffParts);

        if ( !AreEqual(kAffParts.t,kTData.back()) )
        {
            kTTime.push_back(iT);
            kTData.push_back(kAffParts.t);
        }

        // DEBUGGING
        float fAngle0, fAngle1;
        Point3 kAxis0, kAxis1;
        AngAxisFromQ(kRData.back(),&fAngle0,kAxis0);
        AngAxisFromQ(kAffParts.q,&fAngle1,kAxis1);

        if ( !AreEqual(kAffParts.q,kRData.back()) )
        {
            // q and -q represent the same rotation
            if ( !AreEqual(-kAffParts.q,kRData.back()) )
            {
                kRTime.push_back(iT);

                // Always store consecutive quaternions so that their angle
                // as 4D vectors is minimized.
                float fDot = kAffParts.q % kRData.back();
                if ( fDot > 0.0f )
                    kRData.push_back(kAffParts.q);
                else
                    kRData.push_back(-kAffParts.q);
            }
        }

        if ( !AreEqual(kAffParts.k,kSData.back()) )
        {
            kSTime.push_back(iT);
            kSData.push_back(kAffParts.f*kAffParts.k);
        }

        // Determine if the scales are uniform.
        bool bScaleIsUniform =
            fabsf(kAffParts.k.x - kAffParts.k.y) < MIN_DIFFERENCE &&
            fabsf(kAffParts.k.y - kAffParts.k.z) < MIN_DIFFERENCE;

        // Determine if the uniform scales are all 1.
        bool bScaleIsUnity = bScaleIsUniform &&
            fabsf(kAffParts.k.x - 1.0f) < MIN_DIFFERENCE;

        // Determine if the scale-orientation is the identity.
        if ( kAffParts.u.w < 0.0f )
            kAffParts.u = -kAffParts.u;

        bool bOrientIsIdentity = bScaleIsUniform || (
            fabsf(kAffParts.u.w - 1.0f) < MIN_DIFFERENCE &&
            fabsf(kAffParts.u.x) < MIN_DIFFERENCE &&
            fabsf(kAffParts.u.y) < MIN_DIFFERENCE &&
            fabsf(kAffParts.u.z) < MIN_DIFFERENCE);

        int iStopHere = 0;
    }

    if ( kTData.size() == 1 && kRData.size() == 1 && kSData.size() == 1 )
    {
        // This node is not animated.
        return;
    }

    // TO DO.  Determine if any of the time arrays are equal so that they
    // may be shared.

    Wm3::KeyframeController* pkKFCtrl = new Wm3::KeyframeController;
    pkKFCtrl->MinTime = TicksToSec(m_iTimeStart - m_iTimeOffset);
    pkKFCtrl->MaxTime = TicksToSec(m_iTimeEnd - m_iTimeOffset);

    if ( kTData.size() > 1 )
    {
        int iNumTKeys = (int)kTData.size();
        float* afTTime = new float[iNumTKeys];
        Wm3::Vector3f* akTData = new Wm3::Vector3f[iNumTKeys];
        for (int i = 0; i < iNumTKeys; i++)
        {
            afTTime[i] = TicksToSec(kTTime[i] - m_iTimeOffset);
            akTData[i].X() = kTData[i].x;
            akTData[i].Y() = kTData[i].y;
            akTData[i].Z() = kTData[i].z;
        }

        pkKFCtrl->TranslationTimes = new Wm3::FloatArray(iNumTKeys,afTTime);
        pkKFCtrl->TranslationData = new Wm3::Vector3fArray(iNumTKeys,
            akTData);
    }

    if ( kRData.size() > 1 )
    {
        int iNumRKeys = (int)kRData.size();
        float* afRTime = new float[iNumRKeys];
        Wm3::Quaternionf* akRData = new Wm3::Quaternionf[iNumRKeys];
        for (int i = 0; i < iNumRKeys; i++)
        {
            afRTime[i] = TicksToSec(kRTime[i] - m_iTimeOffset);
            akRData[i].W() = kRData[i].w;
            akRData[i].X() = -kRData[i].x;
            akRData[i].Y() = -kRData[i].y;
            akRData[i].Z() = -kRData[i].z;

            // DEBUGGING
            Wm3::Vector3f kAxis;
            float fAngle;
            akRData[i].ToAxisAngle(kAxis,fAngle);
            int iStopHere = 0;
        }

        pkKFCtrl->RotationTimes = new Wm3::FloatArray(iNumRKeys,afRTime);
        pkKFCtrl->RotationData = new Wm3::QuaternionfArray(iNumRKeys,
            akRData);
    }

    if ( kSData.size() > 1 )
    {
        int iNumSKeys = (int)kSData.size();
        float* afSTime = new float[iNumSKeys];
        float* afSData = new float[iNumSKeys];
        for (int i = 0; i < iNumSKeys; i++)
        {
            afSTime[i] = TicksToSec(kSTime[i] - m_iTimeOffset);
            afSData[i] = kSData[i].x;
        }

        pkKFCtrl->ScaleTimes = new Wm3::FloatArray(iNumSKeys,afSTime);
        pkKFCtrl->ScaleData = new Wm3::FloatArray(iNumSKeys,afSData);
    }

    pkWm3Node->SetController(pkKFCtrl);
}
//----------------------------------------------------------------------------
bool WSceneBuilder::GetAnimationTiming (INode* pkNode,
    AnimationTiming& rkTTiming, AnimationTiming& rkRTiming,
    AnimationTiming& rkSTiming)
{
    // Check whether a node is animated by determining if its transform
    // actually changes "significantly" over the time interval of interest.

    int iTicksPerFrame = GetTicksPerFrame();
    Matrix3 kNodeTM;
    AffineParts kAffParts;
    Point3 kPrevTrans, kRotAxis, kPrevRotAxis, kPrevScaleFactor;
    float fRotAngle, fPrevRotAngle;
    rkTTiming.Active = FALSE;
    rkTTiming.Active = FALSE;
    rkSTiming.Active = FALSE;

    // get initial components of node's transform
    kNodeTM = pkNode->GetNodeTM(m_iTimeStart) *
        Inverse(pkNode->GetParentTM(m_iTimeStart));
    decomp_affine(kNodeTM,&kAffParts);
    AngAxisFromQ(kAffParts.q,&fPrevRotAngle,kPrevRotAxis);
    kPrevTrans = kAffParts.t;
    kPrevScaleFactor = kAffParts.k;

    // Loop through all frames, checking for a change in any component of the
    // node's transform.
    TimeValue iTimeNow = m_iTimeStart + iTicksPerFrame;
    for (/**/; iTimeNow <= m_iTimeEnd; iTimeNow += iTicksPerFrame)
    {
        kNodeTM = pkNode->GetNodeTM(iTimeNow) *
            Inverse(pkNode->GetParentTM(iTimeNow));
        decomp_affine(kNodeTM,&kAffParts);
        AngAxisFromQ(kAffParts.q,&fRotAngle,kRotAxis);

        if ( !rkTTiming.Active )
        {
            // Test current translation vector against previous vector for
            // change.
            if ( !AreEqual(kAffParts.t,kPrevTrans) )
            {
                // Translation has changed.  Record time of the frame before
                // the change.
                rkTTiming.Active = TRUE;
                rkTTiming.Start = iTimeNow - iTicksPerFrame;
                rkTTiming.End = iTimeNow;
            }
        }
        else
        {
            // Advance the end time of the animated translation, if the
            // translation vector is still changing.
            if ( !AreEqual(kAffParts.t,kPrevTrans) )
                rkTTiming.End = iTimeNow;
        }

        if ( !rkRTiming.Active )
        {
            // Test current rotation angle and axis against previous angle
            // and axis for change.
            if ( fabsf(fRotAngle - fPrevRotAngle) > MIN_DIFFERENCE
            ||   !AreEqual(kRotAxis,kPrevRotAxis) )
            {
                rkRTiming.Active = TRUE;
                rkRTiming.Start = iTimeNow - iTicksPerFrame;
                rkRTiming.End = iTimeNow;
            }
        }
        else
        {
            if ( fabsf(fRotAngle - fPrevRotAngle) > MIN_DIFFERENCE
            ||   !AreEqual(kRotAxis,kPrevRotAxis) )
            {
                rkRTiming.End = iTimeNow;
            }
        }

        if ( !rkSTiming.Active )
        {
            // Test current vector of scale values against previous vector,
            // for changes.
            if ( !AreEqual(kAffParts.k,kPrevScaleFactor) )
            {
                rkSTiming.Active = TRUE;
                rkSTiming.Start = iTimeNow - iTicksPerFrame;
                rkSTiming.End = iTimeNow;
            }
        }
        else
        {
            if ( !AreEqual(kAffParts.k,kPrevScaleFactor) )
                rkSTiming.End = iTimeNow;
        }

        fPrevRotAngle = fRotAngle;
        kPrevRotAxis = kRotAxis;
        kPrevTrans = kAffParts.t;
        kPrevScaleFactor = kAffParts.k;     
    }
    
    return rkTTiming.Active || rkRTiming.Active || rkSTiming.Active;
}
//----------------------------------------------------------------------------
void WSceneBuilder::GetTrnKeyInfo (int& riNumKeys, Class_ID& rkClassID, 
    IKeyControl* pkKeyCon, AnimationTiming& rkTTiming,
    vector<KeyInfo*>& rkKeyInfo)
{       
    // Extract translation keys from a Max node's position key controller.
    //
    // riNumKeys:
    //     As an input, number of key frames to process.  As an output, the
    //     corresponding number of Magic keyframes required.
    // class_ID:
    //     Max class ID for the type of position key controller.
    // pkKeyCon:
    //     Max node's position key frame controller.
    // rkTTiming:
    //     Details of the node's translation animation timing.
    // rkKeyInfo:
    //     Vector of KeyInfo objects in which to store the key type and
    //     timing info.

    int i, iKeys = riNumKeys;
    riNumKeys = 0;

    if ( rkClassID == Class_ID(TCBINTERP_POSITION_CLASS_ID,0) )
    {
        // process Tension/Continuity/Bias controller
        ITCBPoint3Key kKey;
        for (i = 0; i < iKeys; i++)
        {
            pkKeyCon->GetKey(i,&kKey);
            if ( kKey.time < rkTTiming.Start ) 
                continue;
            else if ( kKey.time > rkTTiming.End ) 
                break;
    
            rkKeyInfo.push_back(new KeyInfo(kKey.time,KT_TRANSLATION));
            riNumKeys++;
        }
    }
    else if ( rkClassID == Class_ID(HYBRIDINTERP_POSITION_CLASS_ID, 0) )
    {
        // process Bezier controller
        IBezPoint3Key kKey;
        for (i = 0; i < iKeys; i++)
        {
            pkKeyCon->GetKey(i,&kKey);          
            if ( kKey.time < rkTTiming.Start ) 
                continue;
            else if ( kKey.time > rkTTiming.End ) 
                break;

            rkKeyInfo.push_back(new KeyInfo(kKey.time,KT_TRANSLATION));
            riNumKeys++;
        }
    }
    else if ( rkClassID == Class_ID(LININTERP_POSITION_CLASS_ID, 0) )
    {
        // process Linear controller
        ILinPoint3Key kKey;
        for (int i = 0; i < iKeys; i++)
        {
            pkKeyCon->GetKey(i,&kKey);          
            if ( kKey.time < rkTTiming.Start ) 
                continue;
            else if ( kKey.time > rkTTiming.End ) 
                break;

            rkKeyInfo.push_back(new KeyInfo(kKey.time,KT_TRANSLATION));
            riNumKeys++;
        }   
    }
}
//----------------------------------------------------------------------------
void WSceneBuilder::GetRotKeyInfo (int& riNumKeys, Class_ID& rkClassID, 
    IKeyControl* pkKeyCon, AnimationTiming& rkRTiming,
    vector<KeyInfo*>& rkKeyInfo)
{
    // Extract rotation keys from a MAX node's rotation key controller.
    //
    // riNumKeys:
    //     As an input, number of key frames to process.  As an output, the
    //     corresponding number of Magic keyframes required.
    // class_ID:
    //     Max class ID for the type of rotation key controller.
    // pkKeyCon:
    //     Max node's rotation key frame controller.
    // rkTTiming:
    //     Details of the node's rotation animation timing.
    // rkKeyInfo:
    //     Vector of KeyInfo objects in which to store the key type and
    //     timing info.

    int i, iKeys = riNumKeys;
    riNumKeys = 0;

    if ( rkClassID == Class_ID(TCBINTERP_ROTATION_CLASS_ID,0) )
    {
        // process Tension/Continuity/Bias controller
        ITCBRotKey kKey;
        for (i = 0; i < iKeys; i++)
        {
            pkKeyCon->GetKey(i,&kKey);
            if(  kKey.time < rkRTiming.Start ) 
                continue;
            else if ( kKey.time > rkRTiming.End ) 
                break;
            
            rkKeyInfo.push_back(new KeyInfo(kKey.time,KT_ROTATION));
            riNumKeys++;
        }       
    }
    else if ( rkClassID == Class_ID(HYBRIDINTERP_ROTATION_CLASS_ID, 0) )
    {
        // process Bezier controller
        IBezQuatKey kKey;
        for (i = 0; i < iKeys; i++)
        {
            pkKeyCon->GetKey(i,&kKey);          
            if ( kKey.time < rkRTiming.Start )
                continue;
            if ( kKey.time > rkRTiming.End ) 
                break;

            rkKeyInfo.push_back(new KeyInfo(kKey.time,KT_ROTATION));
            riNumKeys++;
        }
    }
    else if ( rkClassID == Class_ID(LININTERP_ROTATION_CLASS_ID, 0) )
    {
        // process Linear controller
        ILinRotKey kKey;
        for (i = 0; i < iKeys; i++)
        {
            pkKeyCon->GetKey(i,&kKey);           
            if ( kKey.time < rkRTiming.Start ) 
                continue;
            else if ( kKey.time > rkRTiming.End ) 
                break;

            rkKeyInfo.push_back(new KeyInfo(kKey.time,KT_ROTATION));
            riNumKeys++;
        }   
    }
}
//----------------------------------------------------------------------------
void WSceneBuilder::GetScaleKeyInfo (int& riNumKeys, Class_ID& rkClassID,
    IKeyControl* pkKeyCon, AnimationTiming& rkSTiming,
    vector<KeyInfo*>& rkKeyInfo)
{
    // Extract scale keys from a Max node's scale key controller.
    //
    // riNumKeys:
    //     As an input, number of key frames to process.  As an output, the
    //     corresponding number of Magic keyframes required.
    // class_ID:
    //     Max class ID for the type of scale key controller.
    // pkKeyCon:
    //     Max node's scale key frame controller.
    // rkSTiming:
    //     Details of the node's scale animation timing.
    // rkKeyInfo:
    //     Cector of KeyInfo objects in which to store the key type and
    //     timing info.

    int i, iKeys = riNumKeys;
    riNumKeys = 0;

    if ( rkClassID == Class_ID(TCBINTERP_SCALE_CLASS_ID,0) )
    {
        // process Tension/Continuity/Bias controller
        ITCBScaleKey kKey;
        for (i = 0; i < iKeys; i++)
        {
            pkKeyCon->GetKey(i,&kKey);
            if ( kKey.time < rkSTiming.Start )
                continue;
            else if ( kKey.time > rkSTiming.End ) 
                break;
                
            rkKeyInfo.push_back(new KeyInfo(kKey.time,KT_SCALE));
            riNumKeys++;
        }
    }
    else if ( rkClassID == Class_ID(HYBRIDINTERP_SCALE_CLASS_ID, 0) )
    {
        // process Bezier controller
        IBezScaleKey kKey;
        for (i = 0; i < iKeys; i++)
        {
            pkKeyCon->GetKey(i,&kKey);          
            if ( kKey.time < rkSTiming.Start ) 
                continue;
            else if ( kKey.time > rkSTiming.End ) 
                break;

            rkKeyInfo.push_back(new KeyInfo(kKey.time,KT_SCALE));
            riNumKeys++;
        }
    }
    else if ( rkClassID == Class_ID(LININTERP_SCALE_CLASS_ID,0) )
    {
        // process Linear controller
        ILinScaleKey kKey;
        for (i = 0; i < iKeys; i++) 
        {
            pkKeyCon->GetKey(i,&kKey);          
            if ( kKey.time < rkSTiming.Start ) 
                continue;
            else if ( kKey.time > rkSTiming.End ) 
                break;

            rkKeyInfo.push_back(new KeyInfo(kKey.time,KT_SCALE));
            riNumKeys++;
        }   
    }
}
//----------------------------------------------------------------------------
