// File: ZFXD3D_misc.cpp

#include "math.h"          // sqrt function
#include "ZFX.h"           // return values and stuff
#include "ZFXD3D.h"        // class definition
#include "d3dx9.h"         // shader compiler




extern bool g_bLF;

/*-----------------------------------------------------------*/
/* ZFXD3D class implementation                               *
/*-----------------------------------------------------------*/

/**
 * Set mode for projection and view for given Stage.
 * -> IN: ZFXENGINEMODE - mode perspective, ortho or 2D
 *        int           - stage, not needed for 2D mode
 */
HRESULT ZFXD3D::SetMode(ZFXENGINEMODE Mode, int nStage) {
   D3DVIEWPORT9 d3dVP;

   if (!m_bRunning) return E_FAIL;
   if ((nStage > 3) || (nStage < 0)) nStage=0;
   
   if (m_Mode != Mode)
      m_Mode  = Mode;

   // we change fundamentals settings so fush all content
   m_pVertexMan->ForcedFlushAll();

   m_nStage = nStage;

   // set viewport
   d3dVP.X      = m_VP[nStage].X;
   d3dVP.Y      = m_VP[nStage].Y;
   d3dVP.Width  = m_VP[nStage].Width;
   d3dVP.Height = m_VP[nStage].Height;
   d3dVP.MinZ   = 0.0f;
   d3dVP.MaxZ   = 1.0f;

   // if 2D mode set orthogonal projection and view
   if (Mode==EMD_TWOD) {
      if (FAILED(m_pDevice->SetViewport(&d3dVP)))
         return ZFX_FAIL;

      if (!m_bUseShaders) {
         if (FAILED(m_pDevice->SetTransform(D3DTS_PROJECTION, &m_mProj2D)))
            return ZFX_FAIL;

         if (FAILED(m_pDevice->SetTransform(D3DTS_VIEW, &m_mView2D)))
            return ZFX_FAIL;
         }
      }

   // perspective or orthogonal projection
   else {
      if (FAILED(m_pDevice->SetViewport(&d3dVP)))
         return ZFX_FAIL;

      if (!m_bUseShaders) {
         if (FAILED(m_pDevice->SetTransform(D3DTS_VIEW, &m_mView3D)))
            return ZFX_FAIL;

         if (m_Mode == EMD_PERSPECTIVE) {
            if (FAILED(m_pDevice->SetTransform(D3DTS_PROJECTION, 
                                               &m_mProjP[nStage])))
               return ZFX_FAIL;
            }
         else { // EMD_ORTHOGONAL
            if (FAILED(m_pDevice->SetTransform(D3DTS_PROJECTION, 
                                               &m_mProjO[nStage])))
               return ZFX_FAIL;
            }
         }
      CalcViewProjMatrix();
      CalcWorldViewProjMatrix();
      }
   return ZFX_OK;
   } // SetMode
/*----------------------------------------------------------------*/

/**
 * Calculate perspective and orthogonal projection matrix for given
 * stage using given values. Thi will not activate any settings.
 * -> IN: float       - field of view (horizontal)
 *        ZFXVIEWPORT - viewport dimensions or NULL for screen dimensions
 *        int         - which stage to set (4 possible)
 */
HRESULT ZFXD3D::InitStage(float fFOV, ZFXVIEWPORT *pView, int nStage) {
   float fAspect;
   bool  bOwnRect=false;
   
   if (!pView) {
      ZFXVIEWPORT vpOwn = { 0, 0, m_dwWidth, m_dwHeight};
      memcpy(&m_VP[nStage], &vpOwn, sizeof(RECT));
      }
   else
      memcpy(&m_VP[nStage], pView, sizeof(RECT));

   if ( (nStage>3) || (nStage<0) ) nStage=0;

   fAspect = ((float)(m_VP[nStage].Height)) / (m_VP[nStage].Width);

   // PERSPECTIVE PROJEKTION MATRIX
   if (FAILED(this->CalcPerspProjMatrix(fFOV, fAspect, &m_mProjP[nStage])))
      return ZFX_FAIL;

   // ORTHOGONAL PROJECTION MATRIX
   memset(&m_mProjO[nStage], 0, sizeof(float)*16); 
   m_mProjO[nStage]._11 =  2.0f/m_VP[nStage].Width;
   m_mProjO[nStage]._22 =  2.0f/m_VP[nStage].Height;
   m_mProjO[nStage]._33 =  1.0f/(m_fFar-m_fNear);

   m_mProjO[nStage]._43 =  m_fNear/(m_fNear-m_fFar);
   m_mProjO[nStage]._44 =  1.0f;

   return ZFX_OK;
   } // InitStage
/*----------------------------------------------------------------*/


/**
 * Recalculate view-projection combo matrix each time one of them
 * is changing to maintain integrity.
 */
void ZFXD3D::CalcViewProjMatrix(void) {
   ZFXMatrix *pA;
   ZFXMatrix *pB;

   // 2D, perspective or orthogonal mode
   if (m_Mode == EMD_TWOD) {
      pA = (ZFXMatrix*)&m_mProj2D;
      pB = (ZFXMatrix*)&m_mView2D;
      }
   else {
      pB = (ZFXMatrix*)&m_mView3D;

      if (m_Mode == EMD_PERSPECTIVE)
         pA = (ZFXMatrix*)&(m_mProjP[m_nStage]);
      else
         pA = (ZFXMatrix*)&(m_mProjO[m_nStage]);
      }

   ZFXMatrix *pM = (ZFXMatrix*)&m_mViewProj;
   (*pM) = (*pB) * (*pA);
   } // CalcViewProjMatrix
/*----------------------------------------------------------------*/



void ZFXD3D::CalcOrthoProjMatrix(float l, float r, float b, float t,
                                 float fN, float fF, int nStage) {
   float x =  2.0f / (r-l);
   float y =  2.0f / (t-b);
   float z =  2.0f / (fF-fN);
   float tx = -(r+l) / (r-l);
   float ty = -(t+b) / (t-b);
   float tz = -(fF+fN) / (fF-fN);

   memset(&m_mProjO[nStage], 0, sizeof(D3DMATRIX));
   m_mProjO[nStage]._11 = x;
   m_mProjO[nStage]._22 = y;
   m_mProjO[nStage]._33 = z;
   m_mProjO[nStage]._44 = 1.0f;
   m_mProjO[nStage]._41 = tx;
   m_mProjO[nStage]._42 = ty;
   m_mProjO[nStage]._43 = tz;
   } // CalcOrthoProjMatrix
