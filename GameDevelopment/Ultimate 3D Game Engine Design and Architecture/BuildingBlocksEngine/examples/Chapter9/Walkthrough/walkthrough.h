/*
   Walk Through Demo - Chapter 9
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#ifndef _WALK_THROUGH_3D_H_
#define _WALK_THROUGH_3D_H_


#include<OpenGL.h>
#include<ModelData.h>
#include<Camera.h>
#include<Frustum.h>
#include"PortalSystem.h"


// General Defines.
#define WINDOW_NAME        "Walkthrough 3D"
#define WINDOW_CLASS       "UPGCLASS"
#define WINDOW_WIDTH       1024
#define WINDOW_HEIGHT      768
#define WINDOW_FULLSCREEN  0


// Total number of sectors in this demo.
#define TOTAL_SECTORS      2


// Finite state machine's states.
enum WALKTHROUGH_GAME_STATE { WT_GAME_INITIALIZING,
                              WT_GAME_MENU,
                              WT_GAME_PLAY,
                              WT_GAME_QUIT };


// Used for testing if a keyboard button is up.
enum WALKTHROUGH_KEYS { WT_S_KEY = 0, WT_ESC_KEY,
                        WT_TOTAL_KEYS };


// Demo class.
class WalkThrough3D
{
   public:
      WalkThrough3D();
      virtual ~WalkThrough3D();

      void Resize(int width, int height);

      virtual bool GameInitialize(bbe::RenderParams &params);
      virtual void GameUpdate();
      void GameRender();
      virtual void GameShutdown();

      virtual int EnterGameLoop() = 0;

   protected:
      void ProcessMainMenu();
      void ProcessGameLevel();
      void RenderSector(int index);

   protected:
      // Rendering system.
      bbe::OpenGLRenderer m_render;

      // List of sectors and "was rendered" flags.
      Sector m_areas[TOTAL_SECTORS];
      bool m_areaFlags[TOTAL_SECTORS];

      // 3D camera and view frustum.
      bbe::Camera m_camera;
      bbe::Frustum m_viewFrustum;

      // Menu.
      bbe::ModelData m_menu;
      bbe::BB_TEXTURE m_menuTexture;

      // States for game's finite state machine.
      WALKTHROUGH_GAME_STATE m_currentGameState;

      // For keyboard input (button up detection).
      bool m_buttonKeys[WT_TOTAL_KEYS];

      // Window width and height copy.
      int m_width, m_height;

      // Flag to quit application.
      bool m_quitDemo;
};


#endif
