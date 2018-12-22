// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Loader3ds.h"
#include "Wm3TriMesh.h"
#include "Wm3FloatArray.h"
#include "Wm3QuaternionArray.h"

// For the Warn function
//#include <stdio.h>
#include <cstdarg>
using namespace Wm3;

// This class is designed to read back a 3ds file and return a NodePtr to a
// scene graph from that file.  The functions are organized in the same manner
// as the file itself and so it closely approximates a functional programming
// methodology.  There are individual functions for most chunks.  They get
// called in this hierarchical manner:
//
// Main tree:
// -LoadSceneGraph()
//     -ReadEditChunk()
//         -ReadMaterialChunk()
//             -ReadTextureChunk()
//             -ReadColorChunks()
//             -ReadPercentageChunks()
//         -ReadObjectChunk()
//             -ReadObjLightChunk()
//             -ReadObjMeshChunk()
//                 -ReadObjMatChunk()
//     -ReadKeyfChunk()
//         -ReadObjDesChunk()
//             -ReadKeyfHierPosChunk()
//                 -ReadKeyfFlagChunk()
//             -ReadKeyfHierRotChunk()
//                 -ReadKeyfFlagChunk()
//             -ReadKeyfHierSclChunk()
//                 -ReadKeyfFlagChunk()
// 
// Can be called anywhere:
// -ReadUnknownChunk()

// Future todo:
// -Lights
// -Light targets (0xB006)
// -Camera information (from some separate accessor?)
// -Fold non-uniform node scalings down into the geometry itself

