/*
   OpenAL Demo - Chapter 3
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod

   COMPILATION NOTE: Set the include paths to point to where you have
                     installed the OpenAL SDK.
*/


#include<iostream>
#include<Input.h>

#ifdef _WIN32
   #include<al.h>
   #include<alc.h>
   #include<al/alut.h>
#else
#ifdef _MAC
   #include<OpenAL/al.h>
   #include<OpenAL/alc.h>
   #include<OpenAL/alut.h>
   #define alutLoadWAVFile(a, b, c, d, e, f) alutLoadWAVFile(a, b, c, d, e)
#else
   #include<AL/al.h>
   #include<AL/alc.h>
   #include<AL/alut.h>
   #define alutLoadWAVFile(a, b, c, d, e, f) alutLoadWAVFile((ALbyte*)a, b, c, d, e, f)
#endif
#endif

using namespace std;


int main()
{
   ALuint SoundBuffer;
   ALuint Source;

   ALfloat SourcePosition[] = {0.0f, 0.0f, 0.0f};
   ALfloat SourceVelocity[] = {0.0f, 0.0f, 0.0f};
   ALfloat ListenerPosition[] = {0.0f, 0.0f, 0.0f};
   ALfloat ListenerVelocity[] = {0.0f, 0.0f, 0.0f};
   ALfloat ListenerOrientation[] = {0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f};


   cout << "OpenAL Demo created by Allen Sherrod\n";
   cout << "Loading and playing clip.wav" << endl << endl;


	bool loaded = true;

   alutInit(NULL, 0);

	if(alGetError() != AL_NO_ERROR)
	   loaded = false;

   /*if(alutGetMajorVersion() != ALUT_API_MAJOR_VERSION &&
      alutGetMinorVersion() != ALUT_API_MINOR_VERSION)
      {
         return 0;
      }*/

   // Can load like this...
	// SoundBuffer = alutCreateBufferFromFile("clip.wav");

   // .. or can load the long way.

   ALenum format;
	ALsizei size, frequency;
	ALvoid* data;
	ALboolean loop;
   alGenBuffers(1, &SoundBuffer);

	if(alGetError() != AL_NO_ERROR)
	   loaded = false;

	alutLoadWAVFile("clip.wav", &format, &data, &size, &frequency, &loop);
	alBufferData(SoundBuffer, format, data, size, frequency);
	alutUnloadWAV(format, data, size, frequency);

   // Sources...

	alGenSources(1, &Source);

	if(alGetError() != AL_NO_ERROR)
	   loaded = false;

	alSourcei(Source, AL_BUFFER, SoundBuffer);
	alSourcefv(Source, AL_POSITION, SourcePosition);
	alSourcefv(Source, AL_VELOCITY, SourceVelocity);
   alListenerfv(AL_POSITION, ListenerPosition);
	alListenerfv(AL_VELOCITY, ListenerVelocity);
	alListenerfv(AL_ORIENTATION, ListenerOrientation);
	alSourcei(Source, AL_LOOPING, true);

	if(alGetError() != AL_NO_ERROR)
	   loaded = false;

   if(loaded)
      {
         alSourcePlay(Source);
         cout << "Press ESCAPE (CTRL-ALT-Z on Linux)  to stop the sound and exit." << endl;
      }
   else
      cout << "Error loading sound file." << endl;

   while(bbe::isButtonDown(BB_BUTTON_ESCAPE) != true)
   {
      // Wait
   }
   
   alSourceStop(Source);

   if(SoundBuffer) alDeleteBuffers(1, &SoundBuffer);
	if(Source) alDeleteSources(1, &Source);

	alutExit();

   return 1;
}
