/**************************************************
WinMain.cpp
Chapter 11 Map Inventory Control System Demo

Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)

Required libraries:
  D3D9.LIB, D3DX9.LIB, and DINPUT8.LIB
**************************************************/

#include "Core_Global.h"
#include "Frustum.h"
#include "NodeTree.h"
#include "MapICS.h"
#include "MIL.h"

class cApp : public cApplication
{
  private:
    cGraphics       m_Graphics;
    cCamera         m_Camera;
    cLight          m_Light;
    cFont           m_Font;

    cInput          m_Input;
    cInputDevice    m_Keyboard;
    cInputDevice    m_Mouse;

    cMesh           m_Mesh;
    cNodeTreeMesh   m_NodeTreeMesh;

    sItem           m_MILItems[1024];

    cMapICS         m_Items;
    cMesh           m_ItemMesh;
    cObject         m_ItemObject;

    cMesh           m_TargetMesh;
    cObject         m_TargetObject;

    float           m_XPos, m_YPos, m_ZPos;

  public:
    cApp();

    BOOL Init();
    BOOL Shutdown();
    BOOL Frame();
};

cApp::cApp()
{ 
  m_Width  = 640; 
  m_Height = 480;
  m_Style  = WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
  strcpy(m_Class, "MapICSlass");
  strcpy(m_Caption, "Map ICS Demo by Jim Adams");
}

BOOL cApp::Init()
{
  FILE *fp;
  long i;

  // Initialize the graphics device and set display mode
  m_Graphics.Init();
  m_Graphics.SetMode(GethWnd(), TRUE, TRUE);
  m_Graphics.SetPerspective(D3DX_PI/4, 1.3333f, 1.0f, 20000.0f);
  ShowMouse(TRUE);

  // Get a font to use
  m_Font.Create(&m_Graphics, "Arial", 16);

  // Enable lighting and setup light
  m_Graphics.EnableLighting(TRUE);
  m_Graphics.SetAmbientLight(48,48,48);
  m_Graphics.EnableLight(0, TRUE);
  m_Light.SetRange(1000.0f);

  // Initialize input and input devices
  m_Input.Init(GethWnd(), GethInst());
  m_Keyboard.Create(&m_Input, KEYBOARD);
  m_Mouse.Create(&m_Input, MOUSE, TRUE);  

  // Load the mesh and create an NodeTree mesh from it
  m_Mesh.Load(&m_Graphics, "..\\Data\\Map.x", "..\\Data\\");
  m_NodeTreeMesh.Create(&m_Graphics, &m_Mesh, QUADTREE);

  // Position view at origin
  m_XPos = m_YPos = m_ZPos = 0.0f;

  // Load the item file
  m_Items.Load("..\\Data\\MapItems.mi");

  // Load a generic item mesh and targeting mesh and
  // setup their objects
  m_ItemMesh.Load(&m_Graphics, "..\\Data\\Item.x", "..\\Data\\");
  m_ItemObject.Create(&m_Graphics, &m_ItemMesh);

  m_TargetMesh.Load(&m_Graphics, "..\\Data\\Target.x", "..\\Data\\");
  m_TargetObject.Create(&m_Graphics, &m_TargetMesh);

  // Load the MIL list
  for(i=0;i<1024;i++)
    ZeroMemory(&m_MILItems[i], sizeof(sItem));
  if((fp=fopen("..\\Data\\Default.mil", "rb"))!=NULL) {
    for(i=0;i<1024;i++)
      fread(&m_MILItems[i], 1, sizeof(sItem), fp);
    fclose(fp);
  }

  return TRUE;
}

BOOL cApp::Shutdown()
{
  // Save the item file
  m_Items.Save("..\\Data\\MapItems.mi");

  // Free the font
  m_Font.Free();

  // Free meshes and objects
  m_NodeTreeMesh.Free();
  m_Mesh.Free();
  m_ItemMesh.Free();
  m_ItemObject.Free();
  m_TargetMesh.Free();
  m_TargetObject.Free();

  // Shutdown input
  m_Mouse.Free();
  m_Keyboard.Free();
  m_Input.Shutdown();

  // Shutdown graphics
  m_Graphics.Shutdown();

  return TRUE;
}

