// File: DlgCallbacks.cpp

#include "include/Globals.h"                        // GLOBALS struct
#include "include/gui.h"                            // gui id's
#include "include/ComboBoxHelper.h"                 // cbhXXX functions
#include "include/DlgCallbacks.h"
#include "include/CApplication.h"


// for StrTrim() only
#include "Shlwapi.h"
#pragma comment(lib, "shlwapi.lib")


extern CApplication g_MyApp;


// HELPER FUNCTIONS: Get/Set window text as float or int
float GetWndTextToFloat(HWND hWnd, float *pf) {
   char buffer[1024];
   GetWindowText(hWnd, buffer, 1024);
   sscanf(buffer, "%f", pf);
   return *pf;
   }
int GetWndTextToInt(HWND hWnd, int *pn) {
   char buffer[1024];
   GetWindowText(hWnd, buffer, 1024);
   sscanf(buffer, "%d", pn);
   return *pn;
   }
void SetWndTextToInt(HWND hWnd, int n) {
   char buffer[512];
   sprintf(buffer, "%d", n);
   SetWindowText(hWnd, buffer);
   }
void SetWndTextToFloat(HWND hWnd, float f) {
   char buffer[512];
   sprintf(buffer, "%f", f);
   SetWindowText(hWnd, buffer);
   }
/*----------------------------------------------------------------*/


// info box callback
BOOL CALLBACK CB_InfoBox(HWND hDlg, UINT msg, WPARAM w, LPARAM l) {
   switch (msg) {
      // render if user drags the dialog
      case WM_SIZING: case WM_MOVING: {
         g_MyApp.InvalidateAllViews();
         g_MyApp.RenderTick();
         return TRUE;
         }

      case WM_INITDIALOG: {
         return TRUE;
         } break;

      case WM_COMMAND: {
         switch (LOWORD(w)) {
            case IDOK:
               EndDialog(hDlg, 1);
               g_MyApp.InvalidateAllViews();
               return TRUE;
            }
         } break;
      }
   return 0;
   }
/*----------------------------------------------------------------*/


// sector ambient light box callback
BOOL CALLBACK CB_SectorAmbientBox(HWND hDlg, UINT msg, WPARAM w, LPARAM l) {
   ZFXCOLOR clr;
   CSector *pSector=NULL;
   char buffer[2048];

   HWND hName = GetDlgItem(hDlg, IDC_NAME);
   HWND hR    = GetDlgItem(hDlg, IDC_RED);
   HWND hG    = GetDlgItem(hDlg, IDC_GREEN);
   HWND hB    = GetDlgItem(hDlg, IDC_BLUE);

   pSector = CApplication::m_G.Level.GetSelectedSector();

   switch (msg) {
      // render if user drags the dialog
      case WM_SIZING: case WM_MOVING: {
         g_MyApp.InvalidateAllViews();
         g_MyApp.RenderTick();
         return TRUE;
         }

      case WM_INITDIALOG: {
         pSector->GetName(buffer);
         clr = pSector->GetAmbient();
         SetWindowText(hName, buffer);
         sprintf(buffer, "%f", clr.fR);
         SetWindowText(hR, buffer);
         sprintf(buffer, "%f", clr.fG);
         SetWindowText(hG, buffer);
         sprintf(buffer, "%f", clr.fB);
         SetWindowText(hB, buffer);
         return TRUE;
         } break;

      case WM_COMMAND: {
         switch (LOWORD(w)) {
            case IDOK:
               GetWindowText(hR, buffer, 256);
               sscanf(buffer,"%f",&clr.fR);
               GetWindowText(hG, buffer, 256);
               sscanf(buffer,"%f",&clr.fG);
               GetWindowText(hB, buffer, 256);
               sscanf(buffer,"%f",&clr.fB);
               pSector->SetAmbient(clr.fR, clr.fG, clr.fB);
               EndDialog(hDlg, 0);
               g_MyApp.InvalidateAllViews();
               return TRUE;

            case IDCANCEL:
               EndDialog(hDlg, 0);
               g_MyApp.InvalidateAllViews();
               return TRUE;
            }
         } break;
      }
   return 0;
   } // CB_SectorAmbientBox
/*----------------------------------------------------------------*/


// level attribute dialog callback
BOOL CALLBACK CB_LevelAttribBox(HWND hDlg, UINT msg, WPARAM w, LPARAM l) {
   HWND hName    = GetDlgItem(hDlg, IDC_NAME);
   HWND hSectors = GetDlgItem(hDlg, IDC_SECTORS);
   HWND hVerts   = GetDlgItem(hDlg, IDC_VERTS);
   HWND hIndis   = GetDlgItem(hDlg, IDC_INDIS);
   HWND hPolys   = GetDlgItem(hDlg, IDC_POLYS);
   HWND hMeshs   = GetDlgItem(hDlg, IDC_MESHS);
   HWND hPorts   = GetDlgItem(hDlg, IDC_PORTS);
   HWND hLights  = GetDlgItem(hDlg, IDC_LIGHTS);
   HWND hEntys   = GetDlgItem(hDlg, IDC_ENTITIES);

   UINT NumV=0, NumI=0, NumM=0, NumPl=0, NumPt=0,
        NumL=0, NumE=0;

   char ch[1024];

   switch (msg) {
      // render if user drags the dialog
      case WM_SIZING: case WM_MOVING: {
         g_MyApp.InvalidateAllViews();
         g_MyApp.RenderTick();
         return TRUE;
         }

      case WM_INITDIALOG: {
         CApplication::m_G.Level.GetNumLobs(&NumV, &NumI, 
                                            &NumM, &NumPl, 
                                            &NumPt, &NumL,
                                            &NumE);
         CApplication::m_G.Level.GetName(ch);
         SetWindowText(hName, ch);

         // set counts:
         SetWndTextToInt(hSectors, CApplication::m_G.Level.GetNumSectors());
         SetWndTextToInt(hVerts,  NumV);
         SetWndTextToInt(hIndis,  NumI);
         SetWndTextToInt(hPolys,  NumPl);
         SetWndTextToInt(hMeshs,  NumM);
         SetWndTextToInt(hPorts,  NumPt);
         SetWndTextToInt(hLights, NumL);
         SetWndTextToInt(hEntys,  NumE);
         return TRUE;
         } break;

      case WM_COMMAND: {
         switch (LOWORD(w)) {
            case IDOK:
               EndDialog(hDlg, 1);
               g_MyApp.InvalidateAllViews();
               return TRUE;
            }
         } break;
      }
   return 0;
   } // level attrib
