//--------------------------------------------------------------------------------------------------------------------//
//                                                TEXTURE LOADER BASE CLASS                                           //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_TEXTUREIO_H_
#define   _GPG4_TEXTUREIO_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "BasicTypes.h"
#include  "RTTI.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

namespace GPG4
{
  class CEngineTexture;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                CTextureIO                                                          //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CTextureIO
  {
  DECLARE_ROOT_RTTI;

  // public methods

  public:

    // constructors & destructor

                              CTextureIO          (void);
    virtual                  ~CTextureIO          (void);

    // streaming

    virtual u32               Load                (CEngineTexture* pTex,const CStdString& strDir,const CStdString& strFile) = 0;
    virtual u32               Load                (CEngineTexture* pTex,const u8* pu8MemFile,    const u32 u32Size) = 0;
    virtual u32               Save                (CEngineTexture* pTex,const CStdString& strDir,const CStdString& strFile,const u32 u32Depth) = 0;

  // protected data

  protected:

    CEngineTexture*           m_pTexture;

    u8*                       m_pu8File;
    u32                       m_u32Size;

    u32                       m_u32Width,m_u32Height,m_u32Depth;
    u32                       m_u32Bytes,m_u32Mem;
    u32                       m_u32Format;
  };
}

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_TEXTUREIO_H_
