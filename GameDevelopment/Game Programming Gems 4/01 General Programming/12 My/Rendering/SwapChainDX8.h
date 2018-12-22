//--------------------------------------------------------------------------------------------------------------------//
//                                                DX8 SWAP CHAIN CLASS                                                //
//                                                (MULTIPLE VIEWS)                                                    //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_SWAPCHAINDX8_H_
#define   _GPG4_SWAPCHAINDX8_H_

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#define   D3D_OVERLOADS
#include  <d3d8.h>

#include  "SwapChain.h"

//----------------------------------------------- CLASSES ------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------------------//
//                                                CSwapChainDX8                                                       //
//--------------------------------------------------------------------------------------------------------------------//

namespace GPG4
{
class CSwapChainDX8 : public CSwapChain
  {
  // public methods

  public:

    // constructors & destructor

                              CSwapChainDX8       (u32 u32Handle,bool boWindowed,bool boZBuffer,HWND hWnd,u32 u32Width,u32 u32Height);
    virtual                  ~CSwapChainDX8       (void);

    // get/set

    IDirect3DSwapChain8*      GetInterface        (void) const;
    void                      SetInterface        (IDirect3DSwapChain8* pInterface);
    IDirect3DSurface8*        GetBackBuffer       (void) const;
    void                      SetBackBuffer       (IDirect3DSurface8* pBackBuffer);
    IDirect3DSurface8*        GetZStencil         (void) const;
    void                      SetZStencil         (IDirect3DSurface8* pZStencil);

  // protected data

  protected:

    IDirect3DSwapChain8*      m_pInterface;
    IDirect3DSurface8*        m_pBackBuffer;
    IDirect3DSurface8*        m_pZStencil;
  };

  // smart ptr

  SMART_POINTER(CSwapChainDX8);
}

//----------------------------------------------- INLINES ------------------------------------------------------------//

#ifndef   _DEBUG
#include  "SwapChainDX8.inl"
#endif

//--------------------------------------------------------------------------------------------------------------------//

#endif // _GPG4_SWAPCHAINDX8_H_
