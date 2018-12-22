//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE TEXTURE CLASS                                                //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "EngineTexture.h"
#ifdef    _DEBUG
#include  "EngineTexture.inl"
#endif

#include  "Properties.h"
#include  "VectorMatrix.h"

#include  "FindFileC.h"
#include  "TexIoTGA.h"

#include  "Engine.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI_PROP (CEngineTexture,CEngineObj);
IMPLEMENT_PROPERTIES(CEngineTexture,CExtraProp);

u32         CEngineTexture::m_u32UsedRAM        = 0;        // total RAM   used
u32         CEngineTexture::m_u32UsedVideo      = 0;        // total video used

const char* CEngineTexture::m_pszFormat[]       = { "0000","0555","0565","0888","1555","4444","8888" };

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

CEngineTexture::CEngineTexture() : m_u32Type(TEXTYPE_RAW), m_u32RGBAOrder(ORDER_BGRA)
  {                                                         // BGRA is Windows default
  Init();
  }

//

CEngineTexture::~CEngineTexture()
  {
  IRenderer* pRenderer = CEngine::GetEngine()->m_spRenderer;
  if(pRenderer) pRenderer->RemoveTexture(this);             // remove from map

  Destroy();
  }

//----------------------------------------------- Init ---------------------------------------------------------------//
// initialize "this" object
// in :
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CEngineTexture::Init()
  {
  m_strFileName = "";

  m_u32Width    = m_u32Height = m_u32Depth = 0;
  m_u32Format   = TEXFORMAT_NONE;
  m_u32MemBytes = 0;

  m_pu8Video    = m_pu8RAM  = NULL;
  m_boInVideo   = m_boInRAM = false;
  }

