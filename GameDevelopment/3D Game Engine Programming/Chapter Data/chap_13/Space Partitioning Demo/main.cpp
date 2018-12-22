// FILE: main.cpp

#define WIN32_MEAN_AND_LEAN

#include "ZFXRenderer.h"  // our render interface
#include "ZFX.h"          // return values and stuff
#include "main.h"         // prototypes and stuff
#include "ZFXGeneral.h"   // movement controller


//include our library
#pragma comment(lib, "ZFXRenderer.lib")
#pragma comment(lib, "zfx3d.lib")
#pragma comment(lib, "ZFXGeneral.lib")
 

// windows stuff
HWND      g_hWnd  = NULL;
HINSTANCE g_hInst = NULL;
TCHAR     g_szAppClass[] = TEXT("BSP Tree Demo");

// application stuff
BOOL g_bIsActive = FALSE;
bool g_bWired    = false;
bool g_bDone     = false;
bool g_bFtB      = true;
bool g_bBsp      = true;

FILE *pLog=NULL;

UINT g_nFontID=0;

// renderer object
LPZFXRENDERER     g_pRenderer = NULL;
LPZFXRENDERDEVICE g_pDevice   = NULL;

// movement controller / timer
ZFXMCEgo *g_pMCEgo = NULL;
ZFXTimer *g_pTimer = NULL;

// bsp tree and octree
ZFXBspTree *g_pBSPTree = NULL;
ZFXOctree  *g_pOctree  = NULL;


/**
 * WinMain function to get the thing started.
 */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, 
                   LPSTR lpCmdLine, int nCmdShow) {
   WNDCLASSEX	wndclass;
   HRESULT     hr;
   HWND		   hWnd;
   MSG			msg;

   // Set up window attributes
   wndclass.cbSize         = sizeof(wndclass);
   wndclass.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
   wndclass.lpfnWndProc    = MsgProc;
   wndclass.cbClsExtra     = 0;
   wndclass.cbWndExtra     = 0;
   wndclass.hInstance      = hInst;
   wndclass.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
   wndclass.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wndclass.hbrBackground  = (HBRUSH)(COLOR_WINDOW);
   wndclass.lpszMenuName   = NULL;
   wndclass.lpszClassName  = g_szAppClass;
   wndclass.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);
    
   if(RegisterClassEx(&wndclass) == 0)
      return 0;

   // create window
   if (!(hWnd = CreateWindowEx(NULL, g_szAppClass,
	   		                   "ZFXEngine v2.0 - Demo Application",
                               WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
                               WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE,
			 	                   GetSystemMetrics(SM_CXSCREEN)/2 -250,
                               GetSystemMetrics(SM_CYSCREEN)/2 -187,
				                   500, 375, NULL, NULL, hInst, NULL)))
      return 0;
   
   // dont show everything off yet
   ShowWindow(hWnd, SW_HIDE);
     
   g_hWnd  = hWnd;
   g_hInst = hInst;

   pLog = fopen("log_main.txt", "w");

   // init 3dmath lib
   ZFX3DInitCPU();

   // try to start the engine
   if (FAILED( hr = ProgramStartup("Direct3D") )) {
      if (hr == ZFX_FAIL)
         fprintf(pLog, "error: ProgramStartup() failed\n");
      else if (hr == ZFX_CANCELED)
         fprintf(pLog, "error: ProgramStartup() canceled by user \n");
      else if (hr == ZFX_NOTCOMPATIBLE) {
         MessageBox(g_hWnd, 
                    "Sorry, no suitable graphics hardware found.", 
                    "ZFX Engine Report", 
                    MB_OK | MB_ICONERROR);
         fprintf(pLog, "error: no compatible graphics adapter \n");
         }
      fflush(pLog);
      g_bDone = true;
      }
   // everything went smooth
   else {
      ShowWindow(hWnd, SW_SHOW);
      
      if ( !LoadLevel("testlevel.pbp") ) 
         g_bDone = true;
      }

   // main loop
   while (!g_bDone) {
      // handle messages
      while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
         {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
         } 

      if (g_bIsActive) ProgramTick();
      }
   
   // cleanup stuff
   ProgramCleanup();

   UnregisterClass(g_szAppClass, hInst);
   return (int)msg.wParam;
   } // WinMain