/*----------------------------------------------------------------*/


// new sector box callback
BOOL CALLBACK CB_NewSectorBox(HWND hDlg, UINT msg, WPARAM w, LPARAM l) {
   HWND hEdit = GetDlgItem(hDlg, IDC_EDIT);
   char ch[1024];

   switch (msg) {
      // render if user drags the dialog
      case WM_SIZING: case WM_MOVING: {
         g_MyApp.InvalidateAllViews();
         g_MyApp.RenderTick();
         return TRUE;
         }

      case WM_INITDIALOG: {
         return TRUE;
         } break;

      case WM_COMMAND: {
         switch (LOWORD(w)) {

            case IDOK: {
               GetWindowText(hEdit, (LPTSTR)ch, 1024);

               StrTrim(ch, " ");
               
               if (CApplication::m_G.Level.SectorExists(ch)) {
                  MessageBox(hDlg, "Name already exists.", 
                             "PanEdit - Error", MB_OK | MB_ICONERROR);
                  return TRUE;
                  }
               else if ( strcmp(ch, "")==0 ) {
                  MessageBox(hDlg, "Please enter a valid name.", 
                             "PanEdit - Error", MB_OK | MB_ICONERROR);
                  return TRUE;
                  }

               CApplication::m_G.Level.AddSector(ch);
               CApplication::m_G.Level.SelectSector(ch);
               cbhAddItem(CApplication::m_G.hGUI[SECTOR], ch, NULL, true);
               EndDialog(hDlg, 1);
               g_MyApp.InvalidateAllViews();
               return TRUE;
               }

            case IDCANCEL: {
               EndDialog(hDlg, 0);
               g_MyApp.InvalidateAllViews();
               return TRUE;
               }
            }
         } break;
      }
   return 0;
   } // new sector 
/*----------------------------------------------------------------*/


// sector attribute dialog callback
BOOL CALLBACK CB_SectorAttribBox(HWND hDlg, UINT msg, WPARAM w, LPARAM l) {
   CSector *pSector;
   HWND hName   = GetDlgItem(hDlg, IDC_NAME);
   HWND hVerts  = GetDlgItem(hDlg, IDC_VERTS);
   HWND hIndis  = GetDlgItem(hDlg, IDC_INDIS);
   HWND hPolys  = GetDlgItem(hDlg, IDC_POLYS);
   HWND hMeshs  = GetDlgItem(hDlg, IDC_MESHS);
   HWND hPorts  = GetDlgItem(hDlg, IDC_PORTS);
   HWND hLights = GetDlgItem(hDlg, IDC_LIGHTS);
   HWND hEntys  = GetDlgItem(hDlg, IDC_ENTITIES);

   HWND hVINP = GetDlgItem(hDlg, IDC_VINP);
   HWND hIINP = GetDlgItem(hDlg, IDC_IINP);
   HWND hVINM = GetDlgItem(hDlg, IDC_VINM);
   HWND hIINM = GetDlgItem(hDlg, IDC_IINM);
   HWND hPINM = GetDlgItem(hDlg, IDC_PINM);

   UINT NumVerts=0, NumIndis=0;

   char ch[1024];

   switch (msg) {
      // render if user drags the dialog
      case WM_SIZING: case WM_MOVING: {
         g_MyApp.InvalidateAllViews();
         g_MyApp.RenderTick();
         return TRUE;
         }

      case WM_INITDIALOG: {
         pSector = CApplication::m_G.Level.GetSelectedSector();
         pSector->GetName(ch);
         SetWindowText(hName, ch);

         NumVerts = pSector->GetNumVertsM();
         NumIndis = pSector->GetNumIndisM();

         // total counts:
         SetWndTextToInt(hVerts,  pSector->GetNumVerts());
         SetWndTextToInt(hIndis,  pSector->GetNumIndis());
         SetWndTextToInt(hPolys,  pSector->GetNumPolysTotal());
         SetWndTextToInt(hMeshs,  pSector->GetNumMeshs());
         SetWndTextToInt(hPorts,  pSector->GetNumPorts());
         SetWndTextToInt(hLights, pSector->GetNumLights());
         SetWndTextToInt(hEntys,  pSector->GetNumEntities());

         // partial counts:
         SetWndTextToInt(hVINP, pSector->GetNumVerts() - NumVerts);
         SetWndTextToInt(hIINP, pSector->GetNumIndis() - NumIndis);
         SetWndTextToInt(hVINM, NumVerts);
         SetWndTextToInt(hIINM, NumIndis);
         SetWndTextToInt(hPINM, pSector->GetNumPolysTotal()
                                - pSector->GetNumPolys() );

         return TRUE;
         } break;

      case WM_COMMAND: {
         switch (LOWORD(w)) {

            case IDOK: {
               EndDialog(hDlg, 1);
               g_MyApp.InvalidateAllViews();
               return TRUE;
               }

            case IDCANCEL: {
               EndDialog(hDlg, 0);
               g_MyApp.InvalidateAllViews();
               return TRUE;
               }
            }
         } break;
      }
   return 0;
   } // sector attribs
/*----------------------------------------------------------------*/