BOOL cApp::Frame()
{
  static DWORD  Timer = timeGetTime();
  unsigned long Elapsed;
  float         XMove, ZMove;
  D3DXVECTOR2   vecDir;
  cFrustum      Frustum;
  cLight        Light;
  sMapItem     *Item, *ClosestItem;
  float         Closest, XDiff, ZDiff;
  float         Radius;
  float         ItemDistance;
  long ItemNum;
  char Text[256];

  // Reacquire input
  m_Mouse.Acquire(TRUE);

  // Calculate elapsed time)
  Elapsed = (timeGetTime() - Timer);
  Timer = timeGetTime();

  // Get input
  m_Keyboard.Read();
  m_Mouse.Read();

  // Process input and update everything.
  // ESC quits program
  if(m_Keyboard.GetKeyState(KEY_ESC) == TRUE)
    return FALSE;

  // Process movement
  XMove = ZMove = 0.0f;

  if(m_Keyboard.GetKeyState(KEY_UP) == TRUE) {
    XMove = (float)sin(m_Camera.GetYRotation()) * Elapsed;
    ZMove = (float)cos(m_Camera.GetYRotation()) * Elapsed;
  }
  if(m_Keyboard.GetKeyState(KEY_DOWN) == TRUE) {
    XMove = -(float)sin(m_Camera.GetYRotation()) * Elapsed;
    ZMove = -(float)cos(m_Camera.GetYRotation()) * Elapsed;
  }
  if(m_Keyboard.GetKeyState(KEY_LEFT) == TRUE) {
    XMove = (float)sin(m_Camera.GetYRotation() - 1.57f) * Elapsed;
    ZMove = (float)cos(m_Camera.GetYRotation() - 1.57f) * Elapsed;
  }
  if(m_Keyboard.GetKeyState(KEY_RIGHT) == TRUE) {
    XMove = (float)sin(m_Camera.GetYRotation() + 1.57f) * Elapsed;
    ZMove = (float)cos(m_Camera.GetYRotation() + 1.57f) * Elapsed;
  }

  // Check for movement collisions
  if(m_NodeTreeMesh.CheckIntersect(
            m_XPos,         m_YPos + 64.0f, m_ZPos,
            m_XPos + XMove, m_YPos + 64.0f, m_ZPos,
            NULL) == TRUE)
    XMove = 0.0f;
  if(m_NodeTreeMesh.CheckIntersect(
            m_XPos, m_YPos + 64.0f, m_ZPos,
            m_XPos, m_YPos + 64.0f, m_ZPos + ZMove,
            NULL) == TRUE)
    ZMove = 0.0f;

  // Update view coordinates
  m_XPos += XMove;
  m_ZPos += ZMove;

  if(m_Mouse.GetButtonState(MOUSE_RBUTTON) == TRUE) {
    // Lock the button
    m_Mouse.SetButtonState(MOUSE_RBUTTON, FALSE);
    m_Mouse.SetLock(MOUSE_RBUTTON, TRUE);

    // Drop an item - pick a random one
    while(1) {
      ItemNum = rand() % 1024;
      if(m_MILItems[ItemNum].Name[0])
        break;
    }
    m_Items.Add(ItemNum, 1, 
                m_XPos + (float)sin(m_Camera.GetYRotation()) * 200.0f, 
                m_YPos, 
                m_ZPos + (float)cos(m_Camera.GetYRotation()) * 200.0f, 
                NULL);
  }

  // Position camera
  m_Camera.Move(m_XPos + XMove, m_YPos + 100.0f, m_ZPos + ZMove);
  m_Camera.RotateRel((float)m_Mouse.GetYDelta() / 200.0f, 
                     (float)m_Mouse.GetXDelta() / 200.0f, 
                     0.0f);

  // Position light
  m_Light.Move(m_XPos, m_YPos+100.0f, m_ZPos);
  m_Graphics.SetLight(0, &m_Light);

  // Set camera and calculate frustum
  m_Graphics.SetCamera(&m_Camera);
  Frustum.Construct(&m_Graphics);

  // Prepare for closest item
  ClosestItem = NULL;

  // Render everything
  m_Graphics.Clear();
  if(m_Graphics.BeginScene() == TRUE) {

    // Setup rendering states
    m_Graphics.EnableZBuffer(TRUE);
    m_Graphics.EnableLighting(TRUE);

    // Render background
    m_NodeTreeMesh.Render(&Frustum);

    // Draw all items, scanning for closest one to viewer
    if((Item = m_Items.GetParentItem()) != NULL) {
      while(Item != NULL) {

        // Don't bother with child objects
        if(Item->Parent == NULL) {
          // Get the radius of the generic item object
          m_ItemObject.GetBounds(NULL,NULL,NULL,NULL,NULL,NULL,&Radius);

          // Frustum check and draw it if visible
          if(Frustum.CheckSphere(Item->XPos, Item->YPos, Item->ZPos, Radius) == TRUE) {
            m_ItemObject.Move(Item->XPos, Item->YPos, Item->ZPos);
            m_ItemObject.Render();

            // Figure if current item is closest to viewer
            XDiff = (float)fabs(Item->XPos - m_XPos);
            ZDiff = (float)fabs(Item->ZPos - m_ZPos);
            ItemDistance = XDiff * XDiff + ZDiff * ZDiff;
            if(ItemDistance < 100000.0f) {
              if(ClosestItem == NULL) {
                ClosestItem = Item;
                Closest = ItemDistance;
              } else {
                if(ItemDistance < Closest) {
                  Closest = ItemDistance;
                  ClosestItem = Item;
                }
              }
            }
          }
        }

        // Go to next object
        Item = Item->Next;
      }
    }

    // Rotate target and move into position
    if(ClosestItem != NULL) {
      m_TargetObject.RotateRel(0.0f, (float)Elapsed * 0.01f, 0.0f);
      m_TargetObject.Move(ClosestItem->XPos, ClosestItem->YPos+50.0f, ClosestItem->ZPos);
      m_TargetObject.Render();

      // Print item name
      sprintf(Text, "%s x %lu", m_MILItems[ClosestItem->ItemNum].Name, ClosestItem->Quantity);
      m_Font.Print(Text, 0, 0);
    }
 
    m_Graphics.EndScene();
  }
  m_Graphics.Display();

  // Check for picking up item
  // put here to make easier for closest item
  if(m_Mouse.GetButtonState(MOUSE_LBUTTON) == TRUE) {
    // Lock the button
    m_Mouse.SetButtonState(MOUSE_LBUTTON, FALSE);
    m_Mouse.SetLock(MOUSE_LBUTTON, TRUE);

    // Pick up closest item
    if(ClosestItem != NULL)
      m_Items.Remove(ClosestItem);
  }

  return TRUE;
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
  cApp App;
  return App.Run();
}