/*----------------------------------------------------------------*/


/**
 * MsgProc to deal with incoming windows messages. Handle input from
 * user here cause we are too lazy to use our own lib as we only need
 * plain simple steering keys from keyboard.
 */
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
   switch(msg) {
      // our app has the focus
      case WM_ACTIVATE: {
         g_bIsActive = (BOOL)wParam;
         } break;
         
      // key was pressed
      case WM_KEYDOWN: {
         switch (wParam) {
            case VK_ESCAPE: {
               g_bDone = true;
               PostMessage(hWnd, WM_CLOSE, 0, 0);
               return 0;
               } break;
            // switch wireframe mode
            case 'Q': case 'q':
               g_bWired = !g_bWired;
               return 0; break;
            // switch rendering order
            case 'E': case 'e':
               // cylce: BSP FtB, BSP BtF, ZFXOctree
               if ( g_bFtB ) {
                  g_bFtB = false;
                  }
               else {
                  if (g_bBsp ) g_bBsp = false;
                  else g_bBsp = true;
                  }

               // while rendering back to front we dont need
               // DepthBuffer at all, that's BSP treeing baby
               if (!g_bFtB && g_bBsp)
                  g_pDevice->SetDepthBufferMode(RS_DEPTH_NONE);
               else
                  g_pDevice->SetDepthBufferMode(RS_DEPTH_READWRITE);

               return 0; break;
            // rotate left and right (on y axis)
            case VK_RIGHT:
               g_pMCEgo->SetRotationSpeedY(-1.5f);
               return 0; break;
            case VK_LEFT:
               g_pMCEgo->SetRotationSpeedY(1.5f);
               return 0; break;
            // rotate up and down (on X axis)
            case VK_UP:
               g_pMCEgo->SetRotationSpeedX(1.0f);
               return 0; break;
            case VK_DOWN:
               g_pMCEgo->SetRotationSpeedX(-1.0f);
               return 0; break;
            // move fowards/backwards
            case 'w': case 'W':
               g_pMCEgo->SetSpeed(2.0f);
               return 0; break;
            case 's': case 'S':
               g_pMCEgo->SetSpeed(-2.0f);
               return 0; break;
            // strafe sidewards
            case 'a': case 'A':
               g_pMCEgo->SetSlideSpeed(-1.2f);
               return 0; break;
            case 'd': case 'D':
               g_pMCEgo->SetSlideSpeed(1.2f);
               return 0; break;
            }
         } break;

      // we are ordered to sepuku
      case WM_DESTROY: {
         g_bDone = true;
         PostQuitMessage(0);
         return 1;
         } break;

      default: break;
      }
   
   return DefWindowProc(hWnd, msg, wParam, lParam);
   }
/*----------------------------------------------------------------*/
 

/**
 * Create a render device and stuff. 
 */
HRESULT ProgramStartup(char *chAPI) {
   // no opengl render device yet...
   if (strcmp(chAPI, "OpenGL")==0) return S_OK;

   // create a render objekt
   g_pRenderer = new ZFXRenderer(g_hInst);
   
   // create a device for the chosen api
   if (FAILED( g_pRenderer->CreateDevice(chAPI) )) return E_FAIL;
   
   // get a pointer on that device
   g_pDevice = g_pRenderer->GetDevice();
   if (g_pDevice == NULL) return E_FAIL;

   // init render device
   if (FAILED( g_pDevice->Init(g_hWnd, NULL, 0, 16, 8, true) )) {
      fprintf(pLog, "error: Init() failed in ProgramStartup()\n");
      return E_FAIL;
      }

   g_pDevice->UseWindow(0);

   g_pDevice->InitStage(0.8f, NULL, 0);

   if (FAILED( g_pDevice->CreateFont("Arial", 0, false, false,
                                     false, 10, &g_nFontID) )) {
      fprintf(pLog, "error: ZFXRenderDevice::CreateFont failed\n");
      return ZFX_FAIL;
      }

   // set first viewport (whole window)
   g_pDevice->SetMode(EMD_PERSPECTIVE, 0);
   g_pDevice->SetClearColor(0.7f,0.7f,1.0f);

   // load default skin
   ZFXCOLOR clr  = { 1.0f, 1.0f, 1.0f, 1.0f };
   ZFXCOLOR clrW = { 0.0f, 0.0f, 0.0f, 1.0f };
   UINT nDummy=0;
   g_pDevice->GetSkinManager()->AddSkin(&clr,&clr,&clrW,&clr,0.0f,&nDummy);
   g_pDevice->GetSkinManager()->AddTexture(0,"texture.bmp",false,0,0,0);

   // init movement controller / timer
   g_pMCEgo = new ZFXMCEgo();
   g_pTimer = new ZFXTimer(12,0);

   return ZFX_OK;
   } // ProgramStartup
