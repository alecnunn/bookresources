/*
   Simple Black Jack Game Demo - Chapter 9
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#ifndef _BLACK_JACK_GAME_H_
#define _BLACK_JACK_GAME_H_


#include<OpenGL.h>
#include<ModelData.h>


// General Defines.
#define WINDOW_NAME        "Black Jack"
#define WINDOW_CLASS       "UPGCLASS"
#define WINDOW_WIDTH       800
#define WINDOW_HEIGHT      600
#define WINDOW_FULLSCREEN  0


// Finite state machine's states.
enum BLACK_JACK_GAME_STATE { BJ_GAME_INITIALIZING,
                             BJ_GAME_MENU,
                             BJ_GAME_PLAY_START,
                             BJ_GAME_PLAY_DEALING,
                             BJ_GAME_PLAY_PLAYER_MOVE,
                             BJ_GAME_PLAY_CPU_MOVE,
                             BJ_GAME_PLAY_PLAYER_WIN,
                             BJ_GAME_PLAY_CPU_WIN,
                             BJ_GAME_PLAY_PLAYER_CPU_TIE,
                             BJ_GAME_QUIT };


// Background IDs.
enum BLACK_JACK_GUI { BJ_GUI_MENU = 0, BJ_GUI_START,
                      BJ_GUI_P1_MOVE, BJ_GUI_CPU_MOVE,
                      BJ_GUI_P1_WIN, BJ_GUI_CPU_WIN,
                      BJ_GUI_P1_CPU_TIE, BJ_TOTAL_GUI };


// Used for testing if a keyboard button is up.
enum BLACK_JACK_KEYS { BJ_ESC_KEY,
                       BJ_H_KEY, BJ_S_KEY,
                       BJ_TOTAL_KEYS };


// Info dealing with the deck.
#define BJ_TOTAL_SUITS        4
#define BJ_TOTAL_SUIT_CARDS   13
#define BJ_TOTAL_CARDS        BJ_TOTAL_SUITS * BJ_TOTAL_SUIT_CARDS


// Structure for an individual card.
struct Card
{
   bbe::BB_TEXTURE m_decal;
   int m_suite;
   int m_face;
};


// Game class.
class BlackJackGame
{
   public:
      BlackJackGame();
      virtual ~BlackJackGame();

      void Resize(int width, int height);

      virtual bool GameInitialize(bbe::RenderParams &params);
      virtual void GameUpdate();
      void GameRender();
      virtual void GameShutdown();

      virtual int EnterGameLoop() = 0;

   protected:
      void ProcessMainMenu();
      void ProcessGameLevel();
      void DrawCards();
      void DealCards();
      int GiveCard();
      bool IsCardOut(int suite, int face);
      void ProcessAI();
      void PlayerHit();

   protected:
      // Rendering system.
      bbe::OpenGLRenderer m_render;

      // One model data, 52 cards in a deck, and
      // a decal for the back of a card.
      bbe::ModelData m_cardGeometry;
      Card m_cards[BJ_TOTAL_CARDS];
      bbe::BB_TEXTURE m_cardBack;

      // Background geometry.
      bbe::ModelData m_background;

      // GUI (P1 Win, CPU Win, tie, etc).
      bbe::ModelData m_guiElement;
      bbe::BB_TEXTURE m_guiTextures[BJ_TOTAL_GUI];

      // Cards for each player and score totals.
      std::vector<int> m_playerCards;
      std::vector<int> m_cpuCards;
      int m_playerTotal, m_cpuTotal;

      // Flag if the player stays or is over 21.
      bool m_cpuStay;
      bool m_playerStay;

      // States for game's finite state machine.
      BLACK_JACK_GAME_STATE m_currentGameState;

      // For keyboard input (button up detection).
      bool m_buttonKeys[BJ_TOTAL_KEYS];

      // Flag to quit application.
      bool m_quitDemo;
};


#endif