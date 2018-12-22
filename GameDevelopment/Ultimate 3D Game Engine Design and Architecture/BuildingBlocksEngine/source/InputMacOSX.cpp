/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<Carbon/Carbon.h>
#include"Input.h"


DECLARE_ENGINE_NAMESPACE


#define MAC_1_KEY				   0x12
#define MAC_2_KEY				   0x13
#define MAC_3_KEY				   0x14
#define MAC_4_KEY				   0x15
#define MAC_5_KEY				   0x17
#define MAC_6_KEY				   0x16
#define MAC_7_KEY				   0x1A
#define MAC_8_KEY				   0x1C
#define MAC_9_KEY				   0x19
#define MAC_0_KEY				   0x1D
#define MAC_A_KEY				   0x00
#define MAC_B_KEY				   0x0B
#define MAC_C_KEY				   0x08
#define MAC_D_KEY				   0x02
#define MAC_E_KEY				   0x0E
#define MAC_F_KEY				   0x03
#define MAC_G_KEY				   0x05
#define MAC_H_KEY				   0x04
#define MAC_I_KEY				   0x22
#define MAC_J_KEY				   0x26
#define MAC_K_KEY				   0x28
#define MAC_L_KEY				   0x25
#define MAC_M_KEY				   0x2E
#define MAC_N_KEY				   0x2D
#define MAC_O_KEY				   0x1F
#define MAC_P_KEY				   0x23
#define MAC_Q_KEY				   0x0C
#define MAC_R_KEY				   0x0F
#define MAC_S_KEY				   0x01
#define MAC_T_KEY				   0x11
#define MAC_U_KEY				   0x20
#define MAC_V_KEY				   0x09
#define MAC_W_KEY				   0x0D
#define MAC_X_KEY             0x07
#define MAC_Y_KEY             0x10
#define MAC_Z_KEY             0x06
#define MAC_F1_KEY				0x7A
#define MAC_F2_KEY				0x78
#define MAC_F3_KEY				0x63
#define MAC_F4_KEY				0x76
#define MAC_F5_KEY				0x60
#define MAC_F6_KEY				0x61
#define MAC_F7_KEY				0x62
#define MAC_F8_KEY				0x64
#define MAC_F9_KEY				0x65
#define MAC_F10_KEY				0x6D
#define MAC_F11_KEY				0x67
#define MAC_F12_KEY				0x6F
#define MAC_RETURN_KEY			0x24
#define MAC_ENTER_KEY			0x4C
#define MAC_TAB_KEY				0x30
#define MAC_SPACE_KEY			0x31
#define MAC_DELETE_KEY			0x33
#define MAC_ESCAPE_KEY			0x35
#define MAC_COMMAND_KEY			0x37
#define MAC_SHIFT_KEY			0x38
#define MAC_CAPS_LOCK_KEY		0x39
#define MAC_OPTION_KEY			0x3A
#define MAC_CONTROL_KEY			0x3B
#define MAC_PAGE_UP_KEY			0x74
#define MAC_PAGE_DOWN_KEY		0x79
#define MAC_ARROW_UP_KEY		0x7E
#define MAC_ARROW_DOWN_KEY    0x7D
#define MAC_ARROW_LEFT_KEY    0x7B
#define MAC_ARROW_RIGHT_KEY   0x7C


int GetButtonKeyCode(BB_INPUT_BUTTON btn, int &key)
{
   switch(btn)
      {
         case BB_BUTTON_ESCAPE: key = MAC_ESCAPE_KEY; break;
         case BB_BUTTON_SPACE: key = MAC_SPACE_KEY; break;
         case BB_BUTTON_ENTER: key = MAC_ENTER_KEY; break;

         case BB_BUTTON_ARROW_DOWN: key = MAC_ARROW_DOWN_KEY; break;
         case BB_BUTTON_ARROW_UP: key = MAC_ARROW_UP_KEY; break;
         case BB_BUTTON_ARROW_LEFT: key = MAC_ARROW_LEFT_KEY; break;
         case BB_BUTTON_ARROW_RIGHT: key = MAC_ARROW_RIGHT_KEY; break;
         
         case BB_BUTTON_A: key = MAC_A_KEY; break;
         case BB_BUTTON_B: key = MAC_B_KEY; break;
         case BB_BUTTON_C: key = MAC_C_KEY; break;
         case BB_BUTTON_D: key = MAC_D_KEY; break;
         case BB_BUTTON_E: key = MAC_E_KEY; break;
         case BB_BUTTON_F: key = MAC_F_KEY; break;
         case BB_BUTTON_G: key = MAC_G_KEY; break;
         case BB_BUTTON_H: key = MAC_H_KEY; break;
         case BB_BUTTON_I: key = MAC_I_KEY; break;
         case BB_BUTTON_J: key = MAC_J_KEY; break;
         case BB_BUTTON_K: key = MAC_K_KEY; break;
         case BB_BUTTON_L: key = MAC_L_KEY; break;
         case BB_BUTTON_M: key = MAC_M_KEY; break;
         case BB_BUTTON_N: key = MAC_N_KEY; break;
         case BB_BUTTON_O: key = MAC_O_KEY; break;
         case BB_BUTTON_P: key = MAC_P_KEY; break;
         case BB_BUTTON_Q: key = MAC_Q_KEY; break;
         case BB_BUTTON_R: key = MAC_R_KEY; break;
         case BB_BUTTON_S: key = MAC_S_KEY; break;
         case BB_BUTTON_T: key = MAC_T_KEY; break;
         case BB_BUTTON_U: key = MAC_U_KEY; break;
         case BB_BUTTON_V: key = MAC_V_KEY; break;
         case BB_BUTTON_W: key = MAC_W_KEY; break;
         case BB_BUTTON_X: key = MAC_X_KEY; break;
         case BB_BUTTON_Y: key = MAC_Y_KEY; break;
         case BB_BUTTON_Z: key = MAC_Z_KEY; break;

         default:
            key = -1;
            return 0;
            break;
      }

   return 1;
}


bool isButtonDown(BB_INPUT_BUTTON btn)
{
   int key = 0;
   //BigEndianLong key = 0;
   long index;
   short bit;
   unsigned char keys[16];

   if(btn == BB_BUTTON_MOUSE_LEFT)
      return Button();

   if(!GetButtonKeyCode(btn, key))
      return false;

   if(key == -1)
      return false;

   GetKeys((BigEndianLong*)keys);

   index = keys[key / 8];
   bit = key % 8;

   return ((index >> bit) & 0x01);
}


bool isButtonUp(BB_INPUT_BUTTON btn, bool wasDown)
{
   if(isButtonDown(btn) == false && wasDown == true)
      return true;

   return false;
}


void GetMousePosition(int *mx, int *my)
{
   Point pos;
   GetMouse(&pos);

   if(mx) *mx = pos.h;
   if(my) *my = pos.v;
}


END_ENGINE_NAMESPACE