/*----------------------------------------------------------------*/


/**
 * Calculate a combo matrix for world, view and projection.
 */
void ZFXD3D::CalcWorldViewProjMatrix(void) {
   ZFXMatrix *pProj;
   ZFXMatrix *pView;
   ZFXMatrix *pWorld;

   pWorld = (ZFXMatrix*)&m_mWorld;

   // 2D, perspective or orthogonal mode
   if (m_Mode == EMD_TWOD) {
      pProj = (ZFXMatrix*)&m_mProj2D;
      pView = (ZFXMatrix*)&m_mView2D;
      }
   else {
      pView = (ZFXMatrix*)&m_mView3D;

      if (m_Mode == EMD_PERSPECTIVE)
         pProj = (ZFXMatrix*)&(m_mProjP[m_nStage]);
      else
         pProj = (ZFXMatrix*)&(m_mProjO[m_nStage]);
      }

   ZFXMatrix *pCombo = (ZFXMatrix*)&m_mWorldViewProj;
   (*pCombo) = ((*pWorld) * (*pView)) * (*pProj);
   } // CalcWorldViewProjMatrix
/*----------------------------------------------------------------*/


/**
 * Calculate perspective 3D projection matrix.
 * -> IN: float     - field of horizontal view
 *        float     - aspect ration (viewport height / width)
 *        D3DMATRIX - adress to store result
 */
HRESULT ZFXD3D::CalcPerspProjMatrix(float fFOV, float fAspect, D3DMATRIX *m) {
   if(fabs(m_fFar - m_fNear) < 0.01f)
      return ZFX_FAIL;

   float sinFOV2 = sinf(fFOV/2);

   if(fabs(sinFOV2) < 0.01f)
      return ZFX_FAIL;

   float cosFOV2 = cosf(fFOV/2);

   float w = fAspect * (cosFOV2 / sinFOV2);
   float h =   1.0f  * (cosFOV2 / sinFOV2);
   float Q = m_fFar / (m_fFar - m_fNear);

   memset(m, 0, sizeof(D3DMATRIX));
   (*m)._11 = w;
   (*m)._22 = h;
   (*m)._33 = Q;
   (*m)._34 = 1.0f;
   (*m)._43 = -Q*m_fNear;
   return ZFX_OK;
   } // CalcPerspProjMatrix
/*----------------------------------------------------------------*/


/**
 * Set camera as viewmatrix.
 * -> IN: float* - 4 float building camera's right vector
 *        float* - 4 float building camera's up vector
 *        float* - 4 float building camera's direction vector
 *        float* - 4 float building camera's pos vector
 */
HRESULT ZFXD3D::SetView3D(const ZFXVector &vcRight, 
                          const ZFXVector &vcUp, 
                          const ZFXVector &vcDir,
                          const ZFXVector &vcPos) {
   if (!m_bRunning) return E_FAIL;

   m_mView3D._14 = m_mView3D._21 = m_mView3D._34 = 0.0f;
   m_mView3D._44 = 1.0f;

   m_mView3D._11 = vcRight.x;
   m_mView3D._21 = vcRight.y;
   m_mView3D._31 = vcRight.z;
   m_mView3D._41 = - (vcRight*vcPos);

   m_mView3D._12 = vcUp.x;
   m_mView3D._22 = vcUp.y;
   m_mView3D._32 = vcUp.z;
   m_mView3D._42 = - (vcUp*vcPos);

   m_mView3D._13 = vcDir.x;
   m_mView3D._23 = vcDir.y;
   m_mView3D._33 = vcDir.z;
   m_mView3D._43 = - (vcDir*vcPos);

   if (!m_bUseShaders) {
      // activate viewmatrix for D3D
      if (FAILED(m_pDevice->SetTransform(D3DTS_VIEW, &m_mView3D)))
         return ZFX_FAIL;
      }

   // view changed so recalculate combomatrix
   CalcViewProjMatrix();
   CalcWorldViewProjMatrix();
   return ZFX_OK;
   }
/*----------------------------------------------------------------*/


/**
 * Calculates look at matrix for given point and calls SetView3D
 * to active the look at matrix.
 * -> IN: float* - 3 floats building camera's psoition vector
 *        float* - 3 floats building lookat point vector
 *        float* - 3 floats building world up vector at cam pos
 */
HRESULT ZFXD3D::SetViewLookAt(const ZFXVector &vcPos, 
                              const ZFXVector &vcPoint, 
                              const ZFXVector &vcWorldUp) {
   ZFXVector vcDir, vcTemp, vcUp;

   vcDir = vcPoint - vcPos;
   vcDir.Normalize();

   // calculate up vector
   float fDot = vcWorldUp * vcDir;
   
   vcTemp = vcDir * fDot;
   vcUp = vcWorldUp - vcTemp;
   float fL = vcUp.GetLength();

   // if length too small take normal y axis as up vector
   if (fL < 1e-6f) {
      ZFXVector vcY;
      vcY.Set(0.0f, 1.0f, 0.0f);

      vcTemp = vcDir * vcDir.y;
      vcUp = vcY - vcTemp;

      fL = vcUp.GetLength();

      // if still too small take z axis as up vector
      if (fL < 1e-6f) {
         vcY.Set(0.0f, 0.0f, 1.0f);

         vcTemp = vcDir * vcDir.z;
         vcUp = vcY - vcTemp;

         // if still too small we are lost         
         fL = vcUp.GetLength();
         if(fL < 1e-6f) return ZFX_FAIL;
         }
      }

   vcUp /= fL;

   // build right vector using cross product
   ZFXVector vcRight;
   vcRight.Cross(vcUp, vcDir);

   // build final matrix and set for device
   return SetView3D(vcRight, vcUp, vcDir, vcPos);
   } // SetViewLookAt
/*----------------------------------------------------------------*/


/**
 * Set Clipping Planes and take care of other values that need
 * to be changed then. 
 */
