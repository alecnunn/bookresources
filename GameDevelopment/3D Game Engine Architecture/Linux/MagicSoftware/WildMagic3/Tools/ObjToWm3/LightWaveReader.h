// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef LIGHTWAVEREADER_H
#define LIGHTWAVEREADER_H

#include "MemFile.h"
#include "WildMagic3SceneGraph.h"
using namespace Wm3;

// A LightWave file reader.  The public interface is really minimal: construct
// the object, initialize the reader, and parse the LightWave file.

class LightWaveReader
{
public:
    // possible parsing errors.
    enum ParseError
    {
        NO_LW_ERROR,
        BAD_PARSE_FILE,
        BAD_OBJECT_FILE,
        MISSING_FORM_CHUNK,
        MISSING_LWO2_CHUNK,
        BAD_LAYR_CHUNK,
        BAD_PNTS_CHUNK,
        BAD_BBOX_CHUNK,
        UNSUPPORTED_POLYGON_TYPE,
        UNSUPPORTED_TEXTURE_TYPE,
        MISSING_CHANNEL,
        BAD_IMAP_CHUNK
    };

    // The constructor accepts a pathname to the LightWave file.  The
    // destructor retains the resulting object (if any), and gets rid of any
    // unnecessary auxiliary objects.
    LightWaveReader (const char* acTheFileName);
    LightWaveReader (const std::string& rkTheFileName);
    ~LightWaveReader ();

    // Initialise the reader first, make sure that it accepts the LightWave
    // file, and then parse it.  If the parse result is false, it is advisable
    // to consult the corresponding error.
    bool InitReader (bool bDisplay = false);
    bool Parse (bool bSmooth = true);

    NodePtr GetObject () const;
    ParseError GetError () const;
    bool IsSmoothed () const;

    // Triangle: vertex indices and plane normal.
    class Triangle
    {
    public:
        Triangle ();
        Triangle (UInt16 usV0, UInt16 usV1, UInt16 usV2, Vector3f& rkNormal);
        Triangle (const Triangle& rkTriangle);
        bool HasVertex (UInt16 usV);

        UInt16 m_usV0, m_usV1, m_usV2;
        Vector3f m_kNormal;
    };

    // Vertex map: vertex and texture coords.
    class VMAP
    {
    public:
        VMAP ();
        VMAP (int iVertex, float fU, float fV);
        VMAP (const VMAP& rkVMAP);

        int m_iVertex;
        float m_fU, m_fV;
    };

    // Discontinous vertex map: vertex, polygon and texture coords.
    class VMAD
    {
    public:
        VMAD ();
        VMAD (int iVertex, int iPolygon, float fU, float fV);
        VMAD (const VMAD& rkVMAD);

        int m_iVertex, m_iPolygon;
        float m_fU, m_fV;
    };
    
    // Vertex attribute: point, normal and texture coords.
    // Used for geometry compression.
    class VertexAttribute
    {
    public:
        VertexAttribute ();
        VertexAttribute (const Vector3f& rkVertex, const Vector3f& rkNormal,
            const Vector2f& rkTexture);
        VertexAttribute (const VertexAttribute& rkVertexAttribute);
        
        bool operator< (const VertexAttribute& rkVA) const;
        bool operator== (const VertexAttribute& rkVA) const;

        Vector3f m_kVertex;
        Vector3f m_kNormal;
        Vector2f m_kTexture;
    };

private:
    typedef std::list<UInt16>                       PolygonList;
    typedef std::vector<VMAP>                       VMAPArray;
    typedef std::vector<VMAD>                       VMADArray;
    typedef std::vector<Vector2f>                   Vector2Array;
    typedef std::vector<Vector3f>                   Vector3Array;
    typedef std::vector<PolygonList* >              PolygonListArray;
    typedef std::vector<Triangle>                   TriangleArray;
    typedef std::vector<Texture* >                  TextureArray;
    typedef std::vector<VertexAttribute>            VertexAttributeArray;
    typedef std::map<UInt16, PolygonList* >         TagMap;
    typedef std::map<UInt16, UInt16>                PolygonMap;
    typedef std::map<int, int>                      VertexMap;
    typedef std::map<std::string, Vector2f* >       Vector2ArrayMap;
    typedef std::map<std::string, Vector3f* >       Vector3ArrayMap;
    typedef std::map<std::string, int* >            IntArrayMap;
    typedef std::map<std::string, PolygonList* >    PolygonListMap;
    typedef std::map<std::string, PolygonMap* >     PolygonTagMap;
    typedef std::map<std::string, VertexMap* >      SurfaceVertexMap;
    typedef std::map<std::string, VMAPArray* >      VMAPArrayMap;
    typedef std::map<std::string, VMADArray* >      VMADArrayMap;
    typedef std::map<std::string, float>            SmoothingAngleMap;
    typedef std::map<std::string, TriMesh* >        SurfaceMap;
    typedef std::map<UInt16, TriMesh* >             SurfaceTagMap;
    typedef std::map<VertexAttribute, int>          VertexAttributeMap;

