/*
   Simple Black Jack Game Demo - Chapter 9
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include"blackJack.h"
#include<Input.h>
#include<BBShapes.h>
#include<time.h>


BlackJackGame::BlackJackGame()
{
   m_currentGameState = BJ_GAME_INITIALIZING;
   m_quitDemo = false;
   m_playerStay = false;
   m_cpuStay = false;

   for(int i = 0; i < BJ_TOTAL_KEYS; i++)
      m_buttonKeys[i] = false;
}


BlackJackGame::~BlackJackGame()
{
   GameShutdown();
}


void BlackJackGame::Resize(int width, int height)
{
   m_render.SetViewPort(0, 0, width, height);
   m_render.ResizeWindowOrtho(-width, width, -height, height, 0, 1);
}


bool BlackJackGame::GameInitialize(bbe::RenderParams &params)
{
   params.m_colorBits = 24;
   params.m_depthBits = 16;
   params.m_stencilBits = 8;
   params.m_fullscreen = false;
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


   // Create background full-screen square.
   if(bbe::CreateSquareMesh(WINDOW_WIDTH, WINDOW_HEIGHT,
      &m_background) == false)
      {
         return false;
      }

   // Background images.
   std::string resourceStr[BJ_TOTAL_GUI] =
   {
      "resources/menu.tga",
      "resources/GamePlayStart.tga",
      "resources/GamePlayP1Move.tga",
      "resources/GamePlayCPUMove.tga",
      "resources/GamePlayP1Win.tga",
      "resources/GamePlayCPUWin.tga",
      "resources/GamePlayP1CPUTie.tga"
   };

   // Load all background images.
   for(int i = 0; i < BJ_TOTAL_GUI; i++)
      {
         if(m_render.LoadTexFromFile(const_cast<char*>(resourceStr[i].c_str()),
            BB_TEX2D_TYPE, &m_guiTextures[i]) == false)
            {
               return false;
            }

         m_render.ApplyFilters(m_guiTextures[i], &filters);
      }


   int tempIndex = 0;
   char file[64] = { 0 };

   // Create card geometry.
   if(bbe::CreateSquareMesh(128, 128, &m_cardGeometry) == false)
      return false;

   // Load image for the back of a card.
   if(m_render.LoadTexFromFile("resources/Cards/CardBack.tga",
      BB_TEX2D_TYPE, &m_cardBack) == false)
      {
         return false;
      }

   m_render.ApplyFilters(m_cardBack, &filters);

   // Load all card images.
   for(int f = 0; f < BJ_TOTAL_SUIT_CARDS; f++)
      {
         for(int s = 0; s < BJ_TOTAL_SUITS; s++)
            {
               tempIndex = s * BJ_TOTAL_SUITS + f;
               sprintf(file, "resources/Cards/Card%d%d.tga", s, f);

               m_cards[tempIndex].m_suite = s;
               m_cards[tempIndex].m_face = f;

               if(m_render.LoadTexFromFile(file,
                  BB_TEX2D_TYPE,
                  &m_cards[tempIndex].m_decal) == false)
                  {
                     return false;
                  }
            }
      }

   // Ready to move to menu.
   m_currentGameState = BJ_GAME_MENU;

   // Used for random number generators.
   srand((unsigned)time(0));

   return true;
}


void BlackJackGame::GameUpdate()
{
   if(bbe::isButtonDown(BB_BUTTON_ESCAPE))
      m_buttonKeys[BJ_ESC_KEY] = true;

   if(bbe::isButtonDown(BB_BUTTON_H))
      m_buttonKeys[BJ_H_KEY] = true;

   if(bbe::isButtonDown(BB_BUTTON_S))
      m_buttonKeys[BJ_S_KEY] = true;

   // Input.
   switch(m_currentGameState)
      {
         case BJ_GAME_MENU:
            if(bbe::isButtonUp(BB_BUTTON_ESCAPE,
               m_buttonKeys[BJ_ESC_KEY]))
               {
                  m_buttonKeys[BJ_ESC_KEY] = false;
                  m_quitDemo = true;
               }

            if(bbe::isButtonUp(BB_BUTTON_S,
               m_buttonKeys[BJ_S_KEY]))
               {
                  m_buttonKeys[BJ_S_KEY] = false;
                  m_currentGameState = BJ_GAME_PLAY_START;
               }
            break;

         case BJ_GAME_PLAY_START:
         case BJ_GAME_PLAY_PLAYER_WIN:
         case BJ_GAME_PLAY_CPU_WIN:
         case BJ_GAME_PLAY_PLAYER_CPU_TIE:
            if(bbe::isButtonUp(BB_BUTTON_ESCAPE,
               m_buttonKeys[BJ_ESC_KEY]))
               {
                  m_buttonKeys[BJ_ESC_KEY] = false;
                  m_currentGameState = BJ_GAME_MENU;
               }

            if(bbe::isButtonUp(BB_BUTTON_S,
               m_buttonKeys[BJ_S_KEY]))
               {
                  m_buttonKeys[BJ_S_KEY] = false;
                  m_currentGameState = BJ_GAME_PLAY_DEALING;
               }
            break;

         case BJ_GAME_PLAY_PLAYER_MOVE:
            if(m_playerStay == false)
               {
                  if(bbe::isButtonUp(BB_BUTTON_H,
                     m_buttonKeys[BJ_H_KEY]))
                     {
                        m_buttonKeys[BJ_H_KEY] = false;

                        PlayerHit();
                        m_currentGameState = BJ_GAME_PLAY_CPU_MOVE;
                     }
                  else if(bbe::isButtonUp(BB_BUTTON_S,
                     m_buttonKeys[BJ_S_KEY]))
                     {
                        m_buttonKeys[BJ_S_KEY] = false;

                        m_playerStay = true;
                        m_currentGameState = BJ_GAME_PLAY_CPU_MOVE;
                     }
               }
            break;

         default:
            break;
      }


   // Game logic.
   switch(m_currentGameState)
      {
         case BJ_GAME_PLAY_DEALING:
            m_playerStay = false;
            m_cpuStay = false;

            DealCards();
            m_currentGameState = BJ_GAME_PLAY_PLAYER_MOVE;
            break;

         case BJ_GAME_PLAY_PLAYER_MOVE:
               if(m_playerTotal > 21)
                  {
                     m_playerStay = true;
                     m_currentGameState = BJ_GAME_PLAY_CPU_MOVE;
                  }
            break;

         case BJ_GAME_PLAY_CPU_MOVE:
            if(m_cpuStay != true)
               ProcessAI();

            if(m_playerStay != true)
               m_currentGameState = BJ_GAME_PLAY_PLAYER_MOVE;
            break;

         default:
            break;
      }


   // Victory conditions.
   if(m_cpuStay == true && m_playerStay == true)
      {
         // Any over 21?
         if(m_cpuTotal <= 21 && m_playerTotal > 21)
            m_currentGameState = BJ_GAME_PLAY_CPU_WIN;
         else if(m_playerTotal <= 21 && m_cpuTotal > 21)
            m_currentGameState = BJ_GAME_PLAY_PLAYER_WIN;
         else
            {
               // Any tie...
               if(m_playerTotal > 21 && m_cpuTotal > 21)
                  m_currentGameState = BJ_GAME_PLAY_PLAYER_CPU_TIE;
               else if(m_playerTotal == m_cpuTotal)
                  m_currentGameState = BJ_GAME_PLAY_PLAYER_CPU_TIE;
               else
                  {
                     // Cpu over player else player over Cpu.
                     if(m_cpuTotal > m_playerTotal)
                        m_currentGameState = BJ_GAME_PLAY_CPU_WIN;
                     else
                        m_currentGameState = BJ_GAME_PLAY_PLAYER_WIN;
                  }
            }

         m_cpuStay = m_playerStay = false;
      }
}


void BlackJackGame::GameRender()
{
	m_render.StartRendering(1, 1, 0);
   m_render.LoadIdentityMatrix();

   switch(m_currentGameState)
      {
         case BJ_GAME_MENU:
            ProcessMainMenu();
            break;

         default:
            ProcessGameLevel();
            break;
      }

	m_render.EndRendering();
}


void BlackJackGame::GameShutdown()
{
   m_render.Shutdown();
}


void BlackJackGame::ProcessMainMenu()
{
   m_render.SetView(0, 0, 1, 0, 0, 0, 0, 1, 0);

   m_render.TranslateMatrix(0, 0, 0.1f);
   m_render.ApplyTexture(0, m_guiTextures[BJ_GUI_MENU]);
   m_render.Render(BB_RENDER_MODEL_DATA_PARAMS(m_background));
}


void BlackJackGame::ProcessGameLevel()
{
   m_render.SetView(0, 0, 1, 0, 0, 0, 0, 1, 0);


   // Draw background.
   switch(m_currentGameState)
      {
         case BJ_GAME_PLAY_START:
            m_render.ApplyTexture(0, m_guiTextures[BJ_GUI_START]);
            break;

         case BJ_GAME_PLAY_PLAYER_MOVE:
            m_render.ApplyTexture(0, m_guiTextures[BJ_GUI_P1_MOVE]);
            break;

         case BJ_GAME_PLAY_CPU_MOVE:
            m_render.ApplyTexture(0, m_guiTextures[BJ_GUI_CPU_MOVE]);
            break;

         case BJ_GAME_PLAY_PLAYER_WIN:
            m_render.ApplyTexture(0, m_guiTextures[BJ_GUI_P1_WIN]);
            break;

         case BJ_GAME_PLAY_CPU_WIN:
            m_render.ApplyTexture(0, m_guiTextures[BJ_GUI_CPU_WIN]);
            break;

         case BJ_GAME_PLAY_PLAYER_CPU_TIE:
            m_render.ApplyTexture(0, m_guiTextures[BJ_GUI_P1_CPU_TIE]);
            break;

         default:
            break;
      }

   m_render.TranslateMatrix(0, 0, 0.1f);
   m_render.Render(BB_RENDER_MODEL_DATA_PARAMS(m_background));


   // Draw cards.
   switch(m_currentGameState)
      {
         case BJ_GAME_PLAY_PLAYER_MOVE:
         case BJ_GAME_PLAY_CPU_MOVE:
         case BJ_GAME_PLAY_PLAYER_WIN:
         case BJ_GAME_PLAY_CPU_WIN:
         case BJ_GAME_PLAY_PLAYER_CPU_TIE:
            DrawCards();
            break;

         default:
            break;
      }
}


void BlackJackGame::DrawCards()
{
   float z = 0;
   float x = 200;
   int index = 0;

   for(int i = 0; i < (int)m_playerCards.size(); i++)
      {
         m_render.PushMatrix();

            m_render.TranslateMatrix((float)(i * 40) - x,
                                     -300,
                                     0.2f + z);

            index = m_playerCards[i];
            m_render.ApplyTexture(0, m_cards[index].m_decal);

            m_render.Render(BB_RENDER_MODEL_DATA_PARAMS(m_cardGeometry));

         m_render.PopMatrix();

         z += 0.01f;
      }

   z = 0;

   for(int i = 0; i < (int)m_cpuCards.size(); i++)
      {
         m_render.PushMatrix();

            m_render.TranslateMatrix((float)(i * 40) - x,
                                     300,
                                     0.2f + z);

            // Only draw CPU cards at the end.
            if(m_currentGameState >= BJ_GAME_PLAY_PLAYER_WIN &&
               m_currentGameState <= BJ_GAME_PLAY_PLAYER_CPU_TIE)
               {
                  index = m_cpuCards[i];
                  m_render.ApplyTexture(0, m_cards[index].m_decal);
               }
            else
               {
                  m_render.ApplyTexture(0, m_cardBack);
               }

            m_render.Render(BB_RENDER_MODEL_DATA_PARAMS(m_cardGeometry));

         m_render.PopMatrix();

         z += 0.01f;
      }
}


void BlackJackGame::DealCards()
{
   int cardIndex = -1;

   m_playerCards.clear();
   m_cpuCards.clear();

   m_playerTotal = m_cpuTotal = 0;
   m_cpuStay = m_playerStay = false;

   // Get first two player cards.
   cardIndex = GiveCard();
   m_playerCards.push_back(cardIndex);
   m_playerTotal += (m_cards[cardIndex].m_face + 2) > 11 ? 10 :
                     m_cards[cardIndex].m_face + 2;

   assert(m_cards[cardIndex].m_face >= 0);

   cardIndex = GiveCard();
   m_playerCards.push_back(cardIndex);
   m_playerTotal += (m_cards[cardIndex].m_face + 2) > 11 ? 10 :
                     m_cards[cardIndex].m_face + 2;

   assert(m_cards[cardIndex].m_face >= 0);

   // Get first two CPU cards.
   cardIndex = GiveCard();
   m_cpuCards.push_back(cardIndex);
   m_cpuTotal += (m_cards[cardIndex].m_face + 2) > 11 ? 10 :
                  m_cards[cardIndex].m_face + 2;

   assert(m_cards[cardIndex].m_face >= 0);

   cardIndex = GiveCard();
   m_cpuCards.push_back(cardIndex);
   m_cpuTotal += (m_cards[cardIndex].m_face + 2) > 11 ? 10 :
                  m_cards[cardIndex].m_face + 2;

   assert(m_cards[cardIndex].m_face >= 0);
}


int BlackJackGame::GiveCard()
{
   int suite = 0;
   int face = 0;

   do
   {
      suite = rand() % 4;
      face = rand() % 14;
   }
   while(IsCardOut(suite, face) == true ||
         suite < 0 || face < 0 || suite > 3 ||
         face > 13);

   return suite * BJ_TOTAL_SUITS + face;
}


bool BlackJackGame::IsCardOut(int suite, int face)
{
   int card = suite * BJ_TOTAL_SUITS + face;

   for(int i = 0; i < (int)m_cpuCards.size(); i++)
      {
         if(m_cpuCards[i] == card)
            return true;
      }

   for(int i = 0; i < (int)m_playerCards.size(); i++)
      {
         if(m_playerCards[i] == card)
            return true;
      }

   return false;
}


void BlackJackGame::ProcessAI()
{
   if(m_cpuTotal > 16 && m_cpuTotal >= m_playerTotal ||
      m_cpuTotal > 21)
      {
         m_cpuStay = true;
         return;
      }

   int cardIndex = -1;

   // If the CPU does not stay, it hits.
   cardIndex = GiveCard();
   m_cpuCards.push_back(cardIndex);
   m_cpuTotal += (m_cards[cardIndex].m_face + 2) > 11 ? 10 :
                  m_cards[cardIndex].m_face + 2;

   assert(m_cards[cardIndex].m_face >= 0);
}


void BlackJackGame::PlayerHit()
{
   if(m_playerTotal > 21)
      {
         m_playerStay = true;
         return;
      }

   int cardIndex = -1;

   // Allow the hit.
   cardIndex = GiveCard();
   m_playerCards.push_back(cardIndex);
   m_playerTotal += (m_cards[cardIndex].m_face + 2) > 11 ? 10 :
                     m_cards[cardIndex].m_face + 2;

   assert(m_cards[cardIndex].m_face >= 0);
}