void ZFXD3D::SetClippingPlanes(float fNear, float fFar) {
   m_fNear = fNear;
   m_fFar  = fFar;

   if (m_fNear <= 0.0f)
      m_fNear = 0.01f;

   if (m_fFar <= 1.0f)
      m_fFar = 1.00f;
   
   if (m_fNear >= m_fFar) {
      m_fNear = m_fFar;
      m_fFar  = m_fNear + 1.0f;
      }

   // change 2D projection and view
   Prepare2D();

   // change orthogonal projection
   float Q = 1.0f / (m_fFar-m_fNear);
   float X = m_fNear/(m_fNear-m_fFar);
   m_mProjO[0]._33 = m_mProjO[1]._33 = Q;
   m_mProjO[2]._33 = m_mProjO[3]._33 = Q;
   m_mProjO[0]._43 = m_mProjO[1]._43 = X;
   m_mProjO[2]._43 = m_mProjO[3]._43 = X;

   // change perspective projection
   Q *= m_fFar;
   X = -Q * m_fNear;
   m_mProjP[0]._33 = m_mProjP[1]._33 = Q;
   m_mProjP[2]._33 = m_mProjP[3]._33 = Q;
   m_mProjP[0]._43 = m_mProjP[1]._43 = X;
   m_mProjP[2]._43 = m_mProjP[3]._43 = X;
   } // SetClippingPlanes
/*----------------------------------------------------------------*/


/**
 * Call this prior to but not instead of SetMode(ORTHO,n)
 */
void ZFXD3D::SetOrthoScale(float fScale, int n) {
   float fW = ((float)m_dwWidth)/m_dwHeight * fScale;
   float fH = fScale;
   memset(&m_mProjO[n], 0, sizeof(D3DMATRIX));
   m_mProjO[n]._11 = 2.0f / fW;
   m_mProjO[n]._22 = 2.0f / fH;
   m_mProjO[n]._33 = 1.0f / (m_fFar-m_fNear);
   m_mProjO[n]._43 = m_fNear / (m_fNear-m_fFar);
   m_mProjO[n]._44 = 1.0f;
   } // SetOrthoScale
/*----------------------------------------------------------------*/


/**
 * Calculate orthogonal projection and view matrix to render
 * vertices who's world-coordinates are given in screen space.
 */
void ZFXD3D::Prepare2D(void) {
   // set matrices to identity
   memset(&m_mProj2D, 0, sizeof(float)*16); 
   memset(&m_mView2D, 0, sizeof(float)*16); 
   m_mView2D._11 = m_mView2D._33 = m_mView2D._44 = 1.0f;

   // orthogonal projection matrix
   m_mProj2D._11 =  2.0f/(float)m_dwWidth;
   m_mProj2D._22 =  2.0f/(float)m_dwHeight;
   m_mProj2D._33 =  1.0f/(m_fFar-m_fNear);
   m_mProj2D._43 = -m_fNear*(1.0f/(m_fFar-m_fNear));
   m_mProj2D._44 =  1.0f;

   // 2D view matrix
   float tx, ty, tz;
   tx = -((int)m_dwWidth) + m_dwWidth * 0.5f;
   ty =  m_dwHeight - m_dwHeight  * 0.5f;
   tz =  m_fNear + 0.1f;

   m_mView2D._22 = -1.0f;
   m_mView2D._41 = tx; 
   m_mView2D._42 = ty; 
   m_mView2D._43 = tz;
   }// Prepare2D
/*----------------------------------------------------------------*/


/**
 * Retrieve active frustrum planes, normals pointing outwards.
 * -> IN: ZFXPlane* - address to store 6 planes
 */
HRESULT ZFXD3D::GetFrustrum(ZFXPlane *p) {
   // left plane
   p[0].m_vcN.x = -(m_mViewProj._14 + m_mViewProj._11);
   p[0].m_vcN.y = -(m_mViewProj._24 + m_mViewProj._21);
   p[0].m_vcN.z = -(m_mViewProj._34 + m_mViewProj._31);
   p[0].m_fD    = -(m_mViewProj._44 + m_mViewProj._41);

   // right plane
   p[1].m_vcN.x = -(m_mViewProj._14 - m_mViewProj._11);
   p[1].m_vcN.y = -(m_mViewProj._24 - m_mViewProj._21);
   p[1].m_vcN.z = -(m_mViewProj._34 - m_mViewProj._31);
   p[1].m_fD    = -(m_mViewProj._44 - m_mViewProj._41);

   // top plane
   p[2].m_vcN.x = -(m_mViewProj._14 - m_mViewProj._12);
   p[2].m_vcN.y = -(m_mViewProj._24 - m_mViewProj._22);
   p[2].m_vcN.z = -(m_mViewProj._34 - m_mViewProj._32);
   p[2].m_fD    = -(m_mViewProj._44 - m_mViewProj._42);
   
   // bottom plane
   p[3].m_vcN.x = -(m_mViewProj._14 + m_mViewProj._12);
   p[3].m_vcN.y = -(m_mViewProj._24 + m_mViewProj._22);
   p[3].m_vcN.z = -(m_mViewProj._34 + m_mViewProj._32);
   p[3].m_fD    = -(m_mViewProj._44 + m_mViewProj._42);

   // near plane
   p[4].m_vcN.x = -m_mViewProj._13;
   p[4].m_vcN.y = -m_mViewProj._23;
   p[4].m_vcN.z = -m_mViewProj._33;
   p[4].m_fD    = -m_mViewProj._43;

   // far plane
   p[5].m_vcN.x = -(m_mViewProj._14 - m_mViewProj._13);
   p[5].m_vcN.y = -(m_mViewProj._24 - m_mViewProj._23);
   p[5].m_vcN.z = -(m_mViewProj._34 - m_mViewProj._33);
   p[5].m_fD    = -(m_mViewProj._44 - m_mViewProj._43);

   // normalize frustrum normals
   for (int i=0;i<6;i++) {
      float fL = p[i].m_vcN.GetLength();
      p[i].m_vcN /= fL;
      p[i].m_fD  /= fL;
      }

   return ZFX_OK;
   }
/*----------------------------------------------------------------*/

/**
 * Cast a world ray from a given position on screen.
 * -> IN:  POINT      - point in screen coordinates
 * -> OUT: ZFXVector* - to fill with 4 floats for origin vextor
 *         ZFXVector* - to fill with 4 floats for ray direction
 */