// new shape callback
BOOL CALLBACK CB_InsertShape(HWND hDlg, UINT msg, WPARAM w, LPARAM l) {
   CSector    *pSector=NULL;
   CPolygon    Poly[6];
   CPolymesh   Mesh;
   CGeometry   Geo;

   float fW=0.0f, fH=0.0f, fD=0.0f, // width, height, depth
         fX=0.0f, fY=0.0f, fZ=0.0f; // position x, y, z
   int   i=0;
   static int PosW, PosH, PosD;

   HWND hName     = GetDlgItem(hDlg, IDC_SECNAME);
   HWND hWidth    = GetDlgItem(hDlg, IDC_WIDTH);
   HWND hHeight   = GetDlgItem(hDlg, IDC_HEIGHT);
   HWND hDepth    = GetDlgItem(hDlg, IDC_DEPTH);
   HWND hPosX     = GetDlgItem(hDlg, IDC_POSX);
   HWND hPosY     = GetDlgItem(hDlg, IDC_POSY);
   HWND hPosZ     = GetDlgItem(hDlg, IDC_POSZ);
   HWND hInverted = GetDlgItem(hDlg, IDC_INVERTED);
   HWND hPolys    = GetDlgItem(hDlg, IDC_POLYS);

   char ch[1024];
   char buffer[2048];
   bool bInv=false, bPoly=false;
   
   pSector = CApplication::m_G.Level.GetSelectedSector();
   
   switch (msg) {
      // render if user drags the dialog
      case WM_SIZING: case WM_MOVING: {
         g_MyApp.InvalidateAllViews();
         g_MyApp.RenderTick();
         return TRUE;
         }

      case WM_INITDIALOG: {
         pSector->GetName(ch);
         sprintf(buffer, "\"%s\"", ch);
         SetWindowText(hName, buffer);
         return TRUE;
         } break;

      case WM_COMMAND: {
         switch (LOWORD(w)) {

            case IDOK:
               // get selected dimensions
               GetWndTextToFloat(hWidth, &fW);
               GetWndTextToFloat(hHeight, &fH);
               GetWndTextToFloat(hDepth, &fD);
               GetWndTextToFloat(hPosX, &fX);
               GetWndTextToFloat(hPosY, &fY);
               GetWndTextToFloat(hPosZ, &fZ);

               bInv  = (BST_CHECKED==SendMessage(hInverted,BM_GETCHECK,0,0));
               bPoly = (BST_CHECKED==SendMessage(hPolys,BM_GETCHECK,0,0));
               
               g_MyApp.GetToolbar()->OnInsertShape(bInv, bPoly, 
                                               ZFXVector(fX,fY,fZ),
                                               ZFXVector(fW,fH,fW));
               // move out
               EndDialog(hDlg, 1);
               g_MyApp.InvalidateAllViews();
               return TRUE;

            case IDCANCEL:
               EndDialog(hDlg, 0);
               g_MyApp.InvalidateAllViews();
               return TRUE;
            }
         } break;
      }
   return 0;
   } // insert box
/*----------------------------------------------------------------*/


// texture coordinates callback
BOOL CALLBACK CB_TextureBox(HWND hDlg, UINT msg, WPARAM w, LPARAM l) {
   char buffer[256];
   bool bChanged = false;
   int n, m;
   float fNew, fRepU=0.0f, fRepV=0.0f,
               fOffU=0.0f, fOffV=0.0f;

   HWND hSB_U    = GetDlgItem(hDlg, IDC_SCROLLU);
   HWND hSB_V    = GetDlgItem(hDlg, IDC_SCROLLV);
   HWND hOffU    = GetDlgItem(hDlg, IDC_OFFU);
   HWND hOffV    = GetDlgItem(hDlg, IDC_OFFV);
   HWND hRepU    = GetDlgItem(hDlg, IDC_REPU);
   HWND hRepV    = GetDlgItem(hDlg, IDC_REPV);
   
   HWND hSBar = HWND(l);

   // get current settings
   if (msg != WM_INITDIALOG) {
      GetWindowText(hRepU,buffer,256);
      sscanf(buffer,"%f",&fRepU);
      GetWindowText(hRepV,buffer,256);
      sscanf(buffer,"%f",&fRepV);
      GetWindowText(hOffU,buffer,256);
      sscanf(buffer,"%f",&fOffU);
      GetWindowText(hOffV,buffer,256);
      sscanf(buffer,"%f",&fOffV);
      }

   switch (msg) {
      // render if user drags the dialog
      case WM_SIZING: case WM_MOVING: {
         g_MyApp.InvalidateAllViews();
         g_MyApp.RenderTick();
         return TRUE;
         }

      case WM_INITDIALOG: {
         // try to get values, this will work only if all
         // elements if the buffer have the same values
         if ( !CApplication::m_G.SelBuf.GetTextureTrans(
                         &fOffU, &fOffV, &fRepU, &fRepV) ) 
            {
            // otherwise set to default
            fOffU = fOffV = 0.0f;
            fRepU = fRepV = 1.0f;
            }

         // set scrollbars
         SetScrollRange(hSB_U, SB_CTL, 0, 2000, false);
         SetScrollRange(hSB_V, SB_CTL, 0, 2000, false);
         SetScrollPos(hSB_U, SB_CTL, int(fOffU*1000+1000), true);
         SetScrollPos(hSB_V, SB_CTL, int(fOffV*1000+1000), true);

         // set values to edit fields
         sprintf(buffer, "%f", fOffU);
         SetWindowText(hOffU, buffer);
         sprintf(buffer, "%f", fOffV);
         SetWindowText(hOffV, buffer);
         sprintf(buffer, "%f", fRepU);
         SetWindowText(hRepU, buffer);
         sprintf(buffer, "%f", fRepV);
         SetWindowText(hRepV, buffer);
         return TRUE;
         } break;

      // scrollbars to adjust texture offset
      case WM_HSCROLL: {
            n = m = GetScrollPos(hSBar, SB_CTL);
               // process interesting messages
            switch (LOWORD(w)) {
               case SB_LINEUP:
                  n = ZFXMAX(0, n-1); break;
               case SB_LINEDOWN:
                  n = ZFXMIN(2000, n+1); break;
               case SB_PAGEUP:
                  n = ZFXMAX(0, n-50); break;
               case SB_PAGEDOWN:
                  n = ZFXMIN(2000, n+50); break;
               case SB_THUMBPOSITION:
                  n = HIWORD(w); break;
               case SB_THUMBTRACK:
                  n = HIWORD(w); break;
               default: break;
               } // switch
            
            // any changes to apply?
            if (m != n) {
               SetScrollPos(hSBar, SB_CTL, n, TRUE);
               fNew = (float(n)-1000.0f)/1000.0f;
               sprintf(buffer, "%f", fNew);

               // texture offset U
               if (hSBar == hSB_U) {
                  SetWindowText(hOffU, buffer);
                  CApplication::m_G.SelBuf.TransTextureCoords(
                                       fNew, fOffV, fRepU, fRepV);
                  }
               else if (hSBar == hSB_V) {
                  SetWindowText(hOffV, buffer);
                  CApplication::m_G.SelBuf.TransTextureCoords(
                              fOffU, fNew, fRepU, fRepV);
                  }
               // render to make changes visible
               g_MyApp.InvalidateAllViews();
               g_MyApp.RenderTick();
               } // if [changes]
         return TRUE;
         }

      case WM_COMMAND: {
         switch (LOWORD(w)) {
            case IDC_REPU_P:
               sprintf(buffer, "%f",fRepU+0.1f);
               SetWindowText(hRepU, buffer);
               bChanged = true;
               break;
            case IDC_REPU_M:
               sprintf(buffer, "%f",fRepU-0.1f);
               SetWindowText(hRepU, buffer);
               bChanged = true;
               break;
            case IDC_REPV_P:
               sprintf(buffer, "%f",fRepV+0.1f);
               SetWindowText(hRepV, buffer);
               bChanged = true;
               break;
            case IDC_REPV_M:
               sprintf(buffer, "%f",fRepV-0.1f);
               SetWindowText(hRepV, buffer);
               bChanged = true;
               break;

            case IDOK:
               EndDialog(hDlg, 0);
               g_MyApp.InvalidateAllViews();
               return TRUE;

            default: break;
            } // switch

         if (bChanged) {
            CApplication::m_G.SelBuf.TransTextureCoords(
                              fOffU, fOffV, fRepU, fRepV);
            g_MyApp.InvalidateAllViews();
            g_MyApp.RenderTick();
            }

         } break; // case WM_COMMAND
      }
   return 0;
   } // CB_TextureBox
