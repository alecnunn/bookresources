// File: ZFXD3D_enum.cpp

#include "resource.h"      // control id's
#include "ZFX.h"           // return values and stuff
#include "ZFXD3D.h"        // class definition


// some common backbuffer formats
const UINT g_nFormats_B=9;
const D3DFORMAT g_fmtBackBuffer[] = {
           D3DFMT_R8G8B8,   D3DFMT_A8R8G8B8, 
           D3DFMT_X8R8G8B8, D3DFMT_R5G6B5, 
           D3DFMT_A1R5G5B5, D3DFMT_X1R5G5B5,
           D3DFMT_X4R4G4B4, D3DFMT_A4R4G4B4,
           D3DFMT_A2B10G10R10 
           };


// some good multisample modes, list best ones last
const UINT g_nMS = 8;
const D3DMULTISAMPLE_TYPE g_msType[] = { 
           D3DMULTISAMPLE_NONE,
           D3DMULTISAMPLE_2_SAMPLES,
           D3DMULTISAMPLE_3_SAMPLES,
           D3DMULTISAMPLE_4_SAMPLES,
           D3DMULTISAMPLE_5_SAMPLES,
           D3DMULTISAMPLE_6_SAMPLES,
           D3DMULTISAMPLE_7_SAMPLES,
           D3DMULTISAMPLE_8_SAMPLES,
           };



// F U N C T I O N S ///////////////////////////////////////////////

/**
 * Enumerates all available display modes and fills the given combo
 * boxes with corresponding values.
 * -> IN: HWND - combobox to receive available adapters
 *        HWND - combobox used for display modes
 *        HWND - combobox used for device types
 *        HWND - combobox used for adapter formats
 *        HWND - combobox used for backbuffer formats
 *        HWND - radiobutton windowed
 *        HWND - radiobutton fullscreen
 *        FILE - already open log file
 */
HRESULT ZFXD3DEnum::Enum(HWND hAdapter, HWND hMode,
                         HWND hDevice,  HWND hAdapterFmt,
                         HWND hBackFmt, HWND hWnd,
                         HWND hFull,    FILE *pLog) {
   HRESULT hr;

   if (pLog) m_pLog = pLog;

   // create main Direct3D object
   m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
   if(m_pD3D == NULL){
      return ZFX_CREATEAPI;
      }

   // save handles to comboboxes
   m_hADAPTER    = hAdapter;
   m_hMODE       = hMode;
   m_hDEVICE     = hDevice;
   m_hADAPTERFMT = hAdapterFmt;
   m_hBACKFMT    = hBackFmt;
   m_hWND        = hWnd;
   m_hFULL       = hFull;

   // nothing yet
   m_dwNumAdapters = 0;

   // set misc values
   m_nMinWidth  = 800;
   m_nMinHeight = 600;
   m_nMinBits   = 16;
   
   // set allowed bpp formats
   m_fmtAdapter[0] = D3DFMT_X8R8G8B8;  // 32 Bit
   m_fmtAdapter[1] = D3DFMT_X1R5G5B5;  // 15 Bit
   m_fmtAdapter[2] = D3DFMT_R5G6B5;    // 16 Bit
   m_nNumFmt = 3;

   // get current adapters display mode
   if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &m_dspmd))) {
      if (m_pLog) {
         fprintf(m_pLog, "Enum: GetAdapterDisplayMode() failed \n");
         fflush(m_pLog);
         }
      }

   // enumerate available adapters
   hr = EnumAdapters();
   
   // we don't need this for now
   m_pD3D->Release();
   m_pD3D = NULL;

   if(FAILED(hr) || !hAdapter)
      return ZFX_FAIL;

   // list the found graphics adapter
   SendMessage(m_hADAPTER,CB_RESETCONTENT,0,0);

   for (UINT a=0; a<m_dwNumAdapters; a++) {
      AddItem(m_hADAPTER, m_xAdapterInfo[a].
              d3dAdapterIdentifier.Description, 
              &m_xAdapterInfo[a]);
      }

   // treat enum like adapter change
   SendMessage(m_hADAPTER, CB_SETCURSEL, (WPARAM)0, 0);
   ChangedAdapter();

   // select first entry from each combo
   SendMessage(m_hADAPTER, CB_SETCURSEL, (WPARAM)0, 0);
   SendMessage(m_hMODE,    CB_SETCURSEL, (WPARAM)0, 0);
   SendMessage(m_hDEVICE,  CB_SETCURSEL, (WPARAM)0, 0);
   SendMessage(m_hBACKFMT, CB_SETCURSEL, (WPARAM)0, 0);
   SendMessage(m_hADAPTERFMT, CB_SETCURSEL, (WPARAM)0, 0);

   return ZFX_OK;
   } // Enum
