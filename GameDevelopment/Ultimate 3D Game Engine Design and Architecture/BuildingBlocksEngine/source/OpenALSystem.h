/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_OPENAL_H_
#define _BB_OPENAL_H_

#include<NameSpace.h>

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


DECLARE_ENGINE_NAMESPACE


class OpenALSoundBuffer
{
   public:
      OpenALSoundBuffer() : m_soundBuffer(0)
      {
      
      }

      ~OpenALSoundBuffer()
      {
         Cleanup();
      }
      
      void Cleanup()
      {
         if(m_soundBuffer)
            alDeleteBuffers(1, &m_soundBuffer);

         m_soundBuffer = 0;
      }

      bool LoadSoundFromFile(char *fileName)
      {
         // New shorter way but not yet on all OSes.
         // m_soundBuffer = alutCreateBufferFromFile(fileName);

         // .. or can load the long way.

         ALenum format;
	      ALsizei size, frequency;
	      ALvoid* data;
	      ALboolean loop;

         alGenBuffers(1, &m_soundBuffer);

	      if(alGetError() != AL_NO_ERROR)
	         return false;

	      alutLoadWAVFile(fileName, &format, &data,
	                      &size, &frequency, &loop);
	      alBufferData(m_soundBuffer, format, data,
	                   size, frequency);
	      alutUnloadWAV(format, data, size, frequency);

         if(alGetError() == AL_NO_ERROR)
	         return true;

         return false;
      }

      unsigned int GetSoundBuffer()
      {
         return m_soundBuffer;
      }

   private:
      unsigned int m_soundBuffer;
};


class OpenALSoundSource
{
   public:
      OpenALSoundSource() : m_soundSource(0)
      {

      }

      ~OpenALSoundSource()
      {
         Cleanup();
      }
      
      void Cleanup()
      {
         if(m_soundSource)
            alDeleteSources(1, &m_soundSource);

         m_soundSource = 0;
      }

      bool BindSoundBuffer(OpenALSoundBuffer &buff)
      {
         if(m_soundSource == 0)
            alGenSources(1, &m_soundSource);

         alSourcei(m_soundSource, AL_BUFFER, buff.GetSoundBuffer());

         if(alGetError() == AL_NO_ERROR)
	         return true;

         return false;
      }
      
      void SetSoundPosition(float x, float y, float z)
      {
         alSource3f(m_soundSource, AL_POSITION, x, y, z);
      }
      
      void SetSoundVelocity(float x, float y, float z)
      {
         alSource3f(m_soundSource, AL_VELOCITY, x, y, z);
      }
      
      void SetLooping(bool enable)
      {
         alSourcei(m_soundSource, AL_LOOPING, enable);
      }
      
      unsigned int GetSource()
      {
         return m_soundSource;
      }

   private:
      unsigned int m_soundSource;
};


class OpenALSystem
{
   public:
      OpenALSystem()
      {
         alutInit(NULL, 0);
         m_init = true;
         
         if(alGetError() != AL_NO_ERROR)
	         m_init = false;

         /*if(alutGetMajorVersion() != ALUT_API_MAJOR_VERSION &&
            alutGetMinorVersion() != ALUT_API_MINOR_VERSION)
            {
               m_init = false;
            }*/
      }
      
      ~OpenALSystem()
      {
         Cleanup();
      }
      
      void Cleanup()
      {
         if(m_init == true)
            alutExit();

         m_init = false;
      }

      void SetListenerPosition(float x, float y, float z)
      {
         if(m_init == true)
            alListener3f(AL_POSITION, x, y, z);
      }
      
      void SetListenerVelocity(float x, float y, float z)
      {
         if(m_init == true)
            alListener3f(AL_VELOCITY, x, y, z);
      }
      
      void SetListenerOrientation(float dx, float dy, float dz,
                                  float ux, float uy, float uz)
      {
         float orientation[] = { dx, dy, dz, ux, uy, uz };

         if(m_init == true)
            alListenerfv(AL_ORIENTATION, orientation);
      }
      
      void PlaySound(OpenALSoundSource &source)
      {
         if(m_init == true)
            alSourcePlay(source.GetSource());
      }
      
      void PauseSound(OpenALSoundSource &source)
      {
         if(m_init == true)
            alSourcePause(source.GetSource());
      }
      
      void StopSound(OpenALSoundSource &source)
      {
         if(m_init == true)
            alSourceStop(source.GetSource());
      }
      
      bool isInitialized()
      {
         return m_init;
      }

   private:
      bool m_init;
};

END_ENGINE_NAMESPACE

#endif
