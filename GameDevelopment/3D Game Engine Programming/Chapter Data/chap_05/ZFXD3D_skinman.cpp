/*******************************************************************
 * ZFXEngine!                                                      *
 * (c)2003 by Stefan Zerbst | www.zfx.info                         *
 *-----------------------------------------------------------------*
 * File: ZFXD3D_skinman.cpp                                        *
 * part of render dll implementing direct3d rendering              *
 *******************************************************************/


// I N C L U D E S /////////////////////////////////////////////////

#include "ZFXD3D_skinman.h"   // class definition
#include "ZFX.h"              // return values and stuff

extern bool g_bLF;


// F U N C T I O N S ///////////////////////////////////////////////

/**
 * Constructor: Initializes vertex cache arrays
 */
ZFXD3DSkinManager::ZFXD3DSkinManager(LPDIRECT3DDEVICE9 pDevice,
                                     FILE *pLog) {
   m_nNumMaterials = 0;
   m_nNumTextures  = 0;
   m_nNumSkins     = 0;
   m_pMaterials    = NULL;
   m_pTextures     = NULL;
   m_pSkins        = NULL;
   m_pLog          = pLog;
   m_pDevice       = pDevice;
   Log("online");
   } // constructor
/*----------------------------------------------------------------*/


/**
 * Destructor: Release vertex cache arrays
 */
ZFXD3DSkinManager::~ZFXD3DSkinManager(void) {

   // release direct3d texture objects
   if (m_pTextures) {
      for (UINT i=0; i<m_nNumTextures; i++) {
         if (m_pTextures[i].pData) {
            ((LPDIRECT3DTEXTURE9)(m_pTextures[i].pData))->Release();
            m_pTextures[i].pData = NULL;
            }
         if (m_pTextures[i].chName) {
            delete m_pTextures[i].chName;
            m_pTextures[i].chName = NULL;
            }
         }
      free(m_pTextures);
      m_pTextures = NULL;
      }
   
   // release allocated memory
   if (m_pMaterials) {
      free(m_pMaterials);
      m_pMaterials = NULL;
      }

   if (m_pSkins) {
      free(m_pSkins);
      m_pSkins = NULL;
      }
   Log("offline (ok)");
   } // destructor
/*----------------------------------------------------------------*/


/**
 * Compares if two color values are equal.
 */
inline bool ZFXD3DSkinManager::ColorEqual(const ZFXCOLOR *pCol0, 
                                          const ZFXCOLOR *pCol1) {
   if ( (pCol0->fA != pCol1->fA) ||
        (pCol0->fR != pCol1->fR) ||     
        (pCol0->fG != pCol1->fG) ||
        (pCol0->fB != pCol1->fB) )
      return false;
   return true;
   } // ColorEqual
/*----------------------------------------------------------------*/


/**
 * Compares if two materials are identical.
 */
bool ZFXD3DSkinManager::MaterialEqual(const ZFXMATERIAL *pMat0, 
                                      const ZFXMATERIAL *pMat1) {
   if ( !ColorEqual(&pMat0->cAmbient,  &pMat1->cAmbient)  || 
        !ColorEqual(&pMat0->cDiffuse,  &pMat1->cDiffuse)  || 
        !ColorEqual(&pMat0->cEmissive, &pMat1->cEmissive) ||
        !ColorEqual(&pMat0->cSpecular, &pMat1->cSpecular) || 
        (pMat0->fPower != pMat1->fPower) )
      return false;
   return true;
   } // MaterialEqual
/*----------------------------------------------------------------*/


/**
 * Creates a new skin object using the given material. texture is
 * set to NULL till a texture is added to that skin.
 * -> IN:  ZFXCOLOR - ambient material  color 
 *         ZFXCOLOR - diffuse material color 
 *         ZFXCOLOR - emissive material color 
 *         ZFXCOLOR - specular material color 
 *         float    - power for specular reflection
 * -> OUT: UINT     - ID to reference new skin
 */