/*----------------------------------------------------------------*/

/**
 * Three helper function for combobox-convenience.
 */
void* GetSelectedItem(HWND hWnd) {
   WPARAM nI = (WPARAM)((int)(DWORD)SendMessage(hWnd,CB_GETCURSEL,0,0));
   return ((void*)SendMessage(hWnd,CB_GETITEMDATA,nI,0));
   }
/*----------------------------------------------------------------*/

void AddItem(HWND hWnd, char *ch, void *pData) {
   WPARAM nI = (WPARAM)((int)(DWORD)SendMessage(hWnd,CB_ADDSTRING,0,(LPARAM)ch));
   SendMessage(hWnd,CB_SETITEMDATA, nI, (LPARAM)pData);
   }
/*----------------------------------------------------------------*/

bool ContainsString(HWND hWnd, char *ch) {
   int n = (int)SendMessage(hWnd, CB_GETCOUNT, 0, 0);
   char buffer[200];

   for(int nIndex=0; nIndex<n; nIndex++) {
      SendMessage(hWnd, CB_GETLBTEXT, (WPARAM)nIndex, (LPARAM)buffer);
      if (lstrcmp(buffer, ch) == 0)
         return true;
    }
   return false;
   }
/*----------------------------------------------------------------*/


/**
 * Ask the Comboboxes for the final settings.
 */
void ZFXD3DEnum::GetSelections(ZFXDEVICEINFO *pD, D3DDISPLAYMODE *dspmd,
                               D3DFORMAT *fmtA, D3DFORMAT *fmtB) {

   if (pD)
      memcpy(pD, GetSelectedItem(m_hDEVICE), sizeof(ZFXDEVICEINFO));
   if (fmtA)
      memcpy(fmtA, GetSelectedItem(m_hADAPTERFMT), sizeof(D3DFORMAT));
   if (fmtB)
      memcpy(fmtB, GetSelectedItem(m_hBACKFMT), sizeof(D3DFORMAT));

   if (dspmd) {
      D3DDISPLAYMODE *pDspmd = (D3DDISPLAYMODE*)GetSelectedItem(m_hMODE);
      // fullscreen mode
      if (pDspmd) memcpy(dspmd, pDspmd, sizeof(D3DDISPLAYMODE));
      // windowed mode
      else
         memcpy(dspmd, &m_dspmd, sizeof(D3DDISPLAYMODE));
      }
   } // GetSelections
/*----------------------------------------------------------------*/


/**
 * Call when adapter selection changed. This will reset device
 * combobox, list devices for new adapter, and call ChangedDevice().
 */
void ZFXD3DEnum::ChangedAdapter(void) {
   ZFXADAPTERINFO *pA=NULL;

   // delete content of device combobox
   SendMessage(m_hDEVICE, CB_RESETCONTENT, 0, 0);

   pA = (ZFXADAPTERINFO *)GetSelectedItem(m_hADAPTER);

   for (UINT d=0; d<pA->nNumDevs; d++) {
      AddItem(m_hDEVICE, D3DDevTypeToString(pA->d3dDevs[d].
              d3dDevType), &pA->d3dDevs[d]);
     }
   SendMessage(m_hDEVICE, CB_SETCURSEL, (WPARAM)0, 0);
   
   // treat as if device slectio changed
   ChangedDevice();
   } // ChangedAdapter
/*----------------------------------------------------------------*/

/**
 * Call when device selection changed. This will reset radio button
 * states for windowed/fullscreen mode and call ChangedWindowMode().
 */
void ZFXD3DEnum::ChangedDevice(void) {
   ZFXDEVICEINFO *pD=NULL;
   bool           bWindowed=false,
                  bFullscreen=false;

   pD = (ZFXDEVICEINFO *)GetSelectedItem(m_hDEVICE);

   // check if windowed/fullscreen mode possible
   for (UINT c=0; c<pD->nNumCombo; c++) {
      if (pD->d3dCombo[c].bWindowed)
         bWindowed = true;
      else
         bFullscreen = true;
      }

   // active radio buttons accordingliy
   EnableWindow(m_hWND,  bWindowed);
   EnableWindow(m_hFULL, bFullscreen);

   if (!bWindowed && (SendMessage(m_hWND,BM_GETCHECK,0,0)==BST_CHECKED))
      SendMessage(m_hFULL, BM_SETCHECK, BST_CHECKED, 0);

   // treat as if radio button changed
   ChangedWindowMode();
   } // ChangedDevice
