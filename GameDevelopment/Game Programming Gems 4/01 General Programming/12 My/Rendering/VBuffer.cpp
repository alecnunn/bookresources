//--------------------------------------------------------------------------------------------------------------------//
//                                                VERTEX BUFFER CLASS                                                 //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "VBuffer.h"
#ifdef    _DEBUG
#include  "VBuffer.inl"
#endif

#include  "MemC.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI(CVBuffer,CRefCount);

CVBuffer::VTX_COMPONENTS CVBuffer::m_VtxDefault = { 1,0,0,1,0,0,0,0,0,1 };      // xyz+normal+tex0
CVBuffer::TEX_COMPONENTS CVBuffer::m_TexDefault = { 0,0,0,0,0,0,0,0 };

//--------------------------------------------------------------------------------------------------------------------//
//                                                MANAGEMENT                                                          //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- Create -------------------------------------------------------------//
// copy given parameters, calc vtx size
// in : buffer,nb vtx,vertex & texcoord formats,static & copy flags
// out: 0=OK
// rem: pBuffer's memory is now owned by this class
//--------------------------------------------------------------------------------------------------------------------//

u32 CVBuffer::Create(void* pBuffer,const u32 u32Count,VTX_COMPONENTS& Vtx,TEX_COMPONENTS& Tex,const bool boStatic,const bool boRAMCopy)
  {
  if(!pBuffer || !u32Count) return -1;

  m_pBuffer   = pBuffer;
  m_u32Count  = m_u32ActiveCount = u32Count;
  m_boStatic  = boStatic;
  m_boRAMCopy = true;                                       // if we have come to this point, the buffer is in RAM

  m_VtxFormat = Vtx;
  m_TexFormat = Tex;
  CalcVertexSize();

  return 0;
  }

//----------------------------------------------- CreateCopy ---------------------------------------------------------//
// allocate memory for the buffer, and copy
// in : source buffer,nb vtx,vertex & texcoord formats,static & copy flags
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 CVBuffer::CreateCopy(void* pBuffer,const u32 u32Count,VTX_COMPONENTS& Vtx,TEX_COMPONENTS& Tex,const bool boStatic,const bool boRAMCopy)
  {
  if(!pBuffer || !u32Count) return -1;

  m_VtxFormat = Vtx;
  m_TexFormat = Tex;
  CalcVertexSize();
  
  m_pBuffer = new u8[m_u32VtxSize*u32Count];
  if(!m_pBuffer) return -2;

  Create(m_pBuffer,u32Count,Vtx,Tex,boStatic,boRAMCopy);
  WriteToRAM(pBuffer,u32Count);
  return 0;
  }

//----------------------------------------------- CalcVertexSize -----------------------------------------------------//
// helper : calculate size according to vtx & tex formats
// in :
// out: size
//--------------------------------------------------------------------------------------------------------------------//

u32 CVBuffer::CalcVertexSize()
  {
  return(m_u32VtxSize = CalcVertexSize(m_VtxFormat,m_TexFormat));
  }

//----------------------------------------------- CalcVertexSize -----------------------------------------------------//
// helper : calculate size according to vtx & tex formats - static version
// in : vtx & tex formats
// out: size
//--------------------------------------------------------------------------------------------------------------------//

