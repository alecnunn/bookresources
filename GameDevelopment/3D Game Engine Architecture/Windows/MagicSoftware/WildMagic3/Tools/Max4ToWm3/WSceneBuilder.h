// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WSCENEBUILDER_H
#define WSCENEBUILDER_H

#include "WildMagic3.h"
#include "WExportSettings.h"
#include "WMaterials.h"
#include "WMaterialTree.h"
#include "WUnimaterialMesh.h"
#include "max.h"
#include "stdmat.h"
#include <vector>
using namespace std;

class WSceneBuilder
{
public:
    WSceneBuilder (const TCHAR* acFilename, BOOL bExportSelected, 
        WExportSettings* pkSettings, ExpInterface* pkExport, Interface* pkMax,
        INode* pkRootNode);

    ~WSceneBuilder();

private:
    class AnimationTiming
    {
    public:
        AnimationTiming () : Active(FALSE), Start(0), End(0) { /**/ }

        BOOL Active;
        TimeValue Start;
        TimeValue End;
    };

    class ModifierInfo
    {
    public:
        INode* Node;
        vector<Modifier*> Modifiers;
    };

    enum KeyType
    {
        KT_TRANSLATION,
        KT_ROTATION,
        KT_SCALE
    };

    class KeyInfo
    {
    public:
        KeyInfo (TimeValue iTime, KeyType eType)
        {
            Time = iTime;
            Type = eType;
        }

        TimeValue Time;
        KeyType Type; 
    };

    // scene traversal
    bool Traverse (INode* pkMaxNode, Wm3::Spatial* pkWm3Node);

    // node processing
    Wm3::Spatial* BuildGeometry (INode* pkMaxNode, Wm3::Spatial* pkWm3Node);
    Wm3::Spatial* BuildSpatial (INode* pkMaxNode, Wm3::Spatial* pkWm3Node);
    Wm3::Spatial* BuildMesh (INode* pkMaxNode, Wm3::Spatial* pkWm3Node);
    Wm3::Transformation GetLocalTransform (INode* pkNode, TimeValue iTime);

    // mesh processing
    TriObject* GetTriObject (INode* pkNode, bool* pbNeedDel);
    void SplitGeometry (Mesh* pkMaxMesh, int iMtlID,
        vector<WUnimaterialMesh*>& rkUMeshes);
    Wm3::Vector3f GetVertexNormal (Mesh* pkMaxMesh, int iFace, int iVertex);  
    void PackColors (WUnimaterialMesh* pkMesh, Mesh* pkMaxMesh,
        vector<int>& rkPartition);
    void PackTextures (WUnimaterialMesh* pkUniMesh, Mesh* pkMaxMesh,
        vector<int>& rkPartition);
    void PackVertices (WUnimaterialMesh* pkUniMesh, Mesh* pkMaxMesh,
        vector<int>& rkPartition, Wm3::Vector3f* akNormal);

    // material processing
    void ConvertMaterials (INode* pkTopNode);
    void CollectMaterials (INode* pkNode);
    void ConvertMaterial (Mtl& rkMtl, int iMtlID, int iSubNo,
        WMaterialTree& rkTree);
    void ConvertTexture (Texmap& rkTex, Class_ID kClassID, int iSubNo,
        WTextureTree& rkTree);

    WMaterials m_kMtlList;
    std::vector<WMaterialTree> m_kMtlTree;
    static const TCHAR* ms_aacMapName[NTEXMAPS];
    static const char* ms_aacEnvName[5];
    static const TCHAR* ms_acMapEnvironment;
    static const TCHAR* ms_acMapGeneric;

    // light processing
    void BuildAmbientLight ();
    void BuildLight (INode* pkMaxNode, Wm3::Spatial* pkWm3Node);
    Wm3::Vector3f GetLightLocation (INode* pkNode);
    Wm3::Light* BuildPointLight (INode* pkNode);
    Wm3::Light* BuildSpotLight (INode* pkNode, LightState& rkLightState);
    Wm3::Light* BuildDirectionalLight (INode* pkNode);

    // animation processing
    static bool AreEqual (const Point3& rkPoint1, const Point3& rkPoint2);
    static bool AreEqual (const Quat& rkQuat1, const Quat& rkQuat2);
    static bool CompareKeyTimes (KeyInfo* pkFirst, KeyInfo* pkSecond);
    void BuildKeyFrameController (INode* pkMaxNode, Wm3::Spatial* pkWm3Node);
    void BuildFrameController (INode* pkNode, Wm3::Spatial* pkWm3Node);
    bool GetAnimationTiming (INode* pkNode, AnimationTiming& rkTTiming, 
        AnimationTiming& rkRTiming, AnimationTiming& rkSTiming);
    void GetTrnKeyInfo (int &riNumKeys, Class_ID& rkClassID,
        IKeyControl* pkKeyCon, AnimationTiming& rkTTiming,
        vector<KeyInfo*>& rkKeyInfo);
    void GetRotKeyInfo (int& riNumKeys, Class_ID& rkClassID,
        IKeyControl* pkKeyCon, AnimationTiming& rkRTiming,
        vector<KeyInfo*>& rkKeyInfo);
    void GetScaleKeyInfo (int& riNumKeys, Class_ID& rkClassID,
        IKeyControl* pkKeyCon, AnimationTiming& rkSTiming,
        vector<KeyInfo*>& rkKeyInfo);

    // modifier processing
    void ApplyModifiers ();
    void FindModifiers (INode* pkNode, vector<Modifier*>& rkModifiers);
    void ProcessSkin (INode* pkNode, Modifier* pkSkinMod);
    void ProcessMorpher (INode* pkNode, Modifier* pkMorpherMod);

    TCHAR* m_acFilename;
    BOOL m_bExportSelected;
    WExportSettings* m_pkSettings;
    ExpInterface* m_pkExport;
    Interface* m_pkMax;
    INode* m_pkRootNode;
    int m_iTicksPerFrame;
    TimeValue m_iTimeStart;
    TimeValue m_iTimeEnd;
    TimeValue m_iTimeOffset;
    Wm3::NodePtr m_spkScene;
    vector<ModifierInfo*> m_kModList;

    static const float ATTENUATION;
    static const float MIN_DIFFERENCE;
};

#endif