/*----------------------------------------------------------------*/

/**
 * Call when window/fullscreen selection changed. Will relist adapter-
 * format and mode combobox and call ChangedAdapterFmt().
 */
void ZFXD3DEnum::ChangedWindowMode(void) {
   ZFXADAPTERINFO *pA=NULL;
   ZFXDEVICEINFO  *pD=NULL;
   ZFXCOMBOINFO   *pC=NULL;
   char            buffer[100];

   pA = (ZFXADAPTERINFO *)GetSelectedItem(m_hADAPTER);
   pD = (ZFXDEVICEINFO  *)GetSelectedItem(m_hDEVICE);

   // clear'em out
   SendMessage(m_hADAPTERFMT, CB_RESETCONTENT, 0, 0);
   SendMessage(m_hMODE,       CB_RESETCONTENT, 0, 0);

   // windowed mode?
   if (SendMessage(m_hWND,BM_GETCHECK,0,0)==BST_CHECKED) {
      // use current desktop format
      AddItem(m_hADAPTERFMT, D3DFormatToString(m_dspmd.Format), 
              &m_dspmd.Format);
      // use current desktop resolution
      sprintf(buffer, "%d x %d", m_dspmd.Width, m_dspmd.Height);
      SendMessage(m_hMODE, CB_ADDSTRING, 0, (LPARAM)buffer);

      EnableWindow(m_hADAPTERFMT, false);
      EnableWindow(m_hMODE, false);
      }
   else {
      SendMessage(m_hADAPTERFMT, CB_RESETCONTENT, 0, 0);
      SendMessage(m_hMODE, CB_RESETCONTENT, 0, 0);
      EnableWindow(m_hADAPTERFMT, true);
      EnableWindow(m_hMODE, true);

      // list all combos for this device
      for (UINT c=0; c<pD->nNumCombo; c++) {
         pC = &pD->d3dCombo[c];

         if (!ContainsString(m_hADAPTERFMT, D3DFormatToString(pC->fmtAdapter)))
            AddItem(m_hADAPTERFMT, 
                    D3DFormatToString(pC->fmtAdapter), 
                    &pC->fmtAdapter);
         } // for [combos]
      }

   // select first item each
   SendMessage(m_hADAPTERFMT, CB_SETCURSEL, (WPARAM)0, 0);
   SendMessage(m_hMODE,       CB_SETCURSEL, (WPARAM)0, 0);

   // treat as if adapter format changed
   ChangedAdapterFmt();
   } // ChangedWindowMode
/*----------------------------------------------------------------*/

/**
 * Call when adapter format selection changed. Will relist mode and
 * backbuffer-format combobox.
 */
void ZFXD3DEnum::ChangedAdapterFmt(void) {
   ZFXADAPTERINFO *pA=NULL;
   ZFXDEVICEINFO  *pD=NULL;
   ZFXCOMBOINFO   *pC=NULL;
   D3DDISPLAYMODE *dpsmd=NULL;
   D3DFORMAT      *pFmt=NULL;
   char            buffer[200];
   bool            bWindowed;

   pA   = (ZFXADAPTERINFO *)GetSelectedItem(m_hADAPTER);
   pD   = (ZFXDEVICEINFO *)GetSelectedItem(m_hDEVICE);
   pFmt = (D3DFORMAT *)GetSelectedItem(m_hADAPTERFMT);
   
   bWindowed = (SendMessage(m_hWND,BM_GETCHECK,0,0)==BST_CHECKED);
   SendMessage(m_hBACKFMT, CB_RESETCONTENT, 0, 0);

   // windowed mode?
   if (!bWindowed) {
      SendMessage(m_hMODE, CB_RESETCONTENT, 0, 0);

      // loop through all available modes
      for (UINT m=0; m<pA->nNumModes; m++) {
         dpsmd = &pA->d3ddspmd[m];
         sprintf(buffer, "%d x %d", dpsmd->Width, dpsmd->Height);
         if (!ContainsString(m_hMODE, buffer)) {
            AddItem(m_hMODE, buffer, &pA->d3ddspmd[m]);
            }
         } // for [modes]

      // set backbuffer format to adapter format
      AddItem(m_hBACKFMT,D3DFormatToString(*pFmt),&(*pFmt));

      SendMessage(m_hMODE, CB_SETCURSEL, (WPARAM)0, 0);
      } // [!windowed]
   else {
      for (UINT c=0; c<pD->nNumCombo; c++) {
         pC = &pD->d3dCombo[c];

         // if same window mode and adapter format
         if ((bWindowed == pC->bWindowed) && 
             (*pFmt == pC->fmtAdapter) ) {
            if (!ContainsString(m_hBACKFMT, 
                D3DFormatToString(pC->fmtAdapter)))
                AddItem(m_hBACKFMT, 
                        D3DFormatToString(pC->fmtBackBuffer), 
                        &pC->fmtBackBuffer);
            }
         }
      }

   SendMessage(m_hBACKFMT, CB_SETCURSEL, (WPARAM)0, 0);
   } // ChangedAdapterFmt
