//--------------------------------------------------------------------------------------------------------------------//
//                                                TGA TEXTURE LOADER                                                  //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "TexIoTGA.h"

#include  "EngineTexture.h"
#include  "FileC.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI(CTexIoTGA,CTextureIO);

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

CTexIoTGA::CTexIoTGA()
  {}

CTexIoTGA::~CTexIoTGA()
  {}

//--------------------------------------------------------------------------------------------------------------------//
//                                                STREAMING                                                           //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- Load ---------------------------------------------------------------//
// load texture from file
// in : texture,base dir,file name (including subdir)
// out: 0 or error ID
//--------------------------------------------------------------------------------------------------------------------//

u32 CTexIoTGA::Load(CEngineTexture* pTex,const CStdString& strDir,const CStdString& strFile)
  {
  // replace or add TGA extension

  char szDrive[_MAX_DRIVE];
  char szDir  [_MAX_DIR];
  char szName [_MAX_FNAME];
  char szExt  [_MAX_EXT];
  char szFile [_MAX_PATH];

  CStdString strName(strDir+strFile);
  _splitpath(strName.c_str(),szDrive,szDir,szName,szExt);
  _makepath ( szFile,        szDrive,szDir,szName,".TGA");

  // load file

  File Loader;
  m_pu8File = (u8*)Loader.Load(szFile,&m_u32Size);
  if(!m_pu8File) return -1;

  if(m_u32Size < sizeof(TGAHEADER))
    {
    delete [] m_pu8File;
    m_pu8File = NULL;
    return -2;
    }

  // parse file

  u32 u32Res = Load(pTex,m_pu8File,m_u32Size);
  if(FAILED(u32Res))
    {
    delete [] m_pu8File;
    m_pu8File = NULL;
    return -3;
    }

  // OK

  pTex->SetName    (strFile);
  pTex->SetFileName(strFile);

  delete [] m_pu8File;
  m_pu8File = NULL;
  return 0;
  }

//----------------------------------------------- Load ---------------------------------------------------------------//
// load texture from memory
// in : texture,memory file,file size
// out: 0 or error ID
//--------------------------------------------------------------------------------------------------------------------//

u32 CTexIoTGA::Load(CEngineTexture* pTex,const u8* pu8MemFile,const u32 u32Size)
  {
  if(!pu8MemFile) return -1;

  // read header

  TGAHEADER*    tgaHeader = (TGAHEADER*)pu8MemFile;
  m_u32Width  = tgaHeader->wWidth;
  m_u32Height = tgaHeader->wHeight;
  m_u32Depth  = tgaHeader->bPixelDepth;
  m_u32Bytes  =(tgaHeader->bCmapDepth + m_u32Depth) >> 3;
  m_u32Mem    = m_u32Width*m_u32Height*m_u32Bytes;

  m_boInvertY = (tgaHeader->bImageDescriptor & 32) == 0;    // lines in bottom->top order
  m_boInvertX = (tgaHeader->bImageDescriptor & 16) == 0;

  // check parameters

  if(m_u32Mem > u32Size-sizeof(TGAHEADER)) return -2;

  m_u32Format = CEngineTexture::TEXFORMAT_NONE;
  switch(m_u32Depth)
    {
    case 16 : m_u32Format = CEngineTexture::TEXFORMAT_0565; break;
    case 24 : m_u32Format = CEngineTexture::TEXFORMAT_0888; break;
    case 32 : m_u32Format = CEngineTexture::TEXFORMAT_8888; break;
    }
  if(m_u32Format == CEngineTexture::TEXFORMAT_NONE) return -3;

  // get some memory

  u32 u32Res = pTex->Create(m_u32Width,m_u32Height,m_u32Format);
  if(FAILED(u32Res)) return -4;

  // convert TGA->RAW

  u32Res = ConvertToRAW(pTex);
  if(FAILED(u32Res)) return -5;
  return 0;
  }

//----------------------------------------------- Save ---------------------------------------------------------------//
// save texture to file
// in : texture,base dir,file name (including subdir),wanted depth
// out: 0 or error ID
//--------------------------------------------------------------------------------------------------------------------//