/*----------------------------------------------------------------*/


/**
 * Release the render device and stuff.
 */
HRESULT ProgramCleanup(void) {

   if (g_pRenderer) {
      delete g_pRenderer;
      g_pRenderer = NULL;
      }

   if (g_pBSPTree) {
      delete g_pBSPTree;
      g_pBSPTree = NULL;
      }

   if (g_pOctree) {
      delete g_pOctree;
      g_pOctree = NULL;
      }

   if (g_pMCEgo) {
      delete g_pMCEgo;
      g_pMCEgo = NULL;
      }

   if (g_pTimer) {
      delete g_pTimer;
      g_pTimer = NULL;
      }

   if (pLog) fclose(pLog);

   return S_OK;
   } // ProgramCleanup
/*----------------------------------------------------------------*/


/**
 * Do one frame.
 */
HRESULT ProgramTick(void) {
   ZFXPlane    Frustum[6];
   ZFXPolylist List, AabbList;
   HRESULT     hr = ZFX_FAIL;
   ZFXMatrix   mWorld;
   char        chText[2048];

   static ZFXCOLOR clrR = {1.0f, 0.4f, 0.4f};
   static ZFXCOLOR clrG = {0.0f, 0.7f, 0.0f};

   mWorld.Identity();

   // update and reset controllers
   g_pTimer->Update();


   // get current position and apply movement
   ZFXVector vcOld = g_pMCEgo->GetPos(), vcNew(0,0,0);
   g_pMCEgo->Update( g_pTimer->GetElapsed() );
   g_pMCEgo->SetSpeed(0.0f);
   g_pMCEgo->SetSlideSpeed(0.0f);
   g_pMCEgo->SetRotationSpeedX(0.0f);
   g_pMCEgo->SetRotationSpeedY(0.0f);

   // get new position, build ray ahead
   vcNew = g_pMCEgo->GetPos();
   ZFXRay Ray; Ray.Set(vcNew, g_pMCEgo->GetDir());

   // if collision occured reset to old position
   if ( g_bBsp ) {
      if (g_pBSPTree->TestCollision(Ray, 0.5f, NULL, NULL))
         g_pMCEgo->SetPos(vcOld);
      }
   else {
      if (g_pOctree->TestCollision(Ray, 0.5f, NULL))
         g_pMCEgo->SetPos(vcOld);
      }



   // update movement controller position
   g_pDevice->SetView3D(g_pMCEgo->GetRight(),
                        g_pMCEgo->GetUp(),
                        g_pMCEgo->GetDir(),
                        g_pMCEgo->GetPos());

   // get current view frustum
   g_pDevice->GetFrustrum( Frustum );

   // traverse the tree
   if ( g_bBsp ) {
      if (g_bFtB)
         g_pBSPTree->TraverseFtB( &List, g_pMCEgo->GetPos(), Frustum );
      else
         g_pBSPTree->TraverseBtF( &List, g_pMCEgo->GetPos(), Frustum );
      }
   else {
      g_pOctree->Traverse( &List, &AabbList, Frustum );
      }

   // clear buffers and start scene
   g_pDevice->BeginRendering(true,true,true);

      mWorld.Translate(0.0f, 0.0f, 0.0f);
      g_pDevice->SetWorldTransform(&mWorld);
      
      
      if (g_bBsp) RenderPolylist( List, g_bWired, &clrR );
      else {
         RenderPolylist( List, g_bWired, &clrG );
         if (g_bWired) RenderPolylist(AabbList, g_bWired, &clrR);
         }

   g_pDevice->DrawText(g_nFontID, 10, 10, 255, 255, 0, 
                       "%4.2f fps, %d Polygons rendered", 
                       g_pTimer->GetFPS(), List.GetNum());
   if ( g_bBsp ) {
      if (g_bFtB)
         strcpy(chText, "BSP Tree, rendered front to back\0");
      else
         strcpy(chText, "BSP Tree, rendered back to front without depth buffer\0");
      }
   else {
      strcpy(chText, "ZFXOctree used for rendering\0");
      }

   g_pDevice->DrawText(g_nFontID, 10, 30, 255, 255, 0, chText);
   g_pDevice->DrawText(g_nFontID, 10, 50, 255, 255, 0, 
                       "Press W for wireframe, E to cycle BSP modes / octree");
   g_pDevice->EndRendering();
   return hr;
   } // Tick
