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
#include "WExportSettings.h"
#include "bmmlib.h"

const TCHAR* WSceneBuilder::ms_aacMapName[NTEXMAPS] =
{
    _T("map_ambient"),             // ID_AM
    _T("map_diffuse"),             // ID_DI
    _T("map_specular"),            // ID_SP
    _T("map_shininess"),           // ID_SH
    _T("map_shininess_strength"),  // ID_SS
    _T("map_selfillumination"),    // ID_SI
    _T("map_opacity"),             // ID_OP
    _T("map_filter_color"),        // ID_FI
    _T("map_bump"),                // ID_BU
    _T("map_reflection"),          // ID_RL
    _T("map_refraction"),          // ID_RR
    _T("map_displacment")          // ID_DP
};

const char* WSceneBuilder::ms_aacEnvName[5] =
{
    "env_explicit",
    "env_sphere",
    "env_cylinder",
    "env_shrink",
    "env_screen"
};

const TCHAR* WSceneBuilder::ms_acMapEnvironment = _T("map_environment");
const TCHAR* WSceneBuilder::ms_acMapGeneric = _T("map_generic");

//----------------------------------------------------------------------------
void WSceneBuilder::ConvertMaterials (INode* pkTopNode)
{
    CollectMaterials(pkTopNode);

    if ( !m_pkSettings->IncludeMaterials ) 
        return;

    int iQuantity = m_kMtlList.GetQuantity();
    m_kMtlTree.resize(iQuantity);
    for (int i = 0; i < m_kMtlList.GetQuantity(); i++)
    {
        Mtl* pkMtl = m_kMtlList.Get(i);
        if ( pkMtl )
            ConvertMaterial(*pkMtl,i,-1,m_kMtlTree[i]);
    }
}
//----------------------------------------------------------------------------
void WSceneBuilder::CollectMaterials (INode* pkNode)
{
    Mtl* pkMtl = pkNode->GetMtl();
    if ( pkMtl )
        m_kMtlList.Add(pkMtl);

    for (int i = 0; i < pkNode->NumberOfChildren(); i++)
        CollectMaterials(pkNode->GetChildNode(i));
}
//----------------------------------------------------------------------------
void WSceneBuilder::ConvertMaterial (Mtl& rkMtl, int iMtlID, int iSubNo,
    WMaterialTree& rkTree)
{
    // create a Wild Magic material from the Max material
    Wm3::MaterialStatePtr spkMState = new Wm3::MaterialState;
    Color kColor = rkMtl.GetAmbient();
    float fAlpha = 1.0f - rkMtl.GetXParency();
    spkMState->Ambient = Wm3::ColorRGBA(kColor.r,kColor.g,kColor.b,1.0f);
    kColor = rkMtl.GetDiffuse();
    spkMState->Diffuse = Wm3::ColorRGBA(kColor.r,kColor.g,kColor.b,fAlpha);
    kColor = rkMtl.GetSpecular();
    spkMState->Specular = Wm3::ColorRGBA(kColor.r,kColor.g,kColor.b,1.0f);
    spkMState->Shininess = 128.0f*rkMtl.GetShininess();

    const char* acName = (const char*)rkMtl.GetName();
    spkMState->SetName(acName);
    rkTree.SetMaterial(spkMState);

    // get the subtextures
    int iTQuantity = rkMtl.NumSubTexmaps();
    if ( iTQuantity > 0 )
    {
        rkTree.SetTChildQuantity(iTQuantity);
        for (int i = 0; i < iTQuantity; i++)
        {
            Texmap* pkSubTex = rkMtl.GetSubTexmap(i);
            if ( pkSubTex )
            {
                // Check if the material is "standard" and if the subtexture
                // is enabled.  If not enabled, the subtexture is not
                // converted.
                if ( rkMtl.ClassID() == Class_ID(DMTL_CLASS_ID,0)
                &&   !((StdMat&)rkMtl).MapEnabled(i) )
                {
                    continue;
                }

                ConvertTexture(*pkSubTex,rkMtl.ClassID(),i,
                    rkTree.GetTChild(i));
            }
        }
    }

    // get the submaterials
    int iMQuantity = rkMtl.NumSubMtls();
    if ( iMQuantity > 0 )
    {
        rkTree.SetMChildQuantity(iMQuantity);
        for (int i = 0; i < iMQuantity; i++)
        {
            Mtl* pkSubMtl = rkMtl.GetSubMtl(i);
            if ( pkSubMtl )
                ConvertMaterial(*pkSubMtl,0,i,rkTree.GetMChild(i));
        }
    }
}
//----------------------------------------------------------------------------
void WSceneBuilder::ConvertTexture (Texmap& rkTex, Class_ID kClassID,
    int iSubNo, WTextureTree& rkTree)
{
    if ( rkTex.ClassID() == Class_ID(BMTEX_CLASS_ID,0) )
    {
        // texture is a bitmapped image
        Wm3::Texture* pkTexture = new Wm3::Texture;
        rkTree.SetTexture(pkTexture);

        const TCHAR* acTexName = rkTex.GetName();
        const TCHAR* acMapName;
        if ( kClassID == Class_ID(0,0) )
            acMapName = ms_acMapEnvironment;
        else if ( kClassID == Class_ID(DMTL_CLASS_ID,0) )
            acMapName = ms_aacMapName[iSubNo];
        else
            acMapName = ms_acMapGeneric;

        int iCombineLength = _tcslen(acTexName)+_tcslen(acMapName)+3;
        TCHAR* acCombineName = new TCHAR[iCombineLength];
        _stprintf(acCombineName,"%s: %s",acTexName,acMapName);
        const char* acName = (const char*)acCombineName;
        pkTexture->SetName(acName);

        BitmapTex& rkBmpTex = (BitmapTex&)rkTex;
        Bitmap* pkBmp = rkBmpTex.GetBitmap(m_iTimeStart);
        if ( !pkBmp )
        {
            // bitmap not found
            assert( false );
            return;
        }

        int iWidth = pkBmp->Width();
        int iHeight = pkBmp->Height();   

        TSTR kFileName = rkBmpTex.GetMapName();
        char acDrive[_MAX_DRIVE];
        char acDir[_MAX_DIR];
        char acFName[_MAX_FNAME];
        char acExt[_MAX_EXT];
        _splitpath(kFileName.data(),acDrive,acDir,acFName,acExt);
        strcat(acFName,".wmif");
        
        Wm3::Image* pkWm3Image;
        int eFormat;
        int iBytesPerPixel;
        BOOL bHasAlpha = pkBmp->HasAlpha();
        if ( bHasAlpha )
        {
            eFormat =  Wm3::Image::IT_RGBA8888;
            iBytesPerPixel = 4;
        }
        else
        {
            eFormat = Wm3::Image::IT_RGB888;
            iBytesPerPixel = 3;
        }

        WIN32_FIND_DATA kFindData;
        HANDLE hFile = FindFirstFile(acFName,&kFindData);
        FindClose(hFile);

        if ( hFile == INVALID_HANDLE_VALUE || m_pkSettings->GenerateMaps )
        {
            int iQuantity = iWidth*iHeight*iBytesPerPixel;
            unsigned char* aucDst = new unsigned char[iQuantity];
            unsigned char* pucDst = aucDst;
            BMM_Color_64* pkScanLine = new BMM_Color_64[iWidth];
            BMM_Color_64* pkPixel;
            BMM_Color_32 kScaledColor;
            for (int iY = iHeight - 1; iY >= 0; iY--)
            {
                pkBmp->GetLinearPixels(0,iY,iWidth,pkScanLine);
                pkPixel = pkScanLine;
                for (int iX = 0; iX < iWidth; iX++)
                {
                    AColor kColor(*pkPixel);
                    kScaledColor = (BMM_Color_32)kColor;
                    *pucDst++ = (unsigned char)kScaledColor.r;
                    *pucDst++ = (unsigned char)kScaledColor.g;
                    *pucDst++ = (unsigned char)kScaledColor.b;
                    if ( bHasAlpha )
                        *pucDst++ = (unsigned char)kScaledColor.a;
                    pkPixel++;
                }
            }
            delete[] pkScanLine;
            pkWm3Image = new Wm3::Image(eFormat,iWidth,iHeight,aucDst,
                acFName,false);
            pkWm3Image->Save(acFName);
        }
        else
        {
            pkWm3Image = new Wm3::Image(eFormat,pkBmp->Width(),
                pkBmp->Height(),NULL,acFName,false);
        }

        // TO DO.  Need to add logic for multiple textures per effect,
        // including how to combine.
        pkTexture->SetImage(pkWm3Image);
        pkTexture->Apply = Wm3::Texture::AM_MODULATE;
    
        StdUVGen* pkUVGen = rkBmpTex.GetUVGen();
        if ( pkUVGen )
        {
            pkTexture->SetName(ms_aacEnvName[pkUVGen->GetCoordMapping(0)]);

            // TO DO.  Need to handle animated texture coordinates.  What to
            // do when the UV parameters are not the standard ones?  The
            // functions below are called through pkUVGen.  The "noise"
            // parameters appear to be for procedural generation of textures.
            //
            // U offset:      GetUOffs(time)
            // V offset:      GetVOffs(time)
            // U tiling:      GetUScl(time)
            // V tiling:      GetVScl(time)
            // angle:         GetAng(time)
            // blur:          GetBlur(time)
            // blur offset:   GetBlurOffs(time)
            // noise amount:  GetNoiseAmt(time)
            // noise size:    GetNoiseSize(time)
            // noise level:   GetNoiseLev(time)
            // noise phase:   GetNoisePhs(time)

            // Apparently Max always uses wrapping?
            pkTexture->CoordU = Wm3::Texture::WM_REPEAT;
            pkTexture->CoordV = Wm3::Texture::WM_REPEAT;
        }

        switch ( rkBmpTex.GetFilterType() )
        {
        case FILTER_PYR:
            // bilinear filter, trilinear mipmap
            pkTexture->Filter = Wm3::Texture::FM_LINEAR;
            pkTexture->Mipmap = Wm3::Texture::MM_LINEAR_LINEAR;
            break;
        case FILTER_SAT:
            // TO DO. What is it and how to convert?
            pkTexture->Mipmap = Wm3::Texture::MM_NEAREST;
            break;
        default:
            pkTexture->Mipmap = Wm3::Texture::MM_NEAREST;
            break;
        }
    }

    int iTQuantity = rkTex.NumSubTexmaps();
    if ( iTQuantity > 0 )
    {
        rkTree.SetChildQuantity(iTQuantity);
        for (int i = 0; i < iTQuantity; i++)
        {
            Texmap* pkSubTex = rkTex.GetSubTexmap(i);
            if ( pkSubTex )
            {
                ConvertTexture(*pkSubTex,rkTex.ClassID(),i,
                    rkTree.GetChild(i));
            }
        }
    }
}
//----------------------------------------------------------------------------
