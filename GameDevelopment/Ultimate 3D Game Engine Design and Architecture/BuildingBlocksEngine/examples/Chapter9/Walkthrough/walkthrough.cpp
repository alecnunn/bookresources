/*
   Walk Through Demo - Chapter 9
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include"walkthrough.h"
#include<Input.h>
#include<BBShapes.h>
#include<Matrix.h>
#include<BoundingBox.h>


WalkThrough3D::WalkThrough3D()
{
   m_currentGameState = WT_GAME_INITIALIZING;
   m_quitDemo = false;
   m_width = m_height = 0;
   memset(&m_areaFlags, 0, sizeof(bool) * TOTAL_SECTORS);

   for(int i = 0; i < WT_TOTAL_KEYS; i++)
      m_buttonKeys[i] = false;
}


WalkThrough3D::~WalkThrough3D()
{
   GameShutdown();
}


void WalkThrough3D::Resize(int width, int height)
{
   m_render.SetViewPort(0, 0, width, height);
   m_width = width;
   m_height = height;
}


bool WalkThrough3D::GameInitialize(bbe::RenderParams &params)
{
   params.m_colorBits = 24;
   params.m_depthBits = 16;
   params.m_stencilBits = 8;
   params.m_fullscreen = WINDOW_FULLSCREEN;
   params.m_height = WINDOW_HEIGHT;
   params.m_width = WINDOW_WIDTH;
   params.m_maxCacheBytes = 2000;
   params.m_maxCacheIndices = 2000;

   if(m_render.Initialize(&params) != BB_SUCCESS)
      return false;

   Resize(WINDOW_WIDTH, WINDOW_HEIGHT);

   m_render.SetClearColor(0, 0, 0, 255);
   m_render.Enable(BB_DEPTH_TESTING);
   m_render.Enable(BB_SMOOTH_SHADING);
   m_render.Enable(BB_TEXTURE_2D);

   std::vector<BB_FILTER_TYPE> filters;
   filters.push_back(BB_MIN_LINEAR_FILTER);
   filters.push_back(BB_MAG_LINEAR_FILTER);
   filters.push_back(BB_MIP_LINEAR_FILTER);
   filters.push_back(BB_USE_ANSIO_FILTER);


   // Create background full-screen square.
   if(bbe::CreateSquareMesh(WINDOW_WIDTH, WINDOW_HEIGHT, &m_menu) == false)
      {
         return false;
      }

   // Load background image.
   if(m_render.LoadTexFromFile("resources/menu.tga",
      BB_TEX2D_TYPE, &m_menuTexture) == false)
      {
         return false;
      }

   m_render.ApplyFilters(m_menuTexture, &filters);


   // Load each sector and textures.
   bbe::BB_TEXTURE decal;
   std::string decalFile;
   char sectorFile[128] = {0};

   for(int i = 0; i < TOTAL_SECTORS; i++)
      {
         sprintf(sectorFile, "resources/sector%d.txt", i + 1);
      
         // Load sector data.
         if(m_areas[i].LoadSectorFromFile(sectorFile) == false)
            return false;

         decalFile = m_areas[i].GetDecalFile();

         // Load general texture that this sector uses.
         if(m_render.LoadTexFromFile(const_cast<char*>(decalFile.c_str()),
            BB_TEX2D_TYPE, &decal) == false)
            {
               return false;
            }

         m_render.ApplyFilters(decal, &filters);
         m_areas[i].SetDecal(decal);
      }

   std::vector<Portal> *portals1 = NULL;
   std::vector<Portal> *portals2 = NULL;
   std::string pName1, pName2;

   // Link portals together by finding which portals match
   // in names in each sector by setting their member indexes.

   // i.e. Loop through each sector and match which sectors share
   // a portal of the same name and set their member variables
   // to each of their array indexes.

   for(int i = 0; i < TOTAL_SECTORS; i++)
      {
         // Get this sector's list of portals.
         portals1 = m_areas[i].GetPortals();

         // Loop through list using j to see if any sectors
         // share a portal with i.
         for(int j = 0; j < TOTAL_SECTORS; j++)
            {
               // Self linking not supported.
               if(i == j)
                  continue;

               // Get j's portal list.
               portals2 = m_areas[j].GetPortals();

               // Loop through i's list and compare with j's.
               for(int p = 0; p < (int)(*portals1).size(); p++)
                  {
                     // Is it already connected?
                     if((*portals1)[p].GetConnectedArea() >= 0)
                        continue;

                     // Get name copy.
                     pName1 = (*portals1)[p].GetName();

                     // Loop through j's portals and compare.
                     for(int p2 = 0; p2 < (int)(*portals2).size(); p2++)
                        {
                           // Get name copy.
                           pName2 = (*portals2)[p2].GetName();

                           // Compare and link if match.
                           if(pName1 == pName2)
                              {
                                 (*portals1)[p].SetConnectedArea(j);
                                 (*portals2)[p2].SetConnectedArea(i);
                                 break;
                              }
                        }
                  }
            }
      }


   // Set camera and view frustum.
   bbe::Vector3D pos(0, 0, 1);
   bbe::Vector3D look(0, 0, 0);
   bbe::Vector3D up(0, 1, 0);
   bbe::Vector3D right(1, 0, 0);
   m_camera.SetPosition(pos);
   m_camera.SetLookDirection(look);
   m_camera.SetUpDirection(up);
   m_camera.SetRightDirection(right);


   // Ready to move to menu.
   m_currentGameState = WT_GAME_MENU;

   return true;
}


void WalkThrough3D::GameUpdate()
{
   bbe::Vector3D viewVec;

   // Used for isButtonUp() later.
   if(bbe::isButtonDown(BB_BUTTON_ESCAPE))
      m_buttonKeys[WT_ESC_KEY] = true;

   if(bbe::isButtonDown(BB_BUTTON_S) &&
      m_currentGameState == WT_GAME_MENU)
      {
         m_buttonKeys[WT_S_KEY] = true;
      }


   // Input.
   switch(m_currentGameState)
      {
         case WT_GAME_MENU:
            if(bbe::isButtonUp(BB_BUTTON_ESCAPE,
               m_buttonKeys[WT_ESC_KEY]))
               {
                  // Quit applicaiton.
                  m_buttonKeys[WT_ESC_KEY] = false;
                  m_quitDemo = true;
               }

            if(bbe::isButtonUp(BB_BUTTON_S,
               m_buttonKeys[WT_S_KEY]))
               {
                  // Enter demo.
                  m_buttonKeys[WT_S_KEY] = false;
                  m_currentGameState = WT_GAME_PLAY;
               }
            break;

            case WT_GAME_PLAY:
               if(bbe::isButtonUp(BB_BUTTON_ESCAPE,
                  m_buttonKeys[WT_ESC_KEY]))
                  {
                     // Escape to menu.
                     m_buttonKeys[WT_ESC_KEY] = false;
                     m_currentGameState = WT_GAME_MENU;
                  }

               viewVec = m_camera.GetPosition() -
                         m_camera.GetLookDirection();

               viewVec.Normalize();

               if(bbe::isButtonDown(BB_BUTTON_ARROW_UP))
                  {
                     // Move forward.
                     m_camera.MoveCamera(viewVec, -1);
                  }
               else if(bbe::isButtonDown(BB_BUTTON_ARROW_DOWN))
                  {
                     // Move back.
                     m_camera.MoveCamera(viewVec, 1);
                  }

               if(bbe::isButtonDown(BB_BUTTON_ARROW_LEFT))
                  {
                     // Rotate camera.
                     bbe::Vector3D axis(0, 1, 0);
                     m_camera.RotateCamera(3, axis);
                  }
               else if(bbe::isButtonDown(BB_BUTTON_ARROW_RIGHT))
                  {
                     // Rotate camera.
                     bbe::Vector3D axis(0, -1, 0);
                     m_camera.RotateCamera(3, axis);
                  }
               break;

         default:
            break;
      }
}


void WalkThrough3D::GameRender()
{
	m_render.StartRendering(1, 1, 0);
   m_render.LoadIdentityMatrix();

   switch(m_currentGameState)
      {
         case WT_GAME_MENU:
            ProcessMainMenu();
            break;

         default:
            ProcessGameLevel();
            break;
      }

	m_render.EndRendering();
}


void WalkThrough3D::GameShutdown()
{
   m_render.Shutdown();
}


void WalkThrough3D::ProcessMainMenu()
{
   m_render.ResizeWindowOrtho(-m_width, m_width,
                              -m_height, m_height, 0, 1);

   m_render.SetView(0, 0, 1, 0, 0, 0, 0, 1, 0);

   m_render.TranslateMatrix(0, 0, 0.1f);
   m_render.ApplyTexture(0, m_menuTexture);
   m_render.Render(BB_RENDER_MODEL_DATA_PARAMS(m_menu));
}


void WalkThrough3D::ProcessGameLevel()
{
   m_render.ResizeWindow(60, 0.1f, 2000, m_width, m_height);

   bbe::Vector3D camPos = m_camera.GetPosition();
   bbe::Vector3D look = m_camera.GetLookDirection();
   bbe::Vector3D up = m_camera.GetUpDirection();

   m_render.SetView(camPos.x, camPos.y, camPos.z,
                    look.x, look.y, look.z,
                    up.x, up.y, up.z);

   // Not necessary for each from in a real game but
   // not big deal for this simple demo.  Only need to
   // call when the camera moves.
   m_viewFrustum.CalculateFrustum(60, m_width/m_height, 0.1f,
                                  1000, camPos, look, up);

   bbe::BoundingBox aabb;

   // Draw the sector the camera is in and each of its portals.
   for(int i = 0; i < TOTAL_SECTORS; i++)
      {
         aabb = m_areas[i].GetBoundingBox();

         // Is the camera in this sector?
         if(aabb.CollisionCheck(camPos))
            {
               // Draw it and its portals.
               RenderSector(i);
               break;
            }
      }

   // Reset "was rendered" flags for next pass.
   memset(&m_areaFlags, 0, sizeof(bool) * TOTAL_SECTORS);
}


void WalkThrough3D::RenderSector(int index)
{
   if(index < 0 || index > TOTAL_SECTORS)
      return;

   if(m_areaFlags[index] == true)
      return;

   // Get sector's geometry to draw and mark as rendered.
   SectorMesh *ptr = m_areas[index].GetMesh();
   m_areaFlags[index] = true;

   // Only draw if ptr is not null just in case.
   if(ptr != NULL)
      {
         m_render.ApplyTexture(0, ptr->m_decal);
         m_render.Render(BB_RENDER_MODEL_DATA_PARAMS(ptr->m_geometry));
      }

   bbe::BoundingBox portalAabb;
   std::vector<Portal> *portals = m_areas[index].GetPortals();
   bbe::Vector3D center;
   float size = 0;

   // Loop through portals and draw the ones in the view frustum.
   // TO DO: In future versions we need to clip the view frustum
   // to the portals so that the adjusted view frustum is used
   // instead of the main one for the camera. This is a MUST in
   // order to make any use of the portal system outside of this demo.
   
   // NOTE: Also collision detection is a must if the system
   // is to have any value outside of this demo.  This demo
   // is a simple walk through used to demonstrate portal
   // rendering on a basic level, not collisions.

   for(int i = 0; i < (int)(*portals).size(); i++)
      {
         center = (*portals)[i].GetCenter();
         size = (*portals)[i].GetSize();
         
         portalAabb.m_min = bbe::Vector3D(center - size);
         portalAabb.m_max = bbe::Vector3D(center + size);

         // Test if we can see the portal or if we are
         // literally inside of it and the corners can't
         // be tested.

         if(m_viewFrustum.isCubeVisible(center.x, center.y, center.z, size) ||
            portalAabb.CollisionCheck(m_camera.GetPosition()))
            {
               // Recusion.
               RenderSector((*portals)[i].GetConnectedArea());
            }
      }
}