void  ZFXD3D::Transform2Dto3D(const POINT &pt, 
                              ZFXVector *vcOrig, 
                              ZFXVector *vcDir) {
   D3DMATRIX *pView=NULL, *pProj=NULL;
   ZFXMatrix mInvView;
   ZFXVector vcS;
   DWORD     dwWidth,
             dwHeight;
 
   // if 2D mode
   if (m_Mode == EMD_TWOD) {
      dwWidth  = m_dwWidth;
      dwHeight = m_dwHeight;

      pView = &m_mView2D;
      }
   // else ortho or perspective projection
   else {
      dwWidth  = m_VP[m_nStage].Width,
      dwHeight = m_VP[m_nStage].Height;

      pView = &m_mView3D;

      if (m_Mode == EMD_PERSPECTIVE)
         pProj = &m_mProjP[m_nStage];
      else
         pProj = &m_mProjO[m_nStage];
      }

   // resize to viewportspace [-1,1] -> projection
   vcS.x =  ( ((pt.x*2.0f) / dwWidth) -1.0f) / m_mProjP[m_nStage]._11;
   vcS.y = -( ((pt.y*2.0f) / dwHeight)-1.0f) / m_mProjP[m_nStage]._22;
   vcS.z = 1.0f;

   // invert view matrix
   mInvView.InverseOf(*((ZFXMatrix*)&m_mView3D._11));

   // ray from screen to worldspace
   (*vcDir).x = (vcS.x * mInvView._11)
              + (vcS.y * mInvView._21)
              + (vcS.z * mInvView._31);
   (*vcDir).y = (vcS.x * mInvView._12)
              + (vcS.y * mInvView._22)
              + (vcS.z * mInvView._32);
   (*vcDir).z = (vcS.x * mInvView._13)
              + (vcS.y * mInvView._23)
              + (vcS.z * mInvView._33);
   
   // inverse translation.
   (*vcOrig).x = mInvView._41;
   (*vcOrig).y = mInvView._42;
   (*vcOrig).z = mInvView._43;

   // normalize
   (*vcOrig).Normalize();
   }
/*----------------------------------------------------------------*/

/**
 * Cast given point from world space to screen space.
 * -> IN:     ZFXVector - vector to position in 3d space
 * -> RETURN: POINT     - corresponding point in 2d screen space
 */
POINT ZFXD3D::Transform3Dto2D(const ZFXVector &vcPoint) {
   POINT pt;
   float fClip_x, fClip_y;
   float fXp, fYp, fWp;
   DWORD dwWidth, dwHeight;

   // if 2D mode use whole screen
   if (m_Mode == EMD_TWOD) {
      dwWidth  = m_dwWidth;
      dwHeight = m_dwHeight;
      }
   // else take viewport dimensions
   else {
      dwWidth  = m_VP[m_nStage].Width,
      dwHeight = m_VP[m_nStage].Height;
      }

   fClip_x = (float)(dwWidth  >> 1);
   fClip_y = (float)(dwHeight >> 1);

   fXp = (m_mViewProj._11*vcPoint.x) + (m_mViewProj._21*vcPoint.y) 
       + (m_mViewProj._31*vcPoint.z) + m_mViewProj._41;
   fYp = (m_mViewProj._12*vcPoint.x) + (m_mViewProj._22*vcPoint.y) 
       + (m_mViewProj._32*vcPoint.z) + m_mViewProj._42;
   fWp = (m_mViewProj._14*vcPoint.x) + (m_mViewProj._24*vcPoint.y) 
       + (m_mViewProj._34*vcPoint.z) + m_mViewProj._44;

   float fWpInv = 1.0f / fWp;

   // transform from [-1,1] to actual viewport dimensions
   pt.x = (LONG)( (1.0f + (fXp * fWpInv)) * fClip_x );
   pt.y = (LONG)( (1.0f + (fYp * fWpInv)) * fClip_y );

   return pt;
   }
/*----------------------------------------------------------------*/


/**
 * Cast the current cursor position to world coordinates in ortho
 * views. User has to take care of third coordinate. 
 */
ZFXVector ZFXD3D::Transform2Dto2D(UINT nHwnd, float fScale, 
                                  const POINT *pPt, ZFXAXIS axis) {
   ZFXVector vcResult(0,0,0);
   POINT ptC = { -1, -1 };
   RECT  rect;

   if (!m_d3dpp.Windowed) return vcResult;
   else if (m_pChain[nHwnd]==NULL) return vcResult;

   GetClientRect(m_hWnd[nHwnd], &rect);

   if (!pPt) {
      GetCursorPos(&ptC);
      ScreenToClient(m_hWnd[nHwnd], &ptC);
      }
   else memcpy(&ptC, pPt, sizeof(POINT));

   ptC.x -= long( ((float)rect.right) /2.0f );
   ptC.y -= long( ((float)rect.bottom)/2.0f );

   // use buttom for both as horizontal view angle is not known
   if (axis == Z_AXIS) {
      vcResult.x = ((float)ptC.x)/rect.bottom * fScale;
      vcResult.y = -((float)ptC.y)/rect.bottom * fScale;
      vcResult.z = 0.0f;
      }
   else if (axis == X_AXIS) {
      vcResult.x = 0.0f;
      vcResult.y = -((float)ptC.y)/rect.bottom * fScale;
      vcResult.z = ((float)ptC.x)/rect.bottom * fScale;
      }
   else if (axis == Y_AXIS) {
      vcResult.x = ((float)ptC.x)/rect.bottom * fScale;
      vcResult.y = 0.0f;
      vcResult.z = -((float)ptC.y)/rect.bottom * fScale;
      }
   return vcResult;
   } // Transform2Dto2D
/*----------------------------------------------------------------*/


/**
 * This functions evaluates shader support and initializes everything
 * we need to use shaders if they are available.
 */