/*----------------------------------------------------------------*/


// detail texture coordinates callback
BOOL CALLBACK CB_TextureBox2(HWND hDlg, UINT msg, WPARAM w, LPARAM l) {
   char buffer[256];
   bool bChanged = false;
   int n, m;
   float fNew, fRepU=0.0f, fRepV=0.0f,
               fOffU=0.0f, fOffV=0.0f;

   HWND hSB_U    = GetDlgItem(hDlg, IDC_SCROLLU);
   HWND hSB_V    = GetDlgItem(hDlg, IDC_SCROLLV);
   HWND hOffU    = GetDlgItem(hDlg, IDC_OFFU);
   HWND hOffV    = GetDlgItem(hDlg, IDC_OFFV);
   HWND hRepU    = GetDlgItem(hDlg, IDC_REPU);
   HWND hRepV    = GetDlgItem(hDlg, IDC_REPV);
   
   HWND hSBar = HWND(l);

   // get current settings
   if (msg != WM_INITDIALOG) {
      GetWindowText(hRepU,buffer,256);
      sscanf(buffer,"%f",&fRepU);
      GetWindowText(hRepV,buffer,256);
      sscanf(buffer,"%f",&fRepV);
      GetWindowText(hOffU,buffer,256);
      sscanf(buffer,"%f",&fOffU);
      GetWindowText(hOffV,buffer,256);
      sscanf(buffer,"%f",&fOffV);
      }

   switch (msg) {
      // render if user drags the dialog
      case WM_SIZING: case WM_MOVING: {
         g_MyApp.InvalidateAllViews();
         g_MyApp.RenderTick();
         return TRUE;
         }

      case WM_INITDIALOG: {
         // try to get values, this will work only if all
         // elements if the buffer have the same values
         if ( !CApplication::m_G.SelBuf.GetTextureTrans(
                         &fOffU, &fOffV, &fRepU, &fRepV) ) 
            {
            // otherwise set to default
            fOffU = fOffV = 0.0f;
            fRepU = fRepV = 1.0f;
            }

         // set scrollbars
         SetScrollRange(hSB_U, SB_CTL, 0, 2000, false);
         SetScrollRange(hSB_V, SB_CTL, 0, 2000, false);
         SetScrollPos(hSB_U, SB_CTL, int(fOffU*1000+1000), true);
         SetScrollPos(hSB_V, SB_CTL, int(fOffV*1000+1000), true);

         // set values to edit fields
         sprintf(buffer, "%f", fOffU);
         SetWindowText(hOffU, buffer);
         sprintf(buffer, "%f", fOffV);
         SetWindowText(hOffV, buffer);
         sprintf(buffer, "%f", fRepU);
         SetWindowText(hRepU, buffer);
         sprintf(buffer, "%f", fRepV);
         SetWindowText(hRepV, buffer);
         return TRUE;
         } break;

      // scrollbars to adjust texture offset
      case WM_HSCROLL: {
            n = m = GetScrollPos(hSBar, SB_CTL);
               // process interesting messages
            switch (LOWORD(w)) {
               case SB_LINEUP:
                  n = ZFXMAX(0, n-1); break;
               case SB_LINEDOWN:
                  n = ZFXMIN(2000, n+1); break;
               case SB_PAGEUP:
                  n = ZFXMAX(0, n-50); break;
               case SB_PAGEDOWN:
                  n = ZFXMIN(2000, n+50); break;
               case SB_THUMBPOSITION:
                  n = HIWORD(w); break;
               case SB_THUMBTRACK:
                  n = HIWORD(w); break;
               default: break;
               } // switch
            
            // any changes to apply?
            if (m != n) {
               SetScrollPos(hSBar, SB_CTL, n, TRUE);
               fNew = (float(n)-1000.0f)/1000.0f;
               sprintf(buffer, "%f", fNew);

               // texture offset U
               if (hSBar == hSB_U) {
                  SetWindowText(hOffU, buffer);
                  CApplication::m_G.SelBuf.TransTextureCoords(
                                       fNew, fOffV, fRepU, fRepV);
                  }
               else if (hSBar == hSB_V) {
                  SetWindowText(hOffV, buffer);
                  CApplication::m_G.SelBuf.TransTextureCoords(
                              fOffU, fNew, fRepU, fRepV);
                  }
               // render to make changes visible
               g_MyApp.InvalidateAllViews();
               g_MyApp.RenderTick();
               } // if [changes]
         return TRUE;
         }

      case WM_COMMAND: {
         switch (LOWORD(w)) {
            case IDC_REPU_P:
               sprintf(buffer, "%f",fRepU+0.1f);
               SetWindowText(hRepU, buffer);
               bChanged = true;
               break;
            case IDC_REPU_M:
               sprintf(buffer, "%f",fRepU-0.1f);
               SetWindowText(hRepU, buffer);
               bChanged = true;
               break;
            case IDC_REPV_P:
               sprintf(buffer, "%f",fRepV+0.1f);
               SetWindowText(hRepV, buffer);
               bChanged = true;
               break;
            case IDC_REPV_M:
               sprintf(buffer, "%f",fRepV-0.1f);
               SetWindowText(hRepV, buffer);
               bChanged = true;
               break;

            case IDOK:
               EndDialog(hDlg, 0);
               g_MyApp.InvalidateAllViews();
               return TRUE;

            default: break;
            } // switch

         if (bChanged) {
            CApplication::m_G.SelBuf.TransTextureCoords(
                              fOffU, fOffV, fRepU, fRepV);
            g_MyApp.InvalidateAllViews();
            g_MyApp.RenderTick();
            }

         } break; // case WM_COMMAND
      }
   return 0;
   } // CB_TextureBox2