/*----------------------------------------------------------------*/

static int __cdecl SortModesCallback(const void* arg1, const void* arg2) {
   D3DDISPLAYMODE* pdm1 = (D3DDISPLAYMODE*)arg1;
   D3DDISPLAYMODE* pdm2 = (D3DDISPLAYMODE*)arg2;
   
   if (pdm1->Width > pdm2->Width)
      return 1;
   if (pdm1->Width < pdm2->Width)
      return -1;
   if (pdm1->Height > pdm2->Height)
      return 1;
   if (pdm1->Height < pdm2->Height)
      return -1;
   if (pdm1->Format > pdm2->Format)
      return 1;
   if (pdm1->Format < pdm2->Format)
      return -1;
   if (pdm1->RefreshRate > pdm2->RefreshRate)
      return 1;
   if (pdm1->RefreshRate < pdm2->RefreshRate)
      return -1;
   return 0;
   } // SortModesCallback
/*----------------------------------------------------------------*/


HRESULT ZFXD3DEnum::EnumAdapters(void) {
   D3DDISPLAYMODE d3ddspmd;
   HRESULT        hr;
   UINT           nNumAdapters=0;
   UINT           nNumModes=0;

   if (!m_pD3D) return ZFX_INVALIDPARAM;

   // get number of attached adapters
   nNumAdapters = m_pD3D->GetAdapterCount();

   // loop through adapters
   for (UINT nAdapter=0; nAdapter<nNumAdapters; nAdapter++) {
      m_xAdapterInfo[m_dwNumAdapters].nAdapter  = nAdapter;
      m_xAdapterInfo[m_dwNumAdapters].nNumModes = 0;

      // get adapter information struct
      m_pD3D->GetAdapterIdentifier(nAdapter, 0, 
                                   &m_xAdapterInfo[m_dwNumAdapters].
                                   d3dAdapterIdentifier);

      // loop through allowed bpp formats
      for (UINT nFmt=0; nFmt<m_nNumFmt; nFmt++) {
        // get available display modes for this adapter/bpp format
         nNumModes = m_pD3D->GetAdapterModeCount(nAdapter, m_fmtAdapter[nFmt]);
         
         for (UINT nMode=0; nMode<nNumModes; nMode++) {
            // get number of available display modes
            m_pD3D->EnumAdapterModes(nAdapter, m_fmtAdapter[nFmt], 
                                     nMode, &d3ddspmd);

            // don't use less than we asked for
            if (d3ddspmd.Width           < m_nMinWidth  ||
                d3ddspmd.Height          < m_nMinHeight ||
                GetBits(d3ddspmd.Format) < m_nMinBits) {
               continue;
               }
            // else save this mode
            else {
               m_xAdapterInfo[m_dwNumAdapters].d3ddspmd[
                  m_xAdapterInfo[m_dwNumAdapters].nNumModes] = d3ddspmd;
               m_xAdapterInfo[m_dwNumAdapters].nNumModes++;
               }
            } // for [modes]
         } // for [formats]

      // we got the data, now sort it
      qsort(m_xAdapterInfo[m_dwNumAdapters].d3ddspmd, 
            m_xAdapterInfo[m_dwNumAdapters].nNumModes,
            sizeof(D3DDISPLAYMODE),
            SortModesCallback);

      if (FAILED(hr=EnumDevices(m_xAdapterInfo[m_dwNumAdapters]))) 
         return hr;

      // keep this one if any device is found
      if (m_xAdapterInfo[m_dwNumAdapters].nNumDevs > 0)
         m_dwNumAdapters++;
      } // for [adapters]

   return ZFX_OK;
   } //EnumAdapters