/*----------------------------------------------------------------*/


/**
 * Load a given level from file
 */
bool LoadLevel(const char *chFile) {
   ZFXPolygon *pList=NULL, Poly;
   char        buffer[2048];
   UINT        Num=0, n=0;

   g_pDevice->BeginRendering(true,true,true);
   g_pDevice->DrawText(g_nFontID,10,10,255,255,0,
                       "loading level data from file \"%s\"", 
                       chFile);
   g_pDevice->EndRendering();

   // open level file
   FILE *pFile = fopen(chFile, "rb");
   if (!pFile) return false;

   // read number of polygons to come
   fread(&Num, sizeof(UINT), 1, pFile);
   if (Num == 0) return false;

   // allocate memory
   pList = new ZFXPolygon[Num];

   // load all polygons
   for (UINT i=0; i<Num; i++) {

      // read name of base texture
      fread(&n, sizeof(UINT), 1, pFile);
      fread(buffer, n, 1, pFile);
      buffer[n] = '\0';

      // load polygon and copy to list
      LoadPolygon(pFile, &Poly);
      pList[i].CopyOf( Poly );
      } // for


   g_pDevice->BeginRendering(true,true,true);
   g_pDevice->DrawText(g_nFontID,10,10,255,255,0,
                       "building BSP tree from file \"%s\", " \
                       "please stand by...", chFile);
   g_pDevice->EndRendering();


   // create the BSP tree finally
   g_pBSPTree = new ZFXBspTree;
   g_pBSPTree->BuildTree(pList, Num);


   g_pDevice->BeginRendering(true,true,true);
   g_pDevice->DrawText(g_nFontID,10,10,255,255,0,
                       "building octree from file \"%s\", " \
                       "please stand by...", chFile);
   g_pDevice->EndRendering();

   // create the octree finally
   g_pOctree = new ZFXOctree;
   g_pOctree->BuildTree(pList, Num);

   fclose(pFile);

   if (pList) { delete [] pList; }
   return true;
   } // LoadLevel
/*----------------------------------------------------------------*/


/**
 * Loads a single polygon from the open file.
 */