    void Initialize ();
    Node* BuildObject ();
    void ObjectStats ();
    UInt32 ReadChunk (UInt32 uiFileSize);
    bool ParseChunk (UInt32 uiChunk, UInt32& ruiChunkSize);

    bool ParseTAGS (UInt32 uiChunkSize);
    bool ParseLAYR (UInt32 uiChunkSize);
    bool ParsePNTS (UInt32 uiChunkSize);
    bool ParseBBOX (UInt32 uiChunkSize);
    bool ParseVMAP (UInt32& ruiChunkSize);
    bool ParseVMAD (UInt32& ruiChunkSize);
    bool ParsePOLS (UInt32 uiChunkSize);
    bool ParsePTAG (UInt32 uiChunkSize);
    bool ParseCLIP (UInt32& ruiChunkSize);
    bool ParseSURF (UInt32& ruiChunkSize);

    bool ParseSURFSubchunk (UInt16& rusSubSize, int iIndent);
    UInt16 ParseBLOKSubchunk (int iIndent, bool& rbSupported);
    UInt16 ParseBLOKHeader (int iIndent);
    UInt16 ParseBLOKHeaderComponent (int iIndent);
    UInt16 ParseIMAPSubchunk (UInt16 usSizeIMAP, int iIndent);
    UInt16 ParseTMAPSubchunk (int iIndent);

    bool AddTexture (const std::string& rkTextureName,
        const std::string& rkBaseDirName);
    void BuildTriMesh ();
    void ApplyVertexMap (const std::string& rkName);
    void ApplyDiscontinuousVertexMap (const std::string& rkName);
    void ComputeNormals ();
    Vector3f ComputePlaneNormal (const Vector3f& rkV0, const Vector3f& rkV1,
        const Vector3f& rkV2) const;
    void ExpandGeometry ();
    void CreateVertexNormals ();
    void CompressGeometry ();

    void PrintIndent (int iIndent);
    void PrintChunkType (UInt32 uiNumber);
    void PrintChunkInfo (UInt32 uiChunkType, UInt16 usChunkSize, int iIndent);


    bool m_bDisplayParsing;
    bool m_bSmoothed;
    ParseError m_eError;

    std::string m_kLWOFileName;
    std::string m_kLWODirName;
    std::string m_kLWOPathName;

    MemFile* m_pkLWOFile;
    std::ofstream* m_pkOut;

    NodePtr m_spkLWObject;
    TriMesh* m_pkCurrentMesh;
    std::string m_kCurrentSurface;
    ColorRGBA m_kCurrentColor;
    TexturePtr m_pkCurrentTexture;
    PolygonList* m_pkCurrentPolyList;
    int m_iNumBLOKs;

    std::string m_kName;
    UInt16 m_usNumber;
    UInt16 m_usParent;
    Vector3f m_kPivot;
    float m_fScale;

    Vector3Array m_akVertices;
    Vector3f m_kMinPoint;
    Vector3f m_kMaxPoint;

    TriangleArray m_akPolygons;
    PolygonListArray m_akVertexToPolygon;
    SurfaceMap m_kSurfaces;
    SurfaceTagMap m_kTagSurfaces;
    TagMap m_kTagMap;
    PolygonMap m_kPolygonTags;
    PolygonListMap m_kPolygonLists;
    PolygonTagMap m_kPolygonMaps;
    SurfaceVertexMap m_kVertexMaps;
    SmoothingAngleMap m_kAngles;
    VMAPArrayMap m_kMaps;
    VMADArrayMap m_kDiscontMaps;
    TextureArray m_akTextures;
    Vector3ArrayMap m_kMapVertices;
    Vector3ArrayMap m_kMapNormals;
    Vector2ArrayMap m_kMapTextures;
    IntArrayMap m_kMapConnectivity;

    static const char ms_cTab;
    static const char ms_cDirSeparator;
    static const std::string ms_kCurDirToken;
};

#include "LightWaveReader.inl"

#endif