/*----------------------------------------------------------------*/


// new skin box callback
BOOL CALLBACK CB_NewSkinBox(HWND hDlg, UINT msg, WPARAM w, LPARAM l) {
   HANDLE          hFind;
   WIN32_FIND_DATA fd;
   char            ch[1024], path[1024];
   UINT            nID=0, n=0;
   ZFXCOLOR        clrW = { 1.0f, 1.0f, 1.0f, 1.0f };
   ZFXCOLOR        clrB = { 0.0f, 0.0f, 0.0f, 1.0f };

   HWND hList  = GetDlgItem(hDlg, IDC_LIST);
   HWND hCombo = CApplication::m_G.hGUI[CSK_LIST];

   ZFXSkinManager *pSM = g_MyApp.GetSkinMan();

   switch (msg) {
      // render if user drags the dialog
      case WM_SIZING: case WM_MOVING: {
         g_MyApp.InvalidateAllViews();
         g_MyApp.RenderTick();
         return TRUE;
         }

      case WM_INITDIALOG: {
         SendMessage(hList, LB_RESETCONTENT, 0, 0);
         hFind = FindFirstFile("data/textures/*.bmp", &fd);
         
         if (hFind != INVALID_HANDLE_VALUE) {
            if ( cbhContainsString(hCombo, fd.cFileName) == -1 )
               SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)fd.cFileName);
            
            while (FindNextFile(hFind, &fd)) {
               if ( cbhContainsString(hCombo, fd.cFileName) == -1)
                  SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)fd.cFileName);
               }
            FindClose(hFind);
            SendMessage(hList, LB_SETCURSEL, 0, 0);
            }

         return TRUE;
         } break;

      case WM_COMMAND: {
         switch (LOWORD(w)) {

            case IDC_LIST: {
               return TRUE;
               }

            case IDOK: {
               n = SendMessage(hList, LB_GETCURSEL, 0, 0);
               SendMessage(hList, LB_GETTEXT, (WPARAM)n, (LPARAM)(LPCTSTR)ch);
               strcpy(path, "data/textures/");
               strcat(path, ch);
               
               if (SUCCEEDED(pSM->AddSkin(&clrW, &clrW, &clrB, 
                                          &clrW, 1.0f, &nID))) {
                  if (SUCCEEDED(pSM->AddTexture(nID, path, false, 
                                                1.0f, NULL, 0))) {
                     cbhAddItem(hCombo, ch, NULL, true);
                     }
                  }
               EndDialog(hDlg, 1);
               g_MyApp.InvalidateAllViews();
               return TRUE;
               }

            case IDCANCEL: {
               EndDialog(hDlg, 0);
               g_MyApp.InvalidateAllViews();
               return TRUE;
               }
            }
         } break;
      }
   return 0;
   } // new sector 
/*----------------------------------------------------------------*/


// edit skin box callback
BOOL CALLBACK CB_EditSkinBox(HWND hDlg, UINT msg, WPARAM w, LPARAM l) {
   HANDLE          hFind;
   WIN32_FIND_DATA fd;

   HWND hCombo  = CApplication::m_G.hGUI[CSK_LIST];
   HWND hList   = GetDlgItem(hDlg, IDC_LIST);
   HWND hTex    = GetDlgItem(hDlg, IDC_TEXTURE);
   HWND hDet    = GetDlgItem(hDlg, IDC_DETAIL);
   HWND hBmp    = GetDlgItem(hDlg, IDC_BUMP);
   HWND hAsDet  = GetDlgItem(hDlg, IDC_ASDETAIL);
   HWND hAsBmp  = GetDlgItem(hDlg, IDC_ASBUMP);
   ZFXSKIN Skin;
   UINT n=0;
   char ch[1024], path[1024];;

   ZFXSkinManager *pSM = g_MyApp.GetSkinMan();

   switch (msg) {
      // render if user drags the dialog
      case WM_SIZING: case WM_MOVING: {
         g_MyApp.InvalidateAllViews();
         g_MyApp.RenderTick();
         return TRUE;
         }

      case WM_INITDIALOG: {
         // fill in available textures to listbox
         SendMessage(hList, LB_RESETCONTENT, 0, 0);
         hFind = FindFirstFile("data/textures/*.bmp", &fd);
         
         if (hFind != INVALID_HANDLE_VALUE) {
            SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)fd.cFileName);
            
            while (FindNextFile(hFind, &fd)) {
               SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)fd.cFileName);
               }
            FindClose(hFind);
            SendMessage(hList, LB_SETCURSEL, 0, 0);
            }

         // fill in currently used textures
         n = cbhGetSelectedIndex(hCombo);
         Skin = pSM->GetSkin(n);
         strcpy(ch, pSM->GetTextureName(Skin.nTexture[0],0,0,0));
         SetWindowText(hTex, ch);

         // is there a detail map attached?
         strcpy(ch, "Current Detail: ");
         if (pSM->GetTextureName(Skin.nTexture[1],0,0,0))
            strcat(ch, pSM->GetTextureName(Skin.nTexture[1],0,0,0));
         else 
            strcat(ch, "none");
         SetWindowText(hDet, ch);
         
         // is there a bump map attached?
         strcpy(ch, "Current Bump: ");
         if (pSM->GetTextureName(Skin.nTexture[2],0,0,0))
            strcat(ch, pSM->GetTextureName(Skin.nTexture[2],0,0,0));
         else 
            strcat(ch, "none");
         SetWindowText(hBmp, ch);
         return TRUE;
         } break;

      case WM_COMMAND: {
         switch (LOWORD(w)) {

            case IDC_LIST: {
               return TRUE;
               }

            case IDC_CLEAR: {
               n = cbhGetSelectedIndex(hCombo);
               pSM->ExchangeTexture(n, 1, NULL, false, 0.0f, NULL, 0);
               pSM->ExchangeTexture(n, 2, NULL, false, 0.0f, NULL, 0);
               EndDialog(hDlg, 1);
               g_MyApp.InvalidateAllViews();
               return TRUE;
               }

            case IDC_ASDETAIL: case IDC_ASBUMP: {
               n = SendMessage(hList, LB_GETCURSEL, 0, 0);
               SendMessage(hList, LB_GETTEXT, (WPARAM)n, (LPARAM)(LPCTSTR)ch);
               n = cbhGetSelectedIndex(hCombo);

               // put the correct path
               strcpy(path, "data/textures/");
               strcat(path, ch);

               if (LOWORD(w)==IDC_ASDETAIL)
                  pSM->ExchangeTexture(n, 1, path, false, 0.0f, NULL, 0);
               else
                  pSM->ExchangeTexture(n, 2, path, false, 0.0f, NULL, 0);

               EndDialog(hDlg, 1);
               g_MyApp.InvalidateAllViews();
               return TRUE;
               }

            case IDCANCEL: {
               EndDialog(hDlg, 0);
               g_MyApp.InvalidateAllViews();
               return TRUE;
               }
            }
         } break;
      }
   return 0;
   } // edit skin 
