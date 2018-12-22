//--------------------------------------------------------------------------------------------------------------------//
//                                                DX8 TEXTURE CLASS                                                   //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_TEXTUREDX8_H_
#define   _GPG4_TEXTUREDX8_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#define   D3D_OVERLOADS
#include  <d3d8.h>

#include  "EngineTexture.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CTextureDX8                                                         //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
  class CTextureDX8 : public CEngineTexture
  {
  DECLARE_RTTI;

  // public methods

  public:

    // constructors & destructor

                              CTextureDX8         (const LPDIRECT3DDEVICE8 pDevice);
    virtual                  ~CTextureDX8         (void);

    // get/set

    virtual const CRTTI*      GetWriteRTTI        (void) const;
    LPDIRECT3DTEXTURE8        GetInterface        (void);
    LPDIRECT3DSURFACE8        GetSurface          (void) const;

    // download/unload

    virtual u32               DownloadInit        (void);
    virtual u32               Unload              (void);
    virtual u32               Reload              (void);

  // protected data

  protected:

    LPDIRECT3DDEVICE8         m_pDevice;

    LPDIRECT3DTEXTURE8        m_pTexInterface;              // DX8 texture
    D3DFORMAT                 m_dx8TexFormat;               // format on card
  };

  // smart ptr

  SMART_POINTER(CTextureDX8);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "TextureDX8.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_TEXTUREDX8_H_
