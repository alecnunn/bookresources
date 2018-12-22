//--------------------------------------------------------------------------------------------------------------------//
//                                                ENGINE MESH CLASS                                                   //
// written by Frederic My                                                                                             //
// fmy@fairyengine.com                                                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- INCLUDES -----------------------------------------------------------//

#include  "stdafx.h"
#include  "EngineMesh.h"
#ifdef    _DEBUG
#include  "EngineMesh.inl"
#endif

#include  "VectorMatrix.h"
#include  "Renderer.h"

namespace GPG4
{

//----------------------------------------------- STATIC MEMBERS -----------------------------------------------------//

IMPLEMENT_RTTI(CEngineMesh,CEngineNode);

//--------------------------------------------------------------------------------------------------------------------//
//                                                MANAGEMENT                                                          //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- CreateIBVB ---------------------------------------------------------//
// create empty IB & VB
// in : renderer
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 CEngineMesh::CreateIBVB(IRenderer* pRenderer)
  {
  if(!pRenderer) return -1;
  m_spIB = pRenderer->CreateIB();
  m_spVB = pRenderer->CreateVB();

  if(!m_spIB || !m_spVB)
    {
    m_spIB = NULL;
    m_spVB = NULL;
    return -2;
    }

  return 0;
  }

//----------------------------------------------- Create -------------------------------------------------------------//
// fill VB & IB
// in : vtx array,nb vtx,idx array,nb idx,vtx & texcoord formats,static & RAM flags
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 CEngineMesh::Create(void* pVtx,const u32 u32Vtx,u16* pIdx,const u32 u32Idx,CVBuffer::VTX_COMPONENTS& Vtx,
                        CVBuffer::TEX_COMPONENTS& Tex,const bool boStatic,const bool boRAMCopy)
  {
  if(!m_spIB || !m_spVB) return -1;

  if((0 != m_spVB->Create(pVtx,u32Vtx,Vtx,Tex,boStatic,boRAMCopy)) ||
     (0 != m_spIB->Create(pIdx,u32Idx,        boStatic,boRAMCopy)))
    {
    m_spIB->Destroy();
    m_spVB->Destroy();
    return -2;
    }

  return 0;
  }

//----------------------------------------------- CreateCopy ---------------------------------------------------------//
// fill VB & IB
// in : vtx array,nb vtx,idx array,nb idx,vtx & texcoord formats,static & RAM flags
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 CEngineMesh::CreateCopy(void* pVtx,const u32 u32Vtx,u16* pIdx,const u32 u32Idx,CVBuffer::VTX_COMPONENTS& Vtx,
                            CVBuffer::TEX_COMPONENTS& Tex,const bool boStatic,const bool boRAMCopy)
  {
  if(!m_spIB || !m_spVB) return -1;

  if((0 != m_spVB->CreateCopy(pVtx,u32Vtx,Vtx,Tex,boStatic,boRAMCopy)) ||
     (0 != m_spIB->CreateCopy(pIdx,u32Idx,        boStatic,boRAMCopy)))
    {
    m_spIB->Destroy();
    m_spVB->Destroy();
    return -2;
    }

  return 0;
  }

//----------------------------------------------- Clone --------------------------------------------------------------//
// create a mesh sharing the same data
// in : renderer
// out: new mesh (= clone)
//--------------------------------------------------------------------------------------------------------------------//

CEngineMesh* CEngineMesh::Clone(IRenderer* pRenderer)
  {
  CEngineMesh* pClone = new CEngineMesh;
  if(!pClone) return NULL;

  pClone->m_spIB  = m_spIB;
  pClone->m_spVB  = m_spVB;
  pClone->m_spMat = m_spMat;

  pClone->m_u32PrimType = m_u32PrimType;

  return pClone;
  UNREFERENCED_PARAMETER(pRenderer);
  }

//----------------------------------------------- Clone --------------------------------------------------------------//
// initialize a mesh sharing the same data
// in : clone
// out: OK ?
//--------------------------------------------------------------------------------------------------------------------//

bool CEngineMesh::Clone(CEngineMesh* pClone)
  {
  if(!pClone) return false;

  pClone->m_spIB  = m_spIB;
  pClone->m_spVB  = m_spVB;
  pClone->m_spMat = m_spMat;
  pClone->m_u32PrimType = m_u32PrimType;

  return true;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                HELPER                                                              //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- CreateBox ----------------------------------------------------------//
// create box mesh - static
// in : renderer,min & max coords
// out: box or NULL (= error)
//--------------------------------------------------------------------------------------------------------------------//

CEngineMesh* CEngineMesh::CreateBox(IRenderer* pRenderer,const CVect3D& v3Min,const CVect3D& v3Max)
  {
  if(!pRenderer) return NULL;
  CEngineMesh* pBox = pRenderer->CreateMesh();
  if(!pBox) return NULL;

  // vertices

  float fInvSqrt3    = 1.f/(float)sqrt(3.f);
  VERTEX_SIMPLE VtxArray[8] = { 
    // front face
    VERTEX_SIMPLE(CVect3D(v3Min.x,v3Max.y,v3Max.z), CVect3D(-fInvSqrt3,+fInvSqrt3,+fInvSqrt3), CVect2D(0.f,0.f)),
    VERTEX_SIMPLE(CVect3D(v3Min.x,v3Min.y,v3Max.z), CVect3D(-fInvSqrt3,-fInvSqrt3,+fInvSqrt3), CVect2D(0.f,1.f)),
    VERTEX_SIMPLE(CVect3D(v3Max.x,v3Min.y,v3Max.z), CVect3D(+fInvSqrt3,-fInvSqrt3,+fInvSqrt3), CVect2D(1.f,1.f)),
    VERTEX_SIMPLE(CVect3D(v3Max.x,v3Max.y,v3Max.z), CVect3D(+fInvSqrt3,+fInvSqrt3,+fInvSqrt3), CVect2D(1.f,0.f)),
    // back face
    VERTEX_SIMPLE(CVect3D(v3Min.x,v3Max.y,v3Min.z), CVect3D(-fInvSqrt3,+fInvSqrt3,-fInvSqrt3), CVect2D(1.f,0.f)),
    VERTEX_SIMPLE(CVect3D(v3Min.x,v3Min.y,v3Min.z), CVect3D(-fInvSqrt3,-fInvSqrt3,-fInvSqrt3), CVect2D(1.f,1.f)),
    VERTEX_SIMPLE(CVect3D(v3Max.x,v3Min.y,v3Min.z), CVect3D(+fInvSqrt3,-fInvSqrt3,-fInvSqrt3), CVect2D(0.f,1.f)),
    VERTEX_SIMPLE(CVect3D(v3Max.x,v3Max.y,v3Min.z), CVect3D(+fInvSqrt3,+fInvSqrt3,-fInvSqrt3), CVect2D(0.f,0.f))
    };

  // indices

  u16 IdxArray[6*2*3] = {
                         0,1,2, 0,2,3, 3,2,6, 3,6,7, 7,6,5, 7,5,4, 4,5,1, 4,1,0, 4,0,3, 4,3,7, 1,5,6, 1,6,2
                        }; 

  //

  if(0 != pBox->CreateCopy(VtxArray,8,IdxArray,6*2*3,CVBuffer::m_VtxDefault,CVBuffer::m_TexDefault,true,/*false*/true))
    {
    delete pBox;
    return NULL;
    }

  pBox->m_u32PrimType = IRenderer::TRILIST;
  return pBox;
  }

//----------------------------------------------- CreateEmptyGrid ----------------------------------------------------//
// create empty grid - static
// in : renderer,nb lines (1 of the 3 MUST be 0.f),extents
// out: grid or NULL (= error)
// rem: empty grid = no internal vertex
//      each square is 1*1 meter by construction, then the grid is scaled as wanted
//--------------------------------------------------------------------------------------------------------------------//

CEngineMesh* CEngineMesh::CreateEmptyGrid(IRenderer* pRenderer,const CVect3D& v3Lines,const CVect3D& v3Extent)
  {
  if(!pRenderer) return NULL;
  CEngineMesh* pGrid = pRenderer->CreateMesh();
  if(!pGrid) return NULL;

  // 3D->2D

  CVect3D v3Axis0(0.f),  v3Axis1(0.f);
  if(v3Lines.x == 0.f) { v3Axis0.y = 1.f; v3Axis1.z = 1.f; }
  if(v3Lines.y == 0.f) { v3Axis0.x = 1.f; v3Axis1.z = 1.f; }
  if(v3Lines.z == 0.f) { v3Axis0.x = 1.f; v3Axis1.y = 1.f; }

  u32 u32HalfCol = u32(v3Axis0*v3Lines*0.5f);
  if (u32HalfCol > 64) u32HalfCol = 64;
  u32 u32Cols    = u32HalfCol*2+1;

  u32 u32HalfRow = u32(v3Axis1*v3Lines*0.5f);
  if (u32HalfRow > 64) u32HalfRow = 64;
  u32 u32Rows    = u32HalfRow*2+1;

  float fMaxCol  = u32HalfCol*1.f;
  float fMaxRow  = u32HalfRow*1.f;
  float fExtCol  = v3Axis0*v3Extent*0.5f;
  float fExtRow  = v3Axis1*v3Extent*0.5f;
  u32   u32Num   = 0;
  u32   u32Vtx   = u32Cols*2+u32Rows*2;

  // vtx & idx

  CVect3D* pVtx  = new CVect3D[u32Vtx];
  u16*     pIdx  = new u16    [u32Vtx];
  if(!pVtx || !pIdx)
    {
    delete [] pVtx;
    delete [] pIdx;
    delete pGrid;
    return NULL;
    }

  for(f32 f32Col = -fMaxCol; f32Col <= fMaxCol; f32Col += 1.f, u32Num += 2)
    {
    pVtx[u32Num+0] = f32Col*v3Axis0 - fExtRow*v3Axis1;
    pVtx[u32Num+1] = f32Col*v3Axis0 + fExtRow*v3Axis1;

    pIdx[u32Num+0] = u16(u32Num+0);
    pIdx[u32Num+1] = u16(u32Num+1);
    }

  for(f32 f32Row = -fMaxRow; f32Row <= fMaxRow; f32Row += 1.f, u32Num += 2)
  {
    pVtx[u32Num+0] = -fExtCol*v3Axis0 + f32Row*v3Axis1;
    pVtx[u32Num+1] = +fExtCol*v3Axis0 + f32Row*v3Axis1;

    pIdx[u32Num+0] = u16(u32Num+0);
    pIdx[u32Num+1] = u16(u32Num+1);
  }

  CVBuffer::VTX_COMPONENTS VtxComp;
  CVBuffer::TEX_COMPONENTS TexComp;
  memset(&VtxComp,0,sizeof(VtxComp));
  memset(&TexComp,0,sizeof(TexComp));
  VtxComp.XYZ = 1;

  u32 u32Res = pGrid->CreateCopy(pVtx,u32Vtx,pIdx,u32Vtx,VtxComp,TexComp,true,false);
  delete [] pVtx;
  delete [] pIdx;

  if(u32Res != 0)
    {
    delete pGrid;
    return NULL;
    }

  pGrid->m_u32PrimType = IRenderer::LINELIST;
  return pGrid;
  }

//----------------------------------------------- CreateSphere -------------------------------------------------------//
// create sphere mesh - static
// in : renderer,radius,nb pts on section
// out: sphere or NULL (= error)
//--------------------------------------------------------------------------------------------------------------------//

CEngineMesh* CEngineMesh::CreateSphere(IRenderer* pRenderer,const float fRadius,const u32 u32SectionPts)
  {
  if(!pRenderer) return NULL;
  CEngineMesh* pSphere = pRenderer->CreateMesh();
  if(!pSphere)   return NULL;

  // vertices

  u32 u32SecPts = u32SectionPts;
  if(u32SecPts < 4) u32SecPts = 4;
  if(u32SecPts & 1) u32SecPts++;                            // multiple of 2

  u32 u32NbSec = u32SecPts/2-1;
  u32 u32NbPts = u32NbSec*u32SecPts+2;                      // 2 = poles

  VERTEX_SIMPLE* pVtxArray = new VERTEX_SIMPLE[u32NbPts];
  if(!pVtxArray)
    {
    delete pSphere;
    return NULL;
    }

  pVtxArray[0]          = VERTEX_SIMPLE(CVect3D(0.f, fRadius,0.f), CVect3D(0.f, 1.f,0.f), CVect2D(0.f,0.f));  // north pole
  pVtxArray[u32NbPts-1] = VERTEX_SIMPLE(CVect3D(0.f,-fRadius,0.f), CVect3D(0.f,-1.f,0.f), CVect2D(0.f,1.f));  // south pole

  for(u32 u32Row = 0; u32Row < u32NbSec; u32Row++)
    {
    float fSecAngl = PI/(u32NbSec+1)*(u32Row+1);
    float fX = -(float)sin(fSecAngl);
    float fY =  (float)cos(fSecAngl);

    for(u32 u32Col = 0; u32Col < u32SecPts; u32Col++)
      {
      float fColAngl = 2.f*PI/u32SecPts*u32Col;
      float fX2 =  fX*(float)cos(fColAngl);
      float fZ  = -fX*(float)sin(fColAngl);

      CVect3D v3N(fX2,fY,fZ);
      pVtxArray[u32Row*u32SecPts+u32Col+1] = VERTEX_SIMPLE(v3N*fRadius, v3N, CVect2D(float(u32Col)/u32SecPts,float(u32Row)/u32NbSec));
      }
    }

  // indices

  u32  u32NbIdx  = (2*u32SecPts + (u32NbSec-1)*u32SecPts*2)*3;
  u16* pIdxArray = new u16[u32NbIdx];
  if(!pIdxArray)
    {
    delete [] pVtxArray;
    delete pSphere;
    return NULL;
    }

    // sphere's top

  u32 u32Idx = 0;
  for(u32 u32Col = 0; u32Col < u32SecPts; u32Col++)
    {
    pIdxArray[u32Idx++] = 0;                                // north pole
    pIdxArray[u32Idx++] = u16(u32Col+1);
    pIdxArray[u32Idx++] = u16(u32Col+2);
    }
  pIdxArray[u32Idx-1] = 1;                                  // close shape

    // rings

  for(u32Row = 0; u32Row < u32NbSec-1; u32Row++)
    {
    u32 u32Start = u32SecPts*u32Row+1;

    for(u32Col = 0; u32Col < u32SecPts; u32Col++)
      {
      pIdxArray[u32Idx++] = u16(u32Col+u32Start);
      pIdxArray[u32Idx++] = u16(u32Col+u32Start+u32SecPts);
      pIdxArray[u32Idx++] = u16(u32Col+u32Start+u32SecPts+1);

      pIdxArray[u32Idx++] = u16(u32Col+u32Start);
      pIdxArray[u32Idx++] = u16(u32Col+u32Start+u32SecPts+1);
      pIdxArray[u32Idx++] = u16(u32Col+u32Start+1);
      }
    pIdxArray[u32Idx-4] = u16(u32Start+u32SecPts);          // close ring
    pIdxArray[u32Idx-2] = u16(u32Start+u32SecPts);
    pIdxArray[u32Idx-1] = u16(u32Start);
    }

    // sphere's bottom

  u32 u32Start = u32SecPts*(u32NbSec-1);
  for(u32Col = 0; u32Col < u32SecPts; u32Col++)
    {
    pIdxArray[u32Idx++] = u16(u32Start+u32Col+1);
    pIdxArray[u32Idx++] = u16(u32NbPts-1);                  // south pole
    pIdxArray[u32Idx++] = u16(u32Start+u32Col+2);
    }
  pIdxArray[u32Idx-1] = u16(u32Start+1);                    // close shape

  //

  if(0 != pSphere->CreateCopy(pVtxArray,u32NbPts,pIdxArray,u32NbIdx,CVBuffer::m_VtxDefault,CVBuffer::m_TexDefault,true,/*false*/true))
    {
    delete [] pIdxArray;
    delete [] pVtxArray;
    delete pSphere;
    return NULL;
    }

  pSphere->m_u32PrimType = IRenderer::TRILIST;
  delete [] pIdxArray;
  delete [] pVtxArray;
  return pSphere;
  }

//----------------------------------------------- CreateRectangle ----------------------------------------------------//
// create rectangle mesh - static
// in : renderer,left & right coords,uvs
// out: rectangle or NULL (= error)
//--------------------------------------------------------------------------------------------------------------------//

CEngineMesh* CEngineMesh::CreateRectangle(IRenderer* pRenderer,const CVect3D& v3Left,const CVect3D& v3Right,
                                                               const CVect2D& v2UV0, const CVect2D& v2UV1)
  {
  if(!pRenderer) return NULL;
  CEngineMesh* pRect = pRenderer->CreateMesh();
  if(!pRect) return NULL;

  // vertices

  CVect3D v3Pt[4];
  v3Pt[0].Set(v3Left);
  v3Pt[1].Set(v3Left.x,v3Right.y,v3Left.z);
  v3Pt[2].Set(v3Right);
  v3Pt[3].Set(v3Right.x,v3Left.y,v3Right.z);

  CVect3D v3N = (v3Pt[1]-v3Pt[0]) ^ (v3Pt[3]-v3Pt[0]);
  v3N.Normalize();

  VERTEX_SIMPLE VtxArray[4] = {
    VERTEX_SIMPLE(v3Pt[0], v3N, v2UV0),
    VERTEX_SIMPLE(v3Pt[1], v3N, CVect2D(v2UV0.u,v2UV1.v)),
    VERTEX_SIMPLE(v3Pt[2], v3N, v2UV1),
    VERTEX_SIMPLE(v3Pt[3], v3N, CVect2D(v2UV1.u,v2UV0.v)),
    };

  // indices

  u16 IdxArray[2*3] =  {
                         0,1,2, 0,2,3
                       }; 

  //

  if(0 != pRect->CreateCopy(VtxArray,4,IdxArray,2*3,CVBuffer::m_VtxDefault,CVBuffer::m_TexDefault,true,false))
    {
    delete pRect;
    return NULL;
    }

  pRect->m_u32PrimType = IRenderer::TRILIST;
  return pRect;
  }

//----------------------------------------------- CreateQuads --------------------------------------------------------//
// create quads mesh - static
// in : renderer,nb quads,vertices
// out: mesh or NULL (= error)
//--------------------------------------------------------------------------------------------------------------------//

CEngineMesh* CEngineMesh::CreateQuads(IRenderer* pRenderer,const u32 u32Quads,const VERTEX_SIMPLE* pVtx)
  {
  if(!pRenderer || !pVtx) return NULL;
  CEngineMesh* pQuads = pRenderer->CreateMesh();
  if(!pQuads) return NULL;

  // indices

  u16* pIdxArray = new u16[u32Quads*6];
  if(!pIdxArray)
    {
    delete pQuads;
    return NULL;
    }
  u16* pIdx = pIdxArray;

  for(u32 u32Quad = 0; u32Quad < u32Quads; u32Quad++)
    {
    *pIdx++ = u16(u32Quad*4+0);
    *pIdx++ = u16(u32Quad*4+1);
    *pIdx++ = u16(u32Quad*4+2);

    *pIdx++ = u16(u32Quad*4+0);
    *pIdx++ = u16(u32Quad*4+2);
    *pIdx++ = u16(u32Quad*4+3);
    }

  // mesh

  if(0 != pQuads->CreateCopy((void*)pVtx,u32Quads*4,pIdxArray,u32Quads*6,CVBuffer::m_VtxDefault,CVBuffer::m_TexDefault,true,false))
    {
    delete [] pIdxArray;
    delete pQuads;
    return NULL;
    }

  pQuads->m_u32PrimType = IRenderer::TRILIST;
  delete [] pIdxArray;
  return pQuads;
  }

//----------------------------------------------- CreateBlock --------------------------------------------------------//
// create grid mesh from heightmap - static
// in : renderer,heightmap,corners'UVs & XZs,min/max height,LOD,detail tiling
// out: mesh or NULL (= error)
//--------------------------------------------------------------------------------------------------------------------//

CEngineMesh* CEngineMesh::CreateBlock(IRenderer* pRenderer,const CEngineTexture& HeightMap,const CVect2D* pv2UV,const CVect2D* pv2XZ,
                                      const CVect2D& v2Alti,const u32 u32Resol,const CVect2D& v2Tiling)
  {
  if(!pRenderer) return NULL;
  CEngineMesh* pBlock = pRenderer->CreateMesh();
  if(!pBlock) return NULL;

  u32 u32NbVtx = (u32Resol+1)*(u32Resol+1);
  u32 u32NbIdx = u32Resol*u32Resol*6;

    // TBI : 2nd UV
  VERTEX_SIMPLE* pVtxArray = new VERTEX_SIMPLE[u32NbVtx];
  u16*           pIdxArray = new u16          [u32NbIdx];
  if(!pVtxArray || !pIdxArray)
    {
    delete [] pIdxArray;
    delete [] pVtxArray;
    delete pBlock;
    return NULL;
    }

  // fill vertices

  u32 u32Vtx = 0;

  float fX  =  pv2XZ[0].x;
  float fZ  =  pv2XZ[0].y;
  float fU  =  pv2UV[0].u;
  float fV  =  pv2UV[0].v;
  float fDx = (pv2XZ[1].x-pv2XZ[0].x)/float(u32Resol);
  float fDz = (pv2XZ[1].y-pv2XZ[0].y)/float(u32Resol);
  float fDu = (pv2UV[1].u-pv2UV[0].u)/float(u32Resol);
  float fDv = (pv2UV[1].v-pv2UV[0].v)/float(u32Resol);
  float fDy =  v2Alti.y  -v2Alti.x;

  for(u32 u32Row = 0; u32Row <= u32Resol; u32Row++)
    {
    for(u32 u32Col = 0; u32Col <= u32Resol; u32Col++)
      {
      CVect2D v2UV(fU,fV);
      float   fY = v2Alti.x + HeightMap.GetTexelValue(v2UV) *fDy;

        // TBI : get N  from heightmap
        // TBI : 2nd uv

      pVtxArray[u32Vtx++] = VERTEX_SIMPLE(CVect3D(fX,fY,fZ),CVect3D(0.f,1.f,0.f),v2UV);

      fX += fDx;
      fU += fDu;
      }

    fX  = pv2XZ[0].x;
    fU  = pv2UV[0].u;
    fZ += fDz;
    fV += fDv;
    }

  // fill indices

  u32 u32Idx  = 0;
  u32 u32Idx0 = 0;
  u32Vtx      = u32Resol+1;                                 // vertex per row

  for(u32Row = 0; u32Row < u32Resol; u32Row++)
    {
    u32Idx0 = u32Row*u32Vtx;

    for(u32 u32Col = 0; u32Col < u32Resol; u32Col++)
      {
      pIdxArray[u32Idx++] = u16(u32Idx0);
      pIdxArray[u32Idx++] = u16(u32Idx0+u32Vtx);
      pIdxArray[u32Idx++] = u16(u32Idx0+u32Vtx+1);

      pIdxArray[u32Idx++] = u16(u32Idx0);
      pIdxArray[u32Idx++] = u16(u32Idx0+u32Vtx+1);
      pIdxArray[u32Idx++] = u16(u32Idx0+1);

      u32Idx0++;
      }
    }

  //

  if(0 != pBlock->CreateCopy(pVtxArray,u32NbVtx,pIdxArray,u32NbIdx,CVBuffer::m_VtxDefault,CVBuffer::m_TexDefault,true,/*false*/true))
    {
    delete [] pIdxArray;
    delete [] pVtxArray;
    delete pBlock;;
    return NULL;
    }

  pBlock->m_u32PrimType = IRenderer::TRILIST;
  delete [] pIdxArray;
  delete [] pVtxArray;
  return pBlock;
  }

//----------------------------------------------- CreateFromIBVB -----------------------------------------------------//
// create mesh - static
// in : renderer,vb,ib,primitive
// out: mesh or NULL (= error)
//--------------------------------------------------------------------------------------------------------------------//

CEngineMesh* CEngineMesh::CreateFromIBVB(IRenderer* pRenderer,CVBuffer* pVB,CIBuffer* pIB,IRenderer::RENDERER_PRIMITIVE Prim)
  {
  if(!pRenderer) return NULL;
  CEngineMesh* pMesh = pRenderer->CreateMesh();
  if(!pMesh) return NULL;

  pMesh->m_spVB = pVB;
  pMesh->m_spIB = pIB;
  pMesh->m_u32PrimType = Prim;
  return pMesh;
  }

//--------------------------------------------------------------------------------------------------------------------//
//                                                DRAW                                                                //
//--------------------------------------------------------------------------------------------------------------------//

//----------------------------------------------- Draw ---------------------------------------------------------------//
// render mesh
// in : renderer,flags,condition callback
// out: 0=OK
//--------------------------------------------------------------------------------------------------------------------//

u32 CEngineMesh::Draw(IRenderer* pRenderer,const u32 u32Flags,pfnDrawCondition pfnCondition)
  {
  if(!pRenderer) return -1;
  CEngineNode::Draw(pRenderer,u32Flags,pfnCondition);       // draw subnodes if any
  if(!m_boDrawNode || m_boDeleted) return 0;

  if(pfnCondition && !(*pfnCondition)(this)) return 0;

  if(u32Flags & SELECTED)
    {
    pRenderer->SetWireframe(true);
    pRenderer->SetMaterial(pRenderer->GetMatWhiteSelf());
    pRenderer->SetModel2World(GetWorldTrf());
    u32 u32Res = pRenderer->DrawVBIB(m_u32PrimType,m_spVB,m_spIB);
    pRenderer->SetWireframe(false);
    return u32Res;
    }

  if(m_spMat) pRenderer->SetMaterial(m_spMat);
  else        pRenderer->SetMaterial(pRenderer->GetMatWhiteSelf());
  pRenderer->SetModel2World(GetWorldTrf());
  return pRenderer->DrawVBIB(m_u32PrimType,m_spVB,m_spIB);
  }

//--------------------------------------------------------------------------------------------------------------------//

} // namespace
