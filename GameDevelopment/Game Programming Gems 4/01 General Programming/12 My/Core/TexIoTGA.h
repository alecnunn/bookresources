//--------------------------------------------------------------------------------------------------------------------//
//                                                TGA TEXTURE LOADER                                                  //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_TEXIOTGA_H_
#define   _GPG4_TEXIOTGA_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "TextureIO.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CTexIoTGA                                                           //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CTexIoTGA : public CTextureIO
  {
  DECLARE_RTTI;

  // typedefs

  public:

    // TGA header

    #pragma         pack(1)                                 // do NOT align structure members !

    typedef         struct    {
          BYTE      bIDLength;
          BYTE      bColorMapType;
          BYTE      bImageType;                             // 2=uncompressed RGB
          WORD      wCmapStart;
          WORD      wIntCmapLength;
          BYTE      bCmapDepth;
          WORD      wXOffset;
          WORD      wYOffset;
          WORD      wWidth;                                 // width
          WORD      wHeight;                                // height
          BYTE      bPixelDepth;                            // bits per pixel: 15/16/24/32
          BYTE      bImageDescriptor;                       // b5: y axis direction
      } TGAHEADER;                                          // 18 bytes

    #pragma   pack()

  // public methods

  public:

    // constructors & destructor

                              CTexIoTGA           (void);
    virtual                  ~CTexIoTGA           (void);

    // streaming

    virtual u32               Load                (CEngineTexture* pTex,const CStdString& strDir,const CStdString& strFile);
    virtual u32               Load                (CEngineTexture* pTex,const u8* pu8MemFile,    const u32 u32Size);
    virtual u32               Save                (CEngineTexture* pTex,const CStdString& strDir,const CStdString& strFile,const u32 u32Depth);

  // protected methods

  protected:

    virtual u32               ConvertToRAW        (CEngineTexture* pTex);
    virtual u32               ConvertFromRAW      (CEngineTexture* pTex);

  // protected data

  protected:

    bool                      m_boInvertX;                  // invert RGB   ?
    bool                      m_boInvertY;                  // invert lines ?
  };
}

//----------------------------------------------- MACROS -------------------------------------------------------------//

// read

#define   TGA_READ            {                             \
          switch(u32TGADepth)                               \
            {                                               \
            case 16 : wARGB = *(WORD*)pu8Src; pu8Src += 2;  \
                      bA    = BYTE( wARGB >> 15);           \
                      bR    = BYTE((wARGB >> 10) & 31);     \
                      bG    = BYTE((wARGB >>  5) & 31);     \
                      bB    = BYTE( wARGB        & 31);     \
                      bR <<= 3; bG <<= 3; bB <<= 3;         \
                      if(bA) bA = 0;                        \
                      else   bA = 255;                      \
                      break;                                \
                                                            \
            case 24 : bB = *pu8Src++;                       \
                      bG = *pu8Src++;                       \
                      bR = *pu8Src++;                       \
                      bA = 255;                             \
                      break;                                \
                                                            \
            case 32 : bB = *pu8Src++;                       \
                      bG = *pu8Src++;                       \
                      bR = *pu8Src++;                       \
                      bA = *pu8Src++;                       \
                      break;                                \
            }                                               \
          }

// write

#define   TGA_WRITE           {                                                            \
          switch(u32TGADepth)                                                              \
            {                                                                              \
            case 16 : bA >>= 7; bR >>= 3; bG >>= 3; bB >>= 3;                              \
                      if(bA) bA = 0; else bA = 1;                                          \
                      wARGB = WORD((bA << 15)+(bR << 10)+(bG << 5)+bB);                    \
                      *(WORD*)pu8Dest = wARGB;                                             \
                      pu8Dest += 2;                                                        \
                      break;                                                               \
                                                                                           \
            case 24 : *pu8Dest++ = bB; *pu8Dest++ = bG; *pu8Dest++ = bR;                   \
                      break;                                                               \
                                                                                           \
            case 32 : *pu8Dest++ = bB; *pu8Dest++ = bG; *pu8Dest++ = bR; *pu8Dest++ = bA;  \
                      break;                                                               \
            }                                                                              \
          }

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_TEXIOTGA_H_
