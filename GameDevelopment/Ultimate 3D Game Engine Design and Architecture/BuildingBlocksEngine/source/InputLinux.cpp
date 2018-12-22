/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<string.h>
#include"LinuxInput.h"


DECLARE_ENGINE_NAMESPACE


bool g_keys[256] = {0};
int g_mouseX = 0;
int g_mouseY = 0;


void ClearButtonFlags()
{
   memset(g_keys, 0, sizeof(bool) * 256);
}


void SetButtonFlag(BB_INPUT_BUTTON btn, bool val)
{
   g_keys[btn] = val;
}


bool isButtonDown(BB_INPUT_BUTTON btn)
{
   return g_keys[btn];
}


bool isButtonUp(BB_INPUT_BUTTON btn, bool wasDown)
{
   if(isButtonDown(btn) == false && wasDown == true)
      return true;

   return false;
}


void SetMousePosition(int x, int y)
{
   g_mouseX = x;
   g_mouseY = y;
}


void GetMousePosition(int *mx, int *my)
{
   if(mx) *mx = g_mouseX;
   if(my) *my = g_mouseY;
}


END_ENGINE_NAMESPACE
