//--------------------------------------------------------------------------------------------------------------------//
//                                                DX8 VERTEX BUFFER CLASS                                             //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "VBufferDX8.h"
#ifdef    _DEBUG
#include  "VBufferDX8.inl"
#endif

#include  "MemC.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI(CVBufferDX8,CVBuffer);

//--------------------------------------------------------------------------------------------------------------------//
//                                                MANAGEMENT                                                          //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- Create -------------------------------------------------------------//
// create HW VB
// in : buffer,nb vtx,vertex & texcoord formats,static & copy flags
// out: 0=OK
// rem: pBuffer's memory is now owned by this class if boRAMCopy==true
//--------------------------------------------------------------------------------------------------------------------//

u32 CVBufferDX8::Create(void* pBuffer,const u32 u32Count,VTX_COMPONENTS& Vtx,TEX_COMPONENTS& Tex,const bool boStatic,const bool boRAMCopy)
  {
  if(!pBuffer || !u32Count) return -1;

  m_VtxFormat = Vtx;
  m_TexFormat = Tex;
  CalcVertexSize();
  CalcVertexFVF ();

  D3DVERTEXBUFFER_DESC vbDesc;
  Memory::MemSet(&vbDesc,0,sizeof(vbDesc));
  vbDesc.Format = D3DFMT_VERTEXDATA;
  vbDesc.Size   = u32Count*m_u32VtxSize;
  vbDesc.Type   = D3DRTYPE_VERTEXBUFFER;
  vbDesc.Pool   = D3DPOOL_DEFAULT;
  vbDesc.Usage  = D3DUSAGE_WRITEONLY;
  vbDesc.FVF    = m_u32FVF;
  if(!boStatic) vbDesc.Usage |= D3DUSAGE_DYNAMIC;

  HRESULT hrErr = m_pDevice->CreateVertexBuffer(vbDesc.Size,vbDesc.Usage,vbDesc.FVF,vbDesc.Pool,&m_pVB);
  if(FAILED(hrErr)) return hrErr;

  // base members

  m_u32Count  = m_u32ActiveCount = u32Count;
  m_boStatic  = boStatic;
  m_boRAMCopy = boRAMCopy;
  if(boRAMCopy) m_pBuffer = pBuffer;

  return WriteToHW(pBuffer,u32Count);                       // copy data to VB
  }

//----------------------------------------------- CreateCopy ---------------------------------------------------------//
// allocate memory for the buffer, and copy
// in : source buffer,nb vtx,vertex & texcoord formats,static & copy flags
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 CVBufferDX8::CreateCopy(void* pBuffer,const u32 u32Count,VTX_COMPONENTS& Vtx,TEX_COMPONENTS& Tex,const bool boStatic,const bool boRAMCopy)
  {
  if(!pBuffer || !u32Count) return -1;
  
  m_VtxFormat = Vtx;
  m_TexFormat = Tex;
  CalcVertexSize();
  
  if(boRAMCopy)
    {
    m_pBuffer = new u8[m_u32VtxSize*u32Count];
    if(!m_pBuffer) return -2;
    m_u32Count = m_u32ActiveCount = u32Count;

    WriteToRAM(pBuffer,u32Count);                           // DON'T changer order !
    Create(m_pBuffer,u32Count,Vtx,Tex,boStatic,boRAMCopy);
    }

  else
    {
    Create(pBuffer,u32Count,Vtx,Tex,boStatic,boRAMCopy);
    }

  return 0;
  }

//----------------------------------------------- CalcVertexFVF ------------------------------------------------------//
// helper : calculate FVF according to vtx & tex formats - static
// in : vtx & tex formats
// out: FVF
//--------------------------------------------------------------------------------------------------------------------//