/*----------------------------------------------------------------*/


HRESULT ZFXD3DEnum::EnumDevices(ZFXADAPTERINFO &xAdapter) {
   ZFXDEVICEINFO *pDev;
   HRESULT        hr;

   const D3DDEVTYPE d3dDevTypes[] = { 
               D3DDEVTYPE_HAL, 
               D3DDEVTYPE_SW, 
               D3DDEVTYPE_REF 
               };

   xAdapter.nNumDevs = 0;

   for (UINT nDev=0; nDev<3; nDev++) {
      pDev = &xAdapter.d3dDevs[xAdapter.nNumDevs];

      pDev->nNumCombo   = 0;
      pDev->nAdapter    = xAdapter.nAdapter;
      pDev->d3dDevType  = d3dDevTypes[nDev];

      // will fail if unsupported devtype
      if (FAILED(m_pD3D->GetDeviceCaps(pDev->nAdapter,
                 pDev->d3dDevType, &pDev->d3dCaps)))
         continue;

      // this should not fail
      if (FAILED(hr=EnumCombos(*pDev)))
         return hr;
      
      // at least one supportet combo?
      if (pDev->nNumCombo <= 0)
         continue;

      // keep valid device
      xAdapter.nNumDevs++;
      } // for

   return ZFX_OK;
   } // EnumDevices
/*----------------------------------------------------------------*/