//----------------------------------------------- Destroy ------------------------------------------------------------//
// clean "this" object (release surface)
// in :
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CEngineTexture::Destroy()
  {
  Unload();

  if(m_boInRAM)   m_u32UsedRAM   -= m_u32MemBytes;
  if(m_boInVideo) m_u32UsedVideo -= m_u32MemBytes;

  delete [] m_pu8RAM;
  Init();
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                PROPERTIES                                                          //
//--------------------------------------------------------------------------------------------------------------------//

bool CEngineTexture::DefineProperties()                     // static
  {
  CProperty* pProp = REGISTER_PROP(String,CEngineTexture,m_strFileName,"File",CProperty::EXPOSE|CProperty::STREAM,"");
  pProp->SetCtrlType(CProperty::BUTTON);

  REGISTER_PROP(U32,CEngineTexture,m_u32Width, "Width", CProperty::EXPOSE|CProperty::READONLY,"pixels");
  REGISTER_PROP(U32,CEngineTexture,m_u32Height,"Height",CProperty::EXPOSE|CProperty::READONLY,"pixels");
  REGISTER_PROP(U32,CEngineTexture,m_u32Depth, "Depth", CProperty::EXPOSE|CProperty::READONLY,"bits");
  return true;
  }

//

bool CEngineTexture::PostRead()
  {
  CEngineObj::PostRead();

  if(m_strFileName.empty()) return true;
  if(m_boInRAM || m_boInVideo) return true;
  return(0 == LoadTexture(m_strFileName));
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                SURFACE OPERATIONS                                                  //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- Create -------------------------------------------------------------//
// create RAM surface
// in : w*h size,format
// out: 0 or error ID
//--------------------------------------------------------------------------------------------------------------------//

u32 CEngineTexture::Create(const u32 u32Width,const u32 u32Height,const u32 u32Format)
  {
  // check params

  if(u32Format >= TEXFORMAT_LAST) return -1;

  u32 u32Depth = TEXFORMAT_NONE;
  switch(u32Format)
    {
    case TEXFORMAT_0555 :
    case TEXFORMAT_0565 :
    case TEXFORMAT_1555 :
    case TEXFORMAT_4444 : u32Depth = 16; break;
    case TEXFORMAT_0888 : u32Depth = 24; break;
    case TEXFORMAT_8888 : u32Depth = 32; break;
    }

  if(TEXFORMAT_NONE == u32Depth) return -2;

  // OK

  Destroy();                                                // release old data if any

  m_strName     = "";
  m_strFileName = "";

  m_u32Width    = u32Width;
  m_u32Height   = u32Height;
  m_u32Depth    = u32Depth;
  m_u32Format   = u32Format;
  m_u32MemBytes = m_u32Width*m_u32Height*(m_u32Depth >> 3);

  m_pu8RAM      = NULL;
  m_pu8Video    = NULL;
  m_boInRAM     = false;
  m_boInVideo   = false;

  // create RAM surface

  m_pu8RAM  = new u8[m_u32MemBytes];
  if(!m_pu8RAM) return -3;
  m_boInRAM = true;

  m_u32UsedRAM += m_u32MemBytes;
  return 0;
  }

//----------------------------------------------- Clear --------------------------------------------------------------//
// clean "this" object
// in : color
// out: 0 or error ID
//--------------------------------------------------------------------------------------------------------------------//

u32 CEngineTexture::Clear(const CVect4D& v4Color)
  {
  if(!m_boInRAM || !m_pu8RAM) return -1;

  // convert color

  BYTE bR,bG,bB,bA;
  WORD wARGB;
  u32  u32DestFormat = m_u32Format;
  u32  u32DestOrder  = m_u32RGBAOrder;

  RAW_CONVERT_COLOR;

  // fill surface

  u8* pu8Dest = m_pu8RAM;

  for(u32 u32Pixel = 0; u32Pixel < m_u32Width*m_u32Height; u32Pixel++)
    {
    RAW_WRITE_CONVERTED;
    }

  return 0;
  }

//----------------------------------------------- Resize -------------------------------------------------------------//
// scale texture to a given size (same depth)
// in : new w*h
// out: 0 or error ID
// rem: use ConvertRAW2RAW instead to change the format
//--------------------------------------------------------------------------------------------------------------------//

u32 CEngineTexture::Resize(const u32 u32Width,const u32 u32Height)
  {
  if(!m_boInRAM || !m_pu8RAM) return -1;

  // get new surface

  u32 u32Bytes  = m_u32Depth >> 3;
  u32 u32Mem    = u32Width*u32Height*u32Bytes;
  u8* pu8NewRAM = new u8[u32Mem];
  if(!pu8NewRAM) return -2;

  // scale

  u8* pu8New2   = pu8NewRAM;
  u8* pu8Old    = m_pu8RAM;
  u32 u32OldW   = m_u32Width;
  u32 u32OldH   = m_u32Height;
  u32 u32OldMem = m_u32MemBytes;

  m_u32Width    = u32Width;
  m_u32Height   = u32Height;
  m_u32MemBytes = u32Mem;

  float fRowStep = (float)u32OldH/m_u32Height;              // scale ratio
  float fColStep = (float)u32OldW/m_u32Width;
  float fRowAcc  = 0.f;                                     // error accumulator

  for(u32 u32Row = 0; u32Row < m_u32Height; u32Row++)
    {
    float fColAcc = 0.f;
    u8*   pu8Old2 = pu8Old;

    for(u32 u32Col = 0; u32Col < m_u32Width; u32Col++)
      {
      memcpy(pu8New2,pu8Old2,u32Bytes);                     // copy 1 texel
      pu8New2 += u32Bytes;
      fColAcc += fColStep;

      while(fColAcc >= 1.f)
        {
        fColAcc -= 1.f;
        pu8Old2 += u32Bytes;
        }
      }

    fRowAcc += fRowStep;

    while(fRowAcc >= 1.f)
      {
      fRowAcc -= 1.f;
      pu8Old  += u32OldW*u32Bytes;
      }
    }

  // switch surfaces

  delete [] m_pu8RAM;
  m_pu8RAM = pu8NewRAM;

  m_u32UsedRAM -= u32OldMem;
  m_u32UsedRAM += m_u32MemBytes;
  return 0;
  }

//----------------------------------------------- ExtendSquared ------------------------------------------------------//
// extend dimensions to squared 2^n
// in :
// out: 0 or error ID
//--------------------------------------------------------------------------------------------------------------------//

u32 CEngineTexture::ExtendSquared()
  {
  if(!m_boInRAM || !m_pu8RAM) return -1;

  u32 u32NewWidth  = m_u32Width;
  u32 u32NewHeight = m_u32Height;

  // find new dimensions

  if(u32NewWidth < u32NewHeight) u32NewWidth  = u32NewHeight;
  else                           u32NewHeight = u32NewWidth;

  u32 u32Pow = 0;
  while(u32NewWidth)
    {                                                       // find highest bit
    u32NewWidth >>= 1;
    u32Pow++;
    }

  u32Pow--;
  u32NewWidth = 1 << u32Pow;                                // 1^(highest bit)

  if(u32NewWidth < u32NewHeight) u32NewWidth <<= 1;         // if not 2^n, round up
  u32NewHeight = u32NewWidth;

  // resize

  return Resize(u32NewWidth,u32NewHeight);
  }

//----------------------------------------------- FindBestFormat -----------------------------------------------------//
// return closest texture format
// in : wanted  format
// out: closest format
// rem: in RAM all formats are available; this method is useful when overriden
//--------------------------------------------------------------------------------------------------------------------//

u32 CEngineTexture::FindBestFormat(const u32 u32Format)
  {
  return u32Format;
  }

//----------------------------------------------- ConvertRAW2RAW -----------------------------------------------------//
// convert surface to another format
// in : wanted format,wanted RGBA order
// out: 0 or error ID
//--------------------------------------------------------------------------------------------------------------------//

u32 CEngineTexture::ConvertRAW2RAW(const u32 u32NewFormat,const u32 u32NewOrder)
  {
  if(!m_boInRAM || !m_pu8RAM) return -1;

  // format related parameters

  u32 u32Depth,u32Bytes;

  switch(u32NewFormat)
    {
    case TEXFORMAT_0555 : 
    case TEXFORMAT_0565 : 
    case TEXFORMAT_1555 : 
    case TEXFORMAT_4444 : u32Depth = 16; u32Bytes = 2; break;
    case TEXFORMAT_0888 : u32Depth = 24; u32Bytes = 3; break;
    case TEXFORMAT_8888 : u32Depth = 32; u32Bytes = 4; break;
    default             : return -2;
    }

  // get some memory for destination

  u32 u32Mem  = m_u32Width*m_u32Height*u32Bytes;
  u8* pu8Dest = new u8[u32Mem];
  if(!pu8Dest) return -3;

  // convert

  u8* pu8New        = pu8Dest;
  u8* pu8Src        = m_pu8RAM;
  u32 u32SrcFormat  = m_u32Format;
  u32 u32DestFormat = u32NewFormat;
  u32 u32SrcOrder   = m_u32RGBAOrder;
  u32 u32DestOrder  = u32NewOrder;
  BYTE bA,bR,bG,bB;
  WORD wARGB;

  if(u32DestOrder == ORDER_LAST) u32DestOrder = u32SrcOrder;
  m_u32RGBAOrder = u32DestOrder;

  for(u32 u32Row = 0; u32Row < m_u32Height; u32Row++)
    {
    for(u32 u32Col = 0; u32Col < m_u32Width; u32Col++)
      {
      // read RAW pixel in bA,bR,bG,bB (555,565,888,1555,4444,8888)

      RAW_READ;

      // write RAW pixel

      RAW_WRITE;
      }
    }

  // copy new parameters

  delete [] m_pu8RAM;
  m_pu8RAM    = pu8New;
  m_u32Format = u32NewFormat;
  m_u32Depth  = u32Depth;

  m_u32UsedRAM -= m_u32MemBytes;
  m_u32MemBytes = u32Mem;
  m_u32UsedRAM += m_u32MemBytes;
  return 0;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                IO                                                                  //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- LoadTexture --------------------------------------------------------//
// load texture file
// in : name,file type (default=autodetect),dir (default="data\\textures\\")
// out: 0 or error ID
//--------------------------------------------------------------------------------------------------------------------//

u32 CEngineTexture::LoadTexture(const CStdString& strFileName,const u32 u32FileType,const CStdString& strDir)
  {
  if(u32FileType >= TEXFILE_LAST) return -1;

  // no file format : check extension

  u32 u32Type = u32FileType;
  CStdString strName(strFileName);
  strName.MakeUpper();

  if(u32Type == TEXFILE_NONE)
    {
    if(strstr(strName.c_str(),".TGA")) u32Type = TEXFILE_TGA;
    if(strstr(strName.c_str(),".BMP")) u32Type = TEXFILE_BMP;
    if(strstr(strName.c_str(),".PCX")) u32Type = TEXFILE_PCX;

    // add custom types here
    }

  // try to load

  if(u32Type == TEXFILE_NONE) u32Type++;                    // no extension => begin with 1st format

  while(u32Type < TEXFILE_LAST)
    {
    CTextureIO* pTexLoader = NULL;

    // create loader

    switch(u32Type)
      {
      case TEXFILE_TGA : pTexLoader = new CTexIoTGA; break;
//      case TEXFILE_BMP : pTexLoader = new CTexIoBMP; break;
//      case TEXFILE_PCX : pTexLoader = new CTexIoPCX; break;
      // add custom formats here
      }
    if(!pTexLoader) return -2;

    // load

    u32 u32Res = pTexLoader->Load(this,strDir,strName);
    delete pTexLoader;
    if(SUCCEEDED(u32Res)) return 0;                         // OK

    if(u32FileType == TEXFILE_NONE) u32Type++;              // next one (this doesn't change the extension, though)
    else break;
    }

  return -3;                                                // bad file
  }

//----------------------------------------------- SaveTexture --------------------------------------------------------//
// save texture file
// in : name,file type,texture depth,dir (default="data\\textures\\")
// out: 0 or error ID
//--------------------------------------------------------------------------------------------------------------------//

u32 CEngineTexture::SaveTexture(const CStdString& strFileName,const u32 u32FileType,const u32 u32FileDepth,const CStdString& strDir)
  {
  if(u32FileType >= TEXFILE_LAST) return -1;
  if((u32FileDepth != 16) && (u32FileDepth != 24) && (u32FileDepth != 32)) return -2;

  u32 u32Type = u32FileType;
  if(u32Type == TEXFILE_NONE) u32Type = TEXFILE_TGA;        // default type

  // create saver

  CTextureIO* pTexSaver = NULL;

  switch(u32Type)
    {
    case TEXFILE_TGA : pTexSaver = new CTexIoTGA; break;
//    case TEXFILE_BMP : pTexSaver = new CTexIoBMP; break;
//    case TEXFILE_PCX : pTexSaver = new CTexIoPCX; break;
    }
  if(!pTexSaver) return -3;

  // save

  u32 u32Res = pTexSaver->Save(this,strDir,strFileName,u32FileDepth);
  delete pTexSaver;
  return u32Res;
  }

//----------------------------------------------- Shoot --------------------------------------------------------------//
// save texture as screenshot
// in : directory
// out: 0 or error ID
// rem: name is automatically built, output format=TGA 24b
//--------------------------------------------------------------------------------------------------------------------//

u32 CEngineTexture::Shoot(const CStdString& strDir)
  {
  if(!m_boInRAM || !m_pu8RAM) return -1;

  // look for previous shots

  u32 u32NextNum = 0;
  CStdString strFind = strDir+"\\Shot????.TGA";
  FindFile Finder;

  if(Finder.FindFirst(strFind.c_str(),false))
    {
    u32 u32New;
    u32 u32Len = strDir.GetLength()+strlen("\\Shot");

    do
      {
      if(!Finder.IsDirectory())
        {
        const char* pszFile;
        char  cDigit;
        strFind = Finder.GetFilePath();
        pszFile = strFind.c_str();

        cDigit  = pszFile[u32Len+0];
        if((cDigit < '0') || (cDigit > '9')) continue;
        u32New  = u32(cDigit-'0')*1000;

        cDigit  = pszFile[u32Len+1];
        if((cDigit < '0') || (cDigit > '9')) continue;
        u32New += u32(cDigit-'0')*100;

        cDigit  = pszFile[u32Len+2];
        if((cDigit < '0') || (cDigit > '9')) continue;
        u32New += u32(cDigit-'0')*10;

        cDigit  = pszFile[u32Len+3];
        if((cDigit < '0') || (cDigit > '9')) continue;
        u32New += u32(cDigit-'0')*1;

        if(u32New >= u32NextNum) u32NextNum = u32New+1;
        }
      }
    while(Finder.FindNext());

    Finder.FindClose();
    }

  if(u32NextNum > 9999) return -2;

  // save

  CStdString strFile;
  strFile.Format("%s\\Fairy%s%s%s%u.TGA",strDir.c_str(),u32NextNum<1000 ? "0":"",u32NextNum<100 ? "0":"",u32NextNum<10 ? "0":"",u32NextNum);
  return SaveTexture(strFile,TEXFILE_TGA,24,"");
  }

//----------------------------------------------- Create -------------------------------------------------------------//
// create & load texture file - static
// in : name,file name,file type
// out: 0 or error ID
//--------------------------------------------------------------------------------------------------------------------//

CEngineTexture* CEngineTexture::Create(const CStdString& strName,const CStdString& strFile,const u32 u32FileType)
  {
  if(strName.empty()) return NULL;
  CEngineTexture* pTex = new CEngineTexture;
  if(!pTex) return NULL;
  pTex->SetName(strName);

  u32 u32Res = pTex->LoadTexture(strFile,u32FileType);
  if(FAILED(u32Res))
    {
    delete pTex;
    return NULL;
    }

  return pTex;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                DOWNLOAD/UNLOAD                                                     //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- DownloadInit -------------------------------------------------------//
// inits before DL
// in :
// out: 0 or error ID
// rem: virtual
//--------------------------------------------------------------------------------------------------------------------//

u32 CEngineTexture::DownloadInit()
  {
  return 0;
  }

//----------------------------------------------- Unload -------------------------------------------------------------//
// UL texture from video memory
// in :
// out: 0 or error ID
// rem: virtual
//--------------------------------------------------------------------------------------------------------------------//

u32 CEngineTexture::Unload()
  {
  return 0;
  }

//----------------------------------------------- Reload -------------------------------------------------------------//
// DL already created texture
// in :
// out: 0 or error ID
// rem: virtual
//--------------------------------------------------------------------------------------------------------------------//

u32 CEngineTexture::Reload()
  {
  return 0;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                EDITING                                                             //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- ModifyProp ---------------------------------------------------------//
// attempt to modify a property in the grid; do whatever is needed
// in : property, value
// out: property handled by the function ?
//--------------------------------------------------------------------------------------------------------------------//

bool CEngineTexture::ModifyProp(CProperty* pProp,const CStdString& strNew)
  {
  if(!IsPropOfRTTI(pProp,&CEngineTexture::m_RTTI)) return CEngineObj::ModifyProp(pProp,strNew);

  if(pProp->GetName() == "File")
    {
    CStdString strFile = GetFileName();
    CStdString strNext = strNew;
    strNext.MakeUpper();
    if(strNext == strFile) return true;                     // nothing to do

    // here create surface & load new file

    m_strFileName = strNew;
    if(0 == LoadTexture(strNew)) return true;

    Destroy();
    return true;
    }

  return false;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                HELPERS                                                             //
//--------------------------------------------------------------------------------------------------------------------//

CStdString CEngineTexture::DumpInfos() const
  {
  char* pszBool[] = { "false","true" };
  CStdString strInfo;
  strInfo.Format("name     : %s\nfile     : %s\ndims     : %u*%u*%u\nformat   : %s\nmem size : %u bytes\n" \
                 "in RAM   : %s\nin video : %s\nRAM      : %x\nvideo    : %x\n",
                 GetName().c_str(),GetFileName().c_str(),GetWidth(),GetHeight(),GetDepth(),m_pszFormat[GetFormat()],
                 GetMemBytes(),pszBool[IsInRAM()],pszBool[IsInVideo()],m_pu8RAM,m_pu8Video);
  return strInfo;
  }

//----------------------------------------------- GetTexel -----------------------------------------------------------//
// read texel at given uv (floats)
// in : uv
// out: color (error=>black)
//--------------------------------------------------------------------------------------------------------------------//

CVect4D CEngineTexture::GetTexel(const CVect2D& v2Coords) const
  {
  CVect2D v2UV(v2Coords);

  while(v2UV.u < 0.f) v2UV.u += 1.f;
  while(v2UV.u > 1.f) v2UV.u -= 1.f;
  while(v2UV.v < 0.f) v2UV.v += 1.f;
  while(v2UV.v > 1.f) v2UV.v -= 1.f;

  float fU = v2UV.u*(GetWidth ()-1);
  float fV = v2UV.v*(GetHeight()-1);
  u32 u32U = (u32)floor(fU);
  u32 u32V = (u32)floor(fV);
  fU      -= u32U;
  fV      -= u32V;

  // bilinear interpolation

  CVect4D v4Color;
  v4Color  = GetTexel(u32U,  u32V)  *(1.f-fU)*(1.f-fV);
  v4Color += GetTexel(u32U+1,u32V)  *     fU *(1.f-fV);
  v4Color += GetTexel(u32U,  u32V+1)*(1.f-fU)*     fV;
  v4Color += GetTexel(u32U+1,u32V+1)*     fU *     fV;

  return v4Color;
  }

//----------------------------------------------- GetTexel -----------------------------------------------------------//
// read texel at given uv (u32)
// in : u,v
// out: color (error=>black)
//--------------------------------------------------------------------------------------------------------------------//

CVect4D CEngineTexture::GetTexel(const u32 u32U,const u32 u32V) const
  {
  u32 U = u32U;
  u32 V = u32V;
  u32 W = GetWidth ();
  u32 H = GetHeight();

  while(U > W) U -= W;
  while(V > H) V -= H;

  u8* pu8Src = (u8*)GetSurfacePtr();
  if(!pu8Src) return CVect4D(0.f,0.f,0.f,1.f);

  u32 u32Bytes = GetMemBytes()/W/H;
  pu8Src      += u32Bytes*(U+V*W);

  BYTE bA,bR,bG,bB;
  WORD wARGB;
  u32  u32SrcFormat = GetFormat();
  u32  u32SrcOrder  = GetRGBAOrder();
  RAW_READ;

  float  fK = 1.f/255.f;
  return fK*CVect4D(bR,bG,bB,bA);
  }

//----------------------------------------------- GetTexelValue ------------------------------------------------------//
// read texel at given uv (floats)
// in : uv
// out: value in [0;1]
//--------------------------------------------------------------------------------------------------------------------//

float CEngineTexture::GetTexelValue(const CVect2D& v2Coords) const
  {
  CVect2D v2UV(v2Coords);

  while(v2UV.u < 0.f) v2UV.u += 1.f;
  while(v2UV.u > 1.f) v2UV.u -= 1.f;
  while(v2UV.v < 0.f) v2UV.v += 1.f;
  while(v2UV.v > 1.f) v2UV.v -= 1.f;

  float fU = v2UV.u*(GetWidth ()-1);
  float fV = v2UV.v*(GetHeight()-1);
  u32 u32U = (u32)floor(fU);
  u32 u32V = (u32)floor(fV);
  fU      -= u32U;
  fV      -= u32V;

  // bilinear interpolation

  f32 fAcc;
  fAcc  = (f32)GetTexelValue(u32U,  u32V)  *(1.f-fU)*(1.f-fV);
  fAcc += (f32)GetTexelValue(u32U+1,u32V)  *     fU *(1.f-fV);
  fAcc += (f32)GetTexelValue(u32U,  u32V+1)*(1.f-fU)*     fV;
  fAcc += (f32)GetTexelValue(u32U+1,u32V+1)*     fU *     fV;

  fAcc /= 4294967295.f;
  if(fAcc > 1.f) fAcc = 1.f;
  return fAcc;
  }

//----------------------------------------------- GetTexelValue ------------------------------------------------------//
// read texel at given uv (u32)
// in : u,v
// out: value in [0;2^32-1]
//--------------------------------------------------------------------------------------------------------------------//

u32 CEngineTexture::GetTexelValue(const u32 u32U,const u32 u32V) const
  {
  u32 U = u32U;
  u32 V = u32V;
  u32 W = GetWidth ();
  u32 H = GetHeight();

  while(U > W) U -= W;
  while(V > H) V -= H;

  u8* pu8Src = (u8*)GetSurfacePtr();
  if(!pu8Src) return 0;

  u32 u32Bytes = GetMemBytes()/W/H;
  pu8Src      += u32Bytes*(U+V*W);

  BYTE bA,bR,bG,bB;
  WORD wARGB;
  u32  u32SrcFormat = GetFormat();
  u32  u32SrcOrder  = GetRGBAOrder();
  RAW_READ;

  u32 u32Val = (bR << 24)+(bG << 16)+(bB << 8)+bA;
  return u32Val;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