u32 CTexIoTGA::Save(CEngineTexture* pTex,const CStdString& strDir,const CStdString& strFile,const u32 u32Depth)
  {
  // get RAW surface ptr

  if(!pTex) return -1;
  if(strFile.empty()) return -2;

  u8* pu8Dest = (u8*)pTex->GetSurfacePtr();
  if(!pu8Dest) return -3;

  if((u32Depth != 16) && (u32Depth != 24) && (u32Depth != 32)) return -4;

  // replace or add TGA extension

  char szDrive[_MAX_DRIVE];
  char szDir  [_MAX_DIR];
  char szName [_MAX_FNAME];
  char szExt  [_MAX_EXT];
  char szFile [_MAX_PATH];

  CStdString strName(strDir+strFile);
  _splitpath(strName.c_str(),szDrive,szDir,szName,szExt);
  _makepath ( szFile,        szDrive,szDir,szName,".TGA");

  // init members

  m_u32Width  = pTex->GetWidth ();
  m_u32Height = pTex->GetHeight();
  m_u32Depth  = u32Depth;
  m_u32Bytes  = u32Depth >> 3;
  m_u32Mem    = m_u32Width*m_u32Height*m_u32Bytes;

  switch(u32Depth)
    {
    case 16 : m_u32Format = CEngineTexture::TEXFORMAT_0565; break;
    case 24 : m_u32Format = CEngineTexture::TEXFORMAT_0888; break;
    case 32 : m_u32Format = CEngineTexture::TEXFORMAT_8888; break;
    }

  // get some memory

  m_u32Size = sizeof(TGAHEADER)+m_u32Mem;
  m_pu8File = new u8[m_u32Size];
  if(!m_pu8File) return -5;

  // TGA header

  TGAHEADER* pHeader   = (TGAHEADER*)m_pu8File;
  memset(pHeader,0,sizeof(TGAHEADER));
  pHeader->bImageType  = 2;                                 // true color
  pHeader->wWidth      = (WORD)m_u32Width;
  pHeader->wHeight     = (WORD)m_u32Height;
  pHeader->bPixelDepth = (BYTE)m_u32Depth;

  // convert RAW->TGA

  m_boInvertY = true;

  u32 u32Res = ConvertFromRAW(pTex);
  if(FAILED(u32Res))
    {
    delete [] m_pu8File;
    m_pu8File = NULL;
    return -6;
    }

  // save file

  File Saver;
  bool boRes = Saver.Save(szFile,m_pu8File,m_u32Size);
  delete [] m_pu8File;
  m_pu8File = NULL;

  if(!boRes) return -7;
  return 0;
  }

//----------------------------------------------- ConvertToRAW -------------------------------------------------------//
// convert m_pu8File to RAW surface
// in : destination texture
// out: 0 or error ID
//--------------------------------------------------------------------------------------------------------------------//

u32 CTexIoTGA::ConvertToRAW(CEngineTexture* pTex)
  {
  // get RAW surface ptr

  u8* pu8Dest = (u8*)pTex->GetSurfacePtr();
  if(!pu8Dest) return -1;

  // convert (and invert lines order if needed)

  BYTE bA,bR,bG,bB;
  WORD wARGB;
  u8*  pu8Src        = m_pu8File+sizeof(TGAHEADER);
  u32  u32TGADepth   = m_u32Depth;
  u32  u32DestFormat = m_u32Format;
  u32  u32DestOrder  = pTex->GetRGBAOrder();

  if(m_boInvertY) pu8Src += (m_u32Height-1)*m_u32Width*m_u32Bytes;   
                                                            // last line
  for(u32 u32Row = 0; u32Row < m_u32Height; u32Row++)
    { 
    for(u32 u32Col = 0; u32Col < m_u32Width; u32Col++)
      {
      // read TGA pixel (0565,0888,8888)

      TGA_READ;

      // write RAW pixel (same format)

      RAW_WRITE;
      }

    // next line

    if(m_boInvertY) pu8Src -= m_u32Width*m_u32Bytes*2;      // go two lines back
    }

  return 0;
  }

//----------------------------------------------- ConvertFromRAW -----------------------------------------------------//
// convert RAW surface to m_pu8File
// in : source texture
// out: 0 or error ID
//--------------------------------------------------------------------------------------------------------------------//

u32 CTexIoTGA::ConvertFromRAW(CEngineTexture* pTex)
  {
  // get RAW surface ptr

  u8* pu8Src = (u8*)pTex->GetSurfacePtr();
  if(!pu8Src) return -1;

  // convert (and invert lines order if needed)

  BYTE bA,bR,bG,bB;
  WORD wARGB;
  u8*  pu8Dest       = m_pu8File+sizeof(TGAHEADER);
  u32  u32TGADepth   = m_u32Depth;
  u32  u32DestFormat = m_u32Format;
  u32  u32SrcFormat  = pTex->GetFormat();
  u32  u32SrcOrder   = pTex->GetRGBAOrder();

  if(m_boInvertY) pu8Dest += (m_u32Height-1)*m_u32Width*m_u32Bytes;   
                                                            // last line
  for(u32 u32Row = 0; u32Row < m_u32Height; u32Row++)
    { 
    for(u32 u32Col = 0; u32Col < m_u32Width; u32Col++)
      {
      // read RAW pixel

      RAW_READ;

      // write TGA pixel

      TGA_WRITE;
      }

    // next line

    if(m_boInvertY) pu8Dest -= m_u32Width*m_u32Bytes*2;     // go two lines back
    }

  return 0;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