HRESULT ZFXD3DSkinManager::AddSkin(const ZFXCOLOR *pcAmbient,
                                   const ZFXCOLOR *pcDiffuse,
                                   const ZFXCOLOR *pcEmissive,
                                   const ZFXCOLOR *pcSpecular,
                                   float fSpecPower,
                                   UINT  *nSkinID) {
   UINT    nMat, n;
   bool    bMat=false;

   // allocate 50 new memory slots for skins if necessary
   if ( (m_nNumSkins%50) == 0 ) {
      n = (m_nNumSkins+50)*sizeof(ZFXSKIN);
      m_pSkins = (ZFXSKIN*)realloc(m_pSkins, n);
      if (!m_pSkins) return ZFX_OUTOFMEMORY;
      }

   ZFXMATERIAL mat;
   mat.cAmbient  = *pcAmbient;
   mat.cDiffuse  = *pcDiffuse;
   mat.cEmissive = *pcEmissive;
   mat.cSpecular = *pcSpecular;
   mat.fPower    = fSpecPower;

   // do we already have an equal material
   for (nMat=0; nMat<m_nNumMaterials; nMat++) {
      if ( MaterialEqual(&mat, &m_pMaterials[nMat]) ) {
         bMat = true;
         break;
         }
      } // for [MATERIALS]

   // if exists store its ID otherwise create it new
   if (bMat) m_pSkins[m_nNumSkins].nMaterial = nMat;
   else {
      m_pSkins[m_nNumSkins].nMaterial = m_nNumMaterials;

      // allocate 50 new memory slots for materials if necessary
      if ( (m_nNumMaterials%50) == 0 ) {
         n = (m_nNumMaterials+50)*sizeof(ZFXMATERIAL);
         m_pMaterials = (ZFXMATERIAL*)realloc(m_pMaterials, n);
         if (!m_pMaterials) return ZFX_OUTOFMEMORY;
         }
      memcpy(&m_pMaterials[m_nNumMaterials], &mat, sizeof(ZFXMATERIAL));
      m_nNumMaterials++;
      }

   m_pSkins[m_nNumSkins].bAlpha = false;
   for (int i=0; i<8; i++) m_pSkins[m_nNumSkins].nTexture[i] = MAX_ID;

   // save ID and add to count
   (*nSkinID) = m_nNumSkins;
   m_nNumSkins++;

   return ZFX_OK;
   } // AddSkin
/*----------------------------------------------------------------*/


/**
 * Add a texture to a given skin. BMP is only loaded if not used yet.
 * Can also be used to set alpha channels on textures for alpha keys
 * and/or overall transparency. Set bool to true in both cases.
 * -> IN: UINT      - ID of skin to receive texture
 *        char*     - name of 24 bit BMP file used as texture
 *        bool      - use alphablending?
 *        float     - value for overall transparency
 *        ZFXCOLOR* - array of RGB values to receive their A value
 *        DWORD     - number of colors in array
 */
