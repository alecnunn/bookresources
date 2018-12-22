/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/
#ifndef __NVLINK_H
#define __NVLINK_H

typedef DWORD NVLINK_FRAGMENT_ID;
typedef DWORD NVLINK_CONSTANT_ID;
typedef DWORD NVLINK_VERTEX_ID;
static const DWORD NVLINK_NOT_USED = 0xFFFFFFFF;

#define MAKE_NVLINKERROR_HRESULT( code )  MAKE_HRESULT( 1, FACILITY_ITF, code )
#define NVLINK_TOO_MANY_INSTRUCTIONS		MAKE_NVLINKERROR_HRESULT(0x400)
#define NVLINK_TOO_MANY_CONSTANTS			MAKE_NVLINKERROR_HRESULT(0x401)
#define NVLINK_TOO_MANY_VERTICES			MAKE_NVLINKERROR_HRESULT(0x402)
#define NVLINK_TOO_MANY_REGISTERS			MAKE_NVLINKERROR_HRESULT(0x403)
#define NVLINK_TOO_MANY_ADDRESS_REGISTERS	MAKE_NVLINKERROR_HRESULT(0x404)

typedef enum tageOPTIMIZERLEVEL
{
	OPTIMIZERLEVEL_NONE = 0,
	OPTIMIZERLEVEL_BASIC = 1,
	OPTIMIZERLEVEL_AGGRESSIVE = 2, // not implemented in version 2.
	OPTIMIZERLEVEL_FORCE_DWORD = 0x7fffffff, /* force 32-bit size enum */
} eOPTIMIZERLEVEL;

typedef struct tagNVLinkStatistics
{
	DWORD dwSize;
	DWORD dwInstructionsUsed;
	DWORD dwInstructionsRemoved;
	DWORD dwConstantsUsed;
	DWORD dwAddressRegistersUsed;
	DWORD dwRegistersUsed;
	DWORD dwVerticesUsed;
} NVLinkStatistics;

// Buffers returned from NVLink
DECLARE_INTERFACE_(INVLinkBuffer, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

	STDMETHOD_(LPVOID, GetPointer)() PURE;
	STDMETHOD_(DWORD, GetBufferSize)() PURE;
};

DECLARE_INTERFACE_(INVLink, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

	/*** INVLink methods ***/
	STDMETHOD(AddFragments)(const void* pFragmentFile, DWORD dwFileSize) PURE;
	STDMETHOD(GetFragmentID)(const char* szFragmentName, NVLINK_FRAGMENT_ID* pID) PURE;
	STDMETHOD(GetConstantID)(const char* szConstantName, NVLINK_CONSTANT_ID* pID) PURE;
	STDMETHOD(GetVertexID)(const char* szVertexName, NVLINK_VERTEX_ID* pID) PURE;
	STDMETHOD(ReserveConstantRange)(DWORD dwStartConstant, DWORD dwEndConstant) PURE;
	STDMETHOD(ReserveVertexRange)(DWORD dwStartVertex, DWORD dwEndVertex) PURE;
	STDMETHOD(ClearConstantRange)() PURE;
	STDMETHOD(ClearVertexRange)() PURE;
	STDMETHOD(CreateBinaryShader)(NVLINK_FRAGMENT_ID* pID, INVLinkBuffer** pReturn, DWORD dwOptions) PURE;
	STDMETHOD(CreateVertexShader)(LPDIRECT3DDEVICE8 pDevice, CONST DWORD* pDeclaration, NVLINK_FRAGMENT_ID* pIDs, DWORD* pHandle, DWORD Usage, DWORD dwOptions) PURE;
	STDMETHOD(DeleteVertexShader)(LPDIRECT3DDEVICE8 pDevice, DWORD dwHandle) PURE;
	STDMETHOD(GetShaderSource)(INVLinkBuffer** pReturn) PURE;
	STDMETHOD(GetStatistics)(NVLinkStatistics* pStats) PURE;
	STDMETHOD(GetConstantSlot)(NVLINK_CONSTANT_ID Constant, LONG Offset, DWORD* pLocation) PURE;
	STDMETHOD(GetVertexSlot)(NVLINK_VERTEX_ID Vertex, DWORD* pLocation) PURE;
	STDMETHOD(InvalidateCache)() PURE;	
};

// {561E7D82-C854-4ddd-A5E6-1E034047D062}
DEFINE_GUID(IID_INVLink, 0x561e7d82, 0xc854, 0x4ddd, 0xa5, 0xe6, 0x1e, 0x3, 0x40, 0x47, 0xd0, 0x62);

// {B9EF1812-A529-44cb-8341-663F0B2D9B9C}
DEFINE_GUID(IID_INVLinkBuffer, 0xb9ef1812, 0xa529, 0x44cb, 0x83, 0x41, 0x66, 0x3f, 0xb, 0x2d, 0x9b, 0x9c);

typedef INVLink *__cdecl CREATELINKER(DWORD dwCacheSize, eOPTIMIZERLEVEL);
typedef CREATELINKER* LPCREATELINKER;

#endif __NVLINK_H