//----------------------------------------------------------------------------
Loader3ds::Loader3ds ()
    :
    m_kMeshNameHash(8),
    m_kLightNameHash(8),
    m_kMatNameHash(8)
{
    m_bNoKeyf = false;
}
//----------------------------------------------------------------------------
bool Loader3ds::LoadFile (const char* acFilename)
{
    return m_kStream.Load(acFilename);
}
//----------------------------------------------------------------------------
void Loader3ds::Warn (const char* acFormat, ...)
{
    // Prints out a warning (arguments are printf style) if warnings are
    // enabled.  If not, it does nothing.

    if ( m_bDoWarn )
    {
        va_list kArgs;
        va_start(kArgs,acFormat);
        vprintf(acFormat,kArgs);
    }
}
//----------------------------------------------------------------------------
NodePtr Loader3ds::LoadSceneGraph (bool bWarn)
{
    // This is the main public function to load the entire scene graph.  It
    // returns a NodePtr to the entire scene loaded from the 3ds file.  It
    // can be called multiple times to create several instances of a file if
    // need be.

    m_bDoWarn = bWarn;

    // Reset the input stream and output node and object storage in case this
    // function is called multiple times.
    m_spkRootNode = 0;
    m_kStream.SetAbsolutePos(0);
    m_aspkMeshArray.RemoveAll();
    m_aspkLightArray.RemoveAll();
    m_aspkMatArray.RemoveAll();
    m_kMeshNameHash.RemoveAll();
    m_kLightNameHash.RemoveAll();
    m_kMatNameHash.RemoveAll();

    while ( m_kStream.GetPos() < m_kStream.GetMaxPos() )
    {
        int iChunkID = m_kStream.ReadUInt2();
        switch ( iChunkID )
        {
        case L3DS_PRIMARY:
            // finish reading the header, but don't skip the chunk
            m_kStream.ReadInt4();
            break;
        case L3DS_EDIT:
            ReadEditChunk();
            break;
        case L3DS_KEYF:
            ReadKeyfChunk();
            break;
        default:
            ReadUnknownChunk();
            break;
        }
    }

    // If a keyframe chunk with object hierarchy data has not been found in
    // the file then just dumbly attach everything to the root node.
    if ( !m_spkRootNode )
    {
        m_spkRootNode = new Node;

        // attach all meshes
        int i;
        for (i = 0; i < m_aspkMeshArray.GetQuantity(); i++)
            m_spkRootNode->AttachChild(m_aspkMeshArray[i]);

        // attach all lights
        for (i = 0; i < m_aspkLightArray.GetQuantity(); i++)
            m_spkRootNode->AttachChild(m_aspkLightArray[i]);
    }

    m_spkRootNode->SetName("3DSroot");
    m_spkRootNode->UpdateGS();
    return m_spkRootNode;
}
//----------------------------------------------------------------------------
void Loader3ds::ReadUnknownChunk ()
{
    // This function skips past a chunk whose ID is unknown or is not
    // handled.  It assumes that the chunk ID for this chunk has already been
    // read.

    int iNextPos = m_kStream.ReadChunkPointer();
    m_kStream.SetAbsolutePos(iNextPos);
}
//----------------------------------------------------------------------------
void Loader3ds::ReadEditChunk ()
{
    // This function reads the edit chunk.  It is called only by
    // LoadSceneGraph(...).  Once this function is done, all of the material,
    // light, and mesh arrays and hashes will be filled in.  The function
    // assumes that the EDIT_MAT chunks come before the EDIT_OBJ chunks.

    // finish reading chunk header
    int iNextPos = m_kStream.ReadChunkPointer();
    int iStartPos = m_kStream.GetPos();

    while ( m_kStream.GetPos() < iNextPos )
    {
        // read next ID
        int iChunkID = m_kStream.ReadUInt2();

        switch ( iChunkID )
        {
        case L3DS_EDIT_MAT:
            ReadMaterialChunk();
            break;
        case L3DS_EDIT_OBJ:
            ReadObjectChunk();
            break;
        default:
            ReadUnknownChunk();
            break;
        }
    }

    m_kStream.SetAbsolutePos(iNextPos);
}
//----------------------------------------------------------------------------
void Loader3ds::ReadMaterialChunk ()
{
    // This function reads a material chunk.  It is only called by 
    // ReadEditChunk().  It reads a single material and appends it to the
    // material array (and inserts its name into the name hash).
    //
    // NOTE: Colors and shininess default to WildMagic color defaults if they
    // are not specified in the file.  However, I have not seen any files in
    // which they are all not specified, so this may not be a problem.

    // finish reading chunk header
    int iNextPos = m_kStream.ReadChunkPointer();

    String kName;
    MaterialStatePtr spkMat = new MaterialState;
    bool bTwoSided = false;
    TexturePtr spkTex = 0;
    float fUScale = 1.0f, fVScale = 1.0f;
    float fUOffset = 0.0f, fVOffset = 0.0f;

    while ( m_kStream.GetPos() < iNextPos )
    {
        int iChunkID = m_kStream.ReadUInt2();
        int iSubChunkEnd;
        switch ( iChunkID )
        {
        case L3DS_EDIT_MAT_NAME:
            iSubChunkEnd = m_kStream.ReadChunkPointer();
            kName = m_kStream.ReadString();
            spkMat->SetName(kName);
            m_kStream.SetAbsolutePos(iSubChunkEnd);
            break;
        case L3DS_EDIT_MAT_AMBIENT:
            spkMat->Ambient = ReadColorChunks(spkMat->Ambient);
            break;
        case L3DS_EDIT_MAT_DIFFUSE:
            spkMat->Diffuse = ReadColorChunks(spkMat->Diffuse);
            break;
        case L3DS_EDIT_MAT_SPECULAR:
            spkMat->Specular = ReadColorChunks(spkMat->Specular);
            break;
        case L3DS_EDIT_MAT_SHINY:
            spkMat->Shininess = ReadPercentageChunks(spkMat->Shininess);
            break;
        case L3DS_EDIT_MAT_TWOSIDE:
            iSubChunkEnd = m_kStream.ReadChunkPointer();
            bTwoSided = true;
            m_kStream.SetAbsolutePos(iSubChunkEnd);
            break;
        case L3DS_EDIT_MAT_TEX:
            spkTex = ReadTextureChunk(fUScale,fVScale,fUOffset,fVOffset);
            break;
        default:
            ReadUnknownChunk();
            break;
        }
    }
    m_kStream.SetAbsolutePos(iNextPos);

    // insert this material into the materials array
    m_kMatNameHash.Insert(kName,m_aspkMatArray.GetQuantity());
    m_aspkMatArray.Append(Material3ds(bTwoSided,spkMat,spkTex,fUScale,fVScale,
        fUOffset,fVOffset));
}
//----------------------------------------------------------------------------
TexturePtr Loader3ds::ReadTextureChunk (float& rfUScale, float& rfVScale,
    float& rfUOffset, float& rfVOffset)
{
    // This is a subfunction of ReadMaterialChunk.  It handles reading a
    // texture.  The texture offsets and scales are used when setting up the
    // material in ReadMaterialChunk, but are read from the file in this
    // function, so they are passed in as references.
    // 
    // This function returns a TexturePtr pointing to whatever texture was
    // loaded from a file.  If the file was not found or could not be loaded
    // for some reason, it will return 0.

    // finish reading chunk header
    int iNextPos = m_kStream.ReadChunkPointer();

    TexturePtr spkTex = 0;
    ImagePtr spkImage = 0;
    String kName;

    while ( m_kStream.GetPos() < iNextPos )
    {
        // Read subchunk header
        int iChunkID = m_kStream.ReadUInt2();
        int iSubChunkEnd = m_kStream.ReadChunkPointer();
        char* acNewName;

        switch ( iChunkID )
        {
        case L3DS_EDIT_MAT_TEX_NAME:
            // NOTE: Textures are referenced by name (e.g. sample.jpg) and are
            // loaded with a .wmif extension instead, regardless of what the
            // 3ds file says.

            kName = m_kStream.ReadString();

            // walk backwards until the last period is found
            int iP;
            for (iP = kName.GetLength() - 1; iP >= 0; iP--)
            {
                if (kName[iP] == '.')
                    break;
            }

            // The new name is 6 longer than the character that the period is
            // located at (four for the wmif, one because we start at zero,
            // and one more because of the \0 at the end).
            acNewName = new char[iP+6];

            // build new name
            int j;
            for (j = 0; j <= iP; j++)
            {
                acNewName[j] = kName[j];
            }
            acNewName[j++] = 'w';
            acNewName[j++] = 'm';
            acNewName[j++] = 'i';
            acNewName[j++] = 'f';
            acNewName[j++] = 0;
            spkImage = Image::Load(acNewName,false);
            if ( !spkImage )
                Warn("Image '%s' not found.\n",acNewName);
            delete[] acNewName; 
            break;
        case L3DS_EDIT_MAT_TEX_USCALE:
            rfUScale = m_kStream.ReadFloat();
            break;
        case L3DS_EDIT_MAT_TEX_VSCALE:
            rfVScale = m_kStream.ReadFloat();
            break;
        case L3DS_EDIT_MAT_TEX_UOFFSET:
            rfUOffset = m_kStream.ReadFloat();
            break;
        case L3DS_EDIT_MAT_TEX_VOFFSET:
            rfVOffset = m_kStream.ReadFloat();
            break;
        case L3DS_EDIT_MAT_TEX_TILING:
        case L3DS_EDIT_MAT_TEX_BLUR:
        case L3DS_INT_PERCENT:
        case L3DS_FLOAT_PERCENT:
            // TO DO: Not sure what to do with these
            break;
        }
        m_kStream.SetAbsolutePos(iSubChunkEnd);
    }

    // If the image did not load correctly or if the subchunk for the texture
    // name was not found, then do not create the texture.
    if ( spkImage )
        spkTex = new Texture(spkImage);

    m_kStream.SetAbsolutePos(iNextPos);
    return spkTex;
}
//----------------------------------------------------------------------------
ColorRGBA Loader3ds::ReadColorChunks (const ColorRGBA& rkDefault)
{
    // This function reads a set of color subchunks and returns a single color
    // best describing the color it finds.  There are several different kinds
    // of color chunks and this function abstracts that.  There are two kinds
    // of color (each in two formats), the gamma correct color (LININT and
    // LINFLOAT) and the normal color (FLOAT and INT).  Gamma correct color
    // overrides normal color.  If there is more than one color chunk, then
    // the last one will be used.
    
    int iNextPos = m_kStream.ReadChunkPointer();

    ColorRGBA kRet = rkDefault;
    bool bHaveGamma = false;

    while ( m_kStream.GetPos() < iNextPos )
    {
        // Read subchunk header
        int iChunkID = m_kStream.ReadUInt2();
        int iSubChunkEnd = m_kStream.ReadChunkPointer();

        switch ( iChunkID )
        {
        case L3DS_COL_RGBFLOAT:
            if ( bHaveGamma )
                m_kStream.ReadColorFloat();
            else
                kRet = m_kStream.ReadColorFloat();
            break;
        case L3DS_COL_RGBINT:
            if ( bHaveGamma )
                m_kStream.ReadColorInt();
            else
                kRet = m_kStream.ReadColorInt();
            break;
        case L3DS_COL_RGBLININT:
            kRet = m_kStream.ReadColorInt();
            bHaveGamma = true;
            break;
        case L3DS_COL_RGBLINFLOAT:
            kRet = m_kStream.ReadColorFloat();
            bHaveGamma = true;
            break;
        }
        m_kStream.SetAbsolutePos(iSubChunkEnd);
    }
    m_kStream.SetAbsolutePos(iNextPos);

    return kRet;
}
//----------------------------------------------------------------------------
float Loader3ds::ReadPercentageChunks (const float fDefault)
{
    // This function reads a set of percentage subchunks and returns a single
    // float best describing the percentage it finds.  There are two different
    // formats of percentages, int and float.  If there happen to be multiple
    // subchunks with percentages, the last one in the file will be used.
    int iNextPos = m_kStream.ReadChunkPointer();

    float fRet = fDefault;

    while ( m_kStream.GetPos() < iNextPos )
    {
        // Read subchunk header
        int iChunkID = m_kStream.ReadUInt2();
        int iSubChunkEnd = m_kStream.ReadChunkPointer();

        switch ( iChunkID )
        {
        case L3DS_INT_PERCENT:
            fRet = (float)m_kStream.ReadUInt2()/100.0f;
            break;
        case L3DS_FLOAT_PERCENT:
            fRet = m_kStream.ReadFloat();
            break;
        }
        m_kStream.SetAbsolutePos(iSubChunkEnd);
    } 
    m_kStream.SetAbsolutePos(iNextPos);

    return fRet;
}
//----------------------------------------------------------------------------
void Loader3ds::ReadObjectChunk ()
{
    // This function reads in both object and light chunks.  It is called
    // only by ReadEditChunk().  Once this function has finished, all of the
    // light and mesh arrays and name hashes will be filled in.

    // finish reading chunk header
    int iNextPos = m_kStream.ReadChunkPointer();

    // read object name
    String kName = m_kStream.ReadString();
    SpatialPtr spkObj = 0;
    LightPtr spkLight = 0;

    while (m_kStream.GetPos() < iNextPos)
    {
        int iChunkID = m_kStream.ReadUInt2();
        switch (iChunkID )
        {
        case L3DS_EDIT_OBJ_LIGHT:
            m_kLightNameHash.Insert(kName,m_aspkLightArray.GetQuantity());
            spkLight = ReadObjLightChunk();
            if ( spkLight )
            {
                spkLight->SetName(kName);
                m_aspkLightArray.Append(spkLight);
            }
            break;
        case L3DS_EDIT_OBJ_MESH:
            m_kMeshNameHash.Insert(kName,m_aspkMeshArray.GetQuantity());
            spkObj = ReadObjMeshChunk();
            if ( spkObj )
            {
                spkObj->SetName(kName);
                m_aspkMeshArray.Append(spkObj);
            }
            break;
        }
    }

    m_kStream.SetAbsolutePos(iNextPos);
}
//----------------------------------------------------------------------------
LightPtr Loader3ds::ReadObjLightChunk ()
{
    // This function reads in information about a single light object and
    // returns a pointer to a new light containing that information.

    // finish reading chunk header
    int iNextPos = m_kStream.ReadChunkPointer();

    LightPtr spkLight = new Light(Light::LT_POINT);

    // Read light position
    Vector3f kLoc = m_kStream.ReadVector3f();
    spkLight->SetLocation(kLoc);
    spkLight->On = true;

    // The 3ds file doesn't do a good job of differentiating between kinds of
    // lights.  I am just going to assume that lights are not ambient and that
    // the color is for specular and diffuse.

    while ( m_kStream.GetPos() < iNextPos )
    {
        int iChunkID = m_kStream.ReadUInt2();
        int iSubChunkEnd = m_kStream.ReadChunkPointer();

        float fHotspot, fFalloff;
        ColorRGBA kCol;

        switch ( iChunkID )
        {
        // if for some reason there are two color chunks, the latter
        // will set the color
        case L3DS_COL_RGBFLOAT:
            kCol = m_kStream.ReadColorFloat();
            spkLight->Specular = kCol;
            spkLight->Diffuse = kCol;
            break;
        case L3DS_COL_RGBINT:
            kCol = m_kStream.ReadColorInt();
            spkLight->Specular = kCol;
            spkLight->Diffuse = kCol;
            break;
        case L3DS_EDIT_OBJ_LIGHT_OFF:
            spkLight->On = m_kStream.ReadBool();
            break;
        case L3DS_EDIT_OBJ_LIGHT_SPOT:
            spkLight->Type = Light::LT_SPOT;
            spkLight->SetDirection(m_kStream.ReadVector3f() - kLoc);

            // TO DO: Not sure what hotspot is and how falloff correlates
            // with attenuation parameters
            fHotspot = m_kStream.ReadFloat();
            fFalloff = m_kStream.ReadFloat();
            break;
        }
        m_kStream.SetAbsolutePos(iSubChunkEnd);
    }
    m_kStream.SetAbsolutePos(iNextPos);
    return spkLight;
}
//----------------------------------------------------------------------------
SpatialPtr Loader3ds::ReadObjMeshChunk ()
{
    // This function reads a single mesh from the file and returns a
    // SpatialPtr to that mesh.  The SpatialPtr will be either a TriMeshPtr,
    // if an object has only one material, or a NodePtr if there are multiple
    // materials.  If it is a NodePtr, then it has n TriMesh children, where
    // n is the number of materials for the original mesh in the file.  Each
    // TriMesh contained in the SpatialPtr has its vertex data in local
    // coordinates and its local transform is the world transform.  It may
    // return 0 if the mesh cannot be loaded.

    // finish reading chunk header
    int iNextPos = m_kStream.ReadChunkPointer();

    int i, iVertInfo, iNumVertices = 0;
    int iNumFaces = 0;
    int iTexels = 0;
    int* aiFaces = 0;
    Vector3f* akVertex = 0;
    Vector2f* akUVs = 0;
    bool bVisible = true;
    bool bMultiMaterial = false;

    // face->material index mapping
    int* aiMatGroup = 0;

    Matrix3f kScaleRot;
    Vector3f kTrans;

    while ( m_kStream.GetPos() < iNextPos )
    {
        // read subchunk header
        int iChunkID = m_kStream.ReadUInt2();
        int iSubChunkEnd = m_kStream.ReadChunkPointer();

        switch ( iChunkID )
        {
        case L3DS_EDIT_OBJ_MESH_VERTEXL:
            iNumVertices = m_kStream.ReadUInt2();

            // If for some reason there are two chunks of this type, clean up
            // memory first.
            delete[] akVertex;
            akVertex = new Vector3f[iNumVertices];
            for (i = 0; i < iNumVertices; i++)
                akVertex[i] = m_kStream.ReadVector3f();
            break;
        case L3DS_EDIT_OBJ_MESH_WORLD:
            // read world transform
            kScaleRot = m_kStream.ReadMatrix3f();
            kTrans = m_kStream.ReadVector3f();
            break;
        case L3DS_EDIT_OBJ_MESH_SMOOTH:
            // TO DO: ignore smoothing groups for now
            break;
        case L3DS_EDIT_OBJ_MESH_VISIBLE:
            bVisible = m_kStream.ReadBool();
            break;
        case L3DS_EDIT_OBJ_MESH_UVS:
            delete[] akUVs;
            iTexels = m_kStream.ReadUInt2();
            akUVs = new Vector2f[iTexels];
            for (i = 0; i < iTexels; i++)
                akUVs[i] = m_kStream.ReadVector2f();
            break;
        case L3DS_EDIT_OBJ_MESH_FACES:
            iNumFaces = m_kStream.ReadUInt2();
            delete[] aiFaces;
            aiFaces = new int[3*iNumFaces];
            for (i = 0; i < iNumFaces; i++)
            {
                aiFaces[3*i] = m_kStream.ReadUInt2();
                aiFaces[3*i+1] = m_kStream.ReadUInt2();
                aiFaces[3*i+2] = m_kStream.ReadUInt2();
               
                // unknown flag(s)
                iVertInfo = m_kStream.ReadUInt2();
            }

            // read subchunks here
            ReadObjMatChunk(iSubChunkEnd,iNumFaces,iNumVertices,aiMatGroup,
                bMultiMaterial);
            break;
        }
        m_kStream.SetAbsolutePos(iSubChunkEnd);
    }

    // sanity checks:
    if ( !iNumVertices || !iNumFaces )
        return 0;

    // The number of texels is specified separately from the number of
    // vertices, but they must be the same.
    assert( iTexels == 0 || iTexels == iNumVertices );

    SpatialPtr kRet;

    // Calculate node transformation.  The kScaleRot is a diagonal scale
    // matrix (not necessarily uniform) times the rotation matrix.  Since the
    // rotation matrix R is orthogonal, we know that R^t*R = I, thus if we
    // take (R*S)^t*(R*S) we are left with S^t*(R^t*R)*S = S^t*S = S*S.
    Transformation kTrf;
    Matrix3f kScaleMat = kScaleRot.Transpose()*kScaleRot;
    Vector3f kScaleVec(Mathf::Sqrt(kScaleMat[0][0]),
        Mathf::Sqrt(kScaleMat[1][1]),Mathf::Sqrt(kScaleMat[2][2]));

    // create matrix of inverse scales
    kScaleMat.MakeDiagonal(1.0f/kScaleVec[0], 1.0f/kScaleVec[1], 
        1.0f/kScaleVec[2]);

    // unscale the rotation matrix
    Matrix3f kRot = kScaleRot*kScaleMat;
    float fDet = kRot.Determinant();
    if ( Mathf::FAbs(Mathf::FAbs(fDet) - 1.0f) > 1e-06f )
        Warn("Possible bad rotation matrix (det = %f)\n",fDet);

    if ( fDet < 0.0f )
        Warn("Improper rotation matrix (det =%f)\n",fDet);

    kTrf.SetRotate(kRot);
    kTrf.SetTranslate(kTrans);
    kTrf.SetScale(kScaleVec);

    // Geometry comes in world space (ugh) and so apply inverse transformation
    // to get back to local.
    for (int i = 0; i < iNumVertices; i++)
        akVertex[i] = kTrf.ApplyInverse(akVertex[i]);

    // Create shared arrays
    Vector3fArrayPtr spkVertices = new Vector3fArray(iNumVertices,
        akVertex);
    IntArrayPtr spkIndices = new IntArray(iNumFaces*3,aiFaces);
    Vector2fArrayPtr spkUVs = 0;
    if ( akUVs )
        spkUVs = new Vector2fArray(iNumVertices,akUVs);

    if ( bMultiMaterial )
    {
        // Mesh with 2+ materials.  We need to split into a node with multiple
        // geometry children since a piece of geometry can only have one
        // material at a time.  This function also attaches transforms and
        // materials locally to each TriMesh
        kRet = (SpatialPtr)SplitMesh(spkVertices,spkIndices,aiMatGroup,spkUVs,
            kTrf);
    }
    else
    {
        // simple mesh with 0-1 materials
        TriMeshPtr spkMesh = new TriMesh(spkVertices,spkIndices,true);

        // If it has material, then just use the first face's material
        // properties.
        if ( aiMatGroup && aiMatGroup[0] >= 0 )
             m_aspkMatArray[aiMatGroup[0]].AttachStateTo(spkMesh,spkUVs);

        spkMesh->Local = kTrf;

        kRet = (SpatialPtr)spkMesh;
    }

    // clean up memory usage for this function
    delete[] aiMatGroup;

    m_kStream.SetAbsolutePos(iNextPos);
    return kRet;
}
//----------------------------------------------------------------------------
NodePtr Loader3ds::SplitMesh (Vector3fArrayPtr spkVertices,
    IntArrayPtr spkFaces, int* aiMatGroup, Vector2fArrayPtr spkUVs,
    Transformation& rkTrans)
{
    // SplitMesh is a utility function to take a single TriMesh and split it
    // into a NodePtr with several TriMesh children.  It takes as input the
    // vertices, faces, texture coordinates, and transformation of the
    // original mesh.  It also has as input an integer array containing a face
    // to material index mapping for the mesh.
    //
    // The split mesh routine returns a node with one child trimesh for each
    // unique material specified by aiMatGroup.
    //
    // Although new face lists will be created, all the child geometries
    // will share the vertex, normal, and texture coordinate arrays.
    //
    // Create a mesh with all of the faces and vertices to calculate the
    // correct normals
    TriMeshPtr spkRootMesh = new TriMesh(spkVertices,spkFaces,true);
    Vector3fArrayPtr spkNorm = spkRootMesh->Normals;

    // node that is the parent of the TriMesh objects
    NodePtr spkNode = new Node;

    // abUsedMat keeps track of which materials have already been split
    bool* abUsedMat = new bool[m_aspkMatArray.GetQuantity()];
    bool bUsedInvalidMat = false;
    int i, j;
    int iNumFaces = spkFaces->GetQuantity()/3;

    // Reset used material array
    for (i = 0; i < m_aspkMatArray.GetQuantity(); i++)
        abUsedMat[i] = false;

    for (i = 0; i < iNumFaces; i++)
    {
        // Test this face
        int iMat = aiMatGroup[i];
        if ( iMat == -1 && !bUsedInvalidMat )
        {
            // It's possible that a material list doesn't include all the
            // faces or that a trimesh specifies a material by name that
            // doesn't exist (leading to an invalid material group) and so
            // this will catch those and create a Trimesh without a
            // MaterialState.
            bUsedInvalidMat = true;
        }
        else if ( (iMat != -1) && !abUsedMat[iMat] )
        {
            // found a material group that hasn't been split yet
            abUsedMat[iMat] = true;
        }
        else 
        {
            // not a new material group
            continue;
        }

        // now create a Trimesh for all faces with material iMat

        // count new faces needed
        int iNewFaces = 0;
        for (j = 0; j < iNumFaces; j++)
        {
            if ( aiMatGroup[j] == iMat )
                iNewFaces++;
        }

        // create new face array
        int* aiNewFaces = new int[3*iNewFaces];
        int iNewF = 0;
        for (j = 0; j < iNumFaces; j++)
        {
            if ( aiMatGroup[j] == iMat )
            {
                aiNewFaces[3*iNewF] = (*spkFaces)[3*j];
                aiNewFaces[3*iNewF+1] = (*spkFaces)[3*j+1];
                aiNewFaces[3*iNewF+2] = (*spkFaces)[3*j+2];
                iNewF++;
            }
        }

        // now that all the information has been created, make the trimesh
        IntArrayPtr spkIndices = new IntArray(iNewFaces*3,aiNewFaces);
        TriMeshPtr spkMesh = new TriMesh(spkVertices,spkIndices,false);
        spkMesh->Normals = spkNorm;
        spkMesh->UpdateMS();
        if ( iMat != -1 )
        {
            m_aspkMatArray[iMat].AttachStateTo(spkMesh,spkUVs);
        }
        spkNode->AttachChild(spkMesh);
        spkMesh->Local = rkTrans;
    }

    // clean up mesh that we built just to get the normals
    spkRootMesh = 0;

    delete[] abUsedMat;
    return spkNode;
}
//----------------------------------------------------------------------------
void Loader3ds::ReadObjMatChunk (int iNextPos, int iNumFaces, int iNumVerts,
    int*& raiMaterials, bool& rbMultiMaterial)
{
    // This function reads a subchunk and is called from ReadObjMeshChunk().
    // iNextPos is the chunk pointer to the end of this subchunk.  iNumFaces
    // is the number of faces in the mesh.  iNumVerts is the number of
    // vertices in the mesh.  raiMaterials is a reference to the array of
    // face->material index mapping.  rbMultiMaterial is a reference to the
    // boolean of if this object has multiple materials or not.  It will get
    // set in this function to true if it finds that an object has multiple
    // materials.

    // chunk pointer already read, passed in as iNextPos

    while ( m_kStream.GetPos() < iNextPos )
    {
        int iChunkID = m_kStream.ReadUInt2();
        int iSubChunkEnd = m_kStream.ReadChunkPointer();
        int i;
        int iFaces;
        int* piMat;
        String kName;

        switch ( iChunkID )
        {
        case L3DS_EDIT_OBJ_MESH_MAT:
            // create and initialize material face array if needed
            if ( raiMaterials == 0 )
            {
                raiMaterials = new int[iNumFaces];
                for (i = 0; i < iNumFaces; i++)
                {
                    // initialize to invalid
                    raiMaterials[i] = -1;
                }
            }
            kName = m_kStream.ReadString();
            piMat = m_kMatNameHash.Find(kName);
            iFaces = m_kStream.ReadUInt2();

            if ( iFaces != iNumFaces )
                rbMultiMaterial = true;

            if ( piMat )
            {
                for (i = 0; i < iFaces; i++)
                    raiMaterials[m_kStream.ReadUInt2()] = *piMat;
            }
            else
            {
                Warn("Unknown material %s requested by an object.\n",kName);
            }
            break;
        case L3DS_EDIT_OBJ_MESH_SMOOTH:
            // TO DO: smoothing groups
            break;
        }
        m_kStream.SetAbsolutePos(iSubChunkEnd);
    }
}
//----------------------------------------------------------------------------
void Loader3ds::ReadKeyfChunk ()
{
    // This function reads the main keyframe chunk.  It is only called by
    // LoadSceneGraph().  Once it is done, a keyframe hierarchy and possibly
    // controllers will have been set up for the scene.

    // finish reading chunk header
    int iNextPos = m_kStream.ReadChunkPointer();

    // array for keeping track of nodes and their ID numbers
    TArray<NodePtr> kHier(0);

    // iStart and iEnd are the start and end frame numbers
    int iStart = 0, iEnd = 0, iSubChunkEnd;

    while ( m_kStream.GetPos() < iNextPos )
    {
        int iChunkID = m_kStream.ReadUInt2();
        switch ( iChunkID )
        {
        case L3DS_KEYF_LIGHTDES:
            ReadUnknownChunk();
            break;
        case L3DS_KEYF_OBJDES:
            ReadObjDesChunk(kHier, iStart, iEnd);
            break;
        case L3DS_KEYF_FRAMES:
            iSubChunkEnd = m_kStream.ReadChunkPointer();
            iStart = m_kStream.ReadInt4();
            iEnd = m_kStream.ReadInt4();
            m_kStream.SetAbsolutePos(iSubChunkEnd);
            break;
        default:
            ReadUnknownChunk();
            break;
        }
    }
    m_kStream.SetAbsolutePos(iNextPos);

    // Cleanup!  If a keyframe hierarchy has been created...
    if ( m_spkRootNode )
    {
        int i;

        // Clean up only-child geometry.  These were created to facilitate
        // making the hierarchy because it's only after the hierarchy is built
        // that it is known how many children a node has.
        for (i = 0; i < m_aspkMeshArray.GetQuantity(); i++)
        {
            Spatial* pkParent = m_aspkMeshArray[i]->GetParent();

            // If mesh unused in hierarchy
            if ( !pkParent )
                continue;

            Node* pkPNode = StaticCast<Node>(pkParent);

            // Note: only single children with the identity transform are
            // merged into the parent node.  This behavior is to preserve the
            // pivot points from MAX.  If the desired behavior is to make the
            // scene tree as shallow as possible take out the IsIdentity()
            // condition from the following if statement.
            if ( pkPNode->GetUsed() == 1 && 
                 m_aspkMeshArray[i]->Local.IsIdentity() )
            {
                // If geometry is attached to a parent, that parent node must
                // be necessarily attached to some other node.
                assert( pkPNode->GetParent()) ;

                // Give the geometry the transform of the node and attach it 
                // to the node's parent.
                pkPNode->DetachChild(m_aspkMeshArray[i]);
                Node* pkGrandparent = StaticCast<Node>(pkPNode->GetParent());
                pkGrandparent->DetachChild(pkPNode);
                pkGrandparent->AttachChild(m_aspkMeshArray[i]);
                m_aspkMeshArray[i]->Local.Product(pkPNode->Local,
                    m_aspkMeshArray[i]->Local);

                // give all controllers as well
                if ( pkPNode->GetControllerQuantity() > 0 )
                {
                    int iCQ = pkPNode->GetControllerQuantity();
                    for (int j = 0; j < iCQ; j++)
                    {
                        ControllerPtr pkCon = pkPNode->GetController(j);
                        pkPNode->RemoveController(pkCon);
                        m_aspkMeshArray[i]->SetController(pkCon);
                    }
                }
            }
        }

        // Clean up dummy nodes without geometry children.  The hierarchy is
        // read as a depth-first search, so reverse walk through the array to
        // eliminate nested dummy nodes.
        for (i = kHier.GetQuantity() - 1; i >= 0; i--)
        {
            if ( kHier[i] && kHier[i]->GetUsed() == 0 )
            {
                Node* pkParent = StaticCast<Node>(kHier[i]->GetParent());

                // Although all nodes in kHier are 0 or have a parent after
                // the hierarchy has been read, they may have been detached
                // in the for loop above.
                if ( pkParent )
                    pkParent->DetachChild(kHier[i]);

                kHier[i] = 0;
            }
        }
    }
}
//----------------------------------------------------------------------------
void Loader3ds::ReadObjDesChunk (TArray<NodePtr>& rkHier, int iStart,
    int iEnd)
{
    // This function reads the hierarchy information for a single object in
    // the scene.  It takes as input a list of the nodes (indexed by object
    // ID) and adds the current node into that list, attaching it to the
    // correct parent, and setting up the transform and controllers.
    //
    // If lights are ever added, an analogous function or a modification of
    // this one is needed.

    // finish reading chunk header
    int iNextPos = m_kStream.ReadChunkPointer();

    String kName;
    SpatialPtr spkCurrGeom = 0, spkOldGeom = 0;
    NodePtr spkCNode = 0, spkPNode = 0;
    int iObjID = -1;
    const String kDummy = "$$$DUMMY";
    bool bDummy = false, bPivot = false;
    int iParent = -1;
    Vector3f kPivot = Vector3f::ZERO;
    KeyframeControllerPtr spkCtrl = 0;

    while ( m_kStream.GetPos() < iNextPos )
    {
        int iChunkID = m_kStream.ReadUInt2();
        int iSubChunkEnd = m_kStream.ReadChunkPointer();
 
        int* piObjNum = 0;
   
        switch ( iChunkID )
        {
        case L3DS_KEYF_OBJDES_ID:
            iObjID = m_kStream.ReadUInt2();
            break;
        case L3DS_KEYF_OBJDES_HIER:
            // the L3DS_KEYF_OBJDES_ID must always preceed this chunk
            assert( iObjID != -1 );

            kName = m_kStream.ReadString();

            // Ignore dummy objects.  They are just connecting nodes without
            // any geometry.  Sometimes they do not even have children, but we
            // cannot check for that here.
            if ( kName != kDummy )
            {
                // Mesh and light name referenced here must have a
                // corresponding L3DS_EDIT_OBJ chunk in the 3DS file.
                piObjNum = m_kMeshNameHash.Find(kName);
                assert( piObjNum );
                spkCurrGeom = m_aspkMeshArray[*piObjNum];
            }
            else
            {
                bDummy = true;
                spkCurrGeom = 0;
            }

            // check to see if this geometry has already been attached
            if ( spkCurrGeom && spkCurrGeom->GetParent() )
            {
                // Clone spkCurrGeom by creating new nodes/trimeshes but
                // sharing the underlying geometry.  Then, replace spkCurrGeom
                // with this new mesh so that the new mesh can be attached
                // properly.

                // If a multi-material mesh, spkCurrGeom will be a node, and
                // so we need to clone all the individual ones.
                if ( spkCurrGeom->IsExactly(Node::TYPE) )
                {
                    NodePtr spkNewNode = new Node;
                    Node* pkOldNode = StaticCast<Node>(spkCurrGeom);
                    spkNewNode->Local = spkCurrGeom->Local;

                    // clone all the attached trimesh children
                    for (int j = 0; j < pkOldNode->GetQuantity(); j++)
                    {
                        SpatialPtr spkChild = pkOldNode->GetChild(j);
                        assert(spkChild->IsExactly(TriMesh::TYPE));

                        spkNewNode->AttachChild(CloneTriMesh(
                            StaticCast<TriMesh>(spkChild)));
                    }

                    spkCurrGeom = spkNewNode;
                }
                else
                {
                    // single material mesh, just clone it
                    assert(spkCurrGeom->IsExactly(TriMesh::TYPE));
                    spkCurrGeom = CloneTriMesh(StaticCast<TriMesh>(
                        spkCurrGeom));
                }
            }

            // unknown flags
            m_kStream.ReadUInt2();
            m_kStream.ReadUInt2();

            iParent = m_kStream.ReadInt2();

            // It is only when this subchunk is found that we know that an
            // object hierarchy exists for this scene.  If this is the first
            // subchunk found of this type, then do some setup.
            if ( !m_spkRootNode )
                m_spkRootNode = new Node();

            // all object IDs in the file should be strictly increasing
            assert( rkHier.GetQuantity() <= iObjID );

            // IDs not in consecutive order?
            while ( rkHier.GetQuantity() < iObjID )
            {
                // 0-fill the array to be the right size
                rkHier.Append(0);
            }

            // a parent of -1 means the root node
            if ( iParent == -1 )
                spkPNode = m_spkRootNode;
            else
                spkPNode = rkHier[iParent];

            // The only time this will fail is if an object ID is skipped (see
            // above) and then a subsequent child tries to use that ID as a
            // parent before the parent gets initialized (which is impossible 
            // by the strictly increasing assumption above).
            assert( spkPNode );

            // do the actual hierarchy attaching here
            spkCNode = new Node;
            spkCNode->Local = Transformation::IDENTITY;
            if ( bDummy )
            {
                spkPNode->AttachChild(spkCNode);
            }
            else
            {
                // 3DS files allow for geometry to be attached to geometry.
                // Here we will attach all geometry with an intermediate node
                // to avoid this problem.  We will clean this up later by
                // checking for only-child geometries.

                // Append "_N" (for node) to the name of this node (which is
                // currently the name of the geometry itself) to
                // differentiate.
                char* acNodeName = new char[kName.GetLength()+3];
                int j;
                for (j = 0; j < kName.GetLength(); j++)
                    acNodeName[j] = kName[j];
                acNodeName[j++] = '_';
                acNodeName[j++] = 'N';
                acNodeName[j] = 0;
                spkCNode->SetName(String(acNodeName));
                delete[] acNodeName;

                // attach geometry through intermediate node
                spkCNode->AttachChild(spkCurrGeom);
                spkPNode->AttachChild(spkCNode);
            }

            // By the above assertions and while loop, this is the same as 
            // "rkHier[iObjID] = spkCNode;" with the append to make sure 
            // there is room.
            rkHier.Append(spkCNode);
            break;
        case L3DS_KEYF_OBJDES_PIVOT:
            kPivot = m_kStream.ReadVector3f();
            bPivot = ( kPivot != Vector3f::ZERO );
            break;
        case L3DS_KEYF_OBJDES_POS:
            // ASSUMPTION: L3DS_KEYF_OBJDES_HIER always precedes the sibling
            // L3DS_KEYF_OBJDES_POS/ROT/SCL chunks.  If not, then break this
            // while loop up into two while loops.  Also, there should only be
            // one of each PIVOT/POS/ROT/SCL chunk.  Subsequent chunks will
            // overwrite the prior ones.
            assert( spkCNode );
            ReadKeyfHierPosChunk(spkCtrl,spkPNode,spkCNode,spkCurrGeom);
            break;
        case L3DS_KEYF_OBJDES_ROT:
            // see L3DS_KEYF_OBJDES_POS note above
            assert( spkCNode );
            ReadKeyfHierRotChunk(spkCtrl,spkPNode,spkCNode,spkCurrGeom);
            break;
        case L3DS_KEYF_OBJDES_SCL:
            // see L3DS_KEYF_OBJDES_POS note above
            assert( spkCNode );
            ReadKeyfHierSclChunk(spkCtrl,spkPNode,spkCNode,spkCurrGeom);
            break;
        case L3DS_KEYF_OBJDES_NAME:
            // see L3DS_KEYF_OBJDES_POS note above
            assert( spkCNode );

            // this name is only for $$$DUMMY objects
            kName = m_kStream.ReadString();
            spkCNode->SetName(kName);
            break;
        }
        m_kStream.SetAbsolutePos(iSubChunkEnd);
    }

    // now that we have read the hierarchy chunk, clean up transformations
    if ( spkCNode )
    {
        // if we have a keyframe controller, set it up and attach it
        if ( spkCtrl )
        {
            spkCtrl->MinTime = iStart;
            spkCtrl->MaxTime = iEnd;
            spkCtrl->RepeatType = Controller::RT_WRAP;
            spkCNode->SetController(spkCtrl);
        }

        if ( spkCurrGeom )
        {
            // If there is local geometry, it takes the transformation of the
            // parent node if there is a keyframe hierarchy, because the
            // keyframe hierarchy controls its transform.  Thus, we need to
            // set the local geometry's transform to the identity.  We cannot
            // do this earlier, because the world transform of the geometry
            // might be necessary to keep around if there is no keyframe
            // hierarchy.

            // spkCurrGeom is either a TriMesh (single-material) or a Node
            // (multi-material) and because the transformation is stored
            // locally on each TriMesh in case there are non-uniform scales,
            // we need to explicitly handle this here.

            // set pivot points, if need be
            if ( spkCurrGeom->IsExactly(Node::TYPE) )
            {
                Node* pkNode = StaticCast<Node>(spkCurrGeom);
                for (int i = 0; i < pkNode->GetQuantity(); i++)
                {
                    SpatialPtr spkChild = pkNode->GetChild(i);
                    assert(spkChild->IsExactly(TriMesh::TYPE));
                    spkChild->Local.MakeIdentity();
                    if ( bPivot )
                        spkChild->Local.SetTranslate(-kPivot);
                }
            }
            else
            {
                spkCurrGeom->Local.MakeIdentity();
                if ( bPivot )
                    spkCurrGeom->Local.SetTranslate(-kPivot);
            }
        }
        else // non-leaf node
        {
            if ( bPivot )
            {
                // create another node to handle this pivot
                NodePtr spkPivotNode = new Node;
                spkPivotNode->Local.MakeIdentity();
                spkPivotNode->Local.SetTranslate(-kPivot);
                spkCNode->AttachChild(spkPivotNode);

                // Replace spkCNode in the node array with spkPivotNode.  This
                // way, any child nodes that attach to spkCNode will instead
                // attach to the pivot.
                rkHier[iObjID] = spkPivotNode;
            }
        }
    }
    
    spkCtrl = 0;

    m_kStream.SetAbsolutePos(iNextPos);
}
//----------------------------------------------------------------------------
void Loader3ds::ReadKeyfFlagChunk ()
{
    // Every single keyframe has a frame number, some flags, and then the
    // transform for that keyframe.  This function reads the flags.
    // Currently, what these flags do is entirely unknown, but this advances
    // the file pointer to the right place to read the rest of the chunk.

    int i = m_kStream.ReadUInt2();
    if ( i & 0x0001 )
        m_kStream.ReadInt4();
    if ( i & 0x0002 )
        m_kStream.ReadInt4();
    if ( i & 0x0004 )
        m_kStream.ReadInt4();
    if ( i & 0x0008 )
        m_kStream.ReadInt4();
    if ( i & 0x0010 )
        m_kStream.ReadInt4();
}
//----------------------------------------------------------------------------
TriMeshPtr Loader3ds::CloneTriMesh (const TriMesh* pkOrig)
{
    // Instance pkOrig.  Create all new meshes, but share the vertex, index,
    // and normal data.  Basically, a more intelligent Copy().  It takes as
    // input pkOrig (the mesh to be copied) and outputs the new TriMesh.

    // NOTE: If more states are read from the 3ds file then this function will
    // need to be changed.

    TriMeshPtr spkClone = new TriMesh(pkOrig->Vertices,pkOrig->Indices,false);
    spkClone->Local = pkOrig->Local;
    spkClone->Normals = pkOrig->Normals;

    Effect* pkEffect = pkOrig->GetEffect();
    if ( pkEffect )
        spkClone->SetEffect(pkEffect->Clone());

    GlobalState* pkState = pkOrig->GetGlobalState(GlobalState::CULL);
    if ( pkState )
        spkClone->SetGlobalState(pkState);
    pkState = pkOrig->GetGlobalState(GlobalState::MATERIAL);
    if ( pkState )
        spkClone->SetGlobalState(pkState);
    pkState = pkOrig->GetGlobalState(GlobalState::SHADE);
    if ( pkState )
        spkClone->SetGlobalState(pkState);

    return spkClone;
}
//----------------------------------------------------------------------------
void Loader3ds::ReadKeyfHierPosChunk (KeyframeControllerPtr& rspkCon,
    NodePtr spkPNode, NodePtr spkCNode, SpatialPtr spkCurrGeom)
{
    // This function reads all the position frames for a particular node
    // in the hierarchy.  It takes as input the current keyframe controller
    // (rspkCon, which may become instantiated here), the parent node
    // (spkPNode), the child/current node (spkCNode), and the current geometry
    // (spkCurrGeom).  If there are one or fewer keyframes, then the keyframe
    // controller will be skipped entirely and the current node's local
    // transform will be set to whatever it will rigidly be.

    // unknown flags
    m_kStream.ReadInt2();
    m_kStream.ReadInt4();
    m_kStream.ReadInt4();

    int iKeys = m_kStream.ReadInt4();
    if ( iKeys == 0 )
    {
        // No frames, although there are other objects with them.  currGeom
        // local has the world transformation for the geometry which will
        // eventually be expressed as PNode*CNode.  So, solve for CNode's
        // transform.
        Transformation kParent = spkPNode->World;
        kParent.Inverse(kParent);
        Transformation kChild;
        kChild.Product(kParent,spkCurrGeom->Local);

        // Only take the translate component in case there are keyframes for
        // other components
        spkCNode->Local.SetTranslate(kChild.GetTranslate());
    }
    else if ( m_bNoKeyf || iKeys == 1 )
    {
        // frame number
        m_kStream.ReadInt4();

        ReadKeyfFlagChunk();
        spkCNode->Local.SetTranslate(m_kStream.ReadVector3f());
    }
    else
    {
        // multiple keyframes, so set up controller
        if ( !rspkCon )
            rspkCon = new KeyframeController();

        float* pkTArray = new float[iKeys];
        Vector3f* pkPArray = new Vector3f[iKeys];
        for (int i = 0; i < iKeys; i++)
        {
            pkTArray[i] = (float)m_kStream.ReadInt4();                                  
            ReadKeyfFlagChunk();
            pkPArray[i] = m_kStream.ReadVector3f();
        }
        FloatArrayPtr spkTimes = new FloatArray(iKeys,pkTArray);
        Vector3fArrayPtr spkPos = new Vector3fArray(iKeys,pkPArray);
        rspkCon->TranslationTimes = spkTimes;
        rspkCon->TranslationData = spkPos;
    }
}
//----------------------------------------------------------------------------
void Loader3ds::ReadKeyfHierRotChunk (KeyframeControllerPtr& rspkCon,
    NodePtr spkPNode, NodePtr spkCNode, SpatialPtr spkCurrGeom)
{
    // This function reads all the rotation frames for a particular node
    // in the hierarchy.  It takes as input the current keyframe controller
    // (rspkCon, which may become instantiated here), the parent node
    // (spkPNode), the child/current node (spkCNode), and the current geometry
    // (spkCurrGeom).  If there are one or fewer keyframes, then the keyframe
    // controller will be skipped entirely and the current node's local
    // transform will be set to whatever it will rigidly be.

    // unknown flags
    m_kStream.ReadInt2();
    m_kStream.ReadInt4();
    m_kStream.ReadInt4();

    int iKeys = m_kStream.ReadInt4();
    if ( iKeys == 0 )
    {
        // No frames, although there are other objects with them.  currGeom
        // local has the world transformation for the geometry which will
        // eventually be expressed as PNode*CNode.  So, solve for CNode's
        // transform.
        Transformation kParent = spkPNode->World;
        kParent.Inverse(kParent);
        Transformation kChild;
        kChild.Product(kParent,spkCurrGeom->Local);

        // Only take the rotate component in case there are keyframes for
        // other components
        spkCNode->Local.SetRotate(kChild.GetRotate());
    }
    else if ( m_bNoKeyf || iKeys == 1 )
    {
        // frame number
        m_kStream.ReadInt4();

        ReadKeyfFlagChunk();
 
        float fA = m_kStream.ReadFloat();
        Matrix3f kRot;
        kRot.FromAxisAngle(m_kStream.ReadVector3f(),-fA);
        spkCNode->Local.SetRotate(kRot);

    }
    else
    {
        // multiple keyframes, so set up controller
        if ( !rspkCon )
            rspkCon = new KeyframeController();

        float* pkTArray = new float[iKeys];
        Quaternionf* pkQArray = new Quaternionf[iKeys];
        for (int i = 0; i < iKeys; i++)
        {
            pkTArray[i] = (float)m_kStream.ReadInt4();                                  
            ReadKeyfFlagChunk();
            float fA = m_kStream.ReadFloat();
            pkQArray[i] = Quaternionf(m_kStream.ReadVector3f(),-fA);

            // Rotations are relative to the prior rotation, so multiply
            // by previous to get actual local transform for this node
            // during this frame.
            if ( i > 0 )
                pkQArray[i] = pkQArray[i]*pkQArray[i-1];

            pkQArray[i].Normalize();
        }
        FloatArrayPtr spkTimes = new FloatArray(iKeys,pkTArray);
        QuaternionfArrayPtr spkRot = new QuaternionfArray(iKeys,pkQArray);
        rspkCon->RotationTimes = spkTimes;
        rspkCon->RotationData = spkRot;

        for (int j = 1; j < iKeys; j++)
        {
            if ( pkQArray[j].Dot(pkQArray[j-1]) < 0.0f )
                pkQArray[j] = -pkQArray[j];
        }
    }
}
//----------------------------------------------------------------------------
void Loader3ds::ReadKeyfHierSclChunk (KeyframeControllerPtr& rspkCon,
    NodePtr spkPNode, NodePtr spkCNode, SpatialPtr spkCurrGeom)
{
    // This function reads all the scale frames for a particular node
    // in the hierarchy.  It takes as input the current keyframe controller
    // (rspkCon, which may become instantiated here), the parent node
    // (spkPNode), the child/current node (spkCNode), and the current geometry
    // (spkCurrGeom).  If there are one or fewer keyframes, then the keyframe
    // controller will be skipped entirely and the current node's local
    // transform will be set to whatever it will rigidly be.
    // 
    // Non-uniform scales will be coerced into a uniform (average) scale, but
    // it will warn when it does this.

    // unknown flags
    m_kStream.ReadInt2();
    m_kStream.ReadInt4();
    m_kStream.ReadInt4();

    int iKeys = m_kStream.ReadInt4();
    if ( iKeys == 0 )
    {
        // No frames, although there are other objects with them.  currGeom
        // local has the world transformation for the geometry which will
        // eventually be expressed as PNode*CNode.  So, solve for CNode's
        // transform.

        Transformation kParent = spkPNode->World;
        kParent.Inverse(kParent);
        Transformation kChild;
        kChild.Product(kParent,spkCurrGeom->Local);

        // Only take the scale component in case there are keyframes for
        // other components
        Vector3f kScale = kChild.GetScale();
        if ( kChild.IsUniformScale() )
        {
            spkCNode->Local.SetUniformScale(kChild.GetUniformScale());
        }
        else
        {
            float fA = (kScale.X()+kScale.Y()+kScale.Z())/3.0f;

            // non-uniform scale warning
            const float TOL = 1e-06f;
            if ( (Mathf::FAbs(kScale.X()-kScale.Y()) > TOL) || 
                 (Mathf::FAbs(kScale.Z()-kScale.Y()) > TOL) ||
                 (Mathf::FAbs(kScale.X()-kScale.Z()) > TOL) )
            {
                Warn("Possible non-uniform scale (%f, %f,"\
                    " %f) being set to %f.\n",kScale.X(),kScale.Y(),
                    kScale.Z(),fA);
            }

            spkCNode->Local.SetUniformScale(fA);
        }
    }
    else if ( m_bNoKeyf || iKeys == 1 )
    {
        // frame number
        m_kStream.ReadInt4();

        ReadKeyfFlagChunk();
        float fX = m_kStream.ReadFloat();
        float fY = m_kStream.ReadFloat();
        float fZ = m_kStream.ReadFloat();
        if ( fX < 0.0f || fY < 0.0f || fZ < 0.0f )
            Warn("Inverted scale found (%f, %f, %f)\n",fX,fY,fZ);
        fX = Mathf::FAbs(fX);
        fY = Mathf::FAbs(fY);
        fZ = Mathf::FAbs(fZ);

        float fA = (fX+fY+fZ)/3.0f;

        // non-uniform scale warning
        const float TOL = 1e-06f;
        if ( (Mathf::FAbs(fX-fY) > TOL) || 
             (Mathf::FAbs(fZ-fY) > TOL) ||
             (Mathf::FAbs(fX-fZ) > TOL) )
        {
            Warn("Possible non-uniform scale (%f, %f,"\
                " %f) being set to %f.\n",fX,fY,fZ,fA);
        }
        spkCNode->Local.SetUniformScale(fA);
    }
    else
    {
        // multiple keyframes, so set up controller
        if ( !rspkCon )
            rspkCon = new KeyframeController();

        float* pkTArray = new float[iKeys];
        float* pkSArray = new float[iKeys];
        for (int i = 0; i < iKeys; i++)
        {
            pkTArray[i] = (float)m_kStream.ReadInt4();                                  
            ReadKeyfFlagChunk();
            float fX = m_kStream.ReadFloat();
            float fY = m_kStream.ReadFloat();
            float fZ = m_kStream.ReadFloat();
            if ( fX < 0.0f || fY < 0.0f || fZ < 0.0f )
                Warn("Inverted scale found (%f, %f, %f)\n",fX,fY,fZ);
            fX = Mathf::FAbs(fX);
            fY = Mathf::FAbs(fY);
            fZ = Mathf::FAbs(fZ);
            float fA = (fX+fY+fZ)/3.0f;

            // non-uniform scale warning
            const float TOL = 1e-06f;
            if ( (Mathf::FAbs(fX-fY) > TOL) || 
                 (Mathf::FAbs(fZ-fY) > TOL) ||
                 (Mathf::FAbs(fX-fZ) > TOL) )
            {
                Warn("Possible non-uniform scale (%f, %f,"\
                    " %f) being set to %f.\n",fX,fY,fZ,fA);
            }

            pkSArray[i] = fA;
        }
        FloatArrayPtr spkTimes = new FloatArray(iKeys,pkTArray);
        FloatArrayPtr spkScale = new FloatArray(iKeys,pkSArray);
        rspkCon->ScaleTimes = spkTimes;
        rspkCon->ScaleData = spkScale;
    }
}
//----------------------------------------------------------------------------
