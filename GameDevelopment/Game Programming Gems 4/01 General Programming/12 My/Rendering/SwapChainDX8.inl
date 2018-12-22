//--------------------------------------------------------------------------------------------------------------------//
//                                                DX8 SWAP CHAIN CLASS                                                //
//                                                (MULTIPLE VIEWS)                                                    //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_SWAPCHAINDX8_INL_
#define   _GPG4_SWAPCHAINDX8_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CSwapChainDX8::CSwapChainDX8(u32 u32Handle,bool boWindowed,bool boZBuffer,HWND hWnd,u32 u32Width,u32 u32Height) :
                      CSwapChain(u32Handle,boWindowed,boZBuffer,hWnd,u32Width,u32Height)
  {
  m_pInterface  = NULL;
  m_pBackBuffer = NULL;
  m_pZStencil   = NULL;
  }

INLINE CSwapChainDX8::~CSwapChainDX8()
  {
  if(m_pBackBuffer) m_pBackBuffer->Release();
  if(m_pZStencil)   m_pZStencil  ->Release();
  if(m_pInterface)  m_pInterface ->Release();               // order is important to avoid D3D warnings in output

  m_pInterface  = NULL;
  m_pBackBuffer = NULL;
  m_pZStencil   = NULL;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET/SET                                                             //
//--------------------------------------------------------------------------------------------------------------------//

INLINE IDirect3DSwapChain8* CSwapChainDX8::GetInterface() const
  {
  return m_pInterface;
  }

INLINE void CSwapChainDX8::SetInterface(IDirect3DSwapChain8* pInterface)
  {
  if(m_pInterface) m_pInterface->Release();
  m_pInterface = pInterface;
  }

INLINE IDirect3DSurface8* CSwapChainDX8::GetBackBuffer() const
  {
  return m_pBackBuffer;
  }

INLINE void CSwapChainDX8::SetBackBuffer(IDirect3DSurface8* pBackBuffer)
  {
  if(m_pBackBuffer) m_pBackBuffer->Release();
  m_pBackBuffer = pBackBuffer;
  }

INLINE IDirect3DSurface8* CSwapChainDX8::GetZStencil() const
  {
  return m_pZStencil;
  }

INLINE void CSwapChainDX8::SetZStencil(IDirect3DSurface8* pZStencil)
  {
  if(m_pZStencil) m_pZStencil->Release();
  m_pZStencil = pZStencil;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_SWAPCHAINDX8_INL_
