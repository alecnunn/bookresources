// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Terrain.h"
#include "Wm3Camera.h"
#include "Wm3TextureEffect.h"
#include "Wm3ZBufferState.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,Terrain,Node);
WM3_IMPLEMENT_ABSTRACT_STREAM(Terrain);
WM3_IMPLEMENT_DEFAULT_NAME_ID(Terrain,Node);

//----------------------------------------------------------------------------
Terrain::Terrain (const char* acHeightPrefix, const char* acImagePrefix,
    Camera* pkCamera, float fUVBias, ColorRGBA* pkBorderColor)
    :
    m_spkCamera(pkCamera),
    m_fUVBias(fUVBias),
    m_kBorderColor(pkBorderColor ? *pkBorderColor : ColorRGBA::BLACK)
{
    assert( acHeightPrefix && acImagePrefix );
    m_iCameraRow = -1;
    m_iCameraCol = -1;

    m_fUVBias = fUVBias;

    // load global terrain information
    LoadHeader(acHeightPrefix);

    // load terrain pages
    System::Allocate(m_iCols,m_iRows,m_aaspkPage);
    int iRow, iCol;
    for (iRow = 0; iRow < m_iRows; iRow++)
    {
        for (iCol = 0; iCol < m_iCols; iCol++)
        {
            char acName[64];
            sprintf(acName,"%d.%d",iRow,iCol);
            LoadPage(iRow,iCol,acHeightPrefix,acImagePrefix,acName,acName);
        }
    }

    // attach the terrain pages to the terrain node
    m_kChild.SetMaxQuantity(m_iRows*m_iCols,false);
    SetGlobalState(new ZBufferState);
    for (iRow = 0; iRow < m_iRows; iRow++)
    {
        for (iCol = 0; iCol < m_iCols; iCol++)
            AttachChild(m_aaspkPage[iRow][iCol]);
    }
}
//----------------------------------------------------------------------------
Terrain::Terrain ()
    :
    m_kBorderColor(ColorRGBA::BLACK)
{
    m_iRows = 0;
    m_iCols = 0;
    m_usSize = 0;
    m_fMinElevation = 0.0f;
    m_fMaxElevation = 0.0f;
    m_fSpacing = 0.0f;;
    m_aaspkPage = 0;
    m_iCameraRow = -1;
    m_iCameraCol = -1;
    m_fUVBias = 0.0f;
}
//----------------------------------------------------------------------------
Terrain::~Terrain ()
{
    for (int iRow = 0; iRow < m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < m_iCols; iCol++)
            m_aaspkPage[iRow][iCol] = 0;
    }
    System::Deallocate(m_aaspkPage);
}
//----------------------------------------------------------------------------
TerrainPage* Terrain::GetPage (int iRow, int iCol)
{
    assert( 0 <= iRow && iRow < m_iRows && 0 <= iCol && iCol < m_iCols );
    if ( 0 <= iRow && iRow < m_iRows && 0 <= iCol && iCol < m_iCols )
        return m_aaspkPage[iRow][iCol];

    return 0;
}
//----------------------------------------------------------------------------
TerrainPage* Terrain::GetCurrentPage (float fX, float fY) const
{
    float fInvLength = 1.0f/(m_fSpacing*(m_usSize-1));

    int iCol = (int)Mathf::Floor(fX*fInvLength);
    iCol %= m_iCols;
    if ( iCol < 0 )
        iCol += m_iCols;

    int iRow = (int)Mathf::Floor(fY*fInvLength);
    iRow %= m_iRows;
    if ( iRow < 0 )
        iRow += m_iRows;

    return m_aaspkPage[iRow][iCol];
}
//----------------------------------------------------------------------------
float Terrain::GetHeight (float fX, float fY) const
{
    TerrainPage* pkPage = GetCurrentPage(fX,fY);

    // subtract off the translation due to wrap-around
    fX -= pkPage->Local.GetTranslate().X();
    fY -= pkPage->Local.GetTranslate().Y();

    return pkPage->GetHeight(fX,fY);
}
//----------------------------------------------------------------------------
void Terrain::LoadHeader (const char* acHeightPrefix)
{
    // Load the data into temporary variables.  EndianCopy uses memcpy, so
    // if you were to load directly into the class members and use memcpy,
    // the source and destination overlap, in which case memcpy results are
    // unpredictable.
    char acFileName[512];
    sprintf(acFileName,"%s.wmhf",acHeightPrefix);
    FILE* pkHeader = fopen(acFileName,"rb");
    assert( pkHeader );
    System::Read4(pkHeader,1,&m_iRows);
    System::Read4(pkHeader,1,&m_iCols);
    System::Read2(pkHeader,1,&m_usSize);
    System::Read4(pkHeader,1,&m_fMinElevation);
    System::Read4(pkHeader,1,&m_fMaxElevation);
    System::Read4(pkHeader,1,&m_fSpacing);
    fclose(pkHeader);
}
//----------------------------------------------------------------------------
void Terrain::LoadData (const char* acHeightPrefix,
    const char* acImagePrefix, const char* acHeightName,
    const char* acImageName, unsigned short*& rausHeight, Image*& rpkImage)
{
    int iQuantity = (int)(m_usSize*m_usSize);
    rausHeight = new unsigned short[iQuantity];

    char acFileName[512];
    sprintf(acFileName,"%s.%s.wmhf",acHeightPrefix,acHeightName);
    FILE* pkHeightFile = fopen(acFileName,"rb");
    assert ( pkHeightFile );
    if ( pkHeightFile )
    {
        System::Read2(pkHeightFile,iQuantity,rausHeight);
        fclose(pkHeightFile);
    }
    else
    {
        memset(rausHeight,0,iQuantity*sizeof(unsigned short));
    }

    sprintf(acFileName,"%s.%s.wmif",acImagePrefix,acImageName);
    rpkImage = Image::Load(acFileName);
    if ( !rpkImage )
    {
        int iWidth = 4, iHeight = 4;
        unsigned char* aucData = new unsigned char[iWidth*iHeight];
        memset(aucData,0,iWidth*iHeight*sizeof(unsigned char));
        rpkImage = new Image(Image::IT_RGB888,iWidth,iHeight,aucData);
    }
}
//----------------------------------------------------------------------------
void Terrain::LoadPage (int iRow, int iCol, const char* acHeightPrefix,
    const char* acImagePrefix, const char* acHeightName,
    const char* acImageName)
{
    unsigned short* ausHeight;
    Image* pkImage;
    LoadData(acHeightPrefix,acImagePrefix,acHeightName,acImageName,
        ausHeight,pkImage);

    float fLength = m_fSpacing*(m_usSize-1);
    Vector2f kOrigin(iCol*fLength,iRow*fLength);
    TerrainPage* pkPage = new TerrainPage(m_usSize,ausHeight,kOrigin,
        m_fMinElevation,m_fMaxElevation,m_fSpacing,m_fUVBias);

    Texture* pkTexture = new Texture(pkImage);
    pkTexture->CoordU = Texture::WM_CLAMP_EDGE;
    pkTexture->CoordV = Texture::WM_CLAMP_EDGE;
    Effect* pkEffect = new TextureEffect(pkTexture,pkPage->GetUVs());
    pkPage->SetEffect(pkEffect);

    m_aaspkPage[iRow][iCol] = pkPage;
}
//----------------------------------------------------------------------------
TerrainPagePtr Terrain::ReplacePage (int iRow, int iCol,
    const char* acHeightPrefix, const char* acImagePrefix,
    const char* acHeightName, const char* acImageName)
{
    assert( 0 <= iRow && iRow < m_iRows && 0 <= iCol && iCol < m_iCols );
    if ( 0 <= iRow && iRow < m_iRows && 0 <= iCol && iCol < m_iCols )
    {
        TerrainPagePtr spkSave = m_aaspkPage[iRow][iCol];
        LoadPage(iRow,iCol,acHeightPrefix,acImagePrefix,acHeightName,
            acImageName);
        return spkSave;
    }
    return 0;
}
//----------------------------------------------------------------------------
TerrainPagePtr Terrain::ReplacePage (int iRow, int iCol,
    TerrainPage* pkNewPage)
{
    assert( 0 <= iRow && iRow < m_iRows && 0 <= iCol && iCol < m_iCols );
    if ( 0 <= iRow && iRow < m_iRows && 0 <= iCol && iCol < m_iCols )
    {
        TerrainPagePtr spkSave = m_aaspkPage[iRow][iCol];
        m_aaspkPage[iRow][iCol] = pkNewPage;
        return spkSave;
    }
    return 0;
}
//----------------------------------------------------------------------------
void Terrain::OnCameraMotion ()
{
    assert( m_spkCamera );
    if ( !m_spkCamera )
        return;

    // get camera location/direction in model space of terrain
    Vector3f kWorldEye = m_spkCamera->GetWorldLocation();
    Vector3f kWorldDir = m_spkCamera->GetWorldDVector();
    Vector3f kModelEye = World.ApplyInverse(kWorldEye);
    Vector3f kModelDir = kWorldDir*World.GetRotate();

    // Update the model-space origins of the terrain pages.  Start the
    // process by locating the page that contains the camera.
    float fLength = m_fSpacing*(m_usSize-1);
    float fInvLength = 1.0f/fLength;
    int iNewCameraCol = (int)Mathf::Floor(kModelEye.X()*fInvLength);
    int iNewCameraRow = (int)Mathf::Floor(kModelEye.Y()*fInvLength);
    if ( iNewCameraCol != m_iCameraCol || iNewCameraRow != m_iCameraRow )
    {
        m_iCameraCol = iNewCameraCol;
        m_iCameraRow = iNewCameraRow;

        // translate page origins for toroidal wraparound
        int iCMinO = m_iCameraCol - m_iCols/2;
        int iCMinP = iCMinO % m_iCols;
        if ( iCMinP < 0 )
            iCMinP += m_iCols;

        int iRMinO = m_iCameraRow - m_iRows/2;
        int iRMinP = iRMinO % m_iRows;
        if ( iRMinP < 0 )
            iRMinP += m_iRows;

        int iRO = iRMinO, iRP = iRMinP;
        for (int iRow = 0; iRow < m_iRows; iRow++)
        {
            int iCO = iCMinO, iCP = iCMinP;
            for (int iCol = 0; iCol < m_iCols; iCol++)
            {
                TerrainPage* pkPage = m_aaspkPage[iRP][iCP];
                Vector2f kOldOrigin = pkPage->GetOrigin();
                Vector2f kNewOrigin(iCO*fLength,iRO*fLength);
                Vector2f kTrn = kNewOrigin - kOldOrigin;
                Vector3f kPTrn(kTrn.X(),kTrn.Y(),
                    pkPage->Local.GetTranslate().Z());
                pkPage->Local.SetTranslate(kPTrn);

                iCO++;
                if ( ++iCP == m_iCols )
                    iCP = 0;
            }

            iRO++;
            if ( ++iRP == m_iRows )
                iRP = 0;
        }
        UpdateGS();
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Terrain::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Node::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iRows);
    rkStream.Read(m_iCols);
    rkStream.Read(m_usSize);
    rkStream.Read(m_fMinElevation);
    rkStream.Read(m_fMaxElevation);
    rkStream.Read(m_fSpacing);
    rkStream.Read(m_iCameraRow);
    rkStream.Read(m_iCameraCol);
    rkStream.Read(m_fUVBias);
    rkStream.Read(m_kBorderColor);

    // link data
    Object* pkObject;
    rkStream.Read(pkObject);  // m_spkCamera
    for (int iRow = 0; iRow < m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < m_iCols; iCol++)
        {
            rkStream.Read(pkObject);  // m_aaspkPage[iRow][iCol]
            pkLink->Add(pkObject);
        }
    }

    WM3_END_DEBUG_STREAM_LOAD(Terrain);
}
//----------------------------------------------------------------------------
void Terrain::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Node::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    m_spkCamera = (Camera*)rkStream.GetFromMap(pkLinkID);

    System::Allocate(m_iCols,m_iRows,m_aaspkPage);
    for (int iRow = 0; iRow < m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < m_iCols; iCol++)
        {
            pkLinkID = pkLink->GetLinkID();
            m_aaspkPage[iRow][iCol] =
                (TerrainPage*)rkStream.GetFromMap(pkLinkID);
        }
    }
}
//----------------------------------------------------------------------------
bool Terrain::Register (Stream& rkStream) const
{
    if ( !Node::Register(rkStream) )
        return false;

    if ( m_spkCamera )
        m_spkCamera->Register(rkStream);

    for (int iRow = 0; iRow < m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < m_iCols; iCol++)
            m_aaspkPage[iRow][iCol]->Register(rkStream);
    }

    return true;
}
//----------------------------------------------------------------------------
void Terrain::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Node::Save(rkStream);

    // native data
    rkStream.Write(m_iRows);
    rkStream.Write(m_iCols);
    rkStream.Write(m_usSize);
    rkStream.Write(m_fMinElevation);
    rkStream.Write(m_fMaxElevation);
    rkStream.Write(m_fSpacing);
    rkStream.Write(m_iCameraRow);
    rkStream.Write(m_iCameraCol);
    rkStream.Write(m_fUVBias);
    rkStream.Write(m_kBorderColor);

    // link data
    rkStream.Write(m_spkCamera);
    for (int iRow = 0; iRow < m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < m_iCols; iCol++)
            rkStream.Write(m_aaspkPage[iRow][iCol]);
    }

    WM3_END_DEBUG_STREAM_SAVE(Terrain);
}
//----------------------------------------------------------------------------
StringTree* Terrain::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(11,1+m_iRows*m_iCols);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("rows =",m_iRows));
    pkTree->SetString(2,StringTree::Format("cols =",m_iCols));
    pkTree->SetString(3,StringTree::Format("size =",m_usSize));
    pkTree->SetString(4,StringTree::Format("min elevation =",
        m_fMinElevation));
    pkTree->SetString(5,StringTree::Format("max elevation =",
        m_fMaxElevation));
    pkTree->SetString(6,StringTree::Format("spacing =",m_fSpacing));
    pkTree->SetString(7,StringTree::Format("camera row =",m_iCameraRow));
    pkTree->SetString(8,StringTree::Format("camera col =",m_iCameraCol));
    pkTree->SetString(9,StringTree::Format("uv bias =",m_fUVBias));
    pkTree->SetString(10,StringTree::Format("border color =",m_kBorderColor));

    // children
    pkTree->SetChild(0,Node::SaveStrings());
    int j = 1;
    for (int iRow = 0; iRow < m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < m_iCols; iCol++)
        {
            char acPageName[64];
            sprintf(acPageName,"page[%d][%d] =",iRow,iCol);
            pkTree->SetChild(j++,
                m_aaspkPage[iRow][iCol]->SaveStrings(acPageName));
        }
    }

    return pkTree;
}
//----------------------------------------------------------------------------
int Terrain::GetMemoryUsed () const
{
    return sizeof(Terrain) - sizeof(Node) + Node::GetMemoryUsed() +
        m_iRows*sizeof(TerrainPage**) +
        m_iRows*m_iCols*sizeof(TerrainPage*);
}
//----------------------------------------------------------------------------
int Terrain::GetDiskUsed () const
{
    return Node::GetDiskUsed() +
        sizeof(m_iRows) +
        sizeof(m_iCols) +
        sizeof(m_usSize) +
        sizeof(m_fMinElevation) +
        sizeof(m_fMaxElevation) +
        sizeof(m_fSpacing) +
        sizeof(m_iCameraRow) +
        sizeof(m_iCameraCol) +
        sizeof(m_fUVBias) +
        sizeof(m_kBorderColor) +
        sizeof(m_spkCamera) +
        m_iRows*m_iCols*sizeof(m_aaspkPage[0][0]);
}
//----------------------------------------------------------------------------