HRESULT ZFXD3DSkinManager::AddTexture(UINT      nSkinID, 
                                      const     char *chName,
                                      bool      bAlpha, 
                                      float     fAlpha,
                                      ZFXCOLOR *cColorKeys,
                                      DWORD     dwNumColorKeys) {
   HRESULT hr;
   UINT    nTex, n;
   bool    bTex=false;

   // is skin ID valid at all
   if (nSkinID >= m_nNumSkins) return ZFX_INVALIDID;

   // all 8 stages for this skin already set?
   if (m_pSkins[nSkinID].nTexture[7] != MAX_ID)
      return ZFX_BUFFERSIZE;

   // we use alphablending at least from now on
   if (bAlpha) m_pSkins[nSkinID].bAlpha = true;

   // do we already have this texture
   for (nTex=0; nTex<m_nNumTextures; nTex++) {
      if ( strcmp(chName, m_pTextures[nTex].chName) == 0 ) {
         bTex = true;
         break;
         }
      } // for [TEXTURES]

   // load new texture if not yet done
   if (!bTex) {
      // allocate 50 new memory slots for textures if necessary
      if ( (m_nNumTextures%50) == 0 ) {
         n = (m_nNumTextures+50)*sizeof(ZFXTEXTURE);
         m_pTextures = (ZFXTEXTURE*)realloc(m_pTextures, n);
         if (!m_pTextures) return ZFX_OUTOFMEMORY;
         }

      // save texture name
      m_pTextures[m_nNumTextures].chName = new char[strlen(chName)+1];
      memcpy(m_pTextures[m_nNumTextures].chName, chName, strlen(chName)+1);

      // create d3d texture from that pointer
      hr = CreateTexture(&m_pTextures[m_nNumTextures], bAlpha);
      if (FAILED(hr)) return hr;

      // add alpha values if needed
      if (bAlpha) {

         LPDIRECT3DTEXTURE9 pTex = (LPDIRECT3DTEXTURE9)m_pTextures[m_nNumTextures].pData;

         // set alpha keys first
         for (DWORD dw=0; dw<dwNumColorKeys; dw++) {
            hr = SetAlphaKey(&pTex,
                             UCHAR(cColorKeys[dw].fR*255),
                             UCHAR(cColorKeys[dw].fG*255),
                             UCHAR(cColorKeys[dw].fB*255),
                             UCHAR(cColorKeys[dw].fA*255));
            if (FAILED(hr)) return hr;
            }

         // now generell transparency
         hr = SetTransparency(&pTex, UCHAR(fAlpha*255));
         if (FAILED(hr)) return hr;
         }


      // save ID and add to count
      nTex = m_nNumTextures;
      m_nNumTextures++;
      }

   // put texture ID to skin ID
   for (int i=0; i<8; i++) {
      if (m_pSkins[nSkinID].nTexture[i] == MAX_ID) {
         m_pSkins[nSkinID].nTexture[i] = nTex;
         break;
         }
      }
   return ZFX_OK;
   } // AddTexture
/*----------------------------------------------------------------*/

#define RGB16BIT(r,g,b) ((b%32) + ((g%64) << 6) + ((r%32) << 11))

/**
 * Creates a d3dtexture object and loads the image data from disc.
 * -> IN: ZFXTEXTURE - reference to texture object to store data
 */
HRESULT ZFXD3DSkinManager::CreateTexture(ZFXTEXTURE *pTexture, bool bAlpha) {
   D3DLOCKED_RECT     d3dRect;
   D3DFORMAT          fmt;
   DIBSECTION         dibS;
   HRESULT            hr;
   int                LineWidth;
   void              *pMemory=NULL;

   HBITMAP hBMP = (HBITMAP)LoadImage(NULL, pTexture->chName,
                                     IMAGE_BITMAP,0,0,
                                     LR_LOADFROMFILE |
                                     LR_CREATEDIBSECTION);
   if (!hBMP) return ZFX_FILENOTFOUND;

   GetObject(hBMP, sizeof(DIBSECTION), &dibS);

   // we support only 24 bit bitmaps
   if (dibS.dsBmih.biBitCount != 24) {
      DeleteObject(hBMP);
      return ZFX_INVALIDFILE;
      }

   if (bAlpha) fmt = D3DFMT_A8R8G8B8;
   else fmt = D3DFMT_R5G6B5;

   long  lWidth   = dibS.dsBmih.biWidth;
   long  lHeight  = dibS.dsBmih.biHeight;
   BYTE *pBMPBits = (BYTE*)dibS.dsBm.bmBits;

   // build d3d texture object
   hr = m_pDevice->CreateTexture(lWidth, lHeight, 1, 0,
                                 fmt, D3DPOOL_MANAGED,
                                 (LPDIRECT3DTEXTURE9*)(&(pTexture->pData)),
                                 NULL);
   if (FAILED(hr)) return ZFX_FAIL;

   // get a dummy pointer
   LPDIRECT3DTEXTURE9 pTex = ((LPDIRECT3DTEXTURE9)pTexture->pData);

   if (FAILED(pTex->LockRect(0, &d3dRect, NULL, 0)))
      return ZFX_BUFFERLOCK;

   if (bAlpha) {
      LineWidth = d3dRect.Pitch >> 2; // 32 bit = 4 byte
      pMemory = (DWORD*)d3dRect.pBits;
      }
   else {
      LineWidth = d3dRect.Pitch >> 1; // 16 bit = 2 Byte
      pMemory = (USHORT*)d3dRect.pBits;
      }

   // copy each pixel: the magic 3 in the formulars is given by the fact
   //                  that each pixel in the BMP is 3 byte (24 bit)
   for (int cy = 0; cy < lHeight; cy++) {
      for (int cx = 0; cx < lWidth; cx++) {

         if (bAlpha) {
            DWORD Color = 0xff000000;
            int   i = (cy*lWidth + cx)*3;
            memcpy(&Color, &pBMPBits[i], sizeof(BYTE)*3);
            ((DWORD*)pMemory)[cx + (cy*LineWidth)] = Color;
            } // 32 Bit
         else {
            // convert each 24 bit pixel value to 16 bit
            UCHAR B = (pBMPBits[(cy*lWidth + cx)*3 + 0]) >> 3,
                  G = (pBMPBits[(cy*lWidth + cx)*3 + 1]) >> 3,
                  R = (pBMPBits[(cy*lWidth + cx)*3 + 2]) >> 3;
            
            // map values to 5, 6, and 5 bits respectively and call macro
            USHORT Color = RGB16BIT( (int)(((float) R / 255.0f) * 32.0f),
                              (int)(((float) G / 255.0f) * 64.0f),
                              (int)(((float) B / 255.0f) * 32.0f) );

            
            // Pixel in 16 Bit Farbe schreiben
            ((USHORT*)pMemory)[cx + (cy*LineWidth)] = Color;
            } // 16 Bit

         } // for
      } // for

   pTex->UnlockRect(0);
   DeleteObject(hBMP);
   return ZFX_OK;
   } // CreateTexture
