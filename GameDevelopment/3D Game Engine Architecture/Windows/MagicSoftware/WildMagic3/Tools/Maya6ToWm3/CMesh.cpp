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
#include "MultiattributeMesh.h"
#include "BumpMapData.h"

#include "Wm3AlphaState.h"
#include "Wm3MaterialState.h"
#include "Wm3TriMesh.h"
#include "Wm3Image.h"
#include "Wm3Texture.h"
#include "Wm3TextureEffect.h"

#include <maya/MFloatPointArray.h>
#include <maya/MFnMesh.h>
#include <maya/MGlobal.h>
#include <maya/MIntArray.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MItDependencyNodes.h>
using namespace Wm3;

//----------------------------------------------------------------------------
static bool IsValidShader (const MObject& rkMatObj)
{
    // This function returns true if the surface shader rkMatObj is
    // a material that can be handled in ConvertMaterial.
    return(rkMatObj.hasFn(MFn::kBlinn) || rkMatObj.hasFn(MFn::kLambert));
}
//----------------------------------------------------------------------------
MaterialState* Exporter::ConvertMaterial (const MObject& rkMatObj, 
    bool &rbIsTransparent, bool &rbHasSpecular)
{
    // Returns a material state for this shader.  Returns NULL if it
    // cannot convert the shader into a material state correctly.

    bool bLambert;
    rbIsTransparent = false;
    MFnDependencyNode kMat(rkMatObj,&m_kStatus);
    assert(m_kStatus);

    if (rkMatObj.hasFn(MFn::kBlinn))
    {
        bLambert = false;
    }
    else if (rkMatObj.hasFn(MFn::kLambert))
    {
        bLambert = true;
    }
    else if (rkMatObj.hasFn(MFn::kLayeredShader))
    {
        // More intelligent error, in this case
        DisplayWarning("Layered shaders not supported."\
            "  Use layered textures for multi-texturing.");
        return NULL;
    }
    else
    {
        DisplayWarning("Unknown surface shader.");
        return NULL;
    }

    rbHasSpecular = !bLambert;

    // convert material
    MaterialState* pkMatState = new MaterialState();
    pkMatState->SetName(kMat.name().asChar());
    
    ColorRGBA kCol = ColorRGBA::BLACK;
    float fR = 0.0f, fG = 0.0f, fB = 0.0f;
    float fValue = 0.0f;
    
    // ambient
    MPlug kAmbientPlug = kMat.findPlug("ambientColor", &m_kStatus);
    if (m_kStatus)
    {   
        MPlug kAmbientPlugR = kAmbientPlug.child(0);
        MPlug kAmbientPlugG = kAmbientPlug.child(1);
        MPlug kAmbientPlugB = kAmbientPlug.child(2);
        kAmbientPlugR.getValue(fR);
        kAmbientPlugG.getValue(fG);
        kAmbientPlugB.getValue(fB);
        pkMatState->Ambient = ColorRGBA(fR,fG,fB,1.0f);
    }
    else
    {
        DisplayWarning("Couldn't find ambient plug.");
    }
    
    // diffuse
    MPlug kDiffusePlug = kMat.findPlug("color", &m_kStatus);
    if (m_kStatus)
    {
        MPlug kDiffusePlugR = kDiffusePlug.child(0);
        MPlug kDiffusePlugG = kDiffusePlug.child(1);
        MPlug kDiffusePlugB = kDiffusePlug.child(2);
        kDiffusePlugR.getValue(fR);
        kDiffusePlugG.getValue(fG);
        kDiffusePlugB.getValue(fB);
        pkMatState->Diffuse = ColorRGBA(fR,fG,fB,1.0f);
    }
    else
    {
        DisplayWarning("Couldn't find diffuse plug.");
    }

    if (!bLambert)
    {
        // emissive
        MPlug kEmissivePlug = kMat.findPlug("incandescence", &m_kStatus);
        if (m_kStatus)
        {
            MPlug kEmissivePlugR = kEmissivePlug.child(0);
            MPlug kEmissivePlugG = kEmissivePlug.child(1);
            MPlug kEmissivePlugB = kEmissivePlug.child(2);
            kEmissivePlugR.getValue(fR);
            kEmissivePlugG.getValue(fG);
            kEmissivePlugB.getValue(fB);
            pkMatState->Emissive = ColorRGBA(fR,fG,fB,1.0f);
        }
        else
        {
            DisplayWarning("Couldn't find emissive plug.");
        }
        
        // specular
        MPlug kSpecularPlug = kMat.findPlug("specularColor", &m_kStatus);
        if (m_kStatus)
        {
            MPlug kSpecularPlugR = kSpecularPlug.child(0);
            MPlug kSpecularPlugG = kSpecularPlug.child(1);
            MPlug kSpecularPlugB = kSpecularPlug.child(2);
            kSpecularPlugR.getValue(fR);
            kSpecularPlugG.getValue(fG);
            kSpecularPlugB.getValue(fB);
            pkMatState->Specular = ColorRGBA(fR,fG,fB,1.0f);
        }
        else
        {
            DisplayWarning("Couldn't find specular plug.");
        }
        
        // shininess
        MPlug kReflectivityPlug = kMat.findPlug("eccentricity", &m_kStatus);
        if (m_kStatus)
        {
            kReflectivityPlug.getValue(fValue);
            
            // There's no clear way to get the right shininess value.
            // Eccentricity is a value that ranges from [0,1.0].
            // At the very least, it's clear that as eccentricity goes up
            // the highlight gets larger, and so it's an inverse relationship.
            // Inverse squared is my estimate.  Visually, it seems closer than
            // just the inverse for reasonable n values.
            pkMatState->Shininess = 1.0f/(fValue*fValue);
        }
        else
        {
            DisplayWarning("Couldn't find eccentricity plug.");
        }
    }
    
    // alpha-
    // Alpha needs to get checked last because it overwrites the
    // alpha values of the colors that were previously assigned.
    MPlug kTransparencyPlug = kMat.findPlug("transparency",&m_kStatus);
    if (m_kStatus)
    {
        // NOTE: color-based transparency NOT supported
        MPlug kTransparencyPlugR = kTransparencyPlug.child(0);
        kTransparencyPlugR.getValue(fValue);
        
        // Give ambient value
        pkMatState->Ambient.A() = 1.0f - fValue;
        pkMatState->Diffuse.A() = 1.0f - fValue;
        pkMatState->Specular.A() = 1.0f - fValue;
    
        if (fValue > 0.0f)
        {
            rbIsTransparent = true;
        }
    }
    else
    {
        DisplayWarning("Couldn't find transparency plug.");
    }

    return pkMatState;
}
//----------------------------------------------------------------------------
TriMesh* Exporter::GetTriMesh (const MFnMesh& rkMesh, Vector2fArray*& rpkUV0, 
    Vector2fArray*& rpkUV1, Vector2fArray*& rpkUV2, Vector2fArray*& rpkUV3)
{
    // Load just the geometry of a TriMesh from the rkMesh.  Also, return
    // the texture coordinates in the passed in arrays if they exist.  If
    // they don't, then return NULL for those arrays.  If there are errors
    // in the conversion, return a NULL pointer.

    // get vertices
    int iVQuantity = rkMesh.numVertices();
    if (iVQuantity <= 0)
    {
        DisplayError("Number of vertices <= 0.");
        return NULL;
    }

    // Copy into an array
    Vertex* akVertex = new Vertex[iVQuantity];
    MFloatPointArray kMVertex;
    m_kStatus = rkMesh.getPoints(kMVertex);
    assert( m_kStatus );
    MultiattributeMesh kMAMesh;

    // It is crucial (for skin controllers) that the vertices in the
    // WM mesh match up with the vertices in the Maya mesh.  Therefore,
    // they must be added in the same order.
    int iV, iP, iS;
    for (iV = 0; iV < iVQuantity; iV++)
    {
        akVertex[iV][0] = kMVertex[iV].x;
        akVertex[iV][1] = kMVertex[iV].y;
        akVertex[iV][2] = kMVertex[iV].z;
        kMAMesh.AddVertexLoc(iV, akVertex[iV]);
    }

    int iPQuantity = rkMesh.numPolygons();
    if (iPQuantity <= 0)
    {
        DisplayError("Mesh has no polygons! Ignoring.");
        delete[] akVertex;
        return NULL;
    }

    // NQuantity can differ from VQuantity when vertices have
    // different normals, depending on which face is being considered.
    // e.g. a cube with flat sides has 8 vertices, but 24 normals.
    // NQuantity will just be used to test for the existance
    // of normal data.
    int iNQuantity = rkMesh.numNormals();
    MFloatVectorArray kMNorms;
    // catch case when NQ < VQ
    if (iNQuantity < iVQuantity )
    {
        DisplayWarning("Fewer normals than vertices! Ignoring normal data.");
        iNQuantity = 0;
    }
    else if (iNQuantity > 0)
    {
        // Get normal array
        m_kStatus = rkMesh.getNormals(kMNorms,MSpace::kTransform);
        assert(m_kStatus);
    }

    // Texture coordinate quantity
    int iSQuantity = rkMesh.numUVSets();
    if (iSQuantity > MultiattributeMesh::MAX_TEX_COORDS)
    {
        MString kTemp;
        kTemp = "Too many texture coordinates (";
        kTemp += iSQuantity;
        kTemp += ").  Only using the first ";
        kTemp += MultiattributeMesh::MAX_TEX_COORDS;
        kTemp += ".";
        DisplayWarning(kTemp);
        iSQuantity = MultiattributeMesh::MAX_TEX_COORDS;
    }
    // Get names of UV maps
    MStringArray kNames;
    rkMesh.getUVSetNames(kNames);
    for (iS = 0; iS < iSQuantity; iS++)
    {
        if (rkMesh.numUVs(kNames[iS]) < iVQuantity)
        {
            MString kTemp;
            kTemp = "Texture set " + kNames[iS];
            kTemp += " has fewer coordinates (";
            kTemp += (int)rkMesh.numUVs(kNames[iS]);
            kTemp += ") than vertices (";
            kTemp += iVQuantity;
            kTemp += ").  Ignoring this and subsequent texture coordinates.";
            DisplayWarning(kTemp);
            iSQuantity = iS;
            break;
        }
    }

    // get colors
    MColorArray kMColors;
    m_kStatus = ((MFnMesh&)rkMesh).getFaceVertexColors(kMColors);
    assert( m_kStatus );
    int iCQuantity = (int)kMColors.length();
    if ( iCQuantity > 0 )
    {
        if (iCQuantity < iVQuantity)
        {
            DisplayWarning("Fewer colors than vertices! Ignoring colors.");
            iCQuantity = 0;
        }
    }
    int iCIndex = 0;

    // Walk through all the faces...
    for (iP = 0; iP < iPQuantity; iP++)
    {
        int iVertexCount = rkMesh.polygonVertexCount(iP);
        if (iVertexCount < 3)
        {
            MString kTemp = "Polygon ";
            kTemp += iP;
            kTemp += " has fewer than 3 vertices.  Ignoring it.";
            DisplayWarning(kTemp);
            continue;
        }
        MIntArray kVIndices;
        rkMesh.getPolygonVertices(iP,kVIndices);

        // Walk through the vertices and get the per face normal, uv,
        // and color information.  Maya allows for these to be face-dependent
        // quantities.  WildMagic does not allow for this and so these
        // vertices will be split into two vertices in the multiattribute
        // mesh.  We will store the information that we are reading in
        // akVertex[i] for each vertex, overwriting the information that
        // was there for a previous face using that vertex.
        // MultiattributeMesh will handle which vertices need to be
        // split.
        MIntArray kNormIndex;
        if (iNQuantity > 0)
            rkMesh.getFaceNormalIds(iP,kNormIndex);

        for(iV = 0; iV < iVertexCount; iV++)
        {
            int iMIndex = kVIndices[iV];

            // Get normal data per vertex for this face
            if (iNQuantity >= 0)
            {
                MVector kMNormal;
                if (m_abFlags[Exporter::FLAG_CALC_NORM])
                {
                    // Calculate normal from face normal
                    m_kStatus = rkMesh.getVertexNormal(iMIndex,kMNormal,
                        MSpace::kTransform);
                    assert(m_kStatus);
                }
                else
                {
                    kMNormal = kMNorms[kNormIndex[iV]];
                }

                akVertex[iMIndex][3] = (float)kMNormal.x;
                akVertex[iMIndex][4] = (float)kMNormal.y;
                akVertex[iMIndex][5] = (float)kMNormal.z;
            }

            // Get color data per vertex for this face
            if ( iCQuantity > 0 )
            {
                assert(m_kStatus);

                float fR = kMColors[iCIndex+iV].r;
                float fG = kMColors[iCIndex+iV].g;
                float fB = kMColors[iCIndex+iV].b;
                float fA = kMColors[iCIndex+iV].a;

                // NOTE: Maya tags an MColor as invalid by assigning
                // all color components a value of -1.0f.  
                if ((fR != -1.0f) && (fG != -1.0f) && (fB != -1.0f))
                {
                    akVertex[iMIndex][6] = fR;
                    akVertex[iMIndex][7] = fG;
                    akVertex[iMIndex][8] = fB;
                    akVertex[iMIndex][9] = fA;
                }
            }

            // Get texture data per vertex for this face
            if ( iSQuantity > 0 )
            {
                MIntArray kVIndices;
                for (iS = 0; iS < iSQuantity; iS++)
                {
                    m_kStatus = rkMesh.getPolygonUV(iP,iV,
                        akVertex[iMIndex][10+2*iS],
                        akVertex[iMIndex][11+2*iS],
                        &kNames[iS]);
                    assert(m_kStatus);
                }
            }

            // Now, set this information in the MultiattributeMesh
            // It will return the correct vertex index for this vertex
            // which we will store in the array.
            kVIndices[iV] = kMAMesh.UpdateVertexInfo(iMIndex,
                akVertex[iMIndex]);
        }

        // The index into the color array is:
        // F0V0,F0V1,F0V2, ... F0Vn, F1V0,F1V1, ..., FnVn
        // Update color index:
        iCIndex += iVertexCount;

        // Now that the vertex information in akVertex has been updated
        // for face iP, and the correct indices are in kVIndices,
        // now create a triangle fan (if necessary) of this polygon.
        int iV0 = kVIndices[0];
        int iV2 = kVIndices[--iVertexCount];
        while ( iVertexCount > 1 )
        {
            int iV1 = kVIndices[--iVertexCount];
            kMAMesh.AddTriangle(iV0,iV1,iV2);
            iV2 = iV1;
        }
    }

    delete[] akVertex;

    TriMesh* pkMesh = kMAMesh.GetMesh(rpkUV0, rpkUV1, rpkUV2, rpkUV3);
    m_kMeshDataMap.Insert(pkMesh,kMAMesh.GetData());
    return pkMesh;
}
//----------------------------------------------------------------------------
Image* Exporter::LoadImage(MFnDependencyNode& kFileTexture)
{
    // This function loads the texture specified in this FileTexture node.
    // If there are errors loading (or the file doesn't exist) it will
    // return NULL.  Otherwise, it will return a valid texture object.

    // sanity check
    assert(kFileTexture.object().apiType() == MFn::kFileTexture);

    // Note: I tried to look at the Maya API for a way to get back
    // raw image data, but as far as I can tell, it doesn't seem
    // possible.  That would have been the ideal answer here, so
    // that all this conversion nonsense doesn't have to happen.
    //
    // Instead it will try to load the filename (with the extension
    // replaced by .wmif) in the original directory that the
    // image was found in and, if that fails, the output directory.

    Image* pkImage;

    // Debug info
    MPlug kFTName = kFileTexture.findPlug("fileTextureName",&m_kStatus);
    if (!m_kStatus)
    {
        DisplayError("Couldn't find file texture name.  Ignoring image.");
        return NULL;
    }

    MString kImgName;
    kFTName.getValue(kImgName);

    // Build the correct file name.
    MString kWMImgFile;

    // Now strip the suffix file extension to add .wmif
    int iLoc = kImgName.rindex('.');
    if (iLoc == -1)
    {
        // not found, so just tack on .wmif to kImgName
        kWMImgFile = kImgName + ".wmif";
    }
    else
    {
        kWMImgFile = kImgName.substring(0,iLoc) + "wmif";
    }

    pkImage = Image::Load(kWMImgFile.asChar(),false);

    if (!pkImage)
    {
        DisplayWarning("Couldn't load " + kWMImgFile +
            ".  Searching output directory.");
    }

    // Second, we'll try the local directory by stripping all
    // of the path info off of the string.  This string will also
    // become the image name in the WM stream.
    // NOTE: I'm not sure if this filename delimiter is OS specific.
    iLoc = kWMImgFile.rindex('/');
    if (iLoc != -1)
    {
        kWMImgFile = kWMImgFile.substring(iLoc+1,kWMImgFile.length());
    }

    if (!pkImage)
    {
        pkImage = Image::Load(kWMImgFile.asChar(),false);
    }

    // If the image loaded fine on either of the above attempts
    if (pkImage)
    {
        DisplayInfo(kWMImgFile + " loaded correctly.");
    }
    else
    {
        // image didn't load correctly from output directory either
        DisplayError("Couldn't load image " + kWMImgFile +
            " from output directory either.  Texture will be ignored.");
    }

    return pkImage;
}
//----------------------------------------------------------------------------
Image* Exporter::LoadHeightMap(MFnDependencyNode& rkFileTexture, float fScale)
{
    // Loads a height map and converts it into a normal map

    Image* pkImage = LoadImage(rkFileTexture);
    if (!pkImage)
        return NULL;

    // Simple 4 sample nearest neighbor kernel filter

    // It only uses the red component to determine the height (which is fine
    // for all monochrome height maps).
 
    // It also assumes that the image will wrap, and so when it goes off one
    // edge it will reuse the data on the other side.

    // It also assumes that positive Z is the direction of the height map.

    int iWidth = pkImage->GetWidth();
    int iHeight = pkImage->GetHeight();
    int iFac = pkImage->GetBytesPerPixel();

    unsigned char* aucNormal = new unsigned char[iWidth*iHeight*4];

    for (int iX = 0; iX < iWidth; iX++)
    {
        for (int iY = 0; iY < iHeight; iY++)
        {
            int iNegX = (int)(pkImage->GetData()[iFac*((iX-1+iWidth)%iWidth + 
                iY*iWidth)]);
            int iPosX = (int)(pkImage->GetData()[iFac*((iX+1)%iWidth + 
                iY*iWidth)]);;

            int iNegY = (int)(pkImage->GetData()[iFac*(iX+((iY-1+iHeight)%
                iHeight)*iWidth)]);
            int iPosY = (int)(pkImage->GetData()[iFac*(iX+((iY+1)%iHeight)*
                iWidth)]);

            Vector3f kCross(fScale*(iNegX-iPosX)/2.0f, 
                fScale*(iNegY-iPosY)/2.0f, 1.0f);
            kCross.Normalize();

            kCross = 0.5f * kCross + Vector3f(0.5f,0.5f,0.5f);

            unsigned char* aucColor = &aucNormal[4*(iX+iY*iWidth)];
            aucColor[0] = (unsigned char)((int)(kCross.X()*255));
            aucColor[1] = (unsigned char)((int)(kCross.Y()*255));
            aucColor[2] = (unsigned char)((int)(kCross.Z()*255));
            aucColor[3] = (unsigned char)255;
        }
    }

    delete pkImage;
    return new Image(Image::IT_RGBA8888, iWidth, iHeight, aucNormal);
}
//----------------------------------------------------------------------------
static void BakeColor(Image*& rpkImage, float fScale, float fTrans)
{
    // Multiples fFactor by every color in rpkImage.  This image must be
    // in RGBA8888 format.  It returns the new image.
    assert(rpkImage->GetFormat() == Image::IT_RGBA8888);

    int iHeight = rpkImage->GetHeight();
    int iWidth = rpkImage->GetWidth();
    
    unsigned char* aucNew = new unsigned char[4*iHeight*iWidth];
    const unsigned char* aucOld = rpkImage->GetData();

    for(int i = 0; i < iWidth*iHeight; i++)
    {
        aucNew[i*4] = (unsigned char)((float)aucOld[i*4]*fScale+fTrans);
        aucNew[i*4+1] = (unsigned char)((float)aucOld[i*4+1]*fScale+fTrans);
        aucNew[i*4+2] = (unsigned char)((float)aucOld[i*4+2]*fScale+fTrans);
        aucNew[i*4+3] = aucOld[i*4+3];
    }

    Image* pkNew = new Image(Image::IT_RGBA8888,iWidth,iHeight,aucNew,
        rpkImage->GetName(),false);
    delete rpkImage;
    rpkImage = pkNew;
}
//----------------------------------------------------------------------------
static void MultiplyAlpha(Image*& rpkImage, float fAlpha)
{
    // Multiplies every alpha in rpkImage by fAlpha.  If rkImage is
    // not an RGBA8888, this function converts it to be one.

    int iHeight = rpkImage->GetHeight();
    int iWidth = rpkImage->GetWidth();
    
    unsigned char* aucNew = new unsigned char[4*iHeight*iWidth];
    const unsigned char* aucOld = rpkImage->GetData();

    switch(rpkImage->GetFormat())
    {
    case Image::IT_RGB888:
        for(int i = 0; i < iHeight*iWidth; i++)
        {
            aucNew[i*4] = aucOld[i*3];
            aucNew[i*4+1] = aucOld[i*3+1];
            aucNew[i*4+2] = aucOld[i*3+2];
            aucNew[i*4+3] = (unsigned char)255.0f;
        }
        break;
    case Image::IT_RGBA8888:
        for(int i = 0; i < iHeight*iWidth*4; i++)
            aucNew[i] = aucOld[i];
        break;
    case Image::IT_RGBA4444:
        for(int i = 0; i < iHeight*iWidth; i++)
        {
            aucNew[i*4] = (aucOld[i*2] & 0xF0);
            aucNew[i*4+1] = (aucOld[i*2] & 0x0F) << 4;
            aucNew[i*4+2] = (aucOld[i*2+1] & 0xF0);
            aucNew[i*4+3] = (aucOld[i*2+1] & 0x0F) << 4;
        }
        break;
    case Image::IT_RGBA5551:
        for(int i = 0; i < iHeight*iWidth; i++)
        {
            aucNew[i*4] = (aucOld[i*2] & 0xF8) << 3;
            aucNew[i*4+1] = ((aucOld[i*2] & 0x07) << 5) + 
                ((aucOld[i*2+1] & 0xC0) << 1);
            aucNew[i*4+2] = (aucOld[i*2+1] & 0x3E) << 2;
            aucNew[i*4+3] = (aucOld[i*2+1] & 0x01) << 7;
        }
        break;
    default:
        assert(!"Invalid format");
        break;
    }

    // Now, image is in an RGBA8888 format and we can multiply through
    // with alpha.
    for(int p = 0; p < iHeight*iWidth; p++)
    {
        aucNew[p*4+3] = (unsigned char)(fAlpha*(float)aucNew[p*4+3]);
    }

    Image* pkNew = new Image(Image::IT_RGBA8888,iWidth,iHeight,aucNew,
        rpkImage->GetName(),false);

    delete rpkImage;
    rpkImage = pkNew;
}
//----------------------------------------------------------------------------
void Exporter::LoadMultiTexture(MPlug& rkElement, Effect* pkEffect,
    Vector2fArray* pkUVs)
{
    // This loads a single texture layer from a layeredtexture object
    // and appends the texture and UVs (if possible) into the effect.
    // This is just a convenience function to keep the ConvertMesh function
    // from being too sprawling.

    if (rkElement.numChildren() < 4)
    {
        DisplayError("Layer doesn't have enough children.  Ignoring.");
        return;
    }

    // Note: Creating a dependency node here and searching
    // for the plug here doesn't work.  It always ended
    // up returning inputs[1].color instead of 
    // inputs[c].color.  Instead, I'll just use child(i)
    // for the plugs of kElement.

    Image* pkThisImage = NULL;

    MPlug kColorElement = rkElement.child(0);
    MPlugArray kLTexPArray;

    kColorElement.connectedTo(kLTexPArray,true,false,&m_kStatus);
    assert(m_kStatus);
    
    if (kLTexPArray.length() > 0)
    {
        // Load the texture
        if (kLTexPArray.length() > 1)
        {
            DisplayWarning("Texture in multi-texturing cannot have "\
                "multiple children.");
        }
        
        MFnDependencyNode kTex(kLTexPArray[0].node(),&m_kStatus);
        if (!m_kStatus)
        {
            DisplayError("Couldn't convert layer into a Dependency Node.");
            return;
        }

        if (kTex.object().apiType() == MFn::kFileTexture)
        {
            pkThisImage = LoadImage(kTex);
        }
        else
        {
            DisplayError("Invalid shader type used in "\
                "multi-texturing.");
        }
    }
    else // if (kLTexPArray.length() <= 0)
    {
        DisplayWarning("Multi-texturing without a texture."\
            "  Creating one from color.");

        if (kColorElement.numChildren() < 3)
        {
            DisplayError("Color does not have enough children.  "\
                "Ignoring layer.");
            return;
        }

        // Create a 1-pixel texture to represent the color that
        // was used in Maya instead of a fileTexture.
        unsigned char* aucData = new unsigned char[4];
        float fR,fG,fB;

        kColorElement.child(0).getValue(fR);
        kColorElement.child(1).getValue(fG);
        kColorElement.child(2).getValue(fB);

        aucData[0] = (unsigned char)(fR*255.0f);
        aucData[1] = (unsigned char)(fG*255.0f);
        aucData[2] = (unsigned char)(fB*255.0f);
        aucData[3] = (unsigned char)255;

        pkThisImage = new Image(Image::IT_RGBA8888,1,1,aucData,NULL,false);
        pkThisImage->SetName("MaterialImage");
    }

    float fTransparent;
    rkElement.child(1).getValue(fTransparent);

    int iBlend;
    rkElement.child(2).getValue(iBlend);

    bool bIsVisible;
    rkElement.child(3).getValue(bIsVisible);

    if (fTransparent <= 0.0f)
        bIsVisible = false;

    if (bIsVisible && pkThisImage)
    {
        // Maya allows for arbitrary alpha values of entire texture 
        // layers (which may contain alpha texel values).  However, WM 
        // only allows for alpha texel values.  To get around this, we 
        // will multiply this alpha value by every alpha value in the image
        // (creating it if none).
        //
        // The downside of this is that there are several (add, subtract, 
        // multiply, etc...) blending methods WM supports without an alpha 
        // value.  However, Maya allows for the alpha blending and the 
        // operation.
        //
        // There are two solutions to this: one is don't use semi-transparent
        // texture layers.  The second is to "bake" the alpha blending into
        // the texture itself to permanently and statically simulate what Maya
        // would have done.  A warning will get thrown when this occurs.  The
        // only thing it means is that you will not be able to use the 
        // original texture anymore if you want to change the texturing
        // mode.
        bool bBakeWarn = false;

        // This also converts the image to RGBA8888
        MultiplyAlpha(pkThisImage,fTransparent);

        Texture* pkTex = new Texture();
        pkTex->CoordU = Texture::WM_REPEAT;
        pkTex->CoordV = Texture::WM_REPEAT;

        // The definitions of these blending modes have been taken from the 
        // Maya documentation and are here as reference.
        // 
        // f* = foreground/source
        // b* = background/destination/previous
        // *c = color
        // *a = alpha
        // e.g. ba = background alpha

        switch(iBlend)
        {
        // Totally unsupportable (Blend modes 7-9)
        case 7: // Difference
            // color = (abs((fc * fa) - bc)) * fa + 
            //         bc * (1 - fa)
            // alpha = ba
        case 8: // Lighten
            // color = (max((fc * fa), bc)) * fa + 
            //         bc * (1 - fa)
            // alpha = ba
        case 9: // Darken
            // color = (min((fc * fa), bc)) * fa + bc * (1 - fa)
            // alpha = ba
        default:
            DisplayError("Unsupported blend mode!");
            // fall through

        // Totally supportable (Blend modes 0-3)
        case 0: // None
            // color = fc
            // alpha = fa
            pkTex->Apply = Texture::AM_REPLACE;
            break;
        case 1: // Over
            pkTex->Apply = Texture::AM_DECAL;
            // color = fc * fa + (bc * (1 - fa))
            // alpha = 1 - ((1 - ba) * (1 - fa))
            break;
        case 2: // In (stencil)
            // color = bc * fa
            // alpha = ba * fa
            pkTex->Apply = Texture::AM_COMBINE;

            pkTex->CombineFuncRGB = Texture::ACF_MODULATE;
            pkTex->CombineSrc0RGB = Texture::ACS_PREVIOUS;
            pkTex->CombineOp0RGB = Texture::ACO_SRC_COLOR;
            pkTex->CombineSrc1RGB = Texture::ACS_TEXTURE;
            pkTex->CombineOp1RGB = Texture::ACO_SRC_ALPHA;

            pkTex->CombineFuncAlpha = Texture::ACF_MODULATE;
            pkTex->CombineSrc0Alpha = Texture::ACS_PREVIOUS;
            pkTex->CombineOp0Alpha = Texture::ACO_SRC_ALPHA;
            pkTex->CombineSrc1Alpha = Texture::ACS_TEXTURE;
            pkTex->CombineOp1Alpha = Texture::ACO_SRC_ALPHA;
            break;
        case 3: // Out (stencil)
            // color = bc * (1 - fa)
            // alpha = ba * (1 - fa)
            pkTex->Apply = Texture::AM_COMBINE;

            pkTex->CombineFuncRGB = Texture::ACF_MODULATE;
            pkTex->CombineSrc0RGB = Texture::ACS_PREVIOUS;
            pkTex->CombineOp0RGB = Texture::ACO_SRC_COLOR;
            pkTex->CombineSrc1RGB = Texture::ACS_TEXTURE;
            pkTex->CombineOp1RGB = 
                Texture::ACO_ONE_MINUS_SRC_ALPHA;

            pkTex->CombineFuncAlpha = Texture::ACF_MODULATE;
            pkTex->CombineSrc0Alpha = Texture::ACS_PREVIOUS;
            pkTex->CombineOp0Alpha = Texture::ACO_SRC_ALPHA;
            pkTex->CombineSrc1Alpha = Texture::ACS_TEXTURE;
            pkTex->CombineOp1Alpha = 
                Texture::ACO_ONE_MINUS_SRC_ALPHA;
            break;

        // Supportable with baking (Blend modes: 4-8)
        case 4: // Add
            // color = bc + (fc * fa)
            // alpha = ba

            // Bake fc = fc*fa
            BakeColor(pkThisImage,fTransparent,0.0f);
            bBakeWarn = true;
            pkTex->Apply = Texture::AM_COMBINE;

            pkTex->CombineFuncRGB = Texture::ACF_ADD;
            pkTex->CombineSrc0RGB = Texture::ACS_PREVIOUS;
            pkTex->CombineOp0RGB = Texture::ACO_SRC_COLOR;
            pkTex->CombineSrc1RGB = Texture::ACS_TEXTURE;
            pkTex->CombineOp1RGB = Texture::ACO_SRC_COLOR;

            pkTex->CombineFuncAlpha = Texture::ACF_REPLACE;
            pkTex->CombineSrc0Alpha = Texture::ACS_PREVIOUS;
            pkTex->CombineOp0Alpha = Texture::ACO_SRC_ALPHA;
            break;
        case 5: // Subtract
            // color = bc - (fc * fa)
            // alpha = ba

            // Bake fc = fc*fa
            BakeColor(pkThisImage,fTransparent,0.0f);
            bBakeWarn = true;
            pkTex->Apply = Texture::AM_COMBINE;

            pkTex->CombineFuncRGB = Texture::ACF_SUBTRACT;
            pkTex->CombineSrc0RGB = Texture::ACS_PREVIOUS;
            pkTex->CombineOp0RGB = Texture::ACO_SRC_COLOR;
            pkTex->CombineSrc1RGB = Texture::ACS_TEXTURE;
            pkTex->CombineOp1RGB = Texture::ACO_SRC_COLOR;

            pkTex->CombineFuncAlpha = Texture::ACF_REPLACE;
            pkTex->CombineSrc0Alpha = Texture::ACS_PREVIOUS;
            pkTex->CombineOp0Alpha = Texture::ACO_SRC_ALPHA;
            break;
        case 6: // Multiply
            // color = bc * (fc * fa + 1 - fa)
            // alpha = ba

            // Bake fc = fc*fa + 1 - fa
            BakeColor(pkThisImage,fTransparent,1.0f-fTransparent);
            bBakeWarn = true;
            pkTex->Apply = Texture::AM_COMBINE;

            pkTex->CombineFuncRGB = Texture::ACF_MODULATE;
            pkTex->CombineSrc0RGB = Texture::ACS_PREVIOUS;
            pkTex->CombineOp0RGB = Texture::ACO_SRC_COLOR;
            pkTex->CombineSrc1RGB = Texture::ACS_TEXTURE;
            pkTex->CombineOp1RGB = Texture::ACO_SRC_COLOR;

            pkTex->CombineFuncAlpha = Texture::ACF_REPLACE;
            pkTex->CombineSrc0Alpha = Texture::ACS_PREVIOUS;
            pkTex->CombineOp0Alpha = Texture::ACO_SRC_ALPHA;
            break;
        case 10: // Saturate
            // color = bc * (1 + (fc * fa))
            // alpha = ba

            // Bake fc = fc*fa + 1
            BakeColor(pkThisImage,fTransparent,1.0f);
            bBakeWarn = true;
            pkTex->Apply = Texture::AM_COMBINE;

            pkTex->CombineFuncRGB = Texture::ACF_MODULATE;
            pkTex->CombineSrc0RGB = Texture::ACS_PREVIOUS;
            pkTex->CombineOp0RGB = Texture::ACO_SRC_COLOR;
            pkTex->CombineSrc1RGB = Texture::ACS_TEXTURE;
            pkTex->CombineOp1RGB = Texture::ACO_SRC_COLOR;

            pkTex->CombineFuncAlpha = Texture::ACF_REPLACE;
            pkTex->CombineSrc0Alpha = Texture::ACS_PREVIOUS;
            pkTex->CombineOp0Alpha = Texture::ACO_SRC_ALPHA;
            break;
        case 11: // Desaturate
            // color = bc * (1 - (fc * fa))
            // alpha = ba

            // Bake fc = fc*fa + 1
            BakeColor(pkThisImage,-fTransparent,1.0f);
            bBakeWarn = true;
            pkTex->Apply = Texture::AM_COMBINE;

            pkTex->CombineFuncRGB = Texture::ACF_MODULATE;
            pkTex->CombineSrc0RGB = Texture::ACS_PREVIOUS;
            pkTex->CombineOp0RGB = Texture::ACO_SRC_COLOR;
            pkTex->CombineSrc1RGB = Texture::ACS_TEXTURE;
            pkTex->CombineOp1RGB = Texture::ACO_SRC_COLOR;

            pkTex->CombineFuncAlpha = Texture::ACF_REPLACE;
            pkTex->CombineSrc0Alpha = Texture::ACS_PREVIOUS;
            pkTex->CombineOp0Alpha = Texture::ACO_SRC_ALPHA;
            break;
        case 12: // Illuminate
            // color = bc * (2 * fc * fa + 1 - fa)
            // alpha = ba

            // Bake fc = (2 * fc * fa + 1 - fa)
            BakeColor(pkThisImage,2.0f*fTransparent,1.0f-fTransparent);
            bBakeWarn = true;
            pkTex->Apply = Texture::AM_COMBINE;

            pkTex->CombineFuncRGB = Texture::ACF_MODULATE;
            pkTex->CombineSrc0RGB = Texture::ACS_PREVIOUS;
            pkTex->CombineOp0RGB = Texture::ACO_SRC_COLOR;
            pkTex->CombineSrc1RGB = Texture::ACS_TEXTURE;
            pkTex->CombineOp1RGB = Texture::ACO_SRC_COLOR;

            pkTex->CombineFuncAlpha = Texture::ACF_REPLACE;
            pkTex->CombineSrc0Alpha = Texture::ACS_PREVIOUS;
            pkTex->CombineOp0Alpha = Texture::ACO_SRC_ALPHA;
            break;
        }

        pkTex->SetImage(pkThisImage);

        if (bBakeWarn)
        {
            DisplayWarning("Texture was permanently altered"\
                " to support the blending mode.");
        }

        pkEffect->UVs.Append(pkUVs);
        pkEffect->Textures.Append(pkTex);
    }
    else
    {
        // This layer is not visible or the texture did not
        // load correctly, so don't attach it.

        // clean up memory that won't get used.
        if (pkThisImage)
            delete pkThisImage;
    }
}
//----------------------------------------------------------------------------
bool FindSkinCluster(const MFnMesh& rkMesh, MObject& rkResult)
{
    // The skin cluster could be behind multiple layers of indirection
    // in the hypergraph, so here we will search all skin clusters,
    // and find the one that has rkMesh as an output geometry, returning
    // the skin cluster object in rkResult.
    // NOTE: I'm not sure how computationally expensive this search is,
    // and so maybe this lookup should be precomputed once.

    MStatus kStatus;
    MItDependencyNodes* pkSkinScene = new MItDependencyNodes(
        MFn::kSkinClusterFilter,&kStatus);
    assert( kStatus );
    for (/**/; !pkSkinScene->isDone(); pkSkinScene->next())
    {        
        MFnSkinCluster kFnCluster(pkSkinScene->item(),&kStatus);
        assert(kStatus);

        MObjectArray kOutputGeom;
        kStatus = kFnCluster.getOutputGeometry(kOutputGeom);
        assert(kStatus);

        // Add the skin controller to each mesh...
        for(int g = 0; g < (int)kOutputGeom.length(); g++)
        {
            // Get the output mesh
            MFnMesh kOutputMesh(kOutputGeom[g],&kStatus);
            assert(kStatus);

            if (rkMesh.object() == kOutputMesh.object())
            {
                // The output of this cluster is this mesh,
                // so this cluster is the one we're looking for.
                rkResult = kFnCluster.object();
                delete pkSkinScene;
                return true;
            }
        }
    }

    delete pkSkinScene;
    return false;
}
//----------------------------------------------------------------------------
TriMesh* Exporter::ConvertMesh (const MFnMesh& rkMesh)
{
    // This function converts rkMesh as well as setting all the
    // material parameters (and bumpmap effects).  It returns the
    // created TriMesh.

    Vector2fArray* pkUV0;
    Vector2fArray* pkUV1;
    Vector2fArray* pkUV2;
    Vector2fArray* pkUV3;

    TriMesh* pkMesh;

    // If a mesh has a Skin Controller attached to it, then we will convert
    // the mesh input to the skin controller, which will have the skin values
    // at bind pose time, instead of the output mesh (rkMesh) which has
    // the skin values at Maya's currentTime().
    //
    // It is critically important to use this mesh.  When using
    // a Skin Controller, Maya automatically inserts a tweak node, which
    // may change the number of polygons, reorder triangles, etc...  Therefore,
    // in order to look up the correct UVs and material information, we need
    // to use this tweaked, bind pose skin mesh when we convert right here.
    bool bConnected = false;
    MPlug kInputMeshPlug = rkMesh.findPlug("inMesh",&m_kStatus);
    if (m_kStatus)
    {
        bConnected = kInputMeshPlug.isConnected(&m_kStatus);
        assert(m_kStatus);
    }
    else
    {
        DisplayWarning("Couldn't find input mesh plug.");
    }

    // If there is a skin controller, then this mesh should use the bind pose.
    bool bWantBindPose = bConnected && (m_abFlags[FLAG_SKIN] || m_abFlags[FLAG_BIND]);
    MObject kClusterObj;
    MPlugArray kInputMeshPlugArray;

    // Try to find a skin controller
    if (bWantBindPose)
    {
        kInputMeshPlug.connectedTo(kInputMeshPlugArray,true,false,&m_kStatus);
        assert(m_kStatus);
        assert(kInputMeshPlugArray.length() > 0);

        if (kInputMeshPlugArray.length() > 1)
        {
            DisplayWarning("Multiple input meshes for geometry.");
        }

        if (kInputMeshPlugArray[0].node().apiType() == MFn::kSkinClusterFilter)
        {
            // Skin controller output directly connected to input mesh
            kClusterObj = kInputMeshPlugArray[0].node();
        }
        else
        {
            // Do a more thorough search for skin cluster
            if (FindSkinCluster(rkMesh,kClusterObj))
            {
                // At this point, indirectly attached skin clusters are not
                // completely supported.  Connections, such as through
                // a polyTriangulate node, will be ignored and the skin
                // cluster and output geometry will be treated as if they
                // were directly connected.
                DisplayWarning("Intermediate nodes between cluster and output mesh "\
                    "will not be converted.");
            }
        }
    }

    // If bind pose wanted and we found a skin controller...
    if (bWantBindPose && kClusterObj.apiType() == MFn::kSkinClusterFilter)
    {
        // Skin cluster found, extract the bind pose skin
        //
        // From: http://greggman.com/pages/mayastuff.htm#skin
        // Get the mesh coming into the skinCluster. This
        // is the mesh before being deformed but after
        // being edited/tweaked/etc.
        MFnSkinCluster kCluster(kClusterObj,&m_kStatus);
        assert(m_kStatus);

        MPlug kInputPlug = kCluster.findPlug("input", &m_kStatus); 
        assert(m_kStatus);

        unsigned int uiMeshIndex = kCluster.indexForOutputShape(
            rkMesh.object(),&m_kStatus);
        assert(m_kStatus);

        MPlug kChildPlug = kInputPlug.elementByLogicalIndex(uiMeshIndex,
            &m_kStatus); 
        assert(m_kStatus);
        MPlug kGeomPlug = kChildPlug.child(0,&m_kStatus); 
        assert(m_kStatus);
        MObject kBindPoseObj;
        kGeomPlug.getValue(kBindPoseObj);   
        MFnMesh kBindPose(kBindPoseObj,&m_kStatus);
        assert(m_kStatus);

        DisplayInfo("Found skin controller " + kCluster.name() + 
            ".  Using bind pose mesh.");

        pkMesh = GetTriMesh(kBindPose,pkUV0,pkUV1,pkUV2,pkUV3);
    }
    else
    {
        // No skin controller, use rkMesh's geometry
        // get triangle mesh and texture coordinates (NULL if no texcoord i)
        pkMesh = GetTriMesh(rkMesh,pkUV0,pkUV1,pkUV2,pkUV3);
    }

    // Errors in converting?
    if (!pkMesh)
        return NULL;

    // Locate material.
    MObjectArray kShaders;
    MIntArray kIndices;
    m_kStatus = rkMesh.getConnectedShaders(0,kShaders,kIndices);
    assert( m_kStatus );

    // for now: 1 shading group per mesh
    int iShadersLength = kShaders.length();
    if (iShadersLength <= 0)
    {
        DisplayError("No connected shaders.  Ignoring materials.");
        return pkMesh;
    }
    MFnDependencyNode kShadingGroup(kShaders[0]);

    MPlug kSurfaceShader = kShadingGroup.findPlug("surfaceShader",&m_kStatus);
    if (!m_kStatus)
    {
        DisplayError("Couldn't find surfaceShader plug.  Ignoring materials.");
        return pkMesh;
    }

    MPlugArray kUpstreamPlugs;
    kSurfaceShader.connectedTo(kUpstreamPlugs,true,false,&m_kStatus);
    assert( m_kStatus );

    // Find a valid shader (only one shader is allowed)
    int iShade = 0;
    const int iShaders = kUpstreamPlugs.length();
    if (iShaders > 1)
    {
        DisplayWarning("More than one surface shader, " \
            "picking the first valid one.");
        // loop until we find a valid shader or we hit the last surface shader
        while(!IsValidShader(kUpstreamPlugs[iShade].node()) && 
            iShade < iShaders-1)
        {
            iShade++;
        }
    }

    // Create the material state (or use defaults if nothing can be found)
    bool bHasTransparency = false;
    bool bHasSpecular = false;
    MaterialState* pkMatState;
    if (iShaders > 0 && IsValidShader(kUpstreamPlugs[iShade].node()))
    {
        pkMatState = ConvertMaterial(kUpstreamPlugs[iShade].node(),
            bHasTransparency, bHasSpecular);
    }
    else
    {
        DisplayError("No valid surface shaders found.  "\
            "WM3 default materialstate will be used.");
        pkMatState = new MaterialState();
    }

    if (bHasTransparency)
    {
        AlphaState* pkAlpha = new AlphaState();
        pkAlpha->BlendEnabled = true;
        pkMesh->SetGlobalState(pkAlpha);
    }

    pkMesh->SetGlobalState(pkMatState);

    // If no flag set, skip textures
    if (!m_abFlags[FLAG_TEXTURES])
        return pkMesh;

    MFnDependencyNode kMat(kUpstreamPlugs[iShade].node());
    MPlugArray kPlugArray;

    // Look up and create all the Images first for this object and later
    // create the Effect objects once it is known what kind of effect it
    // should be.

    // These are NULL if it doesn't not exist, or a valid texture otherwise.
    Image* pkBumpImg = NULL;
    Image* pkPrimaryImg = NULL;

    // Look for bump map (first)
    MPlug kBumpPlug = kMat.findPlug("normalCamera",&m_kStatus);
    if (m_kStatus)
    {
        kBumpPlug.connectedTo(kPlugArray,true,false,&m_kStatus);
        assert(m_kStatus);
    }
    else
    {
        DisplayWarning("Couldn't find normalCamera plug!");
    }

    if ((kPlugArray.length() > 0) && m_abFlags[FLAG_BUMP])
    {
        if (kPlugArray.length() > 1)
        {
            DisplayWarning(
                "More than one connected normal map.  Using only the first.");
        }

        if (kPlugArray[0].node().apiType() == MFn::kBump)
        {
            MFnDependencyNode kBump(kPlugArray[0].node());
            MPlug kBumpDepth = kBump.findPlug("bumpDepth",&m_kStatus);
            assert(m_kStatus);
            float fBumpDepth;
            kBumpDepth.getValue(fBumpDepth);
            MPlug kBumpTex = kBump.findPlug("bumpValue",&m_kStatus);
            assert(m_kStatus);
            kBumpTex.connectedTo(kPlugArray,true,false,&m_kStatus);
            assert(m_kStatus);
            if (kPlugArray.length() > 0)
            {
                if (kPlugArray.length() > 1)
                {
                    DisplayWarning("More than one texture for "\
                        "this bump map.  Using only the first.");
                }

                if (kPlugArray[0].node().apiType() == MFn::kFileTexture)
                {
                    MFnDependencyNode kTex(kPlugArray[0].node());
                    pkBumpImg = LoadHeightMap(kTex,fBumpDepth);
                }
                else
                {
                    DisplayError("Non-texture connected to a bump"\
                        "map.  It will be ignored.");
                }
            }
            else
            {
                DisplayError("No textures connected to bump map.");
            }
        }
        else
        {
            DisplayError("Normal map connected, but it is not "\
                "of type bump map.");
        }

    }

    // Look for primary texture (second)
    MPlug kTexPlug = kMat.findPlug("color",&m_kStatus);
    if (m_kStatus)
    {
        kTexPlug.connectedTo(kPlugArray,true,false,&m_kStatus);
        assert(m_kStatus);
    }
    else
    {
        DisplayError("Couldn't find color plug.  Ignoring textures.");
        return pkMesh;
    }

    if (kPlugArray.length() > 0)
    {
        // Too many things attached
        if (kPlugArray.length() > 1)
        {
            DisplayWarning("Multiple nodes connected to color."\
                "  Using only the first one.");
        }

        if (kPlugArray[0].node().apiType() == MFn::kFileTexture)
        {
            // Normal texture

            MFnDependencyNode kTNode(kPlugArray[0].node());
            pkPrimaryImg = LoadImage(kTNode);
        }
        else if (kPlugArray[0].node().apiType() == MFn::kLayeredTexture)
        {
            // Multi-texture

            if (pkBumpImg)
            {
                DisplayWarning("Bump mapping and layered textures not"\
                    "supported simultaneously.  Ignoring layered texture.");
            }
            else
            {
                // fix maya default of black when texture is present
                pkMatState->Ambient = ColorRGBA::WHITE;
                pkMatState->Diffuse = ColorRGBA::WHITE;
                if (bHasSpecular)
                    pkMatState->Specular = ColorRGBA::WHITE;

                Effect* pkEffect = new Effect();

                // fix maya default of black when texture is present
                pkMatState->Diffuse = ColorRGBA::WHITE;

                MFnDependencyNode kLNode(kPlugArray[0].node(),&m_kStatus);
                assert(m_kStatus);
          
                MPlug kInputs = kLNode.findPlug("inputs",&m_kStatus);
                assert(m_kStatus);

                // input 0 is the top image in the list and c-1 is the bottom.
                // WM has 0 as the bottom image.
                for(int c = (int)kInputs.numElements()-1; c >=0 ; c--)
                {
                    // Load this layer
                    MString kTmp = "Loading multitexture layer ";
                    kTmp += c;
                    kTmp += "...";
                    DisplayInfo(kTmp);
                    LoadMultiTexture(kInputs[c],pkEffect,pkUV0);
                }

                if (pkEffect->Textures.GetQuantity() > 0)
                {
                    pkMesh->SetEffect(pkEffect);
                }
                else
                {
                    // No textures in effect, or all textures invisible
                    // or unable to load them.
                    delete pkEffect;
                }

                // All done
                pkMesh->SetGlobalState(pkMatState);
                return pkMesh;
            }
        }
        else
        {
            DisplayError("Unsupported node attached "\
                "to color.  It will be ignored.");
        }
    }

    // Now that we have loaded all the textures that exist, we can
    // put them into the correct effects.

    // Sanity check
    if (!pkUV0)
    {
        DisplayError("No primary texture coordinates.  Ignoring"\
            " textures.");
        return pkMesh;
    }

    if (pkPrimaryImg && !pkBumpImg)
    {
        // fix maya default of black when texture is present
        pkMatState->Ambient = ColorRGBA::WHITE;
        pkMatState->Diffuse = ColorRGBA::WHITE;
        if (bHasSpecular)
            pkMatState->Specular = ColorRGBA::WHITE;

        // Just a texture effect
        Texture* pkPrimaryTex = new Texture(pkPrimaryImg);
        pkPrimaryTex->Apply = Texture::AM_MODULATE;
        pkPrimaryTex->CoordU = Texture::WM_REPEAT;
        pkPrimaryTex->CoordV = Texture::WM_REPEAT;
        pkPrimaryTex->Filter = Texture::FM_LINEAR;
        pkPrimaryTex->Mipmap = Texture::MM_LINEAR_LINEAR;
        pkMesh->SetEffect(new TextureEffect(pkPrimaryTex,pkUV0));
    }
    else if (pkBumpImg)
    {
        // Bump map effect

        // fix maya default of black when texture is present
        pkMatState->Ambient = ColorRGBA::WHITE;
        pkMatState->Diffuse = ColorRGBA::WHITE;
        if (bHasSpecular)
            pkMatState->Specular = ColorRGBA::WHITE;

        if (!pkPrimaryImg)
        {
            // Bump map, but no primary image

            DisplayWarning("No primary image for bumpmap."\
                "  Creating image from material color.");

            unsigned char* aucData = new unsigned char[4];
            float fR,fG,fB,fA;
            MPlug kTempPlug;

            kTempPlug = kMat.findPlug("colorR",&m_kStatus);
            if (m_kStatus)
            {
                kTempPlug.getValue(fR);
                aucData[0] = (unsigned char)(fR*255.0f);
            }
            else
            {
                DisplayError("Couldn't find colorR plug.");
                aucData[0] = (unsigned char)0;
            }

            kTempPlug = kMat.findPlug("colorG",&m_kStatus);
            if (m_kStatus)
            {
                kTempPlug.getValue(fG);
                aucData[1] = (unsigned char)(fG*255.0f);
            }
            else
            {
                DisplayError("Couldn't find colorG plug.");
                aucData[1] = (unsigned char)0;
            }

            kTempPlug = kMat.findPlug("colorB",&m_kStatus);
            if (m_kStatus)
            {
                kTempPlug.getValue(fB);
                aucData[2] = (unsigned char)(fB*255.0f);
            }
            else
            {
                DisplayError("Couldn't find colorB plug.");
                aucData[2] = (unsigned char)0;
            }

            kTempPlug = kMat.findPlug("transparency",&m_kStatus);
            if (m_kStatus)
            {
                kTempPlug.getValue(fA);
                aucData[3] = (unsigned char)(fA*255.0f);
            }
            else
            {
                DisplayError("Couldn't find transparency plug.");
                aucData[3] = (unsigned char)0;
            }

            pkPrimaryImg = new Image(Image::IT_RGBA8888,1,1,aucData,NULL,
                false);

            pkPrimaryImg->SetName("MaterialImage");
        }

        // Bump maps need a light to be created.  Since we cannot know at
        // this point if there are any lights or which light needs to be
        // attached, we will store all the data needed for this effect
        // and defer the creation of the effect until the entire scene
        // tree has been read.

        if (!pkUV1)
        {
            DisplayWarning("No secondary texture coordinates for "\
                "bump map.  Reusing primary texture coordinates.");
            pkUV1 = pkUV0;
        }

        BumpMapData kData;
        kData.Primary = pkPrimaryImg;
        kData.PrimaryUVs = pkUV0;
        kData.NormalMap = pkBumpImg;
        kData.NormalUVs = pkUV1;
        kData.Modulated = true;
        kData.MeshKey = GetKey(rkMesh.object());

        int iVertices = pkMesh->Vertices->GetQuantity();
        ColorRGB* akColors = new ColorRGB[iVertices];
        memset(akColors,0,iVertices*sizeof(ColorRGB));
        ColorRGBArray* pkColors = new ColorRGBArray(iVertices,akColors);
        kData.Colors = pkColors;
        kData.Name = rkMesh.name();

        m_kWMBumpMap.Append(kData);
    }


    return pkMesh;
}
//----------------------------------------------------------------------------
static TriMesh* CloneMesh (TriMesh* pkMesh)
{
    // Make a clone of this pkMesh, sharing as much as possible

    // Clone geometry
    TriMesh* pkClone = new TriMesh(pkMesh->Vertices,pkMesh->Indices,false);
    pkClone->Normals = pkMesh->Normals;

    // Clone effects
    // Note: Bumpmaps will have to be cloned later, since bumpmaps
    // are deferred.
    if (pkMesh->GetEffect())
    {
        pkClone->SetEffect(pkMesh->GetEffect()->Clone());
    }

    // Clone state
    for (int iS = 0; iS < GlobalState::MAX_STATE; iS++)
    {
        GlobalState* pkState = pkMesh->GetGlobalState(iS);
        if (pkState)
            pkClone->SetGlobalState(pkState);
    }

    // No need to clone controllers, as no controllers will be attached
    // to a mesh at this point.  (Skin controllers will never be attached
    // to a cloned mesh anyway.)

    return pkClone;
}
//----------------------------------------------------------------------------
void Exporter::CMesh (const MFnDagNode& rkDagNode)
{
    // Convert rkDagNode into a TriMesh object.

    assert( rkDagNode.childCount() == 0 );

    bool bVisible = true;
    MPlug kVisibility = rkDagNode.findPlug("visibility",&m_kStatus);
    if (m_kStatus)
    {
        kVisibility.getValue(bVisible);
        m_bObjectInvisible |= !bVisible;
    }
    else
    {
        DisplayError("Couldn't find visibility plug, assuming visible.");
    }

    if (m_bObjectInvisible)
    {
        DisplayInfo("Ignoring invisible mesh.");
        return;
    }

    MFnMesh kMMesh(rkDagNode.object(),&m_kStatus);
    assert( m_kStatus );

    // NOTE.  Placed here for debugging purposes.  Can be moved to the first
    // line of the "if ( pkMesh )" block.
    const char* acName = rkDagNode.name().asChar();

    // Intermediate objects are source objects that are used to calculate
    // the output of transforming nodes.  i.e. a mesh at bind pose that
    // is used to calculate the output of a skin controller.  That mesh
    // still exists (invisibly) in the scene graph, but is flagged as
    // an intermediate object and so should be ignored.
    if (kMMesh.isIntermediateObject())
    {
        DisplayInfo("Ignoring intermediate object.");
        return;
    }

    TriMesh* pkMesh;
    
    // Check if this object is instanced
    InstanceData* pkFound = m_kMap.Find(GetKey(rkDagNode.object()));
    if (pkFound)
    {
        DisplayInfo("Creating instance of mesh.");
        Spatial* pkSpat = pkFound->GetFirst();
        assert(pkSpat->IsDerived(TriMesh::TYPE));
        pkMesh = CloneMesh(StaticCast<TriMesh>(pkSpat));
    }
    else
    {
        pkMesh = ConvertMesh(kMMesh);
    }

    if ( pkMesh )
    {
        // Mesh created ok

        pkMesh->SetName(acName);
        AttachToParent(rkDagNode,pkMesh);
    }
    else
    {
        DisplayError("Errors converting mesh " + rkDagNode.name() 
            + ".  Ignoring it.");
    }
}
//----------------------------------------------------------------------------

