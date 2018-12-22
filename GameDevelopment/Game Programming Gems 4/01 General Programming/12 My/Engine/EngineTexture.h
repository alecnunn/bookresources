//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE TEXTURE CLASS                                                //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_ENGINETEXTURE_H_
#define   _GPG4_ENGINETEXTURE_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "EngineObj.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class CVect2D;
  class CVect4D;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CEngineTexture                                                      //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CEngineTexture : public CEngineObj
  {
  DECLARE_RTTI;
  DECLARE_PROPERTIES(CEngineTexture,CExtraProp);

  // public enums

  public:

    // texture formats

    enum  TEXFORMATS                                        // corresponding strings in m_pszFormat (EngineTexture.cpp)
            {
            TEXFORMAT_NONE            = 0,                  // MUST stay first
            TEXFORMAT_0555,
            TEXFORMAT_0565,
            TEXFORMAT_0888,
            TEXFORMAT_1555,
            TEXFORMAT_4444,
            TEXFORMAT_8888,

            TEXFORMAT_LAST,                                 // for bound checking: >= LAST => error

            TEXFORMAT_PAL8                                  // PCX reading only - not used anywhere else
            };

    // file formats

    enum  TEXFILE_FORMATS
            {
            TEXFILE_NONE              = 0,
            TEXFILE_TGA,
            TEXFILE_BMP,
            TEXFILE_PCX,

            TEXFILE_CUSTOM,                                 // MUST stay after all other formats
            TEXFILE_LAST                                    // for bound checking: >= LAST => error
            };

    // texture types

    enum  TEXTYPES
            {
            TEXTYPE_RAW               = 0,
            TEXTYPE_GL,
            TEXTYPE_DX8,
            };

    // RGBA order

    enum  RGBA_ORDER
            {
            ORDER_BGRA                = 0,
            ORDER_RGBA,
            ORDER_ABGR,
            ORDER_ARGB,

            ORDER_LAST
            };

  // public methods

  public:

    // constructors & destructor

                              CEngineTexture      (void);
    virtual                  ~CEngineTexture      (void);

    virtual void              Init                (void);
    virtual void              Destroy             (void);

    // get/set

    virtual CStdString        GetEditName         (void) const;
    const   CStdString&       GetFileName         (void) const;
    void                      SetFileName         (const CStdString& strFile);

    u32                       GetWidth            (void) const;
    u32                       GetHeight           (void) const;
    u32                       GetDepth            (void) const;
    u32                       GetFormat           (void) const;
    u32                       GetMemBytes         (void) const;

    virtual void*             GetSurfacePtr       (void) const;       // get surface ptr (generated tex...)
    virtual HRESULT           ReleaseSurfacePtr   (const void* pvSurface);

    bool                      IsInRAM             (void) const;
    bool                      IsInVideo           (void) const;

    static u32                GetUsedRAM          (void);
    static u32                GetUsedVideo        (void);

    u32                       GetType             (void) const;
    u32                       GetRGBAOrder        (void) const;
    void                      SetRGBAOrder        (const u32 u32Order);

    // surface operations

    u32                       Create              (const u32 u32Width,const u32 u32Height,const u32 u32Format);
    u32                       Clear               (const CVect4D& v4Color);

    u32                       Resize              (const u32 u32Width,const u32 u32Height);
    u32                       ExtendSquared       (void);             // extend dimensions to squared 2^n
    virtual u32               FindBestFormat      (const u32 u32Format);
    u32                       ConvertRAW2RAW      (const u32 u32NewFormat,const u32 u32NewOrder=ORDER_LAST);

    // IO

    u32                       LoadTexture         (const CStdString& strFileName,const u32 u32FileType=TEXFILE_NONE,
                                                   const CStdString& strDir="data\\textures\\");
    u32                       SaveTexture         (const CStdString& strFileName,const u32 u32FileType=TEXFILE_TGA,
                                                   const u32 u32FileDepth=24,const CStdString& strDir="data\\textures\\");

    virtual u32               Shoot               (const CStdString& strDir);
    static  CEngineTexture*   Create              (const CStdString& strName,const CStdString& strFile,const u32 u32FileType=TEXFILE_NONE);

    // download/unload

    virtual u32               DownloadInit        (void);
    virtual u32               Unload              (void);
    virtual u32               Reload              (void);

    // streaming

    virtual bool              PostRead            (void);

    // editing

    virtual bool              ModifyProp          (CProperty* pProp,const CStdString& strNew);

    // helpers

    virtual CStdString        DumpInfos           (void) const;
    CVect4D                   GetTexel            (const CVect2D& v2UV) const;
    CVect4D                   GetTexel            (const u32 u32U,const u32 u32V) const;
    float                     GetTexelValue       (const CVect2D& v2UV) const;
    u32                       GetTexelValue       (const u32 u32U,const u32 u32V) const;

  // protected data

  protected:

    CStdString                m_strFileName;

    u32                       m_u32Width,m_u32Height,m_u32Depth;      // size & depth (multiple of 8)
    u32                       m_u32Format;                            // argb format (see enum)
    u32                       m_u32MemBytes;                          // surface size in bytes

    u8*                       m_pu8RAM;                               // surface in RAM
    u8*                       m_pu8Video;                             // surface on 3D card

    u32                       m_u32Type;                              // RAW, GL, DX8...
    u32                       m_u32RGBAOrder;                         // RGBA components order

    // flags

    bool                      m_boInRAM;
    bool                      m_boInVideo;

    // stats

    static u32                m_u32UsedRAM;
    static u32                m_u32UsedVideo;

    // helpers

    static const char*        m_pszFormat[];                          // texture formats strings
  };

  // smart ptr

  SMART_POINTER(CEngineTexture);
}