void ZFXD3D::PrepareShaderStuff(void) {
   D3DCAPS9 d3dCaps;

   if (FAILED(m_pDevice->GetDeviceCaps(&d3dCaps))) {
      Log("error: GetDeviceCaps() in PrepareShaderStuff failed");
      m_bCanDoShaders = false;
      return;
      }

   if (d3dCaps.VertexShaderVersion < D3DVS_VERSION(1,1) ) {
      Log("warning: Vertex Shader Version < 1.1 => no support");
      m_bCanDoShaders = false;
      return;
      }

   if (d3dCaps.PixelShaderVersion < D3DPS_VERSION(1,1) ) {
      Log("warning: Pixel Shader Version < 1.1 => no support");
      m_bCanDoShaders = false;
      return;
      }

   // vertex declarations needed for shaders
   D3DVERTEXELEMENT9 declPVertex[] =
      {
         { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 
         D3DDECL_END()
      };

   D3DVERTEXELEMENT9 declVertex[] =
      {
         { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 
         { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL,   0}, 
         { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0}, 
         D3DDECL_END()
      };

   D3DVERTEXELEMENT9 declLVertex[] =
      {
         { 0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 
         { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_COLOR,    0}, 
         { 0, 16, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0}, 
         D3DDECL_END()
      };

   D3DVERTEXELEMENT9 declCVertex[] =
      {
         { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 
         { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL,   0}, 
         { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0}, 
         { 0, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 1}, 
         D3DDECL_END()
      };

   D3DVERTEXELEMENT9 decl3TVertex[] =
      {
         { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 
         { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL,   0}, 
         { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0}, 
         { 0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 1}, 
         { 0, 40, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 2}, 
         D3DDECL_END()
      };

   D3DVERTEXELEMENT9 declTVertex[] = 
      {
         { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 
         { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL,   0}, 
         { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0}, 
         { 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TANGENT,  0}, 
         D3DDECL_END() 
      };

   // create the declarations
   m_pDevice->CreateVertexDeclaration(declVertex,   &m_pDeclVertex);
   m_pDevice->CreateVertexDeclaration(declPVertex,  &m_pDeclPVertex);
   m_pDevice->CreateVertexDeclaration(declLVertex,  &m_pDeclLVertex);
   m_pDevice->CreateVertexDeclaration(declCVertex,  &m_pDeclCVertex);
   m_pDevice->CreateVertexDeclaration(decl3TVertex, &m_pDecl3TVertex);
   m_pDevice->CreateVertexDeclaration(declTVertex,  &m_pDeclTVertex);
   m_pDevice->SetFVF(NULL);

   m_bCanDoShaders  = true;
   Log("use of shaders activated (VS 1.1, PS 1.1)");
   } // PrepareShaderStuff
/*----------------------------------------------------------------*/


/**
 * Switch use of shaders on and off if shaders are possible.
 */
void ZFXD3D::UseShaders(bool b) { 
   if (!m_bCanDoShaders) return;

   if (m_bUseShaders == b) return;

   // clear all vertex caches
   m_pVertexMan->ForcedFlushAll();
   m_pVertexMan->InvalidateStates();

   m_bUseShaders = b;

   // deactivate use of shaders
   if (!m_bUseShaders) {
      m_pDevice->SetVertexShader(NULL);
      m_pDevice->SetPixelShader(NULL);
      m_pDevice->SetVertexDeclaration(NULL);
      }
   else {
      m_pDevice->SetFVF(0);
      }
   } // UseShaders
/*----------------------------------------------------------------*/


/**
 * Activate a prior created vertex shader for the render device.
 */
HRESULT ZFXD3D::ActivateVShader(UINT nID, ZFXVERTEXID VertexID) {
   if (!m_bCanDoShaders) return ZFX_NOSHADERSUPPORT;
   if (nID >= m_nNumVShaders) return ZFX_INVALIDID;

   // clear all vertex caches
   m_pVertexMan->ForcedFlushAll();
//   m_pVertexMan->InvalidateStates();

   // get size and format of vertex
   switch (VertexID) {
      case VID_PS: { 
         if (FAILED(m_pDevice->SetVertexDeclaration(m_pDeclPVertex)))
            return ZFX_FAIL;
         } break;
      case VID_UU: { 
         if (FAILED(m_pDevice->SetVertexDeclaration(m_pDeclVertex)))
            return ZFX_FAIL;
         } break;
      case VID_UL: {
         if (FAILED(m_pDevice->SetVertexDeclaration(m_pDeclLVertex)))
            return ZFX_FAIL;
         } break;
      case VID_CA: {
         if (FAILED(m_pDevice->SetVertexDeclaration(m_pDeclCVertex)))
            return ZFX_FAIL;
         } break;
      case VID_3T: {
         if (FAILED(m_pDevice->SetVertexDeclaration(m_pDecl3TVertex)))
            return ZFX_FAIL;
         } break;
      case VID_TV: {
         if (FAILED(m_pDevice->SetVertexDeclaration(m_pDeclTVertex)))
            return ZFX_FAIL;
         } break;
      default: return ZFX_INVALIDID;
      } // switch

   if (FAILED(m_pDevice->SetVertexShader(m_pVShader[nID])))
      return ZFX_FAIL;

   m_nActiveVShader = nID;
   m_bUseShaders = true;
   return ZFX_OK;
   } // ActivateVShader
/*----------------------------------------------------------------*/


/**
 * Activate a prior created pixel shader for the render device.
 */
HRESULT ZFXD3D::ActivatePShader(UINT nID) {
   if (!m_bCanDoShaders) return ZFX_NOSHADERSUPPORT;
   if (nID >= m_nNumPShaders) return ZFX_INVALIDID;

   // clear out buffers prior to state changes
   m_pVertexMan->ForcedFlushAll();
//   m_pVertexMan->InvalidateStates();

   if (FAILED(m_pDevice->SetPixelShader(m_pPShader[nID])))
      return ZFX_FAIL;

   m_nActivePShader = nID;
   m_bUseShaders = true;
   return ZFX_OK;
   } // ActivatePShader
/*----------------------------------------------------------------*/


/**
 * Sets constant values to the shaders, select which shader type
 * (pixel or vertex) and which type of data is the input. vertex
 * shader registers from 0-20 are reserved and create an error.
 */
HRESULT ZFXD3D::SetShaderConstant(ZFXSHADERTYPE sht, 
                                  ZFXDATATYPE dat,
                                  UINT nReg, UINT nNum, 
                                  const void *pData) {
   if (!m_bCanDoShaders) return ZFX_NOSHADERSUPPORT;

   if (sht == SHT_VERTEX) {
      if (nReg < 20) return ZFX_INVALIDPARAM;

      switch (dat) {
         case DAT_BOOL:
            m_pDevice->SetVertexShaderConstantB(
                       nReg, (BOOL*)pData, nNum);
            break;
         case DAT_INT:
            m_pDevice->SetVertexShaderConstantI(
                       nReg, (int*)pData, nNum);
            break;
         case DAT_FLOAT:
            m_pDevice->SetVertexShaderConstantF(
                       nReg, (float*)pData, nNum);
            break;
         default: return ZFX_FAIL;
         } // switch
      }
   else {
      switch (dat) {
         case DAT_BOOL:
            m_pDevice->SetPixelShaderConstantB(
                       nReg, (BOOL*)pData, nNum);
            break;
         case DAT_INT:
            m_pDevice->SetPixelShaderConstantI(
                       nReg, (int*)pData, nNum);
            break;
         case DAT_FLOAT:
            m_pDevice->SetPixelShaderConstantF(
                       nReg, (float*)pData, nNum);
            break;
         default: return ZFX_FAIL;
         } // switch
      }
   return ZFX_OK;
   } // SetShaderConstant
/*----------------------------------------------------------------*/


/**
 * Create a vertex shader object for the direct3d device.
 * -> IN: void* - whether file name or shader data stream
 *        bool  - is loading from file needed
 *        bool  - is the shader already compiled
 *        UINT* - id to activate the shader later on
 */
HRESULT ZFXD3D::CreateVShader(const void *pData, UINT nSize, 
                              bool bLoadFromFile,
                              bool bIsCompiled, UINT *pID) {
   LPD3DXBUFFER  pCode=NULL;
   LPD3DXBUFFER  pDebug=NULL;
   HRESULT       hrC=ZFX_OK, hrA=ZFX_OK;
   DWORD        *pVS=NULL;
   HANDLE        hFile, hMap;

   // do we have space for one more?
   if (m_nNumVShaders >= (MAX_SHADER-1)) {
      Log("error: max number of shaders reached in ZFXD3D::CreateVShader");
      return ZFX_OUTOFMEMORY;
      }

   // (1): ALREADY ASSEMBLED
   if (bIsCompiled) {

      // already compiled (from file)
      if (bLoadFromFile) {
         hFile = CreateFile((LPCTSTR)pData, GENERIC_READ,
                            0, 0, OPEN_EXISTING, 
                            FILE_ATTRIBUTE_NORMAL, 0);
         if (hFile == INVALID_HANDLE_VALUE) {
            Log("error: CreateFile() in ZFXD3D::CreateVShader failed");
            return ZFX_FILENOTFOUND;
            }

         hMap = CreateFileMapping(hFile,0,PAGE_READONLY, 0,0,0);
         pVS = (DWORD*)MapViewOfFile(hMap,FILE_MAP_READ,0,0,0);
         }

      // already compiled (from datapointer)
      else { pVS = (DWORD*)pData; }
      } // if

   // (2): STILL NEEDS ASSEMBLING
   else {
      // not yet compiled (from file)
      if (bLoadFromFile) {
         hrA = D3DXAssembleShaderFromFile((char*)pData, 
                                          NULL, NULL, 0,
                                          &pCode, &pDebug);
         }
      // not yet compiled (from datapointer)
      else {
         hrA = D3DXAssembleShader((char*)pData, 
                                  nSize-1,
                                  NULL, NULL, 0,
                                  &pCode, &pDebug);
         }

      // track errors if any
      if (SUCCEEDED(hrA)) {
         pVS = (DWORD*)pCode->GetBufferPointer();
         }
      else {
         Log("error: Assemble[Vertex]Shader[FromFile]() failed");
         if (hrA==D3DERR_INVALIDCALL)
            Log("INVALID_CALL");
         else if (hrA==D3DXERR_INVALIDDATA)
            Log("D3DXERR_INVALIDDATA");
         else if (hrA==E_OUTOFMEMORY)
            Log("E_OUTOFMEMORY");
         if (pDebug->GetBufferPointer())
            Log("Shader debugger says: %s", (char*)pDebug->GetBufferPointer());
         else 
            Log("no debug infos stored");

         return ZFX_FAIL;
         }
      } // else

   // create the shader object
   if (FAILED(hrC=m_pDevice->CreateVertexShader(pVS,
                             &m_pVShader[m_nNumVShaders]))) {
      Log("error: CreateVertexShader() failed");
      if (hrC==D3DERR_INVALIDCALL)
         Log("INVALID_CALL");
      else if (hrC==D3DERR_OUTOFVIDEOMEMORY)
         Log("D3DERR_OUTOFVIDEOMEMORY");
      else if (hrC==E_OUTOFMEMORY)
         Log("E_OUTOFMEMORY");

      return ZFX_FAIL;
      }

   // store index to this shader
   if (pID) (*pID) = m_nNumVShaders;

   // free resources
   if (bIsCompiled && bLoadFromFile) {
      UnmapViewOfFile(pVS);
      CloseHandle(hMap);
      CloseHandle(hFile);
      }

   m_nNumVShaders++;
   return ZFX_OK;
   } // CreateVShader
/*----------------------------------------------------------------*/


/**
 * Create a vertex shader object for the direct3d device.
 * -> IN: void* - whether file name or shader data stream
 *        bool  - is loading from file needed
 *        bool  - is the shader already compiled
 *        UINT* - id to activate the shader later on
 */
HRESULT ZFXD3D::CreatePShader(const void *pData, UINT nSize, 
                              bool bLoadFromFile,
                              bool bIsCompiled, UINT *pID) {
   LPD3DXBUFFER  pCode=NULL;
   LPD3DXBUFFER  pDebug=NULL;
   HRESULT       hrC=ZFX_OK, hrA=ZFX_OK;
   DWORD        *pPS=NULL;
   HANDLE        hFile, hMap;

   // do we have space for one more?
   if (m_nNumPShaders >= (MAX_SHADER-1))
      return ZFX_OUTOFMEMORY;

   // (1): ALREADY ASSEMBLED
   if (bIsCompiled) {

      // already compiled (from file)
      if (bLoadFromFile) {
         hFile = CreateFile((LPCTSTR)pData, GENERIC_READ,
                            0, 0, OPEN_EXISTING, 
                            FILE_ATTRIBUTE_NORMAL, 0);
         if (hFile == INVALID_HANDLE_VALUE) {
            Log("error: CreateFile() in ZFXD3D::CreatePShader failed");
            return ZFX_FILENOTFOUND;
            }

         hMap = CreateFileMapping(hFile,0,PAGE_READONLY, 0,0,0);
         pPS = (DWORD*)MapViewOfFile(hMap,FILE_MAP_READ,0,0,0);
         }

      // already compiled (from datapointer)
      else { pPS = (DWORD*)pData; }
      } // if

   // (2): STILL NEEDS ASSEMBLING
   else {
      // not yet compiled (from file)
      if (bLoadFromFile) {
         hrA = D3DXAssembleShaderFromFile((char*)pData, 
                                          NULL, NULL, 0,
                                          &pCode, &pDebug);
         }
      // not yet compiled (from datapointer)
      else {
         hrA = D3DXAssembleShader((char*)pData, 
                                  nSize-1,
                                  NULL, NULL, 0,
                                  &pCode, &pDebug);
         }

      // track errors if any
      if (SUCCEEDED(hrA)) {
         pPS = (DWORD*)pCode->GetBufferPointer();
         }
      else {
         Log("error: Assemble[Pixel]Shader[FromFile]() failed");
         if (hrA==D3DERR_INVALIDCALL)
            Log("INVALID_CALL");
         else if (hrA==D3DXERR_INVALIDDATA)
            Log("D3DXERR_INVALIDDATA");
         else if (hrA==E_OUTOFMEMORY)
            Log("E_OUTOFMEMORY");
         if (pDebug->GetBufferPointer())
            Log("Shader debugger says: %s", (char*)pDebug->GetBufferPointer());
         else 
            Log("no debug infos stored");

         return ZFX_FAIL;
         }
      } // else

   // create the shader object
   if (FAILED(hrC=m_pDevice->CreatePixelShader(pPS,
                             &m_pPShader[m_nNumPShaders]))) {
      Log("error: CreatePixelShader() failed");
      if (hrC==D3DERR_INVALIDCALL)
         Log("INVALID_CALL");
      else if (hrC==D3DERR_OUTOFVIDEOMEMORY)
         Log("D3DERR_OUTOFVIDEOMEMORY");
      else if (hrC==E_OUTOFMEMORY)
         Log("E_OUTOFMEMORY");

      return ZFX_FAIL;
      }

   // store index to this shader
   if (pID) (*pID) = m_nNumPShaders;

   // free resources
   if (bIsCompiled && bLoadFromFile) {
      UnmapViewOfFile(pPS);
      CloseHandle(hMap);
      CloseHandle(hFile);
      }

   m_nNumPShaders++;
   return ZFX_OK;
   } // CreatePShader
/*----------------------------------------------------------------*/


// enable / disable use of color buffer
void ZFXD3D::UseColorBuffer(bool b) {
   // clear out buffers prior to state changes
   m_pVertexMan->ForcedFlushAll();
   m_pVertexMan->InvalidateStates();

   m_bColorBuffer = b;

   if (!b) {
      m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      m_pDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ZERO);
      m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
      }
   else {
      m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
      m_pDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ONE);
      m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
      }
   } // UseColorBuffer
/*----------------------------------------------------------------*/


// SET VARIOUS RENDERSTATES AND STUFF LIKE THAT
void ZFXD3D::SetBackfaceCulling(ZFXRENDERSTATE rs) {
   // clear out buffers prior to state changes
   m_pVertexMan->ForcedFlushAll();

   if (rs == RS_CULL_CW) 
      m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

   else if (rs == RS_CULL_CCW) 
      m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

   else if (rs == RS_CULL_NONE) 
      m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
   } // SetBackfaceCulling
/*----------------------------------------------------------------*/

void ZFXD3D::SetStencilBufferMode(ZFXRENDERSTATE rs, DWORD dw) {
   // clear out buffers prior to state changes
   m_pVertexMan->ForcedFlushAll();

   switch (rs) {
      // switch on and off
      case RS_STENCIL_DISABLE:
         m_pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
         break;
      case RS_STENCIL_ENABLE:
         m_pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);
         break;
      case RS_DEPTHBIAS:
         m_pDevice->SetRenderState(D3DRS_DEPTHBIAS, dw);

      // function modes and values
      case RS_STENCIL_FUNC_ALWAYS:
         m_pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
         break;
      case RS_STENCIL_FUNC_LESSEQUAL:
         m_pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL);
         break;
      case RS_STENCIL_MASK:
         m_pDevice->SetRenderState(D3DRS_STENCILMASK, dw);
         break;
      case RS_STENCIL_WRITEMASK:
         m_pDevice->SetRenderState(D3DRS_STENCILWRITEMASK, dw);
         break;
      case RS_STENCIL_REF:
         m_pDevice->SetRenderState(D3DRS_STENCILREF, dw);
         break;

      // stencil test fails modes
      case RS_STENCIL_FAIL_DECR:
         m_pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_DECR);
         break;
      case RS_STENCIL_FAIL_INCR:
         m_pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_INCR);
         break;
      case RS_STENCIL_FAIL_KEEP:
         m_pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
         break;

      // stencil test passes but z test fails modes
      case RS_STENCIL_ZFAIL_DECR:
         m_pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_DECR);
         break;
      case RS_STENCIL_ZFAIL_INCR:
         m_pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_INCR);
         break;
      case RS_STENCIL_ZFAIL_KEEP:
         m_pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
         break;

      // stencil test passes modes
      case RS_STENCIL_PASS_DECR:
         m_pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_DECR);
         break;
      case RS_STENCIL_PASS_INCR:
         m_pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);
         break;
      case RS_STENCIL_PASS_KEEP:
         m_pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
         break;
      } // switch
   } // SetStencilBufferMode