/*----------------------------------------------------------------*/


/**
 * Calculates a 32 bit ARGB value for use with d3d from given RGBA
 * color values ranging from 0-255.
 * -> IN: UCHAR - red value
 *        UCHAR - green value
 *        UCHAR - blue value
 *        UCHAR - alpha value
 */
DWORD ZFXD3DSkinManager::MakeD3DColor(UCHAR R, UCHAR G, UCHAR B, UCHAR A) {
   return (A << 24) | (R << 16) | (G << 8) | B;
   } // MakeColor
/*----------------------------------------------------------------*/


/**
 * Sets the aplha channel of all pixels with given RGB value to the
 * amount of transparency specified. Make sure the corresponding
 * RGB values in the texture have alpha value set to 1.0f. So set
 * all alpha key prior to calling SetTransparency().
 * -> IN:  UCHAR              - red (0-255)
 *         UCHAR              - green (0-255)
 *         UCHAR              - blue (0-255)
 *         UCHAR              - alpha (0-255)
 * <- OUT: LPDIRECT3DTEXTURE9 - changed texture
 */
HRESULT ZFXD3DSkinManager::SetAlphaKey(LPDIRECT3DTEXTURE9 *ppTexture,
                                       UCHAR R, UCHAR G, 
                                       UCHAR B, UCHAR A) {
   D3DSURFACE_DESC  d3dDesc;
   D3DLOCKED_RECT   d3dRect;
   DWORD            dwKey, Color;

   // security check: must be ARGB format
   (*ppTexture)->GetLevelDesc(0, &d3dDesc);
   if (d3dDesc.Format != D3DFMT_A8R8G8B8)
      return ZFX_INVALIDPARAM;

   // calculate the key value
   dwKey = MakeD3DColor(R, G, B, 255);

   // calculate new color to set for key pixels
   if (A > 0) Color = MakeD3DColor(R, G, B, A);
   else Color = MakeD3DColor(0, 0, 0, A);

   if (FAILED((*ppTexture)->LockRect(0, &d3dRect, NULL, 0)))
      return ZFX_BUFFERLOCK;

   // overwrite all key pixels with new color value
   for (DWORD y=0; y<d3dDesc.Height; y++) {
      for (DWORD x=0; x<d3dDesc.Width; x++) {
         if ( ((DWORD*)d3dRect.pBits)[d3dDesc.Width*y+x] == dwKey )
            ((DWORD*)d3dRect.pBits)[d3dDesc.Width*y+x] = Color;
         }
      }
   (*ppTexture)->UnlockRect(0);

   return ZFX_OK;
   } // SetAlphaKey
/*----------------------------------------------------------------*/


