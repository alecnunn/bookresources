//--------------------------------------------------------------------------------------------------------------------//
//                                                RENDERER BASE CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "Renderer.h"
#ifdef    _DEBUG
#include  "Renderer.inl"
#endif

#include  "SwapChain.h"
#include  "EngineCamera.h"
#include  "EngineMaterial.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI(IRenderer,CPersistent);
IMPLEMENT_RTTI(CRenderer,IRenderer);

//--------------------------------------------------------------------------------------------------------------------//
//                                                CONSTRUCTORS / DESTRUCTOR                                           //
//--------------------------------------------------------------------------------------------------------------------//

CRenderer::CRenderer()
  {
  m_boInit = false;
  m_u32CurrentChain = 0;
  m_pCurrentChain   = NULL;

  m_m4Proj = CMat4x4::Identity;

  m_pMatWhiteSelf = new CEngineMaterial;
  if(m_pMatWhiteSelf)
    {
    m_pMatWhiteSelf->SetDiffuse(CVect4D(1.f));
    m_pMatWhiteSelf->EnableLighting(false);
    m_pMatWhiteSelf->EnableZTest(false);
    }
  }

//

CRenderer::~CRenderer()
  {
  Destroy();
  delete m_pMatWhiteSelf;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                BASIC OPS                                                           //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- Destroy ------------------------------------------------------------//
// delete & remove all swap chains
// in :
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CRenderer::Destroy()
  {
  iterChain iter = m_SwapChains.begin();

  while(iter != m_SwapChains.end())
    {
    delete (*iter);
    ++iter;
    }

  m_SwapChains.clear();
  m_pCurrentChain = NULL;
  }

//----------------------------------------------- AddSwapChain -------------------------------------------------------//
// add swap chain to list
// in : swap chain
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CRenderer::AddSwapChain(CSwapChain* pSwapChain)
  {
  if(!pSwapChain) return false;

  iterChain iter = std::find(m_SwapChains.begin(),m_SwapChains.end(),pSwapChain);
  if(iter != m_SwapChains.end()) return false;              // already in the list

  m_SwapChains.push_back(pSwapChain);
  return true;
  }

//----------------------------------------------- RemoveSwapChain ----------------------------------------------------//
// remove swap chain from list
// in : swap chain
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CRenderer::RemoveSwapChain(CSwapChain* pSwapChain)
  {
  iterChain iter = std::find(m_SwapChains.begin(),m_SwapChains.end(),pSwapChain);
  if(iter == m_SwapChains.end()) return false;

  m_SwapChains.remove(pSwapChain);

  if(pSwapChain == m_pCurrentChain)
    {
    m_pCurrentChain   = NULL;
    m_u32CurrentChain = 0;
    }
  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                GET                                                                 //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- GetSwapChain -------------------------------------------------------//
// return swap chain object by handle
// in : chain handle
// out: chain object
//--------------------------------------------------------------------------------------------------------------------//

CSwapChain* CRenderer::GetSwapChain(const u32 u32Chain)
  {
  iterChain iter = m_SwapChains.begin();

  while(iter != m_SwapChains.end())
    {
    if((*iter)->GetHandle() == u32Chain) return *iter;
    ++iter;
    }

  return NULL;
  }

//----------------------------------------------- GetClearColor ------------------------------------------------------//
// return clear color of current swap chain
// in :
// out: color
//--------------------------------------------------------------------------------------------------------------------//

CVect3D CRenderer::GetClearColor()
  {
  if(!m_pCurrentChain) return NULL;
  return m_pCurrentChain->GetClearColor();
  }

//----------------------------------------------- SetClearColor ------------------------------------------------------//
// set clear color of current swap chain
// in : color
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CRenderer::SetClearColor(const CVect3D& v3Color)
  {
  if(!m_pCurrentChain) return false;
  m_pCurrentChain->SetClearColor(v3Color);
  return true;
  }

//----------------------------------------------- GetWidth -----------------------------------------------------------//
// return width of current swap chain
// in :
// out: width
//--------------------------------------------------------------------------------------------------------------------//

u32 CRenderer::GetWidth()
  {
  if(!m_pCurrentChain) return 0;
  return m_pCurrentChain->GetWidth();
  }

//----------------------------------------------- GetHeight ----------------------------------------------------------//
// return height of current swap chain
// in :
// out: height
//--------------------------------------------------------------------------------------------------------------------//

u32 CRenderer::GetHeight()
  {
  if(!m_pCurrentChain) return 0;
  return m_pCurrentChain->GetHeight();
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                TEXTURES                                                            //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- AddTexture ---------------------------------------------------------//
// add new texture to map
// in : file,texture
// out:
//--------------------------------------------------------------------------------------------------------------------//

void CRenderer::AddTexture (const CStdString& strFile,CEngineTexture* pTex)
  {
  m_Textures[strFile] = pTex;
  }

//----------------------------------------------- RemoveTexture ------------------------------------------------------//
// remove (deleted) texture from map
// in : texture
// out: found ?
//--------------------------------------------------------------------------------------------------------------------//

bool CRenderer::RemoveTexture(CEngineTexture* pTex)
  {
  iterStr2Tex iter = m_Textures.begin();

  while(iter != m_Textures.end())
    {
    if((*iter).second == pTex)
      {
      m_Textures.erase(iter);
      return true;
      }
    ++iter;
    }

  return false;
  }

//----------------------------------------------- GetTexture ---------------------------------------------------------//
// return already existing texture
// in : file
// out: texture
//--------------------------------------------------------------------------------------------------------------------//

CEngineTexture* CRenderer::GetTexture(const CStdString& strFile)
  {
  iterStr2Tex iter = m_Textures.find(strFile);
  if(iter == m_Textures.end()) return NULL;
  return(*iter).second;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                TRF                                                                 //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- SetCamera ----------------------------------------------------------//
// use camera parameters
// in : camera
// out: OK?
//--------------------------------------------------------------------------------------------------------------------//

bool CRenderer::SetCamera(const CEngineCamera* pCam)
  {
  bool boRes = true;

  if(pCam->IsPerspective()) 
    {
    boRes &= SetPerspective(pCam->GetFov(),pCam->GetNear(),pCam->GetFar(),pCam->GetRatio());
    }
  else
    {
    f32 fRight = tanf(pCam->GetFov()*0.5f)*pCam->GetNear()*pCam->GetScaleOrtho();
    f32 fTop   = fRight/pCam->GetRatio();
    boRes &= SetOrtho(CVect3D(-fRight,-fTop,pCam->GetNear()),CVect3D(fRight,fTop,pCam->GetFar()));
    }

  boRes &= SetWorld2View(const_cast<CEngineCamera*>(pCam)->GetWorldTrf().RotTransInverted());
  return boRes;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
