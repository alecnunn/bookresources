/*
   OpenAL Engine System Demo - Chapter 3
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod

   COMPILATION NOTE: Set the OpenAL include directories to
                     where you've installed the OpenAL SDK.
*/


#include<iostream>
#include<Input.h>
#include<OpenALSystem.h>

using namespace std;


int main()
{
   bbe::OpenALSystem alSoundSystem;
   bbe::OpenALSoundBuffer alBuffer;
   bbe::OpenALSoundSource alSource;

   cout << "OpenAL Demo created by Allen Sherrod\n";
   cout << "Loading and playing clip.wav" << endl << endl;

   if(alSoundSystem.isInitialized() == false)
      {
         cout << "Error initializing OpenAL!" << endl << endl;
         return 0;
      }

	if(alBuffer.LoadSoundFromFile("clip.wav") == false)
      {
         cout << "Error loading sound file!" << endl << endl;
         return 0;
      }

   if(alSource.BindSoundBuffer(alBuffer) == false)
      {
         cout << "Error binding sound buffer!" << endl << endl;
         return 0;
      }

   alSource.SetSoundPosition(0, 0, 0);
   alSource.SetSoundVelocity(0, 0, 0);
   alSource.SetLooping(true);

   alSoundSystem.SetListenerPosition(0, 0, 0);
   alSoundSystem.SetListenerVelocity(0, 0, 0);
   alSoundSystem.SetListenerPosition(0, 0, -1);

   alSoundSystem.PlaySound(alSource);
   cout << "Press ESCAPE (CTRL-ALT-Z on Linux) to stop the sound and exit." << endl;

   while(bbe::isButtonDown(BB_BUTTON_ESCAPE) != true)
   {
      // Wait
   }
   
   alSoundSystem.StopSound(alSource);

   alSource.Cleanup();
   alBuffer.Cleanup();
   alSoundSystem.Cleanup();

   return 1;
}