u32 CVBuffer::CalcVertexSize(const VTX_COMPONENTS& VtxFmt,const TEX_COMPONENTS& TexFmt)
  {
  u32 u32Size = 0;

  if(VtxFmt.XYZ)      u32Size += sizeof(float)*3;
  if(VtxFmt.Weights)  u32Size += sizeof(float)*VtxFmt.Weights;
  if(VtxFmt.Normal)   u32Size += sizeof(float)*3;
  if(VtxFmt.PtSize)   u32Size += sizeof(float)*1;
  if(VtxFmt.Diffuse)  u32Size += sizeof(u32)  *1;
  if(VtxFmt.Specular) u32Size += sizeof(u32)  *1;
  if(VtxFmt.Tangent)  u32Size += sizeof(float)*3;
  if(VtxFmt.Binormal) u32Size += sizeof(float)*3;

  if(VtxFmt.Tex > 0)  u32Size += sizeof(float)*(((TexFmt.Tex0+1) & 3)+1);
  if(VtxFmt.Tex > 1)  u32Size += sizeof(float)*(((TexFmt.Tex1+1) & 3)+1);
  if(VtxFmt.Tex > 2)  u32Size += sizeof(float)*(((TexFmt.Tex2+1) & 3)+1);
  if(VtxFmt.Tex > 3)  u32Size += sizeof(float)*(((TexFmt.Tex3+1) & 3)+1);
  if(VtxFmt.Tex > 4)  u32Size += sizeof(float)*(((TexFmt.Tex4+1) & 3)+1);
  if(VtxFmt.Tex > 5)  u32Size += sizeof(float)*(((TexFmt.Tex5+1) & 3)+1);
  if(VtxFmt.Tex > 6)  u32Size += sizeof(float)*(((TexFmt.Tex6+1) & 3)+1);
  if(VtxFmt.Tex > 7)  u32Size += sizeof(float)*(((TexFmt.Tex7+1) & 3)+1);

  return(u32Size);
  }

//----------------------------------------------- Destroy ------------------------------------------------------------//
// free buffer's memory
// in :
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 CVBuffer::Destroy()
  {
  if(m_pBuffer)
    {
    delete [] m_pBuffer;
    m_pBuffer = NULL;
    }

  m_u32Count  = m_u32ActiveCount = 0;
  m_boRAMCopy = false;
  return 0;
  }

//----------------------------------------------- Write --------------------------------------------------------------//
// modify buffer's content
// in : source buffer,nb vtx,start pos
// out: 0=OK
// rem: DON'T forget to override in derived classes !
//--------------------------------------------------------------------------------------------------------------------//

u32 CVBuffer::Write(void* pBuffer,const u32 u32Count,const u32 u32Pos)
  {
  return WriteToRAM(pBuffer,u32Count,u32Pos);
  }

//----------------------------------------------- Read ---------------------------------------------------------------//
// get buffer's content
// in : dest buffer,nb vtx,start pos
// out: 0=OK
// rem: only allowed if a copy exists in RAM
//--------------------------------------------------------------------------------------------------------------------//

u32 CVBuffer::Read(void* pBuffer,const u32 u32Count,const u32 u32Pos)
  {
  if(!pBuffer      || !u32Count)   return -1;               // bad params
  if(!HasRAMCopy() || !m_u32Count) return -2;               // no buffer in RAM
  if(u32Pos+u32Count > m_u32Count) return -3;               // overflow

  Memory::MemCpy(pBuffer,(u8*)m_pBuffer+u32Pos*m_u32VtxSize,u32Count*m_u32VtxSize);
  return 0;
  }

//----------------------------------------------- WriteToRAM ---------------------------------------------------------//
// modify RAM buffer's content
// in : source buffer,nb vtx,start pos
// out: 0=OK
// rem: use on derived class when several updates are needed consecutively, then call UpdateFromRAM
//      (to update HW buffer if any)
//--------------------------------------------------------------------------------------------------------------------//

u32 CVBuffer::WriteToRAM(void* pBuffer,const u32 u32Count,const u32 u32Pos)
  {
  if(!pBuffer      || !u32Count)   return -1;               // bad params
  if(!m_pBuffer    || !m_u32Count) return -2;               // no buffer
  if(u32Pos+u32Count > m_u32Count) return -3;               // overflow

  Memory::MemCpy((u8*)m_pBuffer+u32Pos*m_u32VtxSize,pBuffer,u32Count*m_u32VtxSize);
  return 0;
  }

//----------------------------------------------- UpdateFromRAM ------------------------------------------------------//
// copy RAM to video
// in : nb vtx,start pos
// out: 0=OK
// rem: designed for derived classes
//--------------------------------------------------------------------------------------------------------------------//

u32 CVBuffer::UpdateFromRAM(const u32 u32Count,const u32 u32Pos)
  {
  return 0;
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
