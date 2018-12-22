/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<windows.h>
#include<mmsystem.h>
#include"Input.h"

#pragma comment(lib, "winmm.lib")


DECLARE_ENGINE_NAMESPACE


bool GetButtonKeyCode(BB_INPUT_BUTTON btn, int &key)
{
   switch(btn)
      {
         case BB_BUTTON_ESCAPE: key = VK_ESCAPE; break;
         case BB_BUTTON_SPACE: key = VK_SPACE; break;
         case BB_BUTTON_ENTER: key = VK_RETURN; break;

         case BB_BUTTON_MOUSE_LEFT: key = VK_LBUTTON; break;
         case BB_BUTTON_MOUSE_RIGHT: key = VK_RBUTTON; break;

         case BB_BUTTON_ARROW_UP: key = VK_UP; break;
         case BB_BUTTON_ARROW_DOWN: key = VK_DOWN; break;
         case BB_BUTTON_ARROW_LEFT: key = VK_LEFT; break;
         case BB_BUTTON_ARROW_RIGHT: key = VK_RIGHT; break;

         case BB_BUTTON_A: key = 0x41; break;
         case BB_BUTTON_B: key = 0x42; break;
         case BB_BUTTON_C: key = 0x43; break;
         case BB_BUTTON_D: key = 0x44; break;
         case BB_BUTTON_E: key = 0x45; break;
         case BB_BUTTON_F: key = 0x46; break;
         case BB_BUTTON_G: key = 0x47; break;
         case BB_BUTTON_H: key = 0x48; break;
         case BB_BUTTON_I: key = 0x49; break;
         case BB_BUTTON_J: key = 0x4A; break;
         case BB_BUTTON_K: key = 0x4B; break;
         case BB_BUTTON_L: key = 0x4C; break;
         case BB_BUTTON_M: key = 0x4D; break;
         case BB_BUTTON_N: key = 0x4E; break;
         case BB_BUTTON_O: key = 0x4F; break;
         case BB_BUTTON_P: key = 0x50; break;
         case BB_BUTTON_Q: key = 0x51; break;
         case BB_BUTTON_R: key = 0x52; break;
         case BB_BUTTON_S: key = 0x53; break;
         case BB_BUTTON_T: key = 0x54; break;
         case BB_BUTTON_U: key = 0x55; break;
         case BB_BUTTON_V: key = 0x56; break;
         case BB_BUTTON_W: key = 0x57; break;
         case BB_BUTTON_X: key = 0x58; break;
         case BB_BUTTON_Y: key = 0x59; break;
         case BB_BUTTON_Z: key = 0x5A; break;

         default:
            return false;
            break;
      }

   return true;
}


bool isButtonDown(BB_INPUT_BUTTON btn)
{
   int key = 0;

   if(GetButtonKeyCode(btn, key) == false)
      return false;

   if(GetKeyState(key) & 0x80)
      return true;

   return false;
}


bool isButtonUp(BB_INPUT_BUTTON btn, bool wasDown)
{
   if(isButtonDown(btn) == false && wasDown == true)
      return true;

   return false;
}


void GetMousePosition(int *mx, int *my)
{
   POINT mousePos;
	GetCursorPos(&mousePos);

	if(mx) *mx = mousePos.x;
	if(my) *my = mousePos.y;
}


END_ENGINE_NAMESPACE