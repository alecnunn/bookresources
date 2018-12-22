// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "LightWaveReader.h"
#include "ImageLoader.h"
#include "Wm3TextureEffect.h"

#define LWREADER_UNITIZE_MODEL 0
#define LWREADER_USE_FUZZ 1

const char LightWaveReader::ms_cTab = '\t';
const char LightWaveReader::ms_cDirSeparator = FS_DIR_SEPARATOR;
const string LightWaveReader::ms_kCurDirToken = FS_DIR_CURTOKEN;

//----------------------------------------------------------------------------
LightWaveReader::LightWaveReader (const char* acTheFileName)
{
    m_kLWOPathName = string(acTheFileName);
    Initialize();
}
//----------------------------------------------------------------------------
LightWaveReader::LightWaveReader (const string& rkTheFileName)
{
    m_kLWOPathName = rkTheFileName;
    Initialize();
}
//----------------------------------------------------------------------------
LightWaveReader::~LightWaveReader ()
{
    // Get rid of LightWave object and possibly parse output file.
    delete m_pkLWOFile;
    m_spkLWObject = NULL;
    if ( m_pkOut && m_pkOut != (ofstream*)&cout )
    {
        m_pkOut->close();
        delete m_pkOut;
    }

    // Destroy all auxiliary objects.
    int iSize = (int)m_akVertexToPolygon.size();
    for (int iIndex = 0; iIndex < iSize; ++iIndex)
    {
        PolygonList* pkList = m_akVertexToPolygon[iIndex];
        pkList->clear();
        delete pkList;
    }

    TagMap::iterator kTMIter = m_kTagMap.begin();
    for (/**/; kTMIter != m_kTagMap.end(); kTMIter++)
    {
        PolygonList* pkList = kTMIter->second;
        pkList->clear();
        delete pkList;
    }

    PolygonTagMap::iterator kPMIter = m_kPolygonMaps.begin();
    for (/**/; kPMIter != m_kPolygonMaps.end(); kPMIter++)
    {
        PolygonMap* pkMap = kPMIter->second;
        pkMap->clear();
        delete pkMap;
    }

    SurfaceVertexMap::iterator kSVMIter = m_kVertexMaps.begin();
    for (/**/; kSVMIter != m_kVertexMaps.end(); kSVMIter++)
    {
        VertexMap* pkMap = kSVMIter->second;
        pkMap->clear();
        delete pkMap;
    }

    VMAPArrayMap::iterator kVPMIter = m_kMaps.begin();
    for (/**/; kVPMIter != m_kMaps.end(); kVPMIter++)
    {
        VMAPArray* pkArray = kVPMIter->second;
        pkArray->clear();
        delete pkArray;
    }

    VMADArrayMap::iterator kVDMIter = m_kDiscontMaps.begin();
    for (/**/; kVDMIter != m_kDiscontMaps.end(); kVDMIter++)
    {
        VMADArray* pkArray = kVDMIter->second;
        pkArray->clear();
        delete pkArray;
    }

    // empty all containers
    m_akVertices.clear();
    m_kSurfaces.clear();
    m_kTagSurfaces.clear();
    m_kPolygonTags.clear();
    m_kPolygonLists.clear();
    m_kPolygonMaps.clear();
    m_kVertexMaps.clear();
    m_kAngles.clear();
    m_akPolygons.clear();
    m_akVertexToPolygon.clear();
    m_kMaps.clear();
    m_kDiscontMaps.clear();
    m_kMapVertices.clear();
    m_kMapNormals.clear();
    m_kMapTextures.clear();
    m_kMapConnectivity.clear();
}
//----------------------------------------------------------------------------
void LightWaveReader::Initialize ()
{
    m_pkLWOFile = NULL;
    m_spkLWObject = NULL;
    m_pkOut = NULL;
    m_pkCurrentMesh = NULL;
    m_pkCurrentTexture = NULL;
    m_pkCurrentPolyList = NULL;
    m_bSmoothed = false;
    m_eError = NO_LW_ERROR;
}
//----------------------------------------------------------------------------
bool LightWaveReader::InitReader (bool bDisplay)
{
    // create filename for LightWave object
    size_t iPos = m_kLWOPathName.find_last_of(ms_cDirSeparator,
        m_kLWOPathName.length());
    if ( iPos < m_kLWOPathName.length() )
    {
        m_kLWODirName = string(m_kLWOPathName,0,iPos) + ms_cDirSeparator;
        m_kLWOFileName = string(m_kLWOPathName,iPos+1,
            m_kLWOPathName.length());
    }
    else
    {
        m_kLWODirName = ms_kCurDirToken;
        m_kLWOFileName = m_kLWOPathName;
    }

    // If requested, create parse output file in the same directory as the
    // LightWave object and delete the parse output file if needed.
    // Otherwise, just use stdout as the default stream.
    m_bDisplayParsing = bDisplay;
    if ( m_bDisplayParsing )
    {
        string kOutFileName(m_kLWOFileName);
        kOutFileName += ".parse";
        string kOutPathName(m_kLWODirName + kOutFileName);

        m_pkOut = new ofstream;
        m_pkOut->open(kOutPathName.c_str());
        if ( (m_pkOut->rdstate() & ios::badbit) ||
             (m_pkOut->rdstate() & ios::failbit) )
        {
#ifdef unlink
            unlink(kOutPathName.c_str());
#endif
            delete m_pkOut;
            m_pkOut = new ofstream;
            m_pkOut->open(kOutPathName.c_str());
            if ( (m_pkOut->rdstate() & ios::badbit) ||
                 (m_pkOut->rdstate() & ios::failbit) )
            {
                cout << "Cannot create parsing result file: " <<
                    kOutPathName << endl;
                m_eError = BAD_PARSE_FILE;
                return false;
            }
        }
    }
    else
    {
        m_pkOut = (ofstream*)&cout;
    }

    // open the LightWave object
    m_pkLWOFile = new MemFile(m_kLWOPathName);
    if ( !m_pkLWOFile->FileLoaded() )
    {
        *m_pkOut << "LightWaveReader(" << this << ")::InitReader cannot open "
            << m_kLWOPathName << endl;
        m_eError = BAD_OBJECT_FILE;
        return false;
    }
    return true;
}
//----------------------------------------------------------------------------
bool LightWaveReader::Parse (bool bSmooth)
{
    // parse LWO2 file headers: IFF marker, file size, LightWave signature
    UInt32 uiForm = m_pkLWOFile->ReadUInt32();
    if ( uiForm != 'FORM' )
    {
        *m_pkOut << "Missing FORM chunk!" << endl;
        m_eError = MISSING_FORM_CHUNK;
        return false;
    }

    UInt32 uiFileSize = m_pkLWOFile->ReadUInt32();
    if ( m_bDisplayParsing )
        *m_pkOut << m_kLWOFileName << ":" << endl;

    UInt32 uiLWO2 = m_pkLWOFile->ReadUInt32();
    if ( uiLWO2 != 'LWO2' )
    {
        *m_pkOut << "Missing LWO2 chunk type!" << endl;
        m_eError = MISSING_LWO2_CHUNK;
        return false;
    }

    // parse LWO2 file
    while (ReadChunk(uiFileSize))
    {
        if (!m_pkLWOFile->DataAvailable() || m_eError != NO_LW_ERROR)
            break;
    }

    // construct object
    m_bSmoothed = bSmooth;
    if ( BuildObject() )
    {
        if ( m_bDisplayParsing )
            ObjectStats();
    }            

    return m_eError == NO_LW_ERROR;
}
//----------------------------------------------------------------------------
Node* LightWaveReader::BuildObject ()
{
    // create a Node and add default states to it
    m_spkLWObject = new Node((int)m_kSurfaces.size());
    m_spkLWObject->SetName(m_kLWOFileName.c_str());

    // NOLAN: Legacy state from viewer code
    /*
    ZBufferStatePtr pkZBufferState = new ZBufferState;
    pkZBufferState->Enabled() = true;
    pkZBufferState->Writeable() = true;
    pkZBufferState->Compare() = ZBufferState::CF_LESS;
    m_spkLWObject->SetRenderState(pkZBufferState);

    ShadeStatePtr pkShadeState = new ShadeState;
    pkShadeState->Shade() = ShadeState::SM_SMOOTH;
    m_spkLWObject->SetRenderState(pkShadeState);

    Light* pkLight = new Light(Light::LT_DIRECTIONAL);
    pkLight->Diffuse = ColorRGBA::WHITE;
    pkLight->SetDirection(Vector3f::UNIT_X);
    pkLight->On = true;
    m_spkLWObject->AttachChild(pkLight);
    */

    // add all surfaces as trimesh children
    SurfaceMap::iterator kSIter = m_kSurfaces.begin();
    for (/**/; kSIter != m_kSurfaces.end(); kSIter++)
    {
        TriMesh* pkMesh = kSIter->second;
        m_spkLWObject->AttachChild(pkMesh);
        pkMesh->UpdateGS();
    }

    // compute normals (if requested) before returning the object
    ComputeNormals();
    return GetObject();
}
//----------------------------------------------------------------------------
void LightWaveReader::ObjectStats ()
{
    // Display simplified stats summary for object.

    *m_pkOut << endl << "STATISTICS for " << m_spkLWObject->GetName() << endl
        << endl;

    *m_pkOut << "Number of trimeshes: " << (int)m_kSurfaces.size() << endl;
    *m_pkOut << "Number of vertices : " << (int)m_akVertices.size() << endl;
    *m_pkOut << "Number of triangles: " << (int)m_akPolygons.size() << endl;
    *m_pkOut << "Number of textures : " << (int)m_akTextures.size() << endl;
    *m_pkOut << "Number of VMAPs    : " << (int)m_kMaps.size() << endl;
    *m_pkOut << "Number of VMADs    : " << (int)m_kDiscontMaps.size() << endl;

    if ( m_kSurfaces.size() )
    {
        *m_pkOut << "Trimeshes: " << endl;
        SurfaceMap::iterator kSIter = m_kSurfaces.begin();
        for (/**/; kSIter != m_kSurfaces.end(); kSIter++)
        {
            string kName = kSIter->first;
            TriMesh* pkMesh = kSIter->second;
            *m_pkOut << ms_cTab << kName << endl;
            *m_pkOut << ms_cTab << ms_cTab << "Triangles: "
                << pkMesh->GetTriangleQuantity() << " (total vertices: "
                << pkMesh->GetTriangleQuantity() * 3 << ")" << endl;
            *m_pkOut << ms_cTab << ms_cTab << "Vertices: "
                << pkMesh->Vertices->GetQuantity() << endl;
            *m_pkOut << ms_cTab << ms_cTab << "LW vertices: "
                << (int)m_kVertexMaps[kName]->size() << endl;

            *m_pkOut << ms_cTab << ms_cTab << "Textured: "
                << (pkMesh->GetEffect() && 
                pkMesh->GetEffect()->Textures.GetQuantity() > 0 ? "yes" : 
                "no") << endl;
        }
    }

    if ( m_kMaps.size() )
    {
        *m_pkOut << "UV maps: " << endl;
        VMAPArrayMap::iterator kVPMIter = m_kMaps.begin();
        for (/**/; kVPMIter != m_kMaps.end(); kVPMIter++)
        {
            string kName = kVPMIter->first;
            VMAPArray* pkVMAP = kVPMIter->second;
            *m_pkOut << ms_cTab << kName << endl;
            *m_pkOut << ms_cTab << ms_cTab << "Elements: "
                << (int)pkVMAP->size() << endl;
        }
    }

    if ( m_kDiscontMaps.size() )
    {
        *m_pkOut << "Discontinuous UV maps: " << endl;
        VMADArrayMap::iterator kVDMIter = m_kDiscontMaps.begin();
        for (/**/;  kVDMIter != m_kDiscontMaps.end(); kVDMIter++)
        {
            string kName = kVDMIter->first;
            VMADArray* pkVMAD = kVDMIter->second;
            *m_pkOut << ms_cTab << kName << endl;
            *m_pkOut << ms_cTab << ms_cTab << "Elements: "
                << (int)pkVMAD->size() << endl;
        }
    }
}
//----------------------------------------------------------------------------
UInt32 LightWaveReader::ReadChunk (UInt32 uiFileSize)
{
    // Ensure enough data available to read chunk type and size.
    long lFilePos = m_pkLWOFile->FilePos();
    if ( lFilePos - uiFileSize <= 8 )
        return 0;

    UInt32 uiChunk = m_pkLWOFile->ReadUInt32();
    UInt32 uiChunkSize = m_pkLWOFile->ReadUInt32();
    if ( m_bDisplayParsing )
    {
        PrintChunkType(uiChunk);
        *m_pkOut << " size = " << uiChunkSize << endl;
    }
    if ( uiChunkSize & 1 )
        ++uiChunkSize;

    // Parse chunk or skip it if bad/unknown.  Return either number of bytes
    // in chunk or zero if end of file.
    bool bParsed = ParseChunk(uiChunk,uiChunkSize);
    if ( !bParsed )
        m_pkLWOFile->Advance(uiChunkSize);

    return m_pkLWOFile->DataAvailable() ? uiChunkSize : 0;
}
//----------------------------------------------------------------------------
bool LightWaveReader::ParseChunk (UInt32 uiChunk, UInt32& ruiChunkSize)
{
    switch ( uiChunk )
    {
        case 'TAGS':  return ParseTAGS(ruiChunkSize);
        case 'LAYR':  return ParseLAYR(ruiChunkSize);
        case 'PNTS':  return ParsePNTS(ruiChunkSize);
        case 'BBOX':  return ParseBBOX(ruiChunkSize);
        case 'VMAP':  return ParseVMAP(ruiChunkSize);
        case 'VMAD':  return ParseVMAD(ruiChunkSize);
        case 'POLS':  return ParsePOLS(ruiChunkSize);
        case 'PTAG':  return ParsePTAG(ruiChunkSize);
        case 'CLIP':  return ParseCLIP(ruiChunkSize);
        case 'SURF':  return ParseSURF(ruiChunkSize);
    }
    return false;
}
//----------------------------------------------------------------------------
bool LightWaveReader::ParseTAGS (UInt32 uiChunkSize)
{
    // Tags are surface names.  Each surface will correspond to a triangle
    // mesh and associated auxiliary objects.

    int iLength = 0;
    int iCurrentTag = 0;
    while (iLength < (int)uiChunkSize && m_pkLWOFile->DataAvailable())
    {
        int iBytes;
        string kTag = m_pkLWOFile->ReadEvenString(&iBytes);
        iLength += iBytes;

        TriMesh* pkMesh = new TriMesh(NULL,NULL,false,false);
        PolygonList* pkPolygonList = new PolygonList;
        VertexMap* pkVertexMap = new VertexMap;

        m_kSurfaces[kTag] = pkMesh;
        m_kVertexMaps[kTag] = pkVertexMap;
        m_kPolygonLists[kTag] = pkPolygonList;
        m_kTagSurfaces[iCurrentTag] = pkMesh;
        m_kTagMap[iCurrentTag] = pkPolygonList;

        if ( m_bDisplayParsing )
            *m_pkOut << ms_cTab << kTag << endl;
        iCurrentTag++;
    }
    return true;
}
//----------------------------------------------------------------------------
bool LightWaveReader::ParseLAYR (UInt32 uiChunkSize)
{
    // There might be multiple layers per file, each containing its own
    // object, but we only support the first object in the first layer.  We
    // parse the chunk anyway to report/prevent errors.

    const int iMinSize = (3*sizeof(UInt16)) + sizeof(Vector3f);
    if ( (int)uiChunkSize < iMinSize )
    {
        if ( m_bDisplayParsing )
            *m_pkOut << ms_cTab << "not enough bytes in LAYR chunk!" << endl;
        m_eError = BAD_LAYR_CHUNK;
        return false;
    }

    UInt16 usLayerNumber = m_pkLWOFile->ReadUInt16();
    UInt16 usLayerFlags = m_pkLWOFile->ReadUInt16();
    Vector3f kLayerPivot = m_pkLWOFile->ReadVector();
    UInt16 usSubSize = sizeof(usLayerNumber) + sizeof(usLayerFlags) +
        sizeof(kLayerPivot);

    int iBytes = 0;
    string kLayerName;
    if ( (int)uiChunkSize > iMinSize )
        kLayerName = m_pkLWOFile->ReadEvenString(&iBytes);
    else
        kLayerName = "(unnamed)";

    UInt16 usLayerParent = 0;
    usSubSize += (UInt16)iBytes;
    if ( usSubSize < (UInt16)uiChunkSize )
        usLayerParent = m_pkLWOFile->ReadUInt16();

    if ( m_bDisplayParsing )
    {
        *m_pkOut << ms_cTab << "number = " << usLayerNumber << endl;
        *m_pkOut << ms_cTab << "flags  = "
            << (usLayerFlags & 1 ? "hidden" : "normal") << endl;
        *m_pkOut << ms_cTab << "pivot  = " << kLayerPivot << endl;
        *m_pkOut << ms_cTab << "name   = " << kLayerName << endl;
        *m_pkOut << ms_cTab << "parent = " << usLayerParent << endl;
    }
    return true;
}
//----------------------------------------------------------------------------
bool LightWaveReader::ParsePNTS (UInt32 uiChunkSize)
{
    // Raw geometric data (e.g. vertices) are represented by points.  Ensure
    // there is enough data in the chunk while reading it.

    UInt32 uiNumPoints = uiChunkSize / sizeof(Vector3f);
    if ( m_bDisplayParsing )
        *m_pkOut << ms_cTab << "number of points = " << uiNumPoints << endl;

    for (int iIndex = 0; iIndex < (int)uiNumPoints; iIndex++)
    {
        if ( !m_pkLWOFile->DataAvailable() )
        {
            m_eError = BAD_PNTS_CHUNK;
            return false;
        }

        Vector3f kPoint(m_pkLWOFile->ReadVector());
        m_akVertices.push_back(kPoint);
        PolygonList* pkPolygonList = new PolygonList;
        m_akVertexToPolygon.push_back(pkPolygonList);

        if ( m_bDisplayParsing )
            *m_pkOut << ms_cTab << kPoint << endl;
    }
    return true;
}
//----------------------------------------------------------------------------
bool LightWaveReader::ParseBBOX (UInt32 uiChunkSize)
{
    // Object bounding box.  This will usually be present, and it is needed if
    // we wish to unitize the model before using it.

    if ( uiChunkSize < 2*sizeof(Vector3f) )
    {
        if ( m_bDisplayParsing )
            *m_pkOut << ms_cTab << "not enough bytes in BBOX chunk!" << endl;
        m_eError = BAD_BBOX_CHUNK;
        return false;
    }

    m_kMinPoint = m_pkLWOFile->ReadVector();
    m_kMaxPoint = m_pkLWOFile->ReadVector();
    if ( m_bDisplayParsing )
    {
        *m_pkOut << ms_cTab << "min = " << m_kMinPoint << endl;
        *m_pkOut << ms_cTab << "max = " << m_kMaxPoint << endl;
    }

#if LWREADER_UNITIZE_MODEL
    // compute required dimensions
    float fW = Math::FAbs(m_kMinPoint.x) + Math::FAbs(m_kMaxPoint.x);
    float fH = Math::FAbs(m_kMinPoint.y) + Math::FAbs(m_kMaxPoint.y);
    float fD = Math::FAbs(m_kMinPoint.z) + Math::FAbs(m_kMaxPoint.z);

    // compute rescaling factor and scale all vertices
    float fScale = 2.0f/max(max(fW,fH),fD);    
    for (int iIndex = 0; iIndex < m_akVertices.size(); iIndex++)
        m_akVertices[iIndex] *= fScale;

#endif

    return true;
}
//----------------------------------------------------------------------------
bool LightWaveReader::ParseVMAP (UInt32& ruiChunkSize)
{
    // Vertex map: vertex and texture coords.  Ensure the chunk is properly
    // structured.

    UInt32 uiChunkType = m_pkLWOFile->ReadUInt32();
    UInt16 usDim = m_pkLWOFile->ReadUInt16();
    UInt32 uiSubSize = sizeof(uiChunkType) + sizeof(usDim);

    if ( uiChunkType != 'TXUV' || usDim != 2 )
    {
        if ( m_bDisplayParsing )
        {
            *m_pkOut << ms_cTab << "only 2D TXUV subchunks are supported!"
                << endl;
        }
        m_pkLWOFile->BackUp(uiSubSize);
        return false;
    }

    int iBytes;
    string kName = m_pkLWOFile->ReadEvenString(&iBytes);
    uiSubSize += iBytes;

    if ( m_bDisplayParsing )
    {
        *m_pkOut << ms_cTab;
        PrintChunkType(uiChunkType);
        *m_pkOut << endl;
        *m_pkOut << ms_cTab << ms_cTab << "dimension = " << usDim << endl;
        *m_pkOut << ms_cTab << ms_cTab << "name      = " << kName << endl;
    }

    VMAPArray* pkMap = new VMAPArray;
    while ( uiSubSize < ruiChunkSize && m_pkLWOFile->DataAvailable() )
    {
        UInt16 usVertex = m_pkLWOFile->ReadUInt16();
        float fCoordU = m_pkLWOFile->ReadFloat();
        float fCoordV = m_pkLWOFile->ReadFloat();
        if ( m_bDisplayParsing )
        {
            *m_pkOut << ms_cTab << ms_cTab << "vertex = " << usVertex
                << " U = " << fCoordU << " V = " << fCoordV << endl;
        }

        pkMap->push_back(VMAP(usVertex,fCoordU,fCoordV));
        uiSubSize += sizeof(usVertex) + sizeof(fCoordU) + sizeof(fCoordV);
    }
    m_kMaps[kName] = pkMap;

    return true;
}
//----------------------------------------------------------------------------
bool LightWaveReader::ParseVMAD (UInt32& ruiChunkSize)
{
    // Discontinous vertex map: vertex, polygon and texture coords.  Ensure
    // the chunk is properly structured.

    UInt32 uiChunkType = m_pkLWOFile->ReadUInt32();
    UInt16 usDim = m_pkLWOFile->ReadUInt16();
    UInt32 uiSubSize = sizeof(uiChunkType) + sizeof(usDim);

    if ( uiChunkType != 'TXUV' || usDim != 2 )
    {
        if ( m_bDisplayParsing )
        {
            *m_pkOut << ms_cTab << "only 2D TXUV subchunks are supported!"
                << endl;
        }
        m_pkLWOFile->BackUp(uiSubSize);
        return false;
    }

    int iBytes;
    string kName = m_pkLWOFile->ReadEvenString(&iBytes);
    uiSubSize += iBytes;

    if ( m_bDisplayParsing )
    {
        *m_pkOut << ms_cTab;
        PrintChunkType(uiChunkType);
        *m_pkOut << endl;
        *m_pkOut << ms_cTab << ms_cTab << "dimension = " << usDim << endl;
        *m_pkOut << ms_cTab << ms_cTab << "name      = " << kName << endl;
    }

    VMADArray* pkMap = new VMADArray;
    while ( uiSubSize < ruiChunkSize && m_pkLWOFile->DataAvailable() )
    {
        UInt16 usVertex = m_pkLWOFile->ReadUInt16();
        UInt16 usPolygon = m_pkLWOFile->ReadUInt16();
        float fCoordU = m_pkLWOFile->ReadFloat();
        float fCoordV = m_pkLWOFile->ReadFloat();
        if ( m_bDisplayParsing )
        {
            *m_pkOut << ms_cTab << ms_cTab << "vertex = " << usVertex
                << " polygon = " << usPolygon << " U = " << fCoordU
                << " V = " << fCoordV << endl;
        }

        pkMap->push_back(VMAD(usVertex,usPolygon,fCoordU,fCoordV));
        uiSubSize += sizeof(usVertex) + sizeof(usPolygon) + sizeof(fCoordU) +
            sizeof(fCoordV);
    }
    m_kDiscontMaps[kName] = pkMap;

    return true;
}
//----------------------------------------------------------------------------
bool LightWaveReader::ParsePOLS (UInt32 uiChunkSize)
{
    // Polygon (that is, triangles) description.  Insist on standard polygons
    // (no funky LightWave 3D entities).

    UInt32 uiChunkType = m_pkLWOFile->ReadUInt32();
    if ( uiChunkType != 'FACE' )
    {
        if ( m_bDisplayParsing )
        {
            *m_pkOut << ms_cTab << "unsupported polygon type: ";
            PrintChunkType(uiChunkType);
            *m_pkOut << endl;
        }
        m_pkLWOFile->BackUp(sizeof(uiChunkType));
        return false;
    }

    if ( m_bDisplayParsing )
    {
        *m_pkOut << ms_cTab;
        PrintChunkType(uiChunkType);
        *m_pkOut << endl;
    }

    // We only support triangles with 2-byte indices.  This imposes a limit of
    // 64K triangles per object.
    int iPolygonIndex = 0;
    UInt32 uiTotalBytes = sizeof(uiChunkType);
    while ( uiTotalBytes < uiChunkSize && m_pkLWOFile->DataAvailable() )
    {
        UInt16 usVertexCount = m_pkLWOFile->ReadUInt16();
        uiTotalBytes += sizeof(usVertexCount);
        UInt16 usFlags = (0xFC00 & usVertexCount) >> 10;
        usVertexCount &= 0x03FF;

        if ( usVertexCount == 3 )
        {
            // For each vertex in the triangle, we compute its plane normal by
            // extracting its vertex data, contained in the previous chunk.
            UInt16 usFirst = m_pkLWOFile->ReadUInt16();
            UInt16 usSecond = m_pkLWOFile->ReadUInt16();
            UInt16 usThird = m_pkLWOFile->ReadUInt16();
            uiTotalBytes += sizeof(usFirst) + sizeof(usSecond) +
                sizeof(usThird);

            Vector3f kNormal = ComputePlaneNormal(m_akVertices[usFirst],
                m_akVertices[usSecond],m_akVertices[usThird]);
            Triangle kTriangle(usFirst,usSecond,usThird,kNormal);
            m_akPolygons.push_back(kTriangle);
            if ( m_bDisplayParsing )
            {
                *m_pkOut << ms_cTab << ms_cTab << "triangle(" << usFirst
                    << ", " << usSecond << ", " << usThird << ")" << endl;
            }

            // We keep a list of polygons per vertex, which helps us process
            // model data while computing LightWave normals.
            PolygonList* pkPolygonList = m_akVertexToPolygon[usFirst];
            pkPolygonList->push_back(iPolygonIndex);
            pkPolygonList = m_akVertexToPolygon[usSecond];
            pkPolygonList->push_back(iPolygonIndex);
            pkPolygonList = m_akVertexToPolygon[usThird];
            pkPolygonList->push_back(iPolygonIndex);
        }
        else
        {
            if ( m_bDisplayParsing )
            {
                *m_pkOut << ms_cTab << ms_cTab << usVertexCount
                    << "-sided polygon: ";
                *m_pkOut << "only triangles are supported!" << endl;
            }
            m_eError = UNSUPPORTED_POLYGON_TYPE;
            return false;
        }

        iPolygonIndex++;
    }

    return true;
}
//----------------------------------------------------------------------------
bool LightWaveReader::ParsePTAG (UInt32 uiChunkSize)
{
    // Polygon tags associate polygons (that is, triangles) to surfaces.
    // Trimesh creation collects all triangles for a given tag.
    UInt32 uiChunkType = m_pkLWOFile->ReadUInt32();
    if ( m_bDisplayParsing) 
    {
        *m_pkOut << ms_cTab;
        PrintChunkType(uiChunkType);
        *m_pkOut << endl;
    }

    if ( uiChunkType != 'SURF' )
    {
        if ( m_bDisplayParsing )
            *m_pkOut << ms_cTab << "we only support SURF subchunks!" << endl;
        m_pkLWOFile->BackUp(sizeof(uiChunkType));
        return false;
    }

    UInt32 uiTotalBytes = sizeof(uiChunkType);
    while ( uiTotalBytes < uiChunkSize && m_pkLWOFile->DataAvailable() )
    {
        UInt16 usPolygonIndex = m_pkLWOFile->ReadUInt16();
        UInt16 usSurfaceTag = m_pkLWOFile->ReadUInt16();
        uiTotalBytes += sizeof(usPolygonIndex) + sizeof(usSurfaceTag);

        PolygonList* pkPolyList = m_kTagMap[usSurfaceTag];
        pkPolyList->push_back(usPolygonIndex);
        m_kPolygonTags[usPolygonIndex] = usSurfaceTag;
        if ( m_bDisplayParsing )
        {
            *m_pkOut << ms_cTab << ms_cTab << "polygon = " << usPolygonIndex
                << " tag = " << usSurfaceTag << endl;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
bool LightWaveReader::ParseCLIP (UInt32& ruiChunkSize)
{
    // Clips are textures.  Ensure the chunk is properly formed and represents
    // a still image.
    UInt32 uiIndex = m_pkLWOFile->ReadUInt32();
    UInt32 uiTotalBytes = sizeof(uiIndex);

    UInt32 uiChunkType = m_pkLWOFile->ReadUInt32();
    if ( m_bDisplayParsing )
    {
        *m_pkOut << ms_cTab << "index = " << uiIndex << endl;
        *m_pkOut << ms_cTab;
        PrintChunkType(uiChunkType);
    }
    uiTotalBytes += sizeof(uiChunkType);

    // We only support still images: no animated textures yet.
    if ( uiChunkType != 'STIL' )
    {
        if ( m_bDisplayParsing )
        {
            *m_pkOut << endl << ms_cTab << "unsupported clip type: ";
            PrintChunkType(uiChunkType);
            *m_pkOut << endl;
        }
        m_pkLWOFile->BackUp(uiTotalBytes);
        return false;
    }

    // read subchunk size
    UInt16 usSubSize = m_pkLWOFile->ReadUInt16();
    uiTotalBytes += sizeof(usSubSize);

    // read image name
    int iBytes;
    string kFileName = m_pkLWOFile->ReadEvenString(&iBytes);
    if ( m_bDisplayParsing )
    {
        *m_pkOut << " size = " << usSubSize << endl;
        *m_pkOut << ms_cTab << ms_cTab << kFileName << endl;
    }
    uiTotalBytes += iBytes;

    if ( uiTotalBytes < ruiChunkSize )
    {
        if ( m_bDisplayParsing )
            *m_pkOut << ms_cTab << "incomplete CLIP chunk!" << endl;
        m_pkLWOFile->BackUp(uiTotalBytes);
        return false;
    }    

    // Load texture and add it to array so that other surfaces can use it
    // without having to duplicate its data.
    bool bTextureLoaded = AddTexture(kFileName,m_kLWODirName);
    if ( !bTextureLoaded )
        m_eError = UNSUPPORTED_TEXTURE_TYPE;

    return bTextureLoaded;
}
//----------------------------------------------------------------------------
bool LightWaveReader::ParseSURF (UInt32& ruiChunkSize)
{
    // Surface description.  We only support a subset of the possible chunks.
    int iBytes;
    string kName = m_pkLWOFile->ReadEvenString(&iBytes);
    UInt32 uiTotalBytes = (UInt32)iBytes;
    string kSource = m_pkLWOFile->ReadEvenString(&iBytes);
    uiTotalBytes += (UInt32)iBytes;

    if ( m_bDisplayParsing )
    {
        *m_pkOut << ms_cTab << "name   = " << kName << endl;
        *m_pkOut << ms_cTab << "source = " << kSource << endl;
    }

    // Set up current surface as default and build preliminary trimesh
    // representation.
    m_kCurrentSurface = kName;
    m_kCurrentColor = ColorRGBA::WHITE;
    m_pkCurrentMesh = m_kSurfaces[kName];
    m_pkCurrentPolyList = m_kPolygonLists[kName];
    m_pkCurrentMesh->SetName(m_kCurrentSurface.c_str());
    BuildTriMesh();

    // scan all known surface subchunks
    int iIndent = 2;
    m_iNumBLOKs = 0;

    while ( uiTotalBytes < ruiChunkSize && m_pkLWOFile->DataAvailable() )
    {
        UInt16 usSubSize = 0;
        bool bSuccess = ParseSURFSubchunk(usSubSize,iIndent);
        uiTotalBytes += (UInt32)usSubSize;
        if ( !bSuccess )
        {
            m_pkLWOFile->BackUp(uiTotalBytes);
            return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool LightWaveReader::ParseSURFSubchunk (UInt16& rusSubSize, int iIndent)
{
    // Parse and possibly represent all relevant surface subchunks.  Note that
    // some of them are not feasible to represent with WildMagic as of version
    // 1.2.
    
    UInt32 uiSubchunk = m_pkLWOFile->ReadUInt32();
    rusSubSize += sizeof(uiSubchunk);
    UInt16 usCurSize = m_pkLWOFile->ReadUInt16();
    rusSubSize += sizeof(usCurSize);

    if ( m_bDisplayParsing )
        PrintChunkInfo(uiSubchunk,usCurSize,iIndent);

    bool bSupported = true;
    switch ( uiSubchunk )
    {
        case 'COLR':
        {
            // Basic surface color.  This will be reset for textured surfaces.
            // For plain surfaces, it corresponds to the ambient color.
            rusSubSize += usCurSize;
            float fRed = m_pkLWOFile->ReadFloat();
            float fGreen = m_pkLWOFile->ReadFloat();
            float fBlue = m_pkLWOFile->ReadFloat();
            UInt16 usEnvelope = m_pkLWOFile->ReadUInt16();

            m_kCurrentColor = ColorRGBA(fRed,fGreen,fBlue,1.0f);
            MaterialStatePtr pkMaterialState = new MaterialState;
            pkMaterialState->Ambient = m_kCurrentColor;
            m_pkCurrentMesh->SetGlobalState(pkMaterialState);

            if ( m_bDisplayParsing )
            {
                PrintIndent(iIndent+1);
                *m_pkOut << "(r:" << fRed << ", g:" << fGreen << ", b:"
                    << fBlue << ")";
                if ( usEnvelope )
                    *m_pkOut << ", envelope = " << usEnvelope;
                *m_pkOut << endl;
            }
        }
        break;

        case 'DIFF':
        {
            // Diffuse reflection component.
            rusSubSize += usCurSize;
            float fDiffuse = m_pkLWOFile->ReadFloat();
            UInt16 usEnvelope = m_pkLWOFile->ReadUInt16();

            ColorRGBA kColor(m_kCurrentColor);
            kColor = fDiffuse*kColor;
            GlobalState* pkGlobalState =
                m_pkCurrentMesh->GetGlobalState(GlobalState::MATERIAL);
            MaterialState* pkMaterialState = 
                DynamicCast<MaterialState>(pkGlobalState);
            pkMaterialState->Diffuse = kColor;

            if ( m_bDisplayParsing )
            {
                PrintIndent(iIndent+1);
                *m_pkOut << "diffuse = " << 100.0f*fDiffuse << "%";
                if ( usEnvelope )
                    *m_pkOut << ", envelope = " << usEnvelope;
                *m_pkOut << endl;
            }
        }
        break;

        case 'LUMI':
        {
            // Surface luminance. Currently unsupported.
            rusSubSize += usCurSize;
            float fLuminance = m_pkLWOFile->ReadFloat();
            UInt16 usEnvelope = m_pkLWOFile->ReadUInt16();

            // TODO. How to add luminance using a RenderState?
            if ( m_bDisplayParsing )
            {
                PrintIndent(iIndent+1);
                *m_pkOut << "luminance = " << 100.0f*fLuminance << "%";
                if ( usEnvelope )
                    *m_pkOut << ", envelope = " << usEnvelope;
                *m_pkOut << endl;
            }
        }
        break;

        case 'SPEC':
        {
            // Specular reflection component.
            rusSubSize += usCurSize;
            float fSpecular = m_pkLWOFile->ReadFloat();
            UInt16 usEnvelope = m_pkLWOFile->ReadUInt16();

            ColorRGBA kColor(m_kCurrentColor);
            kColor = fSpecular*kColor;
            GlobalState* pkGlobalState =
                m_pkCurrentMesh->GetGlobalState(GlobalState::MATERIAL);
            MaterialState* pkMaterialState = 
                DynamicCast<MaterialState>(pkGlobalState);
            pkMaterialState->Specular = kColor;

            if ( m_bDisplayParsing )
            {
                PrintIndent(iIndent+1);
                *m_pkOut << "specular = " << 100.0f*fSpecular << "%";
                if ( usEnvelope )
                    *m_pkOut << ", envelope = " << usEnvelope;
                *m_pkOut << endl;
            }
        }
        break;

        case 'GLOS':
        {
            // Surface glossiness. Currently unsupported.
            rusSubSize += usCurSize;
            float fGlossiness = m_pkLWOFile->ReadFloat();
            UInt16 usEnvelope = m_pkLWOFile->ReadUInt16();

            // TODO. How to add glossiness using a RenderState?
            
            if ( m_bDisplayParsing )
            {
                PrintIndent(iIndent+1);
                *m_pkOut << "glossiness = " << 100.0f*fGlossiness << "%";
                if ( usEnvelope )
                    *m_pkOut << ", envelope = " << usEnvelope;
                *m_pkOut << endl;
            }
        }
        break;

        case 'REFL':
        {
            // Reflection component. Currently unsupported.
            rusSubSize += usCurSize;
            float fReflection = m_pkLWOFile->ReadFloat();
            UInt16 usEnvelope = m_pkLWOFile->ReadUInt16();

            // TODO. How to add reflection using a RenderState?

            if ( m_bDisplayParsing )
            {
                PrintIndent(iIndent+1);
                *m_pkOut << "reflection = " << 100.0f*fReflection << "%";
                if ( usEnvelope )
                    *m_pkOut << ", envelope = " << usEnvelope;
                *m_pkOut << endl;
            }
        }
        break;

        case 'CLRH':
        {
            // Specular highlight component.
            rusSubSize += usCurSize;
            float fHighlights = m_pkLWOFile->ReadFloat();
            UInt16 usEnvelope = m_pkLWOFile->ReadUInt16();

            GlobalState* pkGlobalState =
                m_pkCurrentMesh->GetGlobalState(GlobalState::MATERIAL);
            MaterialState* pkMaterialState = 
                DynamicCast<MaterialState>(pkGlobalState);
            pkMaterialState->Shininess = 128.0f*fHighlights;

            if ( m_bDisplayParsing )
            {
                PrintIndent(iIndent+1);
                *m_pkOut << "color highlights = " << 100.0f*fHighlights
                    << "%";
                if ( usEnvelope )
                    *m_pkOut << ", envelope = " << usEnvelope;
                *m_pkOut << endl;
            }
        }
        break;

        case 'TRAN':
        {
            // Surface transparency. Currently unsupported.
            rusSubSize += usCurSize;
            float fTransparency = m_pkLWOFile->ReadFloat();
            UInt16 usEnvelope = m_pkLWOFile->ReadUInt16();

            // TODO. How to add transparency using a RenderState?

            if ( m_bDisplayParsing )
            {
                PrintIndent(iIndent+1);
                *m_pkOut << "transparency = " << 100.0f*fTransparency << "%";
                if ( usEnvelope )
                    *m_pkOut << ", envelope = " << usEnvelope;
                *m_pkOut << endl;
            }
        }
        break;

        case 'TRNL':
        {
            // Surface translucency. Currently unsupported.
            rusSubSize += usCurSize;
            float fTranslucency = m_pkLWOFile->ReadFloat();
            UInt16 usEnvelope = m_pkLWOFile->ReadUInt16();

            // TODO. How to add translucency using a RenderState?

            if ( m_bDisplayParsing )
            {
                PrintIndent(iIndent+1);
                *m_pkOut << "translucency = " << 100.0f*fTranslucency << "%";
                if ( usEnvelope )
                    *m_pkOut << ", envelope = " << usEnvelope;
                *m_pkOut << endl;
            }
        }
        break;

        case 'SMAN':
        {
            // Smoothing angle for this surface.  Used in normals computation.
            rusSubSize += usCurSize;
            float fSmoothingAngle = m_pkLWOFile->ReadFloat();
            m_kAngles[m_kCurrentSurface] = fSmoothingAngle;

            if ( m_bDisplayParsing )
            {
                PrintIndent(iIndent+1);
                *m_pkOut << "angle = " << Mathf::RAD_TO_DEG*fSmoothingAngle
                    << endl;
            }
        }
        break;

        case 'BLOK':
        {
            // Block description of current surface.
            if ( m_iNumBLOKs < 1 )
            {
                UInt16 usSizeBLOKSubchunk = ParseBLOKSubchunk(iIndent+1,
                    bSupported);
                if ( !bSupported )
                {
                    m_pkLWOFile->Advance(usCurSize - rusSubSize);
                    rusSubSize += usCurSize;
                }
                else
                {
                    m_iNumBLOKs++;
                    rusSubSize += usSizeBLOKSubchunk;
                }
            }
            else
            {
                if ( m_bDisplayParsing )
                {
                    PrintIndent(iIndent+1);
                    *m_pkOut << "(skipped)" << endl;
                }
                rusSubSize += usCurSize;
                m_pkLWOFile->Advance(usCurSize);
            }
        }
        break;

        default:
        {
            rusSubSize += usCurSize;
            m_pkLWOFile->Advance(usCurSize);
        }
    }

    return true;
}
//----------------------------------------------------------------------------
UInt16 LightWaveReader::ParseBLOKSubchunk (int iIndent, bool& rbSupported)
{
    // Generic surface block description.  We only support one per surface,
    // thereby imposing single textured and single coloured surfaces.  Also,
    // we only support image maps.

    UInt32 uiChunkType = m_pkLWOFile->ReadUInt32();
    UInt16 usChunkSize = sizeof(uiChunkType);
    UInt16 usSubSize = m_pkLWOFile->ReadUInt16();
    usChunkSize += sizeof(usSubSize);

    if ( m_bDisplayParsing )
        PrintChunkInfo(uiChunkType,usSubSize,iIndent);

    switch ( uiChunkType )
    {
        case 'IMAP':
        {
            usChunkSize += ParseIMAPSubchunk(usSubSize,iIndent+1);
        }
        break;

        case 'PROC':
        {
            if ( m_bDisplayParsing )
            {
                PrintIndent(iIndent);
                *m_pkOut << "unsupported!" << endl;
            }
            usChunkSize += usSubSize;
            rbSupported = false;
        }
        break;

        case 'GRAD':
        {
            if ( m_bDisplayParsing )
            {
                PrintIndent(iIndent);
                *m_pkOut << "unsupported!" << endl;
            }
            usChunkSize += usSubSize;
            rbSupported = false;
        }
        break;

        case 'SHDR':
        {
            if ( m_bDisplayParsing )
            {
                PrintIndent(iIndent);
                *m_pkOut << "unsupported!" << endl;
            }
            usChunkSize += usSubSize;
            rbSupported = false;
        }
        break;
    }

    return usChunkSize;
}
//----------------------------------------------------------------------------
UInt16 LightWaveReader::ParseBLOKHeader (int iIndent)
{
    // Surface block description header information is currently unused.
    // Read ordinal string.

    int iBytes;
    string kOrdinal = m_pkLWOFile->ReadEvenString(&iBytes);
    UInt16 usChunkSize = (UInt16)iBytes;
    for (int iIndex = 0; iIndex < (int)kOrdinal.length(); iIndex++)
        kOrdinal[iIndex] = (kOrdinal[iIndex] - 0x80) + 0x30;

    if ( m_bDisplayParsing )
    {
        PrintIndent(iIndent);
        *m_pkOut << "ordinal = \"" << kOrdinal << "\"" << endl;
    }

    // Read channel information.
    UInt32 uiChunkType = m_pkLWOFile->ReadUInt32();
    usChunkSize += sizeof(uiChunkType);
    if ( uiChunkType != 'CHAN' )
    {
        if ( m_bDisplayParsing )
        {
            PrintIndent(iIndent);
            *m_pkOut << "missing channel information!" << endl;
        }
        m_eError = MISSING_CHANNEL;
        return usChunkSize;
    }

    UInt16 usSubSize = m_pkLWOFile->ReadUInt16();
    usChunkSize += sizeof(usSubSize);
    if ( m_bDisplayParsing )
    {
        PrintIndent(iIndent);
        PrintChunkType(uiChunkType);
        *m_pkOut << endl;
    }

    // Read channel destination.
    UInt32 uiChan = m_pkLWOFile->ReadUInt32();
    usChunkSize += sizeof(uiChan);
    if ( m_bDisplayParsing )
    {
        PrintIndent(iIndent+1);
        PrintChunkType(uiChan);
        *m_pkOut << endl;
    }

    // Read rest of header.  Up to three components can be found in the BLOK
    // header.
    usChunkSize += ParseBLOKHeaderComponent(iIndent);
    usChunkSize += ParseBLOKHeaderComponent(iIndent);
    usChunkSize += ParseBLOKHeaderComponent(iIndent);

    return usChunkSize;
}
//----------------------------------------------------------------------------
UInt16 LightWaveReader::ParseBLOKHeaderComponent (int iIndent)
{
    // Subcomponents of surface block description header.  We can find either
    // an OPAC, an ENAB or a NEGA subblock, or all of them, but currently we
    // don't support any.

    UInt32 uiHeader = m_pkLWOFile->ReadUInt32();
    UInt16 usChunkSize = sizeof(uiHeader);
    UInt16 usHeaderSize = m_pkLWOFile->ReadUInt16();
    usChunkSize += sizeof(usHeaderSize);

    if ( uiHeader == 'ENAB' || uiHeader == 'OPAC' || uiHeader == 'NEGA' )
    {
        if ( m_bDisplayParsing )
        {
            PrintIndent(iIndent);
            PrintChunkType(uiHeader);
            *m_pkOut << endl;
        }
    }

    switch ( uiHeader )
    {
        case 'ENAB':
        {
            UInt16 usEnabled = m_pkLWOFile->ReadUInt16();
            usChunkSize += sizeof(usEnabled);
            if ( m_bDisplayParsing )
            {
                PrintIndent(iIndent+1);
                *m_pkOut << (usEnabled ? "enabled" : "disabled") << endl;
            }
        }
        break;

        case 'OPAC':
        {
            UInt16 usOpacityType = m_pkLWOFile->ReadUInt16();
            float fOpacity = m_pkLWOFile->ReadFloat();
            UInt16 usEnvelope = m_pkLWOFile->ReadUInt16();
            usChunkSize += sizeof(usOpacityType) + sizeof(fOpacity) +
                sizeof(usEnvelope);

            if ( m_bDisplayParsing )
            {
                PrintIndent(iIndent+1);
                switch ( usOpacityType )
                {
                    case 0:   *m_pkOut << "additive"; break;
                    case 1:   *m_pkOut << "subtractive"; break;
                    case 2:   *m_pkOut << "difference"; break;
                    case 3:   *m_pkOut << "multiply"; break;
                    case 4:   *m_pkOut << "divide"; break;
                    case 5:   *m_pkOut << "alpha"; break;
                    case 6:   *m_pkOut << "texture displacement"; break;
                    default:  *m_pkOut << "unknown opacity type";
                }
                *m_pkOut << " " << 100.0f*fOpacity << "%";
                if ( usEnvelope )
                    *m_pkOut << ", envelope = " << usEnvelope;
                *m_pkOut << endl;
            }
        }
        break;

        case 'NEGA':
        {
            UInt16 usNegated = m_pkLWOFile->ReadUInt16();
            usChunkSize += sizeof(usNegated);

            if ( m_bDisplayParsing )
            {
                PrintIndent(iIndent+1);
                *m_pkOut << (usNegated ? "negated" : "non-negated") << endl;
            }
        }
        break;

        default:
        {
            // Back up, we've entered another subchunk.
            short usOffset = sizeof(uiHeader) + sizeof(usHeaderSize);
            usChunkSize -= usOffset;
            m_pkLWOFile->BackUp((long)usOffset);
        }
    }

    return usChunkSize;
}
//----------------------------------------------------------------------------
UInt16 LightWaveReader::ParseIMAPSubchunk (UInt16 usSizeIMAP, int iIndent)
{
    // Image maps contain texture information and layout.  Get rid of tedious
    // header bookkeeping.

    UInt16 usChunkSize = ParseBLOKHeader(iIndent);    

    // Now the header is behind us, let's do some real parsing.
    usSizeIMAP -= usChunkSize;
    if ( usSizeIMAP != 0 )
    {
        if ( m_bDisplayParsing )
        {
            PrintIndent(iIndent);
            *m_pkOut << "malformed IMAP subchunk!" << endl;
        }
        m_eError = BAD_IMAP_CHUNK;
        return usChunkSize;
    }

    // Parse all known and relevant subchunks.
    // Only texture images are currently used.

    bool bLoop = true;    
    while ( bLoop && m_pkLWOFile->DataAvailable() )
    {
        UInt32 uiChunkType = m_pkLWOFile->ReadUInt32();
        UInt32 uiCurSize = sizeof(uiChunkType);
        UInt16 usSubSize = m_pkLWOFile->ReadUInt16();
        uiCurSize += sizeof(usSubSize);

        usChunkSize += (UInt16)uiCurSize;

        switch ( uiChunkType )
        {
            case 'TMAP':
            {
                if ( m_bDisplayParsing )
                    PrintChunkInfo(uiChunkType,usSubSize,iIndent);
                UInt16 usSizeTMAP = ParseTMAPSubchunk(iIndent+1);
                usChunkSize += usSizeTMAP;
            }
            break;

            case 'PROJ':
            {
                UInt16 usProjectionMode = m_pkLWOFile->ReadUInt16();
                usChunkSize += sizeof(usProjectionMode);
                if ( m_bDisplayParsing )
                {
                    PrintChunkInfo(uiChunkType,usSubSize,iIndent);
                    PrintIndent(iIndent+1);
                    switch ( usProjectionMode )
                    {
                        case 0:   *m_pkOut << "planar"; break;
                        case 1:   *m_pkOut << "cylindrical"; break;
                        case 2:   *m_pkOut << "spherical"; break;
                        case 3:   *m_pkOut << "cubic"; break;
                        case 4:   *m_pkOut << "front projection"; break;
                        case 5:   *m_pkOut << "UV"; break;
                        default:  *m_pkOut << "unknown projection mode";
                    }
                    *m_pkOut << endl;
                }
            }
            break;

            case 'AXIS':
            {
                UInt16 usAxis = m_pkLWOFile->ReadUInt16();
                usChunkSize += sizeof(usAxis);
                if ( m_bDisplayParsing )
                {
                    PrintChunkInfo(uiChunkType,usSubSize,iIndent);
                    PrintIndent(iIndent+1);
                    switch ( usAxis )
                    {
                        case 0:   *m_pkOut << "X"; break;
                        case 1:   *m_pkOut << "Y"; break;
                        case 2:   *m_pkOut << "Z"; break;
                        default:  *m_pkOut << "unknown axis";
                    }
                    *m_pkOut << endl;
                }
            }
            break;

            case 'IMAG':
            {
                UInt16 usIndex = m_pkLWOFile->ReadUInt16();
                usChunkSize += sizeof(usIndex);

                if ( usIndex && usIndex <= (UInt16)m_akTextures.size() )
                    m_pkCurrentTexture = m_akTextures[usIndex-1];
                else
                    m_pkCurrentTexture = NULL;

                if ( m_bDisplayParsing )
                {
                    PrintChunkInfo(uiChunkType,usSubSize,iIndent);
                    PrintIndent(iIndent+1);
                    if ( usIndex )
                        *m_pkOut << "index = " << usIndex << endl;
                    else
                        *m_pkOut << "(no image)" << endl;
                }
            }
            break;

            case 'WRAP':
            {
                UInt16 usWidth = m_pkLWOFile->ReadUInt16();
                UInt16 usHeight = m_pkLWOFile->ReadUInt16();
                usChunkSize += sizeof(usWidth) + sizeof(usHeight);

                if ( m_bDisplayParsing )
                {
                    PrintChunkInfo(uiChunkType,usSubSize,iIndent);
                    PrintIndent(iIndent+1);
                    *m_pkOut << "width = ";
                    switch ( usWidth )
                    {
                        case 0:   *m_pkOut << "reset"; break;
                        case 1:   *m_pkOut << "repeat"; break;
                        case 2:   *m_pkOut << "mirror"; break;
                        case 3:   *m_pkOut << "edge"; break;
                        default:  *m_pkOut << "unknown wrap mode";
                    }
                    *m_pkOut << endl;

                    PrintIndent(iIndent+1);
                    *m_pkOut << "height = ";
                    switch ( usHeight )
                    {
                        case 0:   *m_pkOut << "reset"; break;
                        case 1:   *m_pkOut << "repeat"; break;
                        case 2:   *m_pkOut << "mirror"; break;
                        case 3:   *m_pkOut << "edge"; break;
                        default:  *m_pkOut << "unknown wrap mode";
                    }
                    *m_pkOut << endl;
                }
            }
            break;

            case 'WRPW':
            case 'WRPH':
            {
                float fCycles = m_pkLWOFile->ReadFloat();
                UInt16 usEnvelope = m_pkLWOFile->ReadUInt16();
                usChunkSize += sizeof(fCycles) + sizeof(usEnvelope);
                if ( m_bDisplayParsing )
                {
                    PrintChunkInfo(uiChunkType,usSubSize,iIndent);
                    PrintIndent(iIndent+1);
                    *m_pkOut << "cycles = " << 100.0f*fCycles << "%";
                    if ( usEnvelope )
                        *m_pkOut << ", envelope = " << usEnvelope;
                    *m_pkOut << endl;
                }
            }
            break;

            case 'VMAP':
            {
                int iBytes;
                string kVertexMapName = m_pkLWOFile->ReadEvenString(&iBytes);
                usChunkSize += (UInt16)iBytes;

                if ( kVertexMapName.length() && m_pkCurrentTexture )
                {
                    ApplyVertexMap(kVertexMapName);
                    ApplyDiscontinuousVertexMap(kVertexMapName);
                }

                if ( m_bDisplayParsing )
                {
                    PrintChunkInfo(uiChunkType,usSubSize,iIndent);
                    PrintIndent(iIndent+1);
                    *m_pkOut << "vertex map = " << kVertexMapName << endl;
                }
            }
            break;

            case 'AAST':
            {
                UInt16 usFlags = m_pkLWOFile->ReadUInt16();
                float fStrength = m_pkLWOFile->ReadFloat();
                usChunkSize += sizeof(usFlags) + sizeof(fStrength);
                if ( m_bDisplayParsing )
                {
                    PrintChunkInfo(uiChunkType,usSubSize,iIndent);
                    PrintIndent(iIndent+1);
                    *m_pkOut << "antialiasing       = "
                        << ((usFlags & 1) ? "enabled" : "disabled") << endl;
                    PrintIndent(iIndent+1);
                    *m_pkOut << "antialias strength = " << 100.0f*fStrength
                        << "%" << endl;
                }
            }
            break;

            case 'PIXB':
            {
                UInt16 usFlags = m_pkLWOFile->ReadUInt16();
                usChunkSize += sizeof(usFlags);
                if ( m_bDisplayParsing )
                {
                    PrintChunkInfo(uiChunkType,usSubSize,iIndent);
                    PrintIndent(iIndent+1);
                    *m_pkOut << "blending = "
                        << ((usFlags & 1) ? "enabled" : "disabled") << endl;
                }
            }
            break;

            default:
            {
                // Back up, we've entered another subchunk.
                usChunkSize -= (UInt16)uiCurSize;
                m_pkLWOFile->BackUp((long)uiCurSize);
                bLoop = false;
            }
        }
    }

    return usChunkSize;
}
//----------------------------------------------------------------------------
UInt16 LightWaveReader::ParseTMAPSubchunk (int iIndent)
{
    // Texture map internal description.  Parse all known and relevant
    // subchunks.

    UInt16 usChunkSize = 0;    
    bool bLoop = true;    
    while ( bLoop && m_pkLWOFile->DataAvailable() )
    {
        UInt32 uiChunkType = m_pkLWOFile->ReadUInt32();
        UInt32 uiCurSize = sizeof(uiChunkType);
        UInt16 usSubSize = m_pkLWOFile->ReadUInt16();
        uiCurSize += sizeof(usSubSize);

        usChunkSize += (UInt16) uiCurSize;

        switch ( uiChunkType )
        {
            case 'CNTR':
            case 'SIZE':
            case 'ROTA':
            {
                Vector3f kPoint(m_pkLWOFile->ReadVector());
                UInt16 usEnvelope = m_pkLWOFile->ReadUInt16();
                usChunkSize += sizeof(kPoint) + sizeof(usEnvelope);
                if ( m_bDisplayParsing )
                {
                    PrintChunkInfo(uiChunkType,usSubSize,iIndent);
                    PrintIndent(iIndent+1);
                    *m_pkOut << "vector = " << kPoint;
                    if ( usEnvelope )
                        *m_pkOut << ", envelope = " << usEnvelope;
                    *m_pkOut << endl;
                }
            }
            break;

            case 'OREF':
            {
                int iBytes;
                string kObject = m_pkLWOFile->ReadEvenString(&iBytes);
                usChunkSize += (UInt16)iBytes;
                if ( m_bDisplayParsing )
                {
                    PrintChunkInfo(uiChunkType,usSubSize,iIndent);
                    PrintIndent(iIndent+1);
                    if ( kObject.length() )
                        *m_pkOut << "object reference = " << kObject;
                    else
                        *m_pkOut << "(none)";
                    *m_pkOut << endl;
                }
            }
            break;

            case 'FALL':
            {
                UInt16 usFallType = m_pkLWOFile->ReadUInt16();
                Vector3f kPoint(m_pkLWOFile->ReadVector());
                UInt16 usEnvelope = m_pkLWOFile->ReadUInt16();
                usChunkSize += sizeof(usFallType) + sizeof(kPoint) +
                    sizeof(usEnvelope);
                if ( m_bDisplayParsing )
                {
                    PrintChunkInfo(uiChunkType,usSubSize,iIndent);
                    PrintIndent(iIndent+1);
                    switch ( usFallType )
                    {
                        case 0:   *m_pkOut << "cubic"; break;
                        case 1:   *m_pkOut << "spherical"; break;
                        case 2:   *m_pkOut << "linear X"; break;
                        case 3:   *m_pkOut << "linear Y"; break;
                        case 4:   *m_pkOut << "linear Z"; break;
                        default:  *m_pkOut << "unknown fall-off type";
                    }
                    *m_pkOut << ", vector = " << kPoint;
                    if ( usEnvelope )
                        *m_pkOut << ", envelope = " << usEnvelope;
                    *m_pkOut << endl;
                }
            }
            break;

            case 'CSYS':
            {
                UInt16 usCoordType = m_pkLWOFile->ReadUInt16();
                usChunkSize += sizeof(usCoordType);
                if ( m_bDisplayParsing )
                {
                    PrintChunkInfo(uiChunkType,usSubSize,iIndent);
                    PrintIndent(iIndent+1);
                    *m_pkOut << (usCoordType ? "world" : "object")
                        << " coordinates" << endl;
                }
            }
            break;

            default:
            {
                // Back up, we've entered another subchunk.
                usChunkSize -= (UInt16)uiCurSize;
                m_pkLWOFile->BackUp((long)uiCurSize);
                bLoop = false;
            }
        }
    }

    return usChunkSize;
}
//----------------------------------------------------------------------------
bool LightWaveReader::AddTexture (const string& rkTextureName,
    const string& rkBaseDirName)
{
    // Add a texture to the object array, thereby making it available to all
    // surfaces.  Prepare texture file name.
    
    size_t iPos = rkTextureName.find_last_of('/', rkTextureName.length());
    if ( iPos < rkTextureName.length() )
        ++iPos;
    else
        iPos = 0;

    // Load texture file and bail out immediately if it couldn't be read.
    string kTextureFile(rkTextureName,iPos,rkTextureName.length()-iPos);
    kTextureFile = rkBaseDirName + kTextureFile;
    int iWidth, iHeight, iBitsPerPixel;
    char* pcImageData = NULL;
    ImageLoader kTextureImage(kTextureFile);
    if ( !kTextureImage.Load(iWidth,iHeight,iBitsPerPixel,pcImageData) )
    {
        delete[] pcImageData;
        return false;
    }

    int eType =
        (iBitsPerPixel == 24 ? Image::IT_RGB888 : Image::IT_RGBA8888);
    Image* pkImage = new Image(eType,iWidth,iHeight,(UInt8*)pcImageData,
        kTextureFile.c_str());
    Texture* pkTexture = new Texture;
    pkTexture->SetName(rkTextureName.c_str());
    pkTexture->Apply = Texture::AM_MODULATE;
    pkTexture->Filter = Texture::FM_LINEAR;
    pkTexture->Mipmap = Texture::MM_LINEAR_NEAREST;
    pkTexture->SetImage(pkImage);

    m_akTextures.push_back(pkTexture);
    return true;
}
//----------------------------------------------------------------------------
void LightWaveReader::BuildTriMesh ()
{
    // Build vertex array as a vector, containing no duplicate points.
    VertexMap* pkVertexMap = m_kVertexMaps[m_kCurrentSurface];
    PolygonMap* pkPolygonMap = new PolygonMap;
    m_kPolygonMaps[m_kCurrentSurface] = pkPolygonMap;
    PolygonList::iterator kPolygon = m_pkCurrentPolyList->begin();
    Vector3Array akVertexArray;
    int iTriangleQuantity = (int)m_pkCurrentPolyList->size();
    int iTriangle, iVertex = 0;
    for (iTriangle = 0; iTriangle < iTriangleQuantity; iTriangle++)
    {
        UInt16 usPolygonID = *kPolygon++;
        (*pkPolygonMap)[usPolygonID] = iTriangle;

        Triangle& rkTriangle = m_akPolygons[usPolygonID];
        UInt16 usFirst = rkTriangle.m_usV0;
        UInt16 usSecond = rkTriangle.m_usV1;
        UInt16 usThird = rkTriangle.m_usV2;

        if ( pkVertexMap->find(usFirst) == pkVertexMap->end() )
        {
            (*pkVertexMap)[usFirst] = iVertex++;
            akVertexArray.push_back(m_akVertices[usFirst]);
        }
        if (pkVertexMap->find(usSecond) == pkVertexMap->end())
        {
            (*pkVertexMap)[usSecond] = iVertex++;
            akVertexArray.push_back(m_akVertices[usSecond]);
        }
        if (pkVertexMap->find(usThird) == pkVertexMap->end())
        {
            (*pkVertexMap)[usThird] = iVertex++;
            akVertexArray.push_back(m_akVertices[usThird]);
        }
    }

    // Create raw vertex array to be used by the trimesh.
    int iVertexQuantity = (int)akVertexArray.size();
    Vector3f* akVertices = new Vector3f[iVertexQuantity];
    copy(akVertexArray.begin(),akVertexArray.end(),akVertices);

    // Build connectivity array.
    int* aiConnectivity = new int[3*iTriangleQuantity];
    kPolygon = m_pkCurrentPolyList->begin();
    iVertex = 0;
    for (iTriangle = 0; iTriangle < iTriangleQuantity; iTriangle++)
    {
        UInt16 usPolygonID = *kPolygon++;
        Triangle& rkTriangle = m_akPolygons[usPolygonID];
        aiConnectivity[iVertex++] = (*pkVertexMap)[rkTriangle.m_usV0];
        aiConnectivity[iVertex++] = (*pkVertexMap)[rkTriangle.m_usV1];
        aiConnectivity[iVertex++] = (*pkVertexMap)[rkTriangle.m_usV2];
    }

    // Build the trimesh.
    Vector3fArray* pkVerts = new Vector3fArray(iVertexQuantity,akVertices);
    IntArray* pkIndices = new IntArray(iTriangleQuantity*3,aiConnectivity);
    m_pkCurrentMesh->Vertices = pkVerts;
    m_pkCurrentMesh->Indices = pkIndices;
    m_pkCurrentMesh->GenerateNormals();
    m_pkCurrentMesh->UpdateMS();
}
//----------------------------------------------------------------------------
void LightWaveReader::ApplyVertexMap (const string& rkName)
{
    if ( m_kMaps.find(rkName) == m_kMaps.end() )
        return;

    // Create texture coordinates array.
    VertexMap* pkVertexMap = m_kVertexMaps[m_kCurrentSurface];
    int iVertexQuantity = m_pkCurrentMesh->Vertices->GetQuantity();
    Vector2f* akTextures = new Vector2f[iVertexQuantity];
    memset(akTextures,0,sizeof(Vector2f)*iVertexQuantity);

    // Fill in texture coordinates array.
    VMAPArray* pakVMAP = m_kMaps[rkName];
    int iVMAPSize = (int)pakVMAP->size();    
    for (int iElement = 0; iElement < iVMAPSize; iElement++)
    {
        VMAP& rkVMAP = (*pakVMAP)[iElement];
        if ( pkVertexMap->find(rkVMAP.m_iVertex) == pkVertexMap->end() )
            continue;

        int iVertex = (*pkVertexMap)[rkVMAP.m_iVertex];
        akTextures[iVertex] = Vector2f(rkVMAP.m_fU,rkVMAP.m_fV);
    }

    // Assign texture coordinates and texture image.
    Vector2fArray* pkUVs = new Vector2fArray(iVertexQuantity,akTextures);
    TextureEffect* pkTextureEffect = new TextureEffect(m_pkCurrentTexture,pkUVs);
    pkTextureEffect->SetName(rkName.c_str());
    m_pkCurrentMesh->SetEffect(pkTextureEffect);

    // Reset default material.
    ColorRGBA kAmbient(0.2f,0.2f,0.2f,1.0f);
    ColorRGBA kDiffuse(0.8f,0.8f,0.8f,1.0f);
    ColorRGBA kSpecular(0.0f,0.0f,0.0f,1.0f);
    GlobalState* pkGlobalState =
        m_pkCurrentMesh->GetGlobalState(GlobalState::MATERIAL);
    MaterialState* pkMaterialState = 
        DynamicCast<MaterialState>(pkGlobalState);
    pkMaterialState->Ambient = kAmbient;
    pkMaterialState->Diffuse = kDiffuse;
    pkMaterialState->Specular = kSpecular;
    pkMaterialState->Shininess = 0.0f;
}
//----------------------------------------------------------------------------
void LightWaveReader::ApplyDiscontinuousVertexMap (const string& rkName)
{
    if ( m_kDiscontMaps.find(rkName) == m_kDiscontMaps.end() )
        return;

    // Prepare trimesh expansion: it will be triggered by per-vertex and
    // per-polygon texture coordinates assignment.  Copy original raw
    // LightWave geometric data.
    int iVertexQuantity = m_pkCurrentMesh->Vertices->GetQuantity();
    const Vector3f* akVertices = m_pkCurrentMesh->Vertices->GetData();
    const Vector3f* akNormals = m_pkCurrentMesh->Normals->GetData();
    const Vector2f* akTextures = m_pkCurrentMesh->GetEffect()->UVs[0]->GetData();
    Vector3Array kVertices(iVertexQuantity);
    Vector3Array kNormals(iVertexQuantity);
    Vector2Array kTextures(iVertexQuantity);
    copy(akVertices,akVertices+iVertexQuantity,kVertices.begin());
    copy(akNormals,akNormals+iVertexQuantity,kNormals.begin());
    copy(akTextures,akTextures+iVertexQuantity,kTextures.begin());
    
    // For each entry in the map, locate the polygon and vertex that
    // correspond to the entry in the current trimesh. Then, add it to the
    // geometric data by expanding the array.
    PolygonMap* pkPolygonMap = m_kPolygonMaps[m_kCurrentSurface];
    VertexMap* pkVertexMap = m_kVertexMaps[m_kCurrentSurface];
    VMADArray* pakVMAD = m_kDiscontMaps[rkName];
    int iVMADSize = (int)pakVMAD->size();
    int* aiConnectivity = m_pkCurrentMesh->Indices->GetData();
    for (int iElement = 0; iElement < iVMADSize; iElement++)
    {
        VMAD& rkVMAD = (*pakVMAD)[iElement];
        if ( pkPolygonMap->find(rkVMAD.m_iPolygon) == pkPolygonMap->end() )
            continue;
        if ( pkVertexMap->find(rkVMAD.m_iVertex) == pkVertexMap->end() )
            continue;

        int iPolygon = (*pkPolygonMap)[rkVMAD.m_iPolygon];
        int iVertex = (*pkVertexMap)[rkVMAD.m_iVertex];

        int iPolygonIndex = 3*iPolygon;
        int iVertexIndex = (int)kVertices.size();

        kVertices.push_back(akVertices[iVertex]);
        kNormals.push_back(akNormals[iVertex]);
        kTextures.push_back(Vector2f(rkVMAD.m_fU,rkVMAD.m_fV));
        
        if ( aiConnectivity[iPolygonIndex] == iVertex )
            aiConnectivity[iPolygonIndex] = iVertexIndex;
        else if ( aiConnectivity[iPolygonIndex+1] == iVertex )
            aiConnectivity[iPolygonIndex+1] = iVertexIndex;
        else if ( aiConnectivity[iPolygonIndex+2] == iVertex )
            aiConnectivity[iPolygonIndex+2] = iVertexIndex;
    }
    
    // Recreate raw geometric data from expanded array.
    iVertexQuantity = (int)kVertices.size();
    Vector3f* akNewVertices = new Vector3f[iVertexQuantity];
    Vector3f* akNewNormals = new Vector3f[iVertexQuantity];
    Vector2f* akNewTextures = new Vector2f[iVertexQuantity];
    copy(kVertices.begin(),kVertices.end(),akNewVertices);
    copy(kNormals.begin(),kNormals.end(),akNewNormals);
    copy(kTextures.begin(),kTextures.end(),akNewTextures);

    Vector3fArray* pkVerts = new Vector3fArray(iVertexQuantity,akNewVertices);
    Vector3fArray* pkNorms = new Vector3fArray(iVertexQuantity,akNewNormals);
    Vector2fArray* pkUVs = new Vector2fArray(iVertexQuantity,akNewTextures);

    m_pkCurrentMesh->Vertices = pkVerts;
    m_pkCurrentMesh->Normals = pkNorms;
    m_pkCurrentMesh->GetEffect()->UVs[0] = pkUVs;
}
//----------------------------------------------------------------------------
void LightWaveReader::ComputeNormals ()
{
    // Just use the default WildMagic normals if we're not interested in
    // actual LightWave smoothing.
    if ( !m_bSmoothed )
        return;

    // Compute the actual vertex normals by first expanding the geometry, then
    // calculating the smoothed normals, and finally compressing the geometry
    // to eliminate duplicate data. The compression step also rebuilds the
    // trimeshes.
    
    ExpandGeometry();
    CreateVertexNormals();
    CompressGeometry();
}
//----------------------------------------------------------------------------
Vector3f LightWaveReader::ComputePlaneNormal (const Vector3f& rkV0,
    const Vector3f& rkV1, const Vector3f& rkV2) const
{
    Vector3f kEdge1 = rkV1 - rkV0;
    Vector3f kEdge2 = rkV2 - rkV0;
    Vector3f kNormal = kEdge1.Cross(kEdge2);
    kNormal.Normalize();
    return kNormal;
}
//----------------------------------------------------------------------------
void LightWaveReader::ExpandGeometry ()
{
    // Expand raw geometric data by creating one vertex per polygon for the
    // whole model. Each vertex normal is reset to the plane normal.
    SurfaceMap::iterator kSIter = m_kSurfaces.begin();
    for (/**/; kSIter != m_kSurfaces.end(); kSIter++)
    {
        TriMesh* pkMesh = kSIter->second;
        int iTriangleQuantity = pkMesh->GetTriangleQuantity();
        int iVertexQuantity = 3*iTriangleQuantity;
        
        Vector3f* akVertex = new Vector3f[iVertexQuantity];
        Vector3f* akNormal = new Vector3f[iVertexQuantity];
        Vector2f* akTexture = NULL;
        int* aiConnect = new int[iVertexQuantity];
        const Vector3f* akMVertex = pkMesh->Vertices->GetData();
        const Vector2f* akMTexture = (pkMesh->GetEffect() && 
            pkMesh->GetEffect()->UVs.GetQuantity() > 0 ? 
            pkMesh->GetEffect()->UVs[0]->GetData() : NULL);
        if ( akMTexture )
            akTexture = new Vector2f[iVertexQuantity];
        const int* aiMConnect = pkMesh->Indices->GetData();

        int iPosition = 0;
        for (int iTriangle = 0; iTriangle < iTriangleQuantity; iTriangle++)
        {
            int iVertex = 3*iTriangle;
            int iV0 = aiMConnect[iVertex];
            int iV1 = aiMConnect[iVertex+1];
            int iV2 = aiMConnect[iVertex+2];
            Vector3f kNormal = ComputePlaneNormal(akMVertex[iV0],
                akMVertex[iV1],akMVertex[iV2]);
            
            akVertex[iPosition] = akMVertex[iV0];
            akNormal[iPosition] = kNormal;
            if ( akMTexture )
                akTexture[iPosition] = akMTexture[iV0];
            aiConnect[iVertex] = iPosition++;
            
            akVertex[iPosition] = akMVertex[iV1];
            akNormal[iPosition] = kNormal;
            if ( akMTexture )
                akTexture[iPosition] = akMTexture[iV1];
            aiConnect[iVertex+1] = iPosition++;

            akVertex[iPosition] = akMVertex[iV2];
            akNormal[iPosition] = kNormal;
            if ( akMTexture )
                akTexture[iPosition] = akMTexture[iV2];
            aiConnect[iVertex+2] = iPosition++;            
        }

        string kName = kSIter->first;

        Vector3fArray* pkVerts = new Vector3fArray(iVertexQuantity,akVertex);
        Vector3fArray* pkNorms = new Vector3fArray(iVertexQuantity,akNormal);
        IntArray* pkIndices = new IntArray(iVertexQuantity,aiConnect);

        pkMesh->Vertices = pkVerts;
        pkMesh->Normals = pkNorms;
        pkMesh->Indices = pkIndices;

        if (akTexture)
        {
            Vector2fArray* pkUVs = 
                new Vector2fArray(iVertexQuantity,akTexture);
            pkMesh->GetEffect()->UVs[0] = pkUVs;
        }

        m_kMapVertices[kName] = akVertex;
        m_kMapNormals[kName] = akNormal;
        m_kMapTextures[kName] = akTexture;
        m_kMapConnectivity[kName] = aiConnect;
    }
}
//----------------------------------------------------------------------------
void LightWaveReader::CreateVertexNormals ()
{
    // Compute vertex normals.
    UInt16 usVertexQuantity = (UInt16)m_akVertices.size();
    UInt16 usPolygonQuantity = (int)m_akPolygons.size();
    for (UInt16 usV = 0; usV < usVertexQuantity; usV++)
    {
        Vector3f& rkVertex = m_akVertices[usV];
        PolygonList* pkPolygonList = m_akVertexToPolygon[usV];
        for (UInt16 usP = 0; usP < usPolygonQuantity; usP++)
        {
            // Do not process this polygon if it doesn't contain the current
            // vertex.
            Triangle& rkTriangle = m_akPolygons[usP];
            if ( !rkTriangle.HasVertex(usV) )
                continue;

            // Retrieve surface and smoothing angle.
            Vector3f& rkFirstFacetNormal = rkTriangle.m_kNormal;
            UInt16 usSurfaceTag = m_kPolygonTags[usP];
            TriMesh* pkMesh = m_kTagSurfaces[usSurfaceTag];
            string kName(pkMesh->GetName());
            if ( m_kAngles.find(kName) == m_kAngles.end() )
                continue;

            // Extract current facet normal and process all polygons sharing
            // this vertex.
            float fSmoothing = m_kAngles[kName];
            bool bAveraged = false;
            Vector3f kAverage = Vector3f::ZERO;
            PolygonList::iterator kPolyIter = pkPolygonList->begin();
            for (/**/; kPolyIter != pkPolygonList->end(); kPolyIter++)
            {
                // Do not average against the same polygon.
                UInt16 usPolygonID = *kPolyIter;
                if ( usPolygonID == usP )
                    continue;

                Triangle& rkSecondTriangle = m_akPolygons[usPolygonID];
                Vector3f& rkSecondFacetNormal = rkSecondTriangle.m_kNormal;
                float fAngle = Mathf::ACos(
                    rkFirstFacetNormal.Dot(rkSecondFacetNormal));
                if ( 0.0f <= fAngle && fAngle <= fSmoothing )
                {
                    bAveraged = true;
                    kAverage += rkSecondFacetNormal;
                }
            }
            
            // If we have averaged this normal, we have to assign it to the
            // current vertex of the current polygon in the current surface.
            if ( bAveraged )
            {
                kAverage.Normalize();                
                Vector3f* akVertices = m_kMapVertices[kName];
                Vector3f* akNormals = m_kMapNormals[kName];
                PolygonMap* pkPolygonMap = m_kPolygonMaps[kName];
                UInt16 usPolygonID = (*pkPolygonMap)[usP];
                UInt16 usPolygonIndex = 3*usPolygonID;
                if ( akVertices[usPolygonIndex] == rkVertex )
                    akNormals[usPolygonIndex] = kAverage;
                else if ( akVertices[usPolygonIndex+1] == rkVertex )
                    akNormals[usPolygonIndex+1] = kAverage;
                else if ( akVertices[usPolygonIndex+2] == rkVertex )
                    akNormals[usPolygonIndex+2] = kAverage;
            }
        }
    }
}
//----------------------------------------------------------------------------
void LightWaveReader::CompressGeometry ()
{
    // Iterate over the expanded geometric data and build an array
    // that contains only unique vertices.
    Vector2f kNoTexture(-1.0f,-1.0f);
    SurfaceMap::iterator kSIter = m_kSurfaces.begin();
    for (/**/; kSIter != m_kSurfaces.end(); kSIter++)
    {
        TriMesh* pkMesh = kSIter->second;
        int iTriangleQuantity = pkMesh->GetTriangleQuantity();
        int iVertexQuantity = 3*iTriangleQuantity;

        string kName = kSIter->first;
        Vector3f* akVertex = m_kMapVertices[kName];
        Vector3f* akNormal = m_kMapNormals[kName];
        Vector2f* akTexture = m_kMapTextures[kName];
        int* aiConnect = m_kMapConnectivity[kName];

        // A map is used to make sure there are no duplicates.
        VertexAttributeMap kUniqueMap;
        VertexAttributeArray kUniqueArray;
        int iPosition = 0;
        for (int iTriangle = 0; iTriangle < iTriangleQuantity; iTriangle++)
        {
            int iVertex = 3*iTriangle;
            int iV0 = aiConnect[iVertex];
            int iV1 = aiConnect[iVertex+1];
            int iV2 = aiConnect[iVertex+2];

            VertexAttribute kVA0(akVertex[iV0], akNormal[iV0],
                akTexture ? akTexture[iV0] : kNoTexture);
            VertexAttribute kVA1(akVertex[iV1], akNormal[iV1],
                akTexture ? akTexture[iV1] : kNoTexture);
            VertexAttribute kVA2(akVertex[iV2], akNormal[iV2],
                akTexture ? akTexture[iV2] : kNoTexture);

            VertexAttributeMap::iterator kPosVA0 = kUniqueMap.find(kVA0);
            if ( kPosVA0 == kUniqueMap.end() )
            {
                kUniqueArray.push_back(kVA0);
                kUniqueMap[kVA0] = iPosition;
                aiConnect[iVertex] = iPosition++;
            }
            else
            {
                aiConnect[iVertex] = kPosVA0->second;
            }

            VertexAttributeMap::iterator kPosVA1 = kUniqueMap.find(kVA1);
            if ( kPosVA1 == kUniqueMap.end() )
            {
                kUniqueArray.push_back(kVA1);
                kUniqueMap[kVA1] = iPosition;
                aiConnect[iVertex+1] = iPosition++;
            }
            else
            {
                aiConnect[iVertex+1] = kPosVA1->second;
            }

            VertexAttributeMap::iterator kPosVA2 = kUniqueMap.find(kVA2);
            if ( kPosVA2 == kUniqueMap.end() )
            {
                kUniqueArray.push_back(kVA2);
                kUniqueMap[kVA2] = iPosition;
                aiConnect[iVertex+2] = iPosition++;
            }
            else
            {
                aiConnect[iVertex+2] = kPosVA2->second;
            }
        }
        
        // Recreate raw geometric data from the unique set computed above.
        iVertexQuantity = (int)kUniqueArray.size();
        akVertex = new Vector3f[iVertexQuantity];
        akNormal = new Vector3f[iVertexQuantity];
        if ( akTexture )
            akTexture = new Vector2f[iVertexQuantity];

        // No need to delete previous values, because the previous values
        // are all mapped from smart pointers (from the TriMesh arrays).
        m_kMapVertices[kName] = akVertex;
        m_kMapNormals[kName] = akNormal;
        m_kMapTextures[kName] = akTexture;
        
        for (int iVertex = 0; iVertex < iVertexQuantity; iVertex++)
        {
            VertexAttribute& rkVA = kUniqueArray[iVertex];
            akVertex[iVertex] = rkVA.m_kVertex;
            akNormal[iVertex] = rkVA.m_kNormal;
            if ( akTexture )
                akTexture[iVertex] = rkVA.m_kTexture;
        }

        Vector3fArray* pkVerts = new Vector3fArray(iVertexQuantity,akVertex);
        Vector3fArray* pkNorms = new Vector3fArray(iVertexQuantity,akNormal);
        pkMesh->Vertices = pkVerts;
        pkMesh->Normals = pkNorms;

        if (akTexture)
        {
            Vector2fArray* pkUVs = new Vector2fArray(iVertexQuantity,
            akTexture);
            pkMesh->GetEffect()->UVs[0] = pkUVs;
        }
    }
}
//----------------------------------------------------------------------------
void LightWaveReader::PrintIndent (int iIndent)
{
    for (int iIndex = 0; iIndex < iIndent; iIndex++)
        *m_pkOut << ms_cTab;
}
//----------------------------------------------------------------------------
void LightWaveReader::PrintChunkType (UInt32 uiNumber)
{
    UInt32 uiChunk = UInt32ToBigEndian(uiNumber);
    UInt8* pucBytes = (UInt8*)&uiChunk;
    *m_pkOut << pucBytes[0] << pucBytes[1] << pucBytes[2] << pucBytes[3];
}
//----------------------------------------------------------------------------
void LightWaveReader::PrintChunkInfo (UInt32 uiChunkType, UInt16 usChunkSize,
    int iIndent)
{
    PrintIndent(iIndent);
    PrintChunkType(uiChunkType);
    (*m_pkOut) << " size = " << usChunkSize << endl;
}
//----------------------------------------------------------------------------
bool LightWaveReader::VertexAttribute::operator<
    (const LightWaveReader::VertexAttribute& rkVA) const
{
    if ( m_kVertex < rkVA.m_kVertex )
        return true;
    if ( m_kVertex > rkVA.m_kVertex )
        return false;
    if ( m_kNormal < rkVA.m_kNormal )
        return true;
    if ( m_kNormal > rkVA.m_kNormal )
        return false;
    if ( m_kTexture < rkVA.m_kTexture )
        return true;
    return false;
}
//----------------------------------------------------------------------------
bool LightWaveReader::VertexAttribute::operator==
    (const LightWaveReader::VertexAttribute& rkVA) const
{
    return (m_kVertex == rkVA.m_kVertex) &&
           (m_kNormal == rkVA.m_kNormal) &&
           (m_kTexture == rkVA.m_kTexture);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// LightWaveReader::Triangle
//----------------------------------------------------------------------------
LightWaveReader::Triangle::Triangle ()
    :
    m_kNormal(Vector3f::ZERO)
{
    m_usV0 = 0;
    m_usV1 = 0;
    m_usV2 = 0;
}
//----------------------------------------------------------------------------
LightWaveReader::Triangle::Triangle (UInt16 usV0, UInt16 usV1, UInt16 usV2,
    Vector3f& rkNormal)
    :
    m_kNormal(rkNormal)
{
    m_usV0 = usV0;
    m_usV1 = usV1;
    m_usV2 = usV2;
}
//----------------------------------------------------------------------------
LightWaveReader::Triangle::Triangle (
    const LightWaveReader::Triangle& rkTriangle)
    :
    m_kNormal(rkTriangle.m_kNormal)
{
    m_usV0 = rkTriangle.m_usV0;
    m_usV1 = rkTriangle.m_usV1;
    m_usV2 = rkTriangle.m_usV2;
}
//----------------------------------------------------------------------------
bool LightWaveReader::Triangle::HasVertex (UInt16 usV)
{
    return (usV == m_usV0) || (usV == m_usV1) || (usV == m_usV2);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// LightWaveReader::VMAP
//----------------------------------------------------------------------------
LightWaveReader::VMAP::VMAP ()
{
    m_iVertex = 0;
    m_fU = 0.0f;
    m_fV = 0.0f;
}
//----------------------------------------------------------------------------
LightWaveReader::VMAP::VMAP (int iVertex, float fU, float fV)
{
    m_iVertex = iVertex;
    m_fU = fU;
    m_fV = fV;
}
//----------------------------------------------------------------------------
LightWaveReader::VMAP::VMAP (const LightWaveReader::VMAP& rkVMAP)
{
    m_iVertex = rkVMAP.m_iVertex;
    m_fU = rkVMAP.m_fU;
    m_fV = rkVMAP.m_fV;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// LightWaveReader::VMAD
//----------------------------------------------------------------------------
LightWaveReader::VMAD::VMAD ()
{
    m_iVertex = 0;
    m_iPolygon = 0;
    m_fU = 0.0f;
    m_fV = 0.0f;
}
//----------------------------------------------------------------------------
LightWaveReader::VMAD::VMAD (int iVertex, int iPolygon, float fU, float fV)
{
    m_iVertex = iVertex;
    m_iPolygon = iPolygon;
    m_fU = fU;
    m_fV = fV;
}
//----------------------------------------------------------------------------
LightWaveReader::VMAD::VMAD (const LightWaveReader::VMAD& rkVMAD)
{
    m_iVertex = rkVMAD.m_iVertex;
    m_iPolygon = rkVMAD.m_iPolygon;
    m_fU = rkVMAD.m_fU;
    m_fV = rkVMAD.m_fV;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// LightWaveReader::VertexAttribute
//----------------------------------------------------------------------------
LightWaveReader::VertexAttribute::VertexAttribute ()
    :
    m_kVertex(Vector3f::ZERO),
    m_kNormal(Vector3f::ZERO),
    m_kTexture(Vector2f(-1.0f,-1.0f))
{
}
//----------------------------------------------------------------------------
LightWaveReader::VertexAttribute::VertexAttribute (const Vector3f& rkVertex,
    const Vector3f& rkNormal, const Vector2f& rkTexture)
    :
    m_kVertex(rkVertex),
    m_kNormal(rkNormal),
    m_kTexture(rkTexture)
{
}
//----------------------------------------------------------------------------
LightWaveReader::VertexAttribute::VertexAttribute (
    const VertexAttribute& rkVertexAttribute)
    :
    m_kVertex(rkVertexAttribute.m_kVertex),
    m_kNormal(rkVertexAttribute.m_kNormal),
    m_kTexture(rkVertexAttribute.m_kTexture)
{
}
//----------------------------------------------------------------------------