/*----------------------------------------------------------------*/


void ZFXD3D::UseStencilShadowSettings(bool b) {
   ZFXMatrix matProj;

   m_pVertexMan->InvalidateStates();

   if (b) {
      m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE,  FALSE);
      m_pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);
      m_pDevice->SetRenderState(D3DRS_SHADEMODE,     D3DSHADE_FLAT);
      m_pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
      m_pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
      m_pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
      m_pDevice->SetRenderState(D3DRS_STENCILREF,       0x1);
      m_pDevice->SetRenderState(D3DRS_STENCILMASK,      0xffffffff);
      m_pDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
      m_pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_INCR);
      UseColorBuffer(false);
      }
   else {
      m_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
      m_pDevice->SetRenderState(D3DRS_CULLMODE,  D3DCULL_CCW);
      m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE,     TRUE);
      m_pDevice->SetRenderState(D3DRS_STENCILENABLE,    FALSE);
      m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
      UseColorBuffer(true);
      SetMode(m_Mode, m_nStage);
      }
   } // UseStencilShadowSettings
/*----------------------------------------------------------------*/


void ZFXD3D::SetDepthBufferMode(ZFXRENDERSTATE rs) {
   // clear out buffers prior to state changes
   m_pVertexMan->ForcedFlushAll();

   if (rs == RS_DEPTH_READWRITE) {
      m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
      m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
      }

   else if (rs == RS_DEPTH_READONLY) {
      m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
      m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
      }

   else if (rs == RS_DEPTH_NONE){
      m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
      m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
      }
   } // SetDepthBufferMode
/*----------------------------------------------------------------*/

inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

void ZFXD3D::SetShadeMode(ZFXRENDERSTATE smd, float f, const ZFXCOLOR *pClr) {

   // copy color if it was given
   if ( pClr  && !m_pSkinMan->ColorEqual( pClr, &m_clrWire) ) { 
      m_pVertexMan->ForcedFlushAll();
      m_pVertexMan->InvalidateStates();
      memcpy(&m_clrWire, pClr, sizeof(ZFXCOLOR));
      }
   
   // no change in current mode
   if (smd == m_ShadeMode) {
      // but maybe change in size
      if (smd==RS_SHADE_POINTS) {
         m_pVertexMan->ForcedFlushAll();
         m_pDevice->SetRenderState(D3DRS_POINTSIZE, FtoDW(f));
         }
      return;
      }
   else {
      m_pVertexMan->ForcedFlushAll();
      }

   if (smd == RS_SHADE_TRIWIRE) {
      // only triangle wireframe ist set via d3d shade mode
      m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
      m_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
      m_ShadeMode = smd;
      }
   else {
      if (smd != RS_SHADE_SOLID) {
         m_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
         }
      m_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
      m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
      m_ShadeMode = smd;
      }

   if (smd == RS_SHADE_POINTS) {
      if (f > 0.0f) {
         m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
         m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE,  TRUE);
         m_pDevice->SetRenderState(D3DRS_POINTSIZE,     FtoDW( f ));
         m_pDevice->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(0.00f));
         m_pDevice->SetRenderState(D3DRS_POINTSCALE_A,  FtoDW(0.00f));
         m_pDevice->SetRenderState(D3DRS_POINTSCALE_B,  FtoDW(0.00f));
         m_pDevice->SetRenderState(D3DRS_POINTSCALE_C,  FtoDW(1.00f));
         }
      else {
         m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
         m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE,  FALSE);
         }
      }
   else {
      m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
      m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE,  FALSE);
      }

   // update depending states
   m_pVertexMan->InvalidateStates();
   } // SetShadeMode