/**
 * Sets all pixels in the texture to the amount of transparency.
 * <- OUT: LPDIRECT3DTEXTURE9 - changed texture
 * -> IN:  UCHAR              - alpha value (0-255)
 */
HRESULT ZFXD3DSkinManager::SetTransparency(LPDIRECT3DTEXTURE9 *ppTexture,
                                           UCHAR Alpha) {
   D3DSURFACE_DESC  d3dDesc;
   D3DLOCKED_RECT   d3dRect;
   DWORD            Color;
   UCHAR            A, R, G, B;

   // security check: must be ARGB format
   (*ppTexture)->GetLevelDesc(0, &d3dDesc);
   if (d3dDesc.Format != D3DFMT_A8R8G8B8)
      return ZFX_INVALIDPARAM;

   if (FAILED((*ppTexture)->LockRect(0, &d3dRect, NULL, 0)))
      return ZFX_BUFFERLOCK;

   // loop through all pixels
   for (DWORD y=0; y<d3dDesc.Height; y++) {
      for (DWORD x=0; x<d3dDesc.Width; x++) {
         // get color value from this pixel
         Color = ((DWORD*)d3dRect.pBits)[d3dDesc.Width*y+x];

         // calculate ARGB values from pixel color
         A = (UCHAR)( (Color & 0xff000000) >> 24);
         R = (UCHAR)( (Color & 0x00ff0000) >> 16);
         G = (UCHAR)( (Color & 0x0000ff00) >> 8);
         B = (UCHAR)( (Color & 0x000000ff) >> 0);
         
         // only set new alpha value if old value is greater
         if (A >= Alpha)
            A = Alpha;

         // put new color value for this pixel
         ((DWORD*)d3dRect.pBits)[d3dDesc.Width*y+x] = 
                        MakeD3DColor(R, G, B, A);
         }
      }
   (*ppTexture)->UnlockRect(0);

   return ZFX_OK;
   } // SetTransparency
/*----------------------------------------------------------------*/


/**
 * write outputstring to attribut outputstream if exists
 * -> IN: bool - flush immediately
 *        char - format string to output
 *        ...  - output values
 */
void ZFXD3DSkinManager::Log(char *chString, ...) {

   char ch[256];
   char *pArgs;

   pArgs = (char*) &chString + sizeof(chString);
   vsprintf(ch, chString, pArgs);
   fprintf(m_pLog, "[ZFXD3DSkinMan]: ");
   fprintf(m_pLog, ch);
   fprintf(m_pLog, "\n");
   
   if (g_bLF)
      fflush(m_pLog);
   } // Log
/*----------------------------------------------------------------*/


/**
 * Used to log values e.g. how many skins, testures, materials loaded.
 */
void ZFXD3DSkinManager::LogCurrentStatus(char *chLog, bool bDetailed) {
   FILE *pLog = fopen(chLog, "a");

   fprintf(pLog, "\nSITREP: skins: %d, mats: %d, texrs: %d \n", 
           m_nNumSkins, m_nNumMaterials, m_nNumTextures);
   
   if (!bDetailed) return;

   fprintf(pLog, "SKINS { \n");
   for (UINT i=0; i<m_nNumSkins; i++) {
      fprintf(pLog, "   ID: %d { matrl: %d, ", i, m_pSkins[i].nMaterial);
      fprintf(pLog, "textr: [%d,%d,%d,%d,%d,%d,%d,%d] }\n", 
              m_pSkins[i].nTexture[0], m_pSkins[i].nTexture[1],
              m_pSkins[i].nTexture[2], m_pSkins[i].nTexture[3],
              m_pSkins[i].nTexture[4], m_pSkins[i].nTexture[5],
              m_pSkins[i].nTexture[6], m_pSkins[i].nTexture[7]);
      }
   fprintf(pLog, "   } \n");

   fprintf(pLog, "TEXTURES { \n");
   for (UINT j=0; j<m_nNumTextures; j++) {
      fprintf(pLog, "   ID: %d {\"%s\"} \n", j, m_pTextures[j].chName);
      }
   fprintf(pLog, "   } \n\n");

   fclose(pLog);
   } // LogCurrentStatus
/*----------------------------------------------------------------*/