//----------------------------------------------- MACROS -------------------------------------------------------------//

// (write) build 16b values from bA,bR,bG,bB

#define   RAW_WBUILD_0555 {                                                             \
          bR >>= 3; bG >>= 3; bB >>= 3;                                                 \
                                                                                        \
          switch(u32DestOrder)                                                          \
            {                                                                           \
            case CEngineTexture::ORDER_BGRA : wARGB = WORD((bR << 10)+(bG << 5)+bB);    \
                                              break;                                    \
                                                                                        \
            case CEngineTexture::ORDER_RGBA : wARGB = WORD((bB << 10)+(bG << 5)+bR);    \
                                              break;                                    \
            }                                                                           \
          }

  //
  
#define   RAW_WBUILD_0565 {                                                             \
          bR >>= 3; bG >>= 2; bB >>= 3;                                                 \
                                                                                        \
          switch(u32DestOrder)                                                          \
            {                                                                           \
            case CEngineTexture::ORDER_BGRA : wARGB = WORD((bR << 11)+(bG << 5)+bB);    \
                                              break;                                    \
                                                                                        \
            case CEngineTexture::ORDER_RGBA : wARGB = WORD((bB << 11)+(bG << 5)+bR);    \
                                              break;                                    \
            }                                                                           \
          }

  //

#define   RAW_WBUILD_1555 {                                                                        \
          bR >>= 3; bG >>= 3; bB >>= 3; bA >>= 7;                                                  \
                                                                                                   \
          switch(u32DestOrder)                                                                     \
            {                                                                                      \
            case CEngineTexture::ORDER_BGRA : wARGB = WORD((bA << 15)+(bR << 10)+(bG << 5)+bB);    \
                                              break;                                               \
                                                                                                   \
            case CEngineTexture::ORDER_RGBA : wARGB = WORD((bA << 15)+(bB << 10)+(bG << 5)+bR);    \
                                              break;                                               \
            }                                                                                      \
          }

  //

#define   RAW_WBUILD_4444 {                                                                       \
          bR >>= 4; bG >>= 4; bB >>= 4; bA >>= 4;                                                 \
                                                                                                  \
          switch(u32DestOrder)                                                                    \
            {                                                                                     \
            case CEngineTexture::ORDER_BGRA : wARGB = WORD((bA << 12)+(bR << 8)+(bG << 4)+bB);    \
                                              break;                                              \
                                                                                                  \
            case CEngineTexture::ORDER_RGBA : wARGB = WORD((bA << 12)+(bG << 8)+(bG << 4)+bR);    \
                                              break;                                              \
            }                                                                                     \
          }

// (read) build bA,bR,bG,bB from 16b values

#define   RAW_RBUILD_0555 {                                                     \
          switch(u32SrcOrder)                                                   \
            {                                                                   \
            case CEngineTexture::ORDER_BGRA : bR = BYTE( wARGB >> 10);          \
                                              bG = BYTE((wARGB >>  5) & 31);    \
                                              bB = BYTE( wARGB        & 31);    \
                                              break;                            \
                                                                                \
            case CEngineTexture::ORDER_RGBA : bB = BYTE( wARGB >> 10);          \
                                              bG = BYTE((wARGB >>  5) & 31);    \
                                              bR = BYTE( wARGB        & 31);    \
                                              break;                            \
            }                                                                   \
                                                                                \
          bA = 255;                                                             \
          bR <<= 3; bG <<= 3; bB <<= 3;                                         \
          }

  //