/*----------------------------------------------------------------*/


// edit material properties
BOOL CALLBACK CB_EditMaterialBox(HWND hDlg, UINT msg, WPARAM w, LPARAM l) {

   HWND hCombo  = CApplication::m_G.hGUI[CSK_LIST];

   HWND hAmb_R  = GetDlgItem(hDlg, IDC_AMB_R);
   HWND hAmb_G  = GetDlgItem(hDlg, IDC_AMB_G);
   HWND hAmb_B  = GetDlgItem(hDlg, IDC_AMB_B);
   HWND hAmb_A  = GetDlgItem(hDlg, IDC_AMB_A);
   HWND hDif_R  = GetDlgItem(hDlg, IDC_DIF_R);
   HWND hDif_G  = GetDlgItem(hDlg, IDC_DIF_G);
   HWND hDif_B  = GetDlgItem(hDlg, IDC_DIF_B);
   HWND hDif_A  = GetDlgItem(hDlg, IDC_DIF_A);
   HWND hEmi_R  = GetDlgItem(hDlg, IDC_EMI_R);
   HWND hEmi_G  = GetDlgItem(hDlg, IDC_EMI_G);
   HWND hEmi_B  = GetDlgItem(hDlg, IDC_EMI_B);
   HWND hEmi_A  = GetDlgItem(hDlg, IDC_EMI_A);
   HWND hSpe_R  = GetDlgItem(hDlg, IDC_SPE_R);
   HWND hSpe_G  = GetDlgItem(hDlg, IDC_SPE_G);
   HWND hSpe_B  = GetDlgItem(hDlg, IDC_SPE_B);
   HWND hSpe_A  = GetDlgItem(hDlg, IDC_SPE_A);
   HWND hSpe_P  = GetDlgItem(hDlg, IDC_SPE_P);
   

   ZFXSKIN Skin;
   ZFXMATERIAL Mat;
   UINT n=0;
   char ch[256];

   ZFXSkinManager *pSM = g_MyApp.GetSkinMan();

   switch (msg) {
      // render if user drags the dialog
      case WM_SIZING: case WM_MOVING: {
         g_MyApp.InvalidateAllViews();
         g_MyApp.RenderTick();
         return TRUE;
         }

      case WM_INITDIALOG: {
         // get currently selected skin
         n = cbhGetSelectedIndex(hCombo);
         Skin = pSM->GetSkin(n);
         Mat  = pSM->GetMaterial( Skin.nMaterial );

         // fill in current settings
         sprintf(ch, "%f", Mat.cAmbient.fR);
         SetWindowText(hAmb_R, ch);
         sprintf(ch, "%f", Mat.cAmbient.fG);
         SetWindowText(hAmb_G, ch);
         sprintf(ch, "%f", Mat.cAmbient.fB);
         SetWindowText(hAmb_B, ch);
         sprintf(ch, "%f", Mat.cAmbient.fA);
         SetWindowText(hAmb_A, ch);
         // fill in current settings
         sprintf(ch, "%f", Mat.cDiffuse.fR);
         SetWindowText(hDif_R, ch);
         sprintf(ch, "%f", Mat.cDiffuse.fG);
         SetWindowText(hDif_G, ch);
         sprintf(ch, "%f", Mat.cDiffuse.fB);
         SetWindowText(hDif_B, ch);
         sprintf(ch, "%f", Mat.cDiffuse.fA);
         SetWindowText(hDif_A, ch);
         // fill in current settings
         sprintf(ch, "%f", Mat.cEmissive.fR);
         SetWindowText(hEmi_R, ch);
         sprintf(ch, "%f", Mat.cEmissive.fG);
         SetWindowText(hEmi_G, ch);
         sprintf(ch, "%f", Mat.cEmissive.fB);
         SetWindowText(hEmi_B, ch);
         sprintf(ch, "%f", Mat.cEmissive.fA);
         SetWindowText(hEmi_A, ch);
         // fill in current settings
         sprintf(ch, "%f", Mat.cSpecular.fR);
         SetWindowText(hSpe_R, ch);
         sprintf(ch, "%f", Mat.cSpecular.fG);
         SetWindowText(hSpe_G, ch);
         sprintf(ch, "%f", Mat.cSpecular.fB);
         SetWindowText(hSpe_B, ch);
         sprintf(ch, "%f", Mat.cSpecular.fA);
         SetWindowText(hSpe_A, ch);
         sprintf(ch, "%f", Mat.fPower);
         SetWindowText(hSpe_P, ch);
         return TRUE;
         } break;

      case WM_COMMAND: {
         switch (LOWORD(w)) {
            case IDOK: {
               GetWindowText(hAmb_R, ch, 16);
               sscanf(ch, "%f", &Mat.cAmbient.fR);
               GetWindowText(hAmb_G, ch, 16);
               sscanf(ch, "%f", &Mat.cAmbient.fG);
               GetWindowText(hAmb_B, ch, 16);
               sscanf(ch, "%f", &Mat.cAmbient.fB);
               GetWindowText(hAmb_A, ch, 16);   
               sscanf(ch, "%f", &Mat.cAmbient.fA);

               GetWindowText(hDif_R, ch, 16);
               sscanf(ch, "%f", &Mat.cDiffuse.fR);
               GetWindowText(hDif_G, ch, 16);
               sscanf(ch, "%f", &Mat.cDiffuse.fG);
               GetWindowText(hDif_B, ch, 16);
               sscanf(ch, "%f", &Mat.cDiffuse.fB);
               GetWindowText(hDif_A, ch, 16);   
               sscanf(ch, "%f", &Mat.cDiffuse.fA);

               GetWindowText(hEmi_R, ch, 16);
               sscanf(ch, "%f", &Mat.cEmissive.fR);
               GetWindowText(hEmi_G, ch, 16);
               sscanf(ch, "%f", &Mat.cEmissive.fG);
               GetWindowText(hEmi_B, ch, 16);
               sscanf(ch, "%f", &Mat.cEmissive.fB);
               GetWindowText(hEmi_A, ch, 16);   
               sscanf(ch, "%f", &Mat.cEmissive.fA);

               GetWindowText(hSpe_R, ch, 16);
               sscanf(ch, "%f", &Mat.cSpecular.fR);
               GetWindowText(hSpe_G, ch, 16);
               sscanf(ch, "%f", &Mat.cSpecular.fG);
               GetWindowText(hSpe_B, ch, 16);
               sscanf(ch, "%f", &Mat.cSpecular.fB);
               GetWindowText(hSpe_A, ch, 16);   
               sscanf(ch, "%f", &Mat.cSpecular.fA);

               GetWindowText(hSpe_P, ch, 16);   
               sscanf(ch, "%f", &Mat.fPower);

               n = cbhGetSelectedIndex(hCombo);
               pSM->ExchangeMaterial(n, &Mat.cAmbient,
                                     &Mat.cDiffuse, 
                                     &Mat.cEmissive,
                                     &Mat.cSpecular, 
                                     Mat.fPower);
               EndDialog(hDlg, 1);
               g_MyApp.InvalidateAllViews();
               return TRUE;
               }
            case IDCANCEL: {
               EndDialog(hDlg, 0);
               g_MyApp.InvalidateAllViews();
               return TRUE;
               }
            }
         } break;
      }
   return 0;
   } // eduit material 
