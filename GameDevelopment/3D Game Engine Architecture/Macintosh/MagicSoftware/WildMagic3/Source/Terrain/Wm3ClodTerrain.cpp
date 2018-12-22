// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ClodTerrain.h"
#include "Wm3TextureEffect.h"
#include "Wm3ZBufferState.h"
#include "Wm3Camera.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,ClodTerrain,Node);
WM3_IMPLEMENT_ABSTRACT_STREAM(ClodTerrain);
WM3_IMPLEMENT_DEFAULT_NAME_ID(ClodTerrain,Node);

//----------------------------------------------------------------------------
ClodTerrain::ClodTerrain (const char* acHeightPrefix,
    const char* acImagePrefix, Renderer* pkRenderer, float fUVBias,
    ColorRGBA* pkBorderColor)
    :
    m_fUVBias(fUVBias),
    m_kBorderColor(pkBorderColor ? *pkBorderColor : ColorRGBA::BLACK)
{
    assert( acHeightPrefix && acImagePrefix && pkRenderer );
    m_pkRenderer = pkRenderer;
    m_iCameraRow = -1;
    m_iCameraCol = -1;

    m_bCloseAssumption = false;
    m_fPixelTolerance = 1.0f;

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

    StitchAll();
    Simplify();
}
//----------------------------------------------------------------------------
ClodTerrain::ClodTerrain ()
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
    m_fPixelTolerance = 0.0f;
    m_pkRenderer = 0;
    m_bCloseAssumption = false;
    m_iCameraRow = -1;
    m_iCameraCol = -1;
    m_fUVBias = 0.0f;
}
//----------------------------------------------------------------------------
ClodTerrain::~ClodTerrain ()
{
    for (int iRow = 0; iRow < m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < m_iCols; iCol++)
            m_aaspkPage[iRow][iCol] = 0;
    }
    System::Deallocate(m_aaspkPage);
}
//----------------------------------------------------------------------------
void ClodTerrain::SetPixelTolerance (float fTolerance)
{
    m_fPixelTolerance = fTolerance;

    for (int iRow = 0; iRow < m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < m_iCols; iCol++)
        {
            m_aaspkPage[iRow][iCol]->SetPixelTolerance(m_pkRenderer,
                m_fPixelTolerance);
        }
    }
}
//----------------------------------------------------------------------------
ClodTerrainPage* ClodTerrain::GetPage (int iRow, int iCol)
{
    assert( 0 <= iRow && iRow < m_iRows && 0 <= iCol && iCol < m_iCols );
    if ( 0 <= iRow && iRow < m_iRows && 0 <= iCol && iCol < m_iCols )
        return m_aaspkPage[iRow][iCol];

    return 0;
}
//----------------------------------------------------------------------------
ClodTerrainPage* ClodTerrain::GetCurrentPage (float fX, float fY) const
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
float ClodTerrain::GetHeight (float fX, float fY) const
{
    ClodTerrainPage* pkPage = GetCurrentPage(fX,fY);

    // subtract off the translation due to wrap-around
    fX -= pkPage->Local.GetTranslate().X();
    fY -= pkPage->Local.GetTranslate().Y();

    return pkPage->GetHeight(fX,fY);
}
//----------------------------------------------------------------------------
void ClodTerrain::LoadHeader (const char* acHeightPrefix)
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
void ClodTerrain::LoadData (const char* acHeightPrefix,
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
void ClodTerrain::LoadPage (int iRow, int iCol, const char* acHeightPrefix,
    const char* acImagePrefix, const char* acHeightName,
    const char* acImageName)
{
    unsigned short* ausHeight;
    Image* pkImage;
    LoadData(acHeightPrefix,acImagePrefix,acHeightName,acImageName,
        ausHeight,pkImage);

    float fLength = m_fSpacing*(m_usSize-1);
    Vector2f kOrigin(iCol*fLength,iRow*fLength);
    ClodTerrainPage* pkPage = new ClodTerrainPage(m_usSize,ausHeight,kOrigin,
        m_fMinElevation,m_fMaxElevation,m_fSpacing,m_fUVBias);

    Texture* pkTexture = new Texture(pkImage);
    pkTexture->CoordU = Texture::WM_CLAMP_EDGE;
    pkTexture->CoordV = Texture::WM_CLAMP_EDGE;
    Effect* pkEffect = new TextureEffect(pkTexture,pkPage->GetUVs());
    pkPage->SetEffect(pkEffect);

    m_aaspkPage[iRow][iCol] = pkPage;
}
//----------------------------------------------------------------------------
ClodTerrainPagePtr ClodTerrain::ReplacePage (int iRow, int iCol,
    const char* acHeightPrefix, const char* acImagePrefix,
    const char* acHeightName, const char* acImageName)
{
    assert( 0 <= iRow && iRow < m_iRows && 0 <= iCol && iCol < m_iCols );
    if ( 0 <= iRow && iRow < m_iRows && 0 <= iCol && iCol < m_iCols )
    {
        ClodTerrainPagePtr spkSave = m_aaspkPage[iRow][iCol];
        LoadPage(iRow,iCol,acHeightPrefix,acImagePrefix,acHeightName,
            acImageName);
        return spkSave;
    }
    return 0;
}
//----------------------------------------------------------------------------
ClodTerrainPagePtr ClodTerrain::ReplacePage (int iRow, int iCol,
    ClodTerrainPage* pkNewPage)
{
    assert( 0 <= iRow && iRow < m_iRows && 0 <= iCol && iCol < m_iCols );
    if ( 0 <= iRow && iRow < m_iRows && 0 <= iCol && iCol < m_iCols )
    {
        ClodTerrainPagePtr spkSave = m_aaspkPage[iRow][iCol];
        m_aaspkPage[iRow][iCol] = pkNewPage;
        return spkSave;
    }
    return 0;
}
//----------------------------------------------------------------------------
void ClodTerrain::Simplify ()
{
    // get camera location/direction in model space of terrain
    const Camera* pkCamera = m_pkRenderer->GetCamera();
    Vector3f kWorldEye = pkCamera->GetWorldLocation();
    Vector3f kWorldDir = pkCamera->GetWorldDVector();
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
                ClodTerrainPage* pkPage = m_aaspkPage[iRP][iCP];
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

    // initialize the pages
    int iRow, iCol;
    for (iRow = 0; iRow < m_iRows; iRow++)
    {
        for (iCol = 0; iCol < m_iCols; iCol++)
            m_aaspkPage[iRow][iCol]->ResetBlocks();
    }

    // Simplify the pages.  The pages are attached to the ClodTerrain parent
    // and should have the identity transformation for local transforms.  The
    // camera can be placed in the model space of the ClodTerrain parent, a
    // calculation that occurs outside the double loop.
    for (iRow = 0; iRow < m_iRows; iRow++)
    {
        for (iCol = 0; iCol < m_iCols; iCol++)
        {
            ClodTerrainPage* pkPage = m_aaspkPage[iRow][iCol];
            if ( pkPage->IntersectFrustum(pkCamera) )
            {
                pkPage->Simplify(m_pkRenderer,kModelEye,kModelDir,
                    m_bCloseAssumption);
            }
        }
    }
}
//----------------------------------------------------------------------------
void ClodTerrain::StitchAll ()
{
    // stitch the pages together
    int iRow, iCol;
    for (iRow = 0; iRow < m_iRows; iRow++)
    {
        for (iCol = 0; iCol+1 < m_iCols; iCol++)
            m_aaspkPage[iRow][iCol]->StitchNextCol(m_aaspkPage[iRow][iCol+1]);
    }

    for (iCol = 0; iCol < m_iCols; iCol++)
    {
        for (iRow = 0; iRow+1 < m_iRows; iRow++)
            m_aaspkPage[iRow][iCol]->StitchNextRow(m_aaspkPage[iRow+1][iCol]);
    }

    // Use toroidal connectivity.  An application is not required to have a
    // toroidal world.  New pages may be loaded and old ones discarded as
    // the camera moves about the world.
    int iColsM1 = m_iCols-1;
    for (iRow = 0; iRow < m_iRows; iRow++)
        m_aaspkPage[iRow][iColsM1]->StitchNextCol(m_aaspkPage[iRow][0]);

    int iRowsM1 = m_iRows-1;
    for (iCol = 0; iCol < m_iCols; iCol++)
        m_aaspkPage[iRowsM1][iCol]->StitchNextRow(m_aaspkPage[0][iCol]);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void ClodTerrain::Load (Stream& rkStream, Stream::Link* pkLink)
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
    rkStream.Read(m_fPixelTolerance);
    rkStream.Read(m_bCloseAssumption);
    rkStream.Read(m_iCameraRow);
    rkStream.Read(m_iCameraCol);
    rkStream.Read(m_fUVBias);
    rkStream.Read(m_kBorderColor);

    // link data
    for (int iRow = 0; iRow < m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < m_iCols; iCol++)
        {
            Object* pkObject;
            rkStream.Read(pkObject);  // m_aaspkPage[iRow][iCol]
            pkLink->Add(pkObject);
        }
    }

    WM3_END_DEBUG_STREAM_LOAD(ClodTerrain);
}
//----------------------------------------------------------------------------
void ClodTerrain::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Node::Link(rkStream,pkLink);

    System::Allocate(m_iCols,m_iRows,m_aaspkPage);
    for (int iRow = 0; iRow < m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < m_iCols; iCol++)
        {
            Object* pkLinkID = pkLink->GetLinkID();
            m_aaspkPage[iRow][iCol] =
                (ClodTerrainPage*)rkStream.GetFromMap(pkLinkID);
        }
    }

    StitchAll();

    // After loading a ClodTerrain, you need to SetRenderer(...) with the
    // current renderer, and then call Simplify().
}
//----------------------------------------------------------------------------
bool ClodTerrain::Register (Stream& rkStream) const
{
    if ( !Node::Register(rkStream) )
        return false;

    for (int iRow = 0; iRow < m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < m_iCols; iCol++)
            m_aaspkPage[iRow][iCol]->Register(rkStream);
    }

    return true;
}
//----------------------------------------------------------------------------
void ClodTerrain::Save (Stream& rkStream) const
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
    rkStream.Write(m_fPixelTolerance);
    rkStream.Write(m_bCloseAssumption);
    rkStream.Write(m_iCameraRow);
    rkStream.Write(m_iCameraCol);
    rkStream.Write(m_fUVBias);
    rkStream.Write(m_kBorderColor);

    // link data
    for (int iRow = 0; iRow < m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < m_iCols; iCol++)
            rkStream.Write(m_aaspkPage[iRow][iCol]);
    }

    WM3_END_DEBUG_STREAM_SAVE(ClodTerrain);
}
//----------------------------------------------------------------------------
StringTree* ClodTerrain::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(13,1+m_iRows*m_iCols);

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
    pkTree->SetString(7,StringTree::Format("pixel tolerance =",
        m_fPixelTolerance));
    pkTree->SetString(8,StringTree::Format("close assumption =",
        m_bCloseAssumption));
    pkTree->SetString(9,StringTree::Format("camera row =",m_iCameraRow));
    pkTree->SetString(10,StringTree::Format("camera col =",m_iCameraCol));
    pkTree->SetString(11,StringTree::Format("uv bias =",m_fUVBias));
    pkTree->SetString(12,StringTree::Format("border color =",m_kBorderColor));

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
int ClodTerrain::GetMemoryUsed () const
{
    return sizeof(ClodTerrain) - sizeof(Node) + Node::GetMemoryUsed() +
        m_iRows*sizeof(ClodTerrainPage**) +
        m_iRows*m_iCols*sizeof(ClodTerrainPage*);
}
//----------------------------------------------------------------------------
int ClodTerrain::GetDiskUsed () const
{
    return Node::GetDiskUsed() +
        sizeof(m_iRows) +
        sizeof(m_iCols) +
        sizeof(m_usSize) +
        sizeof(m_fMinElevation) +
        sizeof(m_fMaxElevation) +
        sizeof(m_fSpacing) +
        sizeof(m_fPixelTolerance) +
        sizeof(m_bCloseAssumption) +
        sizeof(m_iCameraRow) +
        sizeof(m_iCameraCol) +
        sizeof(m_fUVBias) +
        sizeof(m_kBorderColor) +
        m_iRows*m_iCols*sizeof(m_aaspkPage[0][0]);
}
//----------------------------------------------------------------------------