#define   RAW_RBUILD_0565 {                                                     \
          switch(u32SrcOrder)                                                   \
            {                                                                   \
            case CEngineTexture::ORDER_BGRA : bR = BYTE( wARGB >> 11);          \
                                              bG = BYTE((wARGB >>  5) & 63);    \
                                              bB = BYTE( wARGB        & 31);    \
                                              break;                            \
                                                                                \
            case CEngineTexture::ORDER_RGBA : bB = BYTE( wARGB >> 11);          \
                                              bG = BYTE((wARGB >>  5) & 63);    \
                                              bR = BYTE( wARGB        & 31);    \
                                              break;                            \
            }                                                                   \
                                                                                \
          bA = 255;                                                             \
          bR <<= 3; bG <<= 2; bB <<= 3;                                         \
          }

  //

#define   RAW_RBUILD_1555 {                                                     \
          switch(u32SrcOrder)                                                   \
            {                                                                   \
            case CEngineTexture::ORDER_BGRA : bA = BYTE( wARGB >> 15);          \
                                              bR = BYTE((wARGB >> 10) & 31);    \
                                              bG = BYTE((wARGB >>  5) & 31);    \
                                              bB = BYTE( wARGB        & 31);    \
                                              break;                            \
                                                                                \
            case CEngineTexture::ORDER_RGBA : bA = BYTE( wARGB >> 15);          \
                                              bB = BYTE((wARGB >> 10) & 31);    \
                                              bG = BYTE((wARGB >>  5) & 31);    \
                                              bR = BYTE( wARGB        & 31);    \
                                              break;                            \
            }                                                                   \
                                                                                \
          bR <<= 3; bG <<= 3; bB <<= 3;                                         \
          if(bA) bA = 255;                                                      \
          }

  //

#define   RAW_RBUILD_4444 {                                                     \
          switch(u32SrcOrder)                                                   \
            {                                                                   \
            case CEngineTexture::ORDER_BGRA : bA = BYTE( wARGB >> 12);          \
                                              bR = BYTE((wARGB >>  8) & 15);    \
                                              bG = BYTE((wARGB >>  4) & 15);    \
                                              bB = BYTE( wARGB        & 15);    \
                                              break;                            \
                                                                                \
            case CEngineTexture::ORDER_RGBA : bA = BYTE( wARGB >> 12);          \
                                              bB = BYTE((wARGB >>  8) & 15);    \
                                              bG = BYTE((wARGB >>  4) & 15);    \
                                              bR = BYTE( wARGB        & 15);    \
                                              break;                            \
            }                                                                   \
                                                                                \
          bA = BYTE((bA << 4) + bA);                                            \
          bR = BYTE((bR << 4) + bR);                                            \
          bG = BYTE((bG << 4) + bG);                                            \
          bB = BYTE((bB << 4) + bB);                                            \
          }

// write 16/24/32b values from bA,bR,bG,bB

#define   RAW_WRITE_16b {           \
          *(WORD*)pu8Dest = wARGB;  \
          pu8Dest += 2;             \
          }

  //

#define   RAW_WRITE_0888 {                                                                          \
          switch(u32DestOrder)                                                                      \
            {                                                                                       \
            case CEngineTexture::ORDER_BGRA : *pu8Dest++ = bB; *pu8Dest++ = bG; *pu8Dest++ = bR;    \
                                              break;                                                \
                                                                                                    \
            case CEngineTexture::ORDER_RGBA : *pu8Dest++ = bR; *pu8Dest++ = bG; *pu8Dest++ = bB;    \
                                              break;                                                \
            }                                                                                       \
          }

  //

#define   RAW_WRITE_8888 {                                                                                           \
          switch(u32DestOrder)                                                                                       \
            {                                                                                                        \
            case CEngineTexture::ORDER_BGRA : *pu8Dest++ = bB; *pu8Dest++ = bG; *pu8Dest++ = bR; *pu8Dest++ = bA;    \
                                              break;                                                                 \
                                                                                                                     \
            case CEngineTexture::ORDER_RGBA : *pu8Dest++ = bR; *pu8Dest++ = bG; *pu8Dest++ = bB; *pu8Dest++ = bA;    \
                                              break;                                                                 \
            }                                                                                                        \
          }

// read 16/24/32b values to bA,bR,bG,bB

#define   RAW_READ_16b {           \
          wARGB = *(WORD*)pu8Src;  \
          pu8Src += 2;             \
          }

  //

#define   RAW_READ_0888 {                                        \
          switch(u32SrcOrder)                                    \
            {                                                    \
            case CEngineTexture::ORDER_BGRA : bB = *pu8Src++;    \
                                              bG = *pu8Src++;    \
                                              bR = *pu8Src++;    \
                                              bA = 255;          \
                                              break;             \
                                                                 \
            case CEngineTexture::ORDER_RGBA : bR = *pu8Src++;    \
                                              bG = *pu8Src++;    \
                                              bB = *pu8Src++;    \
                                              bA = 255;          \
                                              break;             \
            }                                                    \
          }

  //