u32 CVBufferDX8::CalcVertexFVF(VTX_COMPONENTS& VtxFormat,TEX_COMPONENTS& TexFormat)
  {
  u32 u32FVF = 0;

  if(VtxFormat.XYZ)
    {
    switch(VtxFormat.Weights)
      {
      case 0 : u32FVF = D3DFVF_XYZ;   break;
      case 1 : u32FVF = D3DFVF_XYZB1; break;
      case 2 : u32FVF = D3DFVF_XYZB2; break;
      case 3 : u32FVF = D3DFVF_XYZB3; break;
      }
    if(VtxFormat.Weights && VtxFormat.Indices)
      u32FVF |= D3DFVF_LASTBETA_UBYTE4;
    }

  if(VtxFormat.Normal)   u32FVF |= D3DFVF_NORMAL;
  if(VtxFormat.PtSize)   u32FVF |= D3DFVF_PSIZE;
  if(VtxFormat.Diffuse)  u32FVF |= D3DFVF_DIFFUSE;
  if(VtxFormat.Specular) u32FVF |= D3DFVF_SPECULAR;
  // rem: tangent & binormal (=pos2 & normal2) are not part of FVF

  switch(VtxFormat.Tex)
    {
    case 1 : u32FVF |= D3DFVF_TEX1; break;
    case 2 : u32FVF |= D3DFVF_TEX2; break;
    case 3 : u32FVF |= D3DFVF_TEX3; break;
    case 4 : u32FVF |= D3DFVF_TEX4; break;
    case 5 : u32FVF |= D3DFVF_TEX5; break;
    case 6 : u32FVF |= D3DFVF_TEX6; break;
    case 7 : u32FVF |= D3DFVF_TEX7; break;
    case 8 : u32FVF |= D3DFVF_TEX8; break;
    }

  if(VtxFormat.Tex >= 1) u32FVF |= TexCoordSize(TexFormat.Tex0,0);
  if(VtxFormat.Tex >= 2) u32FVF |= TexCoordSize(TexFormat.Tex1,1);
  if(VtxFormat.Tex >= 3) u32FVF |= TexCoordSize(TexFormat.Tex2,2);
  if(VtxFormat.Tex >= 4) u32FVF |= TexCoordSize(TexFormat.Tex3,3);
  if(VtxFormat.Tex >= 5) u32FVF |= TexCoordSize(TexFormat.Tex4,4);
  if(VtxFormat.Tex >= 6) u32FVF |= TexCoordSize(TexFormat.Tex5,5);
  if(VtxFormat.Tex >= 7) u32FVF |= TexCoordSize(TexFormat.Tex6,6);
  if(VtxFormat.Tex >= 8) u32FVF |= TexCoordSize(TexFormat.Tex7,7);

  return u32FVF;
  }

//----------------------------------------------- CalcVertexFVF ------------------------------------------------------//
// helper : calculate FVF according to vtx & tex formats
// in :
// out: FVF
//--------------------------------------------------------------------------------------------------------------------//

u32 CVBufferDX8::CalcVertexFVF()
  {
  m_u32FVF = CalcVertexFVF(m_VtxFormat,m_TexFormat);
  return m_u32FVF;
  }

//----------------------------------------------- TexCoordSize -------------------------------------------------------//
// helper : return TexCoordSize FVF flag - static
// in : nb tex coords,tex stage
// out: FVF flag
//--------------------------------------------------------------------------------------------------------------------//

u32 CVBufferDX8::TexCoordSize(const u32 u32Dim,const u32 u32Stage)
  {
  switch(u32Dim)
    {
    case 0 : return D3DFVF_TEXCOORDSIZE2(u32Stage);
    case 1 : return D3DFVF_TEXCOORDSIZE3(u32Stage);
    case 2 : return D3DFVF_TEXCOORDSIZE4(u32Stage);
    case 3 : return D3DFVF_TEXCOORDSIZE1(u32Stage);
    }
  return 0;
  }

//----------------------------------------------- Destroy ------------------------------------------------------------//
// release buffer's interface
// in :
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 CVBufferDX8::Destroy()
  {
  if(m_pVB) m_pVB->Release();
  m_pVB = NULL;
  return 0;
  }

//----------------------------------------------- Write --------------------------------------------------------------//
// modify buffer's content
// in : source buffer,nb vtx,start pos
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 CVBufferDX8::Write(void* pBuffer,const u32 u32Count,const u32 u32Pos)
  {
  WriteToRAM(pBuffer,u32Count,u32Pos);
  return WriteToHW(pBuffer,u32Count,u32Pos);
  }

//----------------------------------------------- WriteToHW ----------------------------------------------------------//
// modify HW buffer's content
// in : source buffer,nb vtx,start pos
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 CVBufferDX8::WriteToHW(void* pBuffer,const u32 u32Count,const u32 u32Pos)
  {
  if(!pBuffer      || !u32Count)   return -1;               // bad params
  if(!m_pVB)                       return -2;               // no buffer
  if(u32Pos+u32Count > m_u32Count) return -3;               // overflow

  // lock

  u32 u32Flags = D3DLOCK_NOSYSLOCK;
  if(IsDynamic()) u32Flags |= D3DLOCK_DISCARD;              // flush
  void* pLockedData = NULL;

  HRESULT hrErr = m_pVB->Lock(u32Pos*m_u32VtxSize,u32Count*m_u32VtxSize,(BYTE**)&pLockedData,u32Flags);
  if(FAILED(hrErr)) return -4;

  // copy

  Memory::MemCpy(pLockedData,pBuffer,u32Count*m_u32VtxSize);

  // unlock

  hrErr = m_pVB->Unlock();
  if(FAILED(hrErr)) return -5;
  return 0;
  }

//----------------------------------------------- UpdateFromRAM ------------------------------------------------------//
// copy RAM to video
// in : nb vtx,start pos
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 CVBufferDX8::UpdateFromRAM(const u32 u32Count,const u32 u32Pos)
  {
  if(!HasRAMCopy()) return -1;

  // TBI
  return 0;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