/*----------------------------------------------------------------*/


// edit light properties
BOOL CALLBACK CB_EditLightBox(HWND hDlg, UINT msg, WPARAM w, LPARAM l) {
   ZFXVector vcPos;
   ZFXCOLOR  clr = { 1.0f, 1.0f, 1.0f, 1.0f };
   float     fR=0.0f;
   CLight   *pLight=NULL;
   char      buffer[2048];
   bool      b=false;

   HWND hP    = GetDlgItem(hDlg, IDC_POS);
   HWND hR    = GetDlgItem(hDlg, IDC_R);
   HWND hG    = GetDlgItem(hDlg, IDC_G);
   HWND hB    = GetDlgItem(hDlg, IDC_B);
   HWND hRad  = GetDlgItem(hDlg, IDC_RAD);
   HWND hF    = GetDlgItem(hDlg, IDC_FLICKER);
   HWND hS    = GetDlgItem(hDlg, IDC_SHADOW);

   // get the light, application has to make sure that the first
   // element in the selection buffer is a CLight object when this
   // dialog is called
   pLight = (LPLIGHT)CApplication::m_G.SelBuf.GetHeadPointer();

   switch (msg) {
      // render if user drags the dialog
      case WM_SIZING: case WM_MOVING: {
         g_MyApp.InvalidateAllViews();
         g_MyApp.RenderTick();
         return TRUE;
         }

      case WM_INITDIALOG: {
         vcPos = pLight->GetPosition();
         clr   = pLight->GetColor();
         fR    = pLight->GetRadius();

         sprintf(buffer, "(%f,%f,%f)", vcPos.x, vcPos.y, vcPos.z);
         SetWindowText(hP, buffer);
         sprintf(buffer, "%f", clr.fR);
         SetWindowText(hR, buffer);
         sprintf(buffer, "%f", clr.fG);
         SetWindowText(hG, buffer);
         sprintf(buffer, "%f", clr.fB);
         SetWindowText(hB, buffer);
         sprintf(buffer, "%f", fR);
         SetWindowText(hRad, buffer);
         if (pLight->IsFlickering())
            SendMessage(hF,BM_SETCHECK,(WPARAM)BST_CHECKED,0);
         
         if (pLight->IsShadowCasting())
            SendMessage(hS,BM_SETCHECK,(WPARAM)BST_CHECKED,0);

         return TRUE;
         } break;

      case WM_COMMAND: {
         switch (LOWORD(w)) {
            case IDOK:
               // get new light color
               GetWindowText(hR, buffer, 256);
               sscanf(buffer,"%f",&clr.fR);
               GetWindowText(hG, buffer, 256);
               sscanf(buffer,"%f",&clr.fG);
               GetWindowText(hB, buffer, 256);
               sscanf(buffer,"%f",&clr.fB);
               pLight->SetColor(clr);

               // get new radius
               GetWindowText(hRad, buffer, 256);
               sscanf(buffer, "%f", &fR);
               pLight->SetRadius(fR);
               
               b = (BST_CHECKED==SendMessage(hF,BM_GETCHECK,0,0));
               pLight->SetFlickering(b);

               b = (BST_CHECKED==SendMessage(hS,BM_GETCHECK,0,0));
               pLight->SetShadowCasting(b);

               EndDialog(hDlg, 1);
               g_MyApp.InvalidateAllViews();
               return TRUE;

            case IDCANCEL:
               EndDialog(hDlg, 0);
               g_MyApp.InvalidateAllViews();
               return TRUE;
            }
         } break;
      }
   return 0;
   } // CB_EditLightBox
