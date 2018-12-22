/*
   Simple Black Jack Game Demo - Chapter 9
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include"blackJackLinux.h"


bool BlackJackGameLinux::GameInitialize()
{
   // Setup render params.
   bbe::RenderParams params;
   params.m_handle.m_disp = m_disp;

   // General game initialize.
   return BlackJackGame::GameInitialize(params);
}


int BlackJackGameLinux::EnterGameLoop()
{
   GameUpdate();
   GameRender();

   return 1;
}