void LoadPolygon(FILE *pFile, ZFXPolygon *pPoly) {
   UINT    NumVerts=0,
           NumIndis=0,
           ID=0;
   bool    bln=false;
   ZFXAabb aabb;
   float   f[4] = { 0.0f,0.0f,0.0f,0.0f };

   // read counters
   fread(&NumVerts, sizeof(UINT),    1, pFile);
   fread(&NumIndis, sizeof(UINT),    1, pFile);
   fread(&ID,       sizeof(UINT),    1, pFile);
   fread(&aabb,     sizeof(ZFXAabb), 1, pFile);
   fread(&bln,      sizeof(bool),    1, pFile);
   fread(f,         sizeof(float),   4, pFile);

   // allocate memory
   VERTEX *pvVerts = new VERTEX[NumVerts];
   WORD   *pwIndis = new WORD[NumIndis];
   WORD   *pDummy  = new WORD[NumVerts*2];

   ZFXVector *pvcPoints = new ZFXVector[NumVerts];
   UINT      *pnIndis   = new UINT[NumIndis];

   if ( !pvVerts   || !pwIndis ||
        !pvcPoints || !pnIndis ) return;

   // read data
   fread(pvVerts, sizeof(VERTEX), NumVerts,   pFile);
   fread(pwIndis, sizeof(WORD),   NumIndis,   pFile);
   fread(pDummy,  sizeof(WORD),   NumVerts*2, pFile);

   // convert vertices to vectors
   for (UINT i=0; i<NumVerts; i++)
      pvcPoints[i].Set(pvVerts[i].x, 
                       pvVerts[i].y, 
                       pvVerts[i].z);

   // convert WORD to UINT
   for (UINT j=0; j<NumIndis; j++)
      pnIndis[j] = pwIndis[j];

   // set data to polygon object
   pPoly->Set( pvcPoints, NumVerts, pnIndis, NumIndis );

   // clean up
   delete pvVerts;
   delete pwIndis;
   delete pvcPoints;
   delete pnIndis;
   delete pDummy;
   } // LoadPolygon
/*----------------------------------------------------------------*/


/**
 * Render the list pure brute force. Ain't fast at all. Remember that
 * our bsp code is NOT meant to render geometry, but should be used
 * for collision detection and stuff. So this function is debug only. 
 */
void RenderPolylist(ZFXPolylist &List, bool bWired, const ZFXCOLOR *pClr) {
   ZFXPolygon *pCP = NULL;
   UINT i=0, j=0;
   ZFXAabb aabb;
   float fMaxX=0.0f, 
         fMaxY=0.0;

   ZFXPolygon *pPolys = List.GetPolylist();
   UINT Num = List.GetNum();

   VERTEX Verts[256];
   WORD   Indis[256];

   for (UINT p=0; p<Num; p++) {

      pCP = &pPolys[p];
      aabb = pCP->GetAabb();

      fMaxX = aabb.vcMax.x - aabb.vcMin.x;
      fMaxY = aabb.vcMax.y - aabb.vcMin.y;

      for (i=0; i<pCP->GetNumPoints(); i++) {
         memset(&Verts[i], 0, sizeof(VERTEX));
         Verts[i].x = pCP->m_pPoints[i].x;
         Verts[i].y = pCP->m_pPoints[i].y;
         Verts[i].z = pCP->m_pPoints[i].z;

         // calc random reproducable texture coords
         Verts[i].tu = Verts[i].x - aabb.vcMin.x / fMaxX;
         Verts[i].tv = Verts[i].y - aabb.vcMin.y / fMaxY;
         }

      // render filled triangles
      if (!bWired) {

         g_pDevice->SetBackfaceCulling(RS_CULL_CCW);
         g_pDevice->SetShadeMode(RS_SHADE_SOLID,0,NULL);

         for (j=0; j<pCP->GetNumIndis(); j++)
            Indis[j] = pCP->m_pIndis[j];

         }
      // render wireframe mode
      else {
         ZFXCOLOR clrW = {1.0f, 1.0f, 1.0f};
         g_pDevice->SetBackfaceCulling(RS_CULL_NONE);

         if (pClr)
            g_pDevice->SetShadeMode(RS_SHADE_LINES, 0, pClr);
         else 
            g_pDevice->SetShadeMode(RS_SHADE_LINES, 0, &clrW);

         for (j=0; j<pCP->GetNumPoints(); j++) {
            Indis[(j*2)] = j;
            Indis[(j*2)+1] = j+1;

            if (j==(pCP->GetNumPoints()-1)) Indis[(j*2)+1] = 0;
            }
         j = 2 * pCP->GetNumPoints();
         }
      
      g_pDevice->GetVertexManager()->Render(VID_UU, i, j, Verts, Indis, 0);
      } // for

   } // RenderPolylist
/*----------------------------------------------------------------*/