/*----------------------------------------------------------------*/


// select which type of entity to insert
BOOL CALLBACK CB_EntityType(HWND hDlg, UINT msg, WPARAM w, LPARAM l) {
   HWND hSDoor = GetDlgItem(hDlg, IDC_SLIDEDOOR);

   switch (msg) {
      // render if user drags the dialog
      case WM_SIZING: case WM_MOVING: {
         g_MyApp.InvalidateAllViews();
         g_MyApp.RenderTick();
         return TRUE;
         }

      case WM_INITDIALOG: {
         SendMessage(hSDoor,BM_SETCHECK,(WPARAM)BST_CHECKED,0);
         return TRUE;
         } break;

      case WM_COMMAND: {
         switch (LOWORD(w)) {
            case IDOK:
               EndDialog(hDlg, ENT_DOOR);
               g_MyApp.InvalidateAllViews();
               return TRUE;
            case IDCANCEL:
               EndDialog(hDlg, -1);
               g_MyApp.InvalidateAllViews();
               return TRUE;
            }
         } break;
      }
   return 0;
   } // CB_EntityType
/*----------------------------------------------------------------*/


// select which type of entity to insert
BOOL CALLBACK CB_EditEntDoor(HWND hDlg, UINT msg, WPARAM w, LPARAM l) {
   ENTITYDOOR Data;
   HWND hDist  = GetDlgItem(hDlg, IDC_DIST);
   HWND hPause = GetDlgItem(hDlg, IDC_PAUSE);
   HWND hX     = GetDlgItem(hDlg, IDC_XAXIS);
   HWND hY     = GetDlgItem(hDlg, IDC_YAXIS);
   HWND hZ     = GetDlgItem(hDlg, IDC_ZAXIS);
   char buffer[256];

   // get the concerned entity
   LPENTITY pEntity = (LPENTITY)g_MyApp.m_G.SelBuf.GetHeadPointer();
 
   // get its data, we know that is a ENTITYDOOR struct
   memcpy(&Data, pEntity->GetData(), sizeof(ENTITYDOOR));

   switch (msg) {
      // render if user drags the dialog
      case WM_SIZING: case WM_MOVING: {
         g_MyApp.InvalidateAllViews();
         g_MyApp.RenderTick();
         return TRUE;
         }

      case WM_INITDIALOG: {
         sprintf(buffer, "%f", Data.fDist);
         SetWindowText(hDist, buffer);
         sprintf(buffer, "%f", Data.fPause);
         SetWindowText(hPause, buffer);

         if (Data.Axis == X_AXIS)
            SendMessage(hX,BM_SETCHECK,(WPARAM)BST_CHECKED,0);
         else if (Data.Axis == Y_AXIS)
            SendMessage(hY,BM_SETCHECK,(WPARAM)BST_CHECKED,0);
         else
            SendMessage(hZ,BM_SETCHECK,(WPARAM)BST_CHECKED,0);
         
         return TRUE;
         } break;

      case WM_COMMAND: {
         switch (LOWORD(w)) {
            case IDOK:
               GetWindowText(hDist, buffer, 256);
               sscanf(buffer,"%f",&Data.fDist);
               GetWindowText(hPause, buffer, 256);
               sscanf(buffer,"%f",&Data.fPause);

               if (BST_CHECKED==SendMessage(hX,BM_GETCHECK,0,0))
                  Data.Axis = X_AXIS;
               else if (BST_CHECKED==SendMessage(hY,BM_GETCHECK,0,0))
                  Data.Axis = Y_AXIS;
               else
                  Data.Axis = Z_AXIS;

               pEntity->SetData(Data);
               EndDialog(hDlg, 1);
               g_MyApp.InvalidateAllViews();
               return TRUE;
            case IDCANCEL:
               EndDialog(hDlg, 0);
               g_MyApp.InvalidateAllViews();
               return TRUE;
            }
         } break;
      }
   return 0;
   } // CB_EditEntDoor
/*----------------------------------------------------------------*/


// rotate a selected object
BOOL CALLBACK CB_RotateObject(HWND hDlg, UINT msg, WPARAM w, LPARAM l) {
   HWND hDEG = GetDlgItem(hDlg, IDC_DEG);
   HWND hX   = GetDlgItem(hDlg, IDC_X);
   HWND hY   = GetDlgItem(hDlg, IDC_Y);
   HWND hZ   = GetDlgItem(hDlg, IDC_Z);

   char buffer[256];

   float   fRAD = 0.0f;
   float   fDEG = 0.0f;
   ZFXAXIS Axis = Y_AXIS;

   switch (msg) {
      // render if user drags the dialog
      case WM_SIZING: case WM_MOVING: {
         g_MyApp.InvalidateAllViews();
         g_MyApp.RenderTick();
         return TRUE;
         }

      case WM_INITDIALOG: {
         SetWindowText(hDEG, "90");
         SendMessage(hY,BM_SETCHECK,(WPARAM)BST_CHECKED,0);
         return TRUE;
         } break;

      case WM_COMMAND: {
         switch (LOWORD(w)) {
            case IDOK:
               GetWindowText(hDEG, buffer, 256);
               sscanf(buffer,"%f",&fDEG);

               if (SendMessage(hX,BM_GETCHECK,0,0)==BST_CHECKED)
                  Axis = X_AXIS;
               else if (SendMessage(hZ,BM_GETCHECK,0,0)==BST_CHECKED)
                  Axis = Z_AXIS;

               fRAD = ZFXPI / 180.0f * fDEG;
               g_MyApp.m_G.SelBuf.Rotate(Axis, fRAD);
               EndDialog(hDlg, 0);
               g_MyApp.InvalidateAllViews();
               return TRUE;

            case IDCANCEL:
               EndDialog(hDlg, 0);
               g_MyApp.InvalidateAllViews();
               return TRUE;
            }
         } break;
      }
   return 0;
   } // CB_RotateObject
/*----------------------------------------------------------------*/