HRESULT ZFXD3DEnum::EnumCombos(ZFXDEVICEINFO &xDev) {
   ZFXCOMBOINFO *pCombo;
   bool          bWindowed;
   bool          bFmtCheck=false;

   xDev.nNumCombo = 0;

   // all adapter Formats
   for (UINT nFmt_A=0; nFmt_A<m_nNumFmt; nFmt_A++) {

      // for all allowed backbuffer formats
      for (UINT nFmt_B=0; nFmt_B<g_nFormats_B; nFmt_B++) {

         // check windowed and fullscreen
         for (UINT n=0; n<2; n++) {
            if (n==0) bWindowed=true;
            else bWindowed=false;

            // valid combo?
            if (FAILED(m_pD3D->CheckDeviceType(
                        xDev.nAdapter, 
                        xDev.d3dDevType, 
                        m_fmtAdapter[nFmt_A], 
                        g_fmtBackBuffer[nFmt_B], 
                        bWindowed)))
               continue;

            // at this point we have a valid combo that is supported
            // by the system. still need to find depth/stencil fmt
            // and best vertex processing type. 
            pCombo = &xDev.d3dCombo[xDev.nNumCombo];
            pCombo->nAdapter      = xDev.nAdapter;
            pCombo->d3dDevType    = xDev.d3dDevType;
            pCombo->bWindowed     = bWindowed;
            pCombo->fmtBackBuffer = g_fmtBackBuffer[nFmt_B];
            pCombo->fmtAdapter    = m_fmtAdapter[nFmt_A];

      // SELECT DEPTH/STENCIL FORMAT

             // select depth fmt & stencil buffer
            if (m_nMinStencil) {
               pCombo->fmtDepthStencil = D3DFMT_D24S8;
               bFmtCheck = ConfirmDepthFmt(pCombo);
              
               if (!bFmtCheck) {
                  pCombo->fmtDepthStencil = D3DFMT_D24X4S4;
                  bFmtCheck = ConfirmDepthFmt(pCombo);
                  }
               if (!bFmtCheck) {
                  pCombo->fmtDepthStencil = D3DFMT_D15S1;
                  bFmtCheck = ConfirmDepthFmt(pCombo);
                  }
               }
            // or without stencil buffer
            else {
               if (m_nMinDepth > 24) {
                  pCombo->fmtDepthStencil = D3DFMT_D32;
                  bFmtCheck = ConfirmDepthFmt(pCombo);
                  }
               if (!bFmtCheck && (m_nMinDepth > 16)) {
                  pCombo->fmtDepthStencil = D3DFMT_D24X8;
                  bFmtCheck = ConfirmDepthFmt(pCombo);
                  }
               else {
                  pCombo->fmtDepthStencil = D3DFMT_D16;
                  bFmtCheck = ConfirmDepthFmt(pCombo);
                  }
               }

            // no suitable fmt => ignore this combo
            if (!bFmtCheck) continue;

      // SELECT VERTEX PROCESSING

            // we have hardware T&L
            if ((xDev.d3dCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0) {
               
               // 1.case: pure device
               if ( ((xDev.d3dCaps.DevCaps & D3DDEVCAPS_PUREDEVICE)!=0) 
                  && (ConfirmDevice(&xDev.d3dCaps, D3DCREATE_PUREDEVICE, 
                                    pCombo->fmtBackBuffer)==ZFX_OK) ) {
                  pCombo->dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING 
                                     | D3DCREATE_PUREDEVICE;
                  }
               // 2.case: hardware device
               else if (ConfirmDevice(&xDev.d3dCaps, D3DCREATE_HARDWARE_VERTEXPROCESSING, 
                                      pCombo->fmtBackBuffer)==ZFX_OK) {
                  pCombo->dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
                  }
               // 3.case: mixed sw/hw
               else if (ConfirmDevice(&xDev.d3dCaps, D3DCREATE_MIXED_VERTEXPROCESSING, 
                                      pCombo->fmtBackBuffer)==ZFX_OK) {
                  pCombo->dwBehavior = D3DCREATE_MIXED_VERTEXPROCESSING;
                  }
               } // if [HW]
            // 4.case: must be sw processing
            else {
               if (ConfirmDevice(&xDev.d3dCaps, D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
                                 pCombo->fmtBackBuffer)==ZFX_OK) {
                  pCombo->dwBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
                  }
               }

      // SELECT MULTISAMPLE TYPE

            DWORD msQuality;
            for(UINT nMS=g_nMS-1; nMS>=0; nMS--) {
               if (SUCCEEDED(m_pD3D->
                     CheckDeviceMultiSampleType(
                        pCombo->nAdapter, 
                        pCombo->d3dDevType, 
                        pCombo->fmtBackBuffer, 
                        pCombo->bWindowed,
                        g_msType[nMS], &msQuality))) {
                  pCombo->msType = g_msType[nMS];
                  break;
                  }
               } // for [multisample]


            // NO WE GOT EVERYTHING WE NEE AND KEEP THIS COMBO
            xDev.nNumCombo++;

            } // for [windowed/fullscreen]
         } // for [BackbufferFormats]
      } // for [AdapterFormats]

   return ZFX_OK;
   }
/*----------------------------------------------------------------*/


bool ZFXD3DEnum::ConfirmDepthFmt(ZFXCOMBOINFO *pCombo) {
   // compatible with adapter?
   if (FAILED(m_pD3D->CheckDeviceFormat(pCombo->nAdapter, 
                                  pCombo->d3dDevType, 
                                  pCombo->fmtAdapter, 
                                  D3DUSAGE_DEPTHSTENCIL, 
                                  D3DRTYPE_SURFACE, 
                                  pCombo->fmtDepthStencil)))
      return false;

   // compatible with backbuffer format?
   if (FAILED(m_pD3D->CheckDepthStencilMatch(
                                  pCombo->nAdapter, 
                                  pCombo->d3dDevType, 
                                  pCombo->fmtAdapter, 
                                  pCombo->fmtBackBuffer,
                                  pCombo->fmtDepthStencil)))
      return false;

   return true;
   } // ConfirmDepthFmt
/*----------------------------------------------------------------*/


HRESULT ZFXD3DEnum::ConfirmDevice(D3DCAPS9* pCaps, DWORD dwBehavior, 
                                  D3DFORMAT fmtBackbuffer) {
   if ( (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING) ||
        (dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING) ) {
      // alphablending from texture pixels supported
      if ( !(pCaps->TextureCaps & D3DPTEXTURECAPS_ALPHA) ) {
         fprintf(m_pLog, "[ZFXD3D_ENUM] error: no alphablending from texture \n");
         return ZFX_FAIL;
         }

      if (pCaps->VertexShaderVersion < D3DVS_VERSION(1,0) ) {
         fprintf(m_pLog, "[ZFXD3D_ENUM] error: Vertex Shader Version < 1.0 \n");
         return ZFX_FAIL;
         }
      }
 
   return ZFX_OK;
   } // ConfirmDevice
/*----------------------------------------------------------------*/


UINT ZFXD3DEnum::GetBits(D3DFORMAT fmt) {
   switch(fmt) {
      case D3DFMT_A2B10G10R10: return 30;
      case D3DFMT_R8G8B8:      return 24;
      case D3DFMT_A8R8G8B8:    return 24;
      case D3DFMT_X8R8G8B8:    return 24;
      case D3DFMT_R5G6B5:      return 16;
      case D3DFMT_X1R5G5B5:    return 15;
      case D3DFMT_A1R5G5B5:    return 15;
      case D3DFMT_X4R4G4B4:    return 12;
      case D3DFMT_A4R4G4B4:    return 12;
      case D3DFMT_R3G3B2:      return 8;
      case D3DFMT_A8R3G3B2:    return 8;
      default:                 return 0;
      }
   } // GetBits
/*----------------------------------------------------------------*/


TCHAR* D3DDevTypeToString(D3DDEVTYPE devType) {
   switch (devType) {
      case D3DDEVTYPE_HAL:    return TEXT("D3DDEVTYPE_HAL");
      case D3DDEVTYPE_SW:     return TEXT("D3DDEVTYPE_SW");
      case D3DDEVTYPE_REF:    return TEXT("D3DDEVTYPE_REF");
      default:                return TEXT("Unknown devType");
      }
   }
/*----------------------------------------------------------------*/


TCHAR* D3DFormatToString(D3DFORMAT format) {
   switch (format) {
      case D3DFMT_UNKNOWN:         return TEXT("D3DFMT_UNKNOWN");
      case D3DFMT_R8G8B8:          return TEXT("D3DFMT_R8G8B8");
      case D3DFMT_A8R8G8B8:        return TEXT("D3DFMT_A8R8G8B8");
      case D3DFMT_X8R8G8B8:        return TEXT("D3DFMT_X8R8G8B8");
      case D3DFMT_R5G6B5:          return TEXT("D3DFMT_R5G6B5");
      case D3DFMT_X1R5G5B5:        return TEXT("D3DFMT_X1R5G5B5");
      case D3DFMT_A1R5G5B5:        return TEXT("D3DFMT_A1R5G5B5");
      case D3DFMT_A4R4G4B4:        return TEXT("D3DFMT_A4R4G4B4");
      case D3DFMT_R3G3B2:          return TEXT("D3DFMT_R3G3B2");
      case D3DFMT_A8R3G3B2:        return TEXT("D3DFMT_A8R3G3B2");
      case D3DFMT_X4R4G4B4:        return TEXT("D3DFMT_X4R4G4B4");
      case D3DFMT_A2B10G10R10:     return TEXT("D3DFMT_A2B10G10R10");
      case D3DFMT_D16_LOCKABLE:    return TEXT("D3DFMT_D16_LOCKABLE");
      case D3DFMT_D32:             return TEXT("D3DFMT_D32");
      case D3DFMT_D15S1:           return TEXT("D3DFMT_D15S1");
      case D3DFMT_D24S8:           return TEXT("D3DFMT_D24S8");
      case D3DFMT_D16:             return TEXT("D3DFMT_D16");
      case D3DFMT_D24X8:           return TEXT("D3DFMT_D24X8");
      case D3DFMT_D24X4S4:         return TEXT("D3DFMT_D24X4S4");
      case D3DFMT_VERTEXDATA:      return TEXT("D3DFMT_VERTEXDATA");
      case D3DFMT_INDEX16:         return TEXT("D3DFMT_INDEX16");
      case D3DFMT_INDEX32:         return TEXT("D3DFMT_INDEX32");
      default:                     return TEXT("Unknown format");
      }
   }
/*----------------------------------------------------------------*/


TCHAR* BehaviorTypeToString(DWORD vpt) {
   switch (vpt) {
      case D3DCREATE_SOFTWARE_VERTEXPROCESSING: return TEXT("SOFTWARE_VP");
      case D3DCREATE_MIXED_VERTEXPROCESSING:    return TEXT("MIXED_VP");
      case D3DCREATE_HARDWARE_VERTEXPROCESSING: return TEXT("HARDWARE_VP");
      case D3DCREATE_PUREDEVICE:                return TEXT("PURE_HARDWARE_VP");
      default:                                  return TEXT("Unknown VP");
      }
   }
/*----------------------------------------------------------------*/