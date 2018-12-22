/*
   Simple Black Jack Game Demo - Chapter 9
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include"blackJackOSX.h"


bool BlackJackGameOSX::GameInitialize()
{
   bbe::RenderParams params;

   // Setup and render params.
   params.m_handle.m_window = m_window;

   // General game initialize.
   return BlackJackGame::GameInitialize(params);
}


int BlackJackGameOSX::EnterGameLoop()
{
   GameUpdate();
   GameRender();
}