/*
   XINPUT Demo - Chapter 3
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include<iostream>
#include<X360Input.h>

using namespace std;


int main()
{
   bbe::X360Controller gamepad;

   if(gamepad.Initialize(0) == BB_FAIL)
      {
         cout << "Game Controller not initialized!" << endl << endl;
         return 0;
      }


   if(gamepad.Update() == BB_SUCCESS)
      {
         cout << "XBOX 360 Controller detected!" << endl << endl;
      }
   else
      {
         cout << "No XBOX 360 Controller detected!" << endl << endl;
      }


   cout << "Press the left or right shoulder button to quit" << endl;

   do
   {
      if(gamepad.isButtonDown(BB_BUTTON_LEFT_SHOULDER))
         {
            cout << "Left shoulder pressed" << endl << endl;
            break;
         }
      if(gamepad.isButtonDown(BB_BUTTON_RIGHT_SHOULDER))
         {
            cout << "Right shoulder pressed" << endl << endl;
            break;
         }

      if(gamepad.Update() != BB_SUCCESS)
         {
            cout << "Controller not found!" << endl << endl;
            break;
         }

   }while(1);

   return 1;
}