/*----------------------------------------------------------------*/



HRESULT ZFXD3D::SetTextureStage(UCHAR n, ZFXRENDERSTATE rs) {
   D3DCAPS9 caps;

   m_pDevice->GetDeviceCaps(&caps);
   if (caps.MaxSimultaneousTextures < (n+1)) 
      return ZFX_FAIL;

   switch (rs) {
      case RS_NONE:
         m_TOP[n] = D3DTOP_DISABLE;
         break;
      case RS_TEX_ADDSIGNED:
         m_TOP[n] = D3DTOP_ADDSIGNED;
         break;
      case RS_TEX_MODULATE:
         m_TOP[n] = D3DTOP_MODULATE;
         break;

      default: break;
      } // switch


   return ZFX_OK;
   } // SetTextureStage
/*----------------------------------------------------------------*/


HRESULT ZFXD3D::SetLight(const ZFXLIGHT *pLight, UCHAR nStage) {
   D3DLIGHT9 d3dLight;

   // switch it off
   if (!pLight) {
      m_pDevice->LightEnable(nStage, FALSE);
      return ZFX_OK;
      }

   memset(&d3dLight, 0, sizeof(D3DLIGHT9));

   switch (pLight->Type) {
      case LGT_DIRECTIONAL:
         // set light values
         d3dLight.Type = D3DLIGHT_DIRECTIONAL;

         // copy light direction
         memcpy(&d3dLight.Direction, 
                &pLight->vcDirection,
                sizeof(float)*3);
         break;

      case LGT_POINT:
         // set light values
         d3dLight.Type  = D3DLIGHT_POINT;
         d3dLight.Range = pLight->fRange;

         d3dLight.Attenuation1 = 1.0f;

         // copy light position
         memcpy(&d3dLight.Position, 
                &pLight->vcPosition,
                sizeof(float)*3);
         break;

      case LGT_SPOT:
         // set light values
         d3dLight.Type     = D3DLIGHT_SPOT;
         d3dLight.Range    = pLight->fRange;
         d3dLight.Falloff  = 1.0f;
         d3dLight.Theta    = pLight->fTheta;
         d3dLight.Phi      = pLight->fPhi;

         d3dLight.Attenuation0 = pLight->fAttenuation0;
         d3dLight.Attenuation1 = pLight->fAttenuation1;
         d3dLight.Attenuation2 = 1.0f;

         // copy light position
         memcpy(&d3dLight.Position, 
                &pLight->vcPosition,
                sizeof(float)*3);

         // copy light direction
         memcpy(&d3dLight.Direction, 
                &pLight->vcDirection,
                sizeof(float)*3);
         break;
      } 

   memcpy(&d3dLight.Ambient, &pLight->cAmbient,
          sizeof(float)*4);

   memcpy(&d3dLight.Diffuse, &pLight->cDiffuse,
          sizeof(float)*4);

   memcpy(&d3dLight.Specular, &pLight->cSpecular,
          sizeof(float)*4);


   m_pDevice->SetLight(nStage, &d3dLight);
   m_pDevice->LightEnable(nStage, TRUE);

   return ZFX_OK;
   } // SetLight
/*----------------------------------------------------------------*/


// the user wants to render additive
void ZFXD3D::UseAdditiveBlending(bool b) {
   if (m_bAdditive == b) return;

   // clear all vertex caches
   m_pVertexMan->ForcedFlushAll();
   m_pVertexMan->InvalidateStates();

   m_bAdditive = b;

   if (!m_bAdditive) {
      m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
      m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
      m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
      }
   } // UseAdditiveBlending
/*----------------------------------------------------------------*/


// the user wants to render without textures
void ZFXD3D::UseTextures(bool b) {
   if (m_bTextures == b) return;

   // clear all vertex caches
   m_pVertexMan->ForcedFlushAll();
   m_pVertexMan->InvalidateStates();

   m_bTextures = b;
   } // UseAdditiveBlending
/*----------------------------------------------------------------*/