#define   RAW_READ_8888 {                                        \
          switch(u32SrcOrder)                                    \
            {                                                    \
            case CEngineTexture::ORDER_BGRA : bB = *pu8Src++;    \
                                              bG = *pu8Src++;    \
                                              bR = *pu8Src++;    \
                                              bA = *pu8Src++;    \
                                              break;             \
                                                                 \
            case CEngineTexture::ORDER_RGBA : bR = *pu8Src++;    \
                                              bG = *pu8Src++;    \
                                              bB = *pu8Src++;    \
                                              bA = *pu8Src++;    \
                                              break;             \
            }                                                    \
          }

// convert color: build wARGB from bA,bR,bG,bB (16b formats only)

#define   RAW_CONVERT_COLOR {                                           \
          bR = BYTE(v4Color[R]*255.f);                                  \
          bG = BYTE(v4Color[G]*255.f);                                  \
          bB = BYTE(v4Color[B]*255.f);                                  \
          bA = BYTE(v4Color[A]*255.f);                                  \
                                                                        \
          switch(u32DestFormat)                                         \
            {                                                           \
            case CEngineTexture::TEXFORMAT_0555 : RAW_WBUILD_0555;      \
                                                  break;                \
                                                                        \
            case CEngineTexture::TEXFORMAT_0565 : RAW_WBUILD_0565;      \
                                                  break;                \
                                                                        \
            case CEngineTexture::TEXFORMAT_1555 : RAW_WBUILD_1555;      \
                                                  break;                \
                                                                        \
            case CEngineTexture::TEXFORMAT_4444 : RAW_WBUILD_4444;      \
                                                  break;                \
            }                                                           \
          }

// write converted: color already built (in bytes, word or dword format)

#define   RAW_WRITE_CONVERTED {                                        \
          switch(u32DestFormat)                                        \
            {                                                          \
            case CEngineTexture::TEXFORMAT_0555 :                      \
            case CEngineTexture::TEXFORMAT_0565 :                      \
            case CEngineTexture::TEXFORMAT_1555 :                      \
            case CEngineTexture::TEXFORMAT_4444 : RAW_WRITE_16b;       \
                                                  break;               \
                                                                       \
            case CEngineTexture::TEXFORMAT_0888 : RAW_WRITE_0888;      \
                                                  break;               \
                                                                       \
            case CEngineTexture::TEXFORMAT_8888 : RAW_WRITE_8888;      \
                                                  break;               \
            }                                                          \
          }

// read (store result in bA,bR,bG,bB)

#define   RAW_READ          {                                       \
          switch(u32SrcFormat)                                      \
            {                                                       \
            case CEngineTexture::TEXFORMAT_0555 : RAW_READ_16b;     \
                                                  RAW_RBUILD_0555;  \
                                                  break;            \
                                                                    \
            case CEngineTexture::TEXFORMAT_0565 : RAW_READ_16b;     \
                                                  RAW_RBUILD_0565;  \
                                                  break;            \
                                                                    \
            case CEngineTexture::TEXFORMAT_0888 : RAW_READ_0888;    \
                                                  break;            \
                                                                    \
            case CEngineTexture::TEXFORMAT_1555 : RAW_READ_16b;     \
                                                  RAW_RBUILD_1555;  \
                                                  break;            \
                                                                    \
            case CEngineTexture::TEXFORMAT_4444 : RAW_READ_16b;     \
                                                  RAW_RBUILD_4444;  \
                                                  break;            \
                                                                    \
            case CEngineTexture::TEXFORMAT_8888 : RAW_READ_8888;    \
                                                  break;            \
            }                                                       \
          }

// write (from bA,bR,bG,bB)

#define   RAW_WRITE         {                                       \
          switch(u32DestFormat)                                     \
            {                                                       \
            case CEngineTexture::TEXFORMAT_0555 : RAW_WBUILD_0555;  \
                                                  RAW_WRITE_16b;    \
                                                  break;            \
                                                                    \
            case CEngineTexture::TEXFORMAT_0565 : RAW_WBUILD_0565;  \
                                                  RAW_WRITE_16b;    \
                                                  break;            \
                                                                    \
            case CEngineTexture::TEXFORMAT_0888 : RAW_WRITE_0888;   \
                                                  break;            \
                                                                    \
            case CEngineTexture::TEXFORMAT_1555 : RAW_WBUILD_1555;  \
                                                  RAW_WRITE_16b;    \
                                                  break;            \
                                                                    \
            case CEngineTexture::TEXFORMAT_4444 : RAW_WBUILD_4444;  \
                                                  RAW_WRITE_16b;    \
                                                  break;            \
                                                                    \
            case CEngineTexture::TEXFORMAT_8888 : RAW_WRITE_8888;   \
                                                      break;        \
            }                                                       \
          }

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "EngineTexture.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_ENGINETEXTURE_H_
