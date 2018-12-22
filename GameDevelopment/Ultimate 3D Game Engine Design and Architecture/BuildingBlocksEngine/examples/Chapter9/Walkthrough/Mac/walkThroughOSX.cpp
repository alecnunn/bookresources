/*
   Walk Through Demo - Chapter 9
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include"walkThroughOSX.h"


bool WalkThroughOSX::GameInitialize()
{
   bbe::RenderParams params;

   // Setup and render params.
   params.m_handle.m_window = m_window;

   // General game initialize.
   return WalkThrough3D::GameInitialize(params);
}


int WalkThroughOSX::EnterGameLoop()
{
   GameUpdate();
   GameRender();
}