/*
   Walk Through Demo - Chapter 9
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include"walkThroughLinux.h"


bool WalkThroughLinux::GameInitialize()
{
   // Setup render params.
   bbe::RenderParams params;
   params.m_handle.m_disp = m_disp;

   // General game initialize.
   return WalkThrough3D::GameInitialize(params);
}


int WalkThroughLinux::EnterGameLoop()
{
   GameUpdate();
   GameRender();

   return 1;
}
