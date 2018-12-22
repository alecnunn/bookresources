// File: CLight.cpp:

#include "include/CLight.h"

CLight::CLight(void) {
   Init();
   } // constructor
/*----------------------------------------------------------------*/

CLight::~CLight(void) {
   Release();
   } // destructor
/*----------------------------------------------------------------*/


// initialize to defaults
void CLight::Init(void) {
   CGeometry geo;

   m_NumVerts    = 0;
   m_NumIndis    = 0;
   m_NumPolys    = 1;
   m_vcPos       = ZFXVector(0,0,0);
   m_bSelected   = false;
   m_bHidden     = false;
   m_bFlicker    = false;
   m_bShadow     = false;
   m_Type        = LOB_LIGHT;
   memset(&m_Aabb, 0, sizeof(ZFXAabb));

   geo.CreateLight(&m_Mesh, NULL, 
                   ZFXVector(0,0,0), 
                   0.2f, 0.2f, 0.2f);
   
   m_Mesh.SetSkinID(1);
   } // Init
/*----------------------------------------------------------------*/


// clean up all memory and stuff
void CLight::Release(void) {
   /* nothing to do */
   } // Release
/*----------------------------------------------------------------*/


// set the position of this light object
void CLight::SetPosition(const ZFXVector &vcPos) {
   // use the translate method here which
   // will maintain aabb, mesh and stuff
   ZFXVector vcT = vcPos - m_vcPos;
   Translate(vcT);
   } // SetPosition
/*----------------------------------------------------------------*/


// translate this light object's world position
void CLight::Translate(const ZFXVector &vcT) {
   m_vcPos += vcT;
   m_Mesh.Translate(vcT);
   m_Aabb = m_Mesh.GetAabb();
   } // SetPosition
/*----------------------------------------------------------------*/


// set this to a copy of another light
void CLight::CopyOf(CLevelObject *pLob) {
   
   if ( !pLob->IsOfType(m_Type) ) return;

   LPLIGHT pLight = (LPLIGHT)pLob;

   Reset();

   m_Aabb     = pLob->GetAabb();
   m_vcPos    = pLight->m_vcPos;
   m_Clr      = pLight->m_Clr;
   m_fR       = pLight->m_fR;
   m_bFlicker = pLight->m_bFlicker;
   m_bShadow  = pLight->m_bShadow;
   m_Mesh.CopyOf( &pLight->m_Mesh );
   } // CopyOf
/*----------------------------------------------------------------*/


// saves this light to a file
void CLight::Save(FILE *pFile) {
   if (!pFile) return;

   fwrite(&m_vcPos,    sizeof(ZFXVector), 1, pFile);
   fwrite(&m_Aabb,     sizeof(ZFXAabb),   1, pFile);
   fwrite(&m_Clr,      sizeof(ZFXCOLOR),  1, pFile);
   fwrite(&m_fR,       sizeof(float),     1, pFile);
   fwrite(&m_bFlicker, sizeof(bool),      1, pFile);
   fwrite(&m_bShadow,  sizeof(bool),      1, pFile);

   m_Mesh.Save(pFile);
   } // Save
/*----------------------------------------------------------------*/


// loads this light from a file
bool CLight::Load(FILE *pFile) {
   if (!pFile) return false;

   fread(&m_vcPos,    sizeof(ZFXVector), 1, pFile);
   fread(&m_Aabb,     sizeof(ZFXAabb),   1, pFile);
   fread(&m_Clr,      sizeof(ZFXCOLOR),  1, pFile);
   fread(&m_fR,       sizeof(float),     1, pFile);
   fread(&m_bFlicker, sizeof(bool),      1, pFile);
   fread(&m_bShadow,  sizeof(bool),      1, pFile);

   return m_Mesh.Load(pFile);
   } // Load
/*----------------------------------------------------------------*/


// renders the light using the vc manager
void CLight::Render(ZFXVertexCacheManager *pVCM, bool bHide) {
   if (m_bHidden && bHide) return;
   m_Mesh.Render(pVCM, bHide);
   } // Render
/*----------------------------------------------------------------*/


// sets the color attributes of the light
void CLight::SetColor(const ZFXCOLOR &clr) {
   memcpy(&m_Clr, &clr, sizeof(ZFXCOLOR));
   } // SetColor
/*----------------------------------------------------------------*/


// sets the color attributes of the light
void CLight::SetColor(float fR, float fG, float fB) {
   m_Clr.fR = fR;
   m_Clr.fB = fB;
   m_Clr.fG = fG;
   m_Clr.fA = 1.0f;
   } // SetColor
/*----------------------------------------------------------------*/


// set if the light should cast shadow or light
void CLight::SetShadowCasting(bool b) {
   m_bShadow = b;

   if (b) m_Mesh.SetSkinID(0);
   else m_Mesh.SetSkinID(1);
   } // SetShadowCaster
/*----------------------------------------------------------------*/

