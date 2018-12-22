//--------------------------------------------------------------------------------------------------------------------//
//                                                SWAP CHAIN CLASS                                                    //
//                                                (MULTIPLE VIEWS)                                                    //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

#ifndef   _GPG4_SWAPCHAIN_INL_
#define   _GPG4_SWAPCHAIN_INL_

namespace GPG4
{

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

INLINE CSwapChain::CSwapChain(u32 u32Handle,bool boWindowed,bool boZBuffer,HWND hWnd,u32 u32Width,u32 u32Height)
  {
  m_u32Handle  = u32Handle;
  m_boWindowed = boWindowed;
  m_boZBuffer  = boZBuffer;
  m_hWnd       = hWnd;
  m_u32Width   = u32Width;
  m_u32Height  = u32Height;

  m_v3ClearColor.Set(0.5f);
  }

INLINE CSwapChain::~CSwapChain()
  {}

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET                                                                 //
//--------------------------------------------------------------------------------------------------------------------//

INLINE u32 CSwapChain::GetFreeHandle()
  {
  return m_u32NextHandle++;
  }

INLINE u32 CSwapChain::GetHandle() const
  {
  return m_u32Handle;
  }

INLINE bool CSwapChain::IsWindowed() const
  {
  return m_boWindowed;
  }

INLINE bool CSwapChain::HasZBuffer() const
  {
  return m_boZBuffer;
  }

INLINE HWND CSwapChain::GetHwnd() const
  {
  return m_hWnd;
  }

INLINE u32 CSwapChain::GetWidth() const
  {
  return m_u32Width;
  }

INLINE u32 CSwapChain::GetHeight() const
  {
  return m_u32Height;
  }

INLINE CVect3D CSwapChain::GetClearColor() const
  {
  return m_v3ClearColor;
  }

INLINE void CSwapChain::SetClearColor(const CVect3D& v3Color)
  {
  m_v3ClearColor = v3Color;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace

#endif // _GPG4_SWAPCHAIN_INL_
