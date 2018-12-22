/****************************************************************************

   PROGRAM: audioenv.cpp

   DESCRIPTION: audio object class code

****************************************************************************/
#include "audioenv.h"
#include "Obstruct.h"

#ifndef LINUX // use EAX for Windows and MacOS
#define USE_EAX
#endif

// EAX-test
// add initguid.h under Windows
#ifdef USE_EAX
#define OPENAL
#ifdef WINDOWS
#include <initguid.h>
#else
#define INITGUID
#endif
#include "eax.h"
#endif // USE_EAX

// constructor
AudioEnv::AudioEnv ()
{
	nextBuffer = 0;
	nextSource = 0;
	EAXlevel = 0;
}

// destructor
AudioEnv::~AudioEnv ()
{
	alutExit();
}

// init
void AudioEnv::Init ()
{
   alutInit (NULL, 0); // init OpenAL

   // global settings
   alListenerf(AL_GAIN, 1.0);
   alDopplerFactor(1.0); // don't exaggerate doppler shift
   alDopplerVelocity(343); // using meters/second

#ifdef USE_EAX
   // determine EAX support level
	if (alIsExtensionPresent((ALubyte *)"EAX2.0") == AL_TRUE)
	{
		EAXlevel = 2;
	} else
	{
		if (alIsExtensionPresent((ALubyte *)"EAX") == AL_TRUE)
		{
			EAXlevel = 1;
		}
	}

   // set EAX environment if EAX is available
	EAXSet pfPropSet;
	EAXGet pfPropGet;
	unsigned long ulEAXVal;
	long lGlobalReverb;
	if (EAXlevel != 0)
	{
		pfPropSet = (EAXSet) alGetProcAddress((ALubyte *)"EAXSet");
		if (pfPropSet != NULL)
		{
		    lGlobalReverb = -10000;
			pfPropSet(&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ROOM, 0, &lGlobalReverb, sizeof(unsigned long));
			ulEAXVal = EAX_ENVIRONMENT_GENERIC;
			pfPropSet(&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ENVIRONMENT, 0, &ulEAXVal, sizeof(unsigned long));
		}
	}
#endif // USE_EAX
}

// ListenerPostition -- update listener's position and direction
void AudioEnv::ListenerPosition (ALfloat* position, ALfloat* angle)
{
	alListenerfv(AL_POSITION, position);
	alListenerfv(AL_ORIENTATION, angle);
}

// UpdateObstruction -- update obstruction value for a specific source, using EAX if available
void AudioEnv::UpdateObstruction (int handle)
{
	SVector lposition, sposition;

	alGetListenerfv(AL_POSITION, (float *) &lposition);
	alGetSourcefv(source[handle -1], AL_POSITION, (float *) &sposition);

	// location of each "house"
	SVector svMinHouse1, svMinHouse2, svMaxHouse1, svMaxHouse2;
	svMinHouse1.fX = 0;
	svMaxHouse1.fX = 9;
	svMinHouse1.fY = -100;
	svMaxHouse1.fY = 100;
	svMinHouse1.fZ = -9;
	svMaxHouse1.fZ = 0;

	svMinHouse2.fX = 24;
	svMaxHouse2.fX = 33;
	svMinHouse2.fY = -100;
	svMaxHouse2.fY = 100;
	svMinHouse2.fZ = 9;
	svMaxHouse2.fZ = 18;

	// calculate "obstruction level" from 0 to 1
	float obstructionLevel1 = GetObstructionLevel(lposition, sposition, svMinHouse1, svMaxHouse1);
	float obstructionLevel2 = GetObstructionLevel(lposition, sposition, svMinHouse2, svMaxHouse2);

	// use value for the house that obstructs the most (lower value)
	float obstructionLevel;
	if (obstructionLevel1 < obstructionLevel2)
	{
		obstructionLevel = obstructionLevel1;
	} else
	{
		obstructionLevel = obstructionLevel2;
	}

	//exaggerate obstruction level
	obstructionLevel = (obstructionLevel - 0.98f) * 50.0f;
	if (obstructionLevel < 0)
	{
		obstructionLevel = 0;
	}

#ifdef USE_EAX
	if (EAXlevel == 2)
	{
		// have EAX 2.0, so apply the obstruction
		EAXSet pfPropSet = (EAXSet) alGetProcAddress((ALubyte *)"EAXSet");
		float fvalue = 0.25;
		pfPropSet(&DSPROPSETID_EAX_SourceProperties, DSPROPERTY_EAXBUFFER_OBSTRUCTIONLFRATIO, source[handle-1], &fvalue, sizeof(float));
		long obstruction = (long) ((1.0f - obstructionLevel) * -10000.0f);
		pfPropSet(&DSPROPSETID_EAX_SourceProperties, DSPROPERTY_EAXBUFFER_OBSTRUCTION, source[handle-1], &obstruction, sizeof(long));
	} else
	{
#endif // USE_EAX
		// the non-EAX case should really involve low-pass filtering, but we'll just do a gain control here...
		alSourcef(source[handle -1], AL_GAIN, obstructionLevel);
#ifdef USE_EAX
	}
#endif // USE_EAX
}

// LoadFile -- loads a file into a buffer and source
int AudioEnv::LoadFile (char *filename, bool loop)
{
   // create buffer
   alGetError(); /* clear */
   alGenBuffers(1, &buffer[nextBuffer]);
   if(alGetError() != AL_NO_ERROR) {
	   return 0;
   }

   // create source
   alGetError(); /* clear */
   alGenSources(1, &source[nextSource]);
   if(alGetError() != AL_NO_ERROR) {
	   return 0;
   }

   // load data into buffer
#ifdef WINDOWS
   ALsizei size, freq;
   ALenum format;
   ALvoid *data;
   ALboolean looping;
   
   alutLoadWAVFile(filename, &format, &data, &size, &freq, &looping);
   alBufferData (buffer[nextBuffer], format, data, size, freq);
#endif
#ifdef MACOS
   ALsizei size, freq;
   ALenum format;
   ALvoid *data;
   
   alutLoadWAVFile(filename, &format, &data, &size, &freq);
   alBufferData (buffer[nextBuffer], format, data, size, freq);
#endif
#ifdef LINUX
   ALsizei size, freq, bits;
   ALenum format;
   ALvoid *data;
   ALboolean err;
   
   err = alutLoadWAV(filename, &data, &format, &size, &bits, &freq);
   if(err == AL_FALSE) {
	   fprintf(stderr, "Could not load %s\n", filename);
	   return 0;
   }

   alBufferData (buffer[nextBuffer], format, data, size, freq);
#endif   

   // set static source properties
   alSourcei(source[nextSource], AL_BUFFER, buffer[nextBuffer]);
   alSourcei(source[nextSource], AL_LOOPING, 1);
   alSourcef(source[nextSource], AL_REFERENCE_DISTANCE, 10);

   nextBuffer++;
   nextSource++;
   return nextBuffer;
}

// Playfile -- loads a file into a buffer and source, then plays it
int AudioEnv::PlayFile (char *filename, bool loop)
{
	int loadhandle;

	loadhandle = LoadFile(filename, loop);

	if (loadhandle != 0) { 
		Play(loadhandle); 
	} else
	{
		return 0;
	}

	return loadhandle;
}

//SetSourcePosition
void AudioEnv::SetSourcePosition (int handle, float *position)
{
	alSourcefv(source[handle-1], AL_POSITION, position);
}

//SetSourceVelocity
void AudioEnv::SetSourceVelocity (int handle, float *velocity)
{
	alSourcefv(source[handle-1], AL_VELOCITY, velocity);
}

// Play
void AudioEnv::Play(int handle)
{
	alSourcePlay(source[handle-1]); 
}

// Stop
void AudioEnv::Stop(int handle)
{
	alSourceStop(source[handle-1]);
}

// IncrementEnv -- steps through the global environment presets if available
int AudioEnv::IncrementEnv()
{
#ifdef USE_EAX
    // increment EAX environment if EAX is available
	EAXSet pfPropSet;
	EAXGet pfPropGet;
	long lEAXVal;
	static unsigned long ulEAXEnv = 0;
	if (EAXlevel != 0)
	{
		pfPropSet = (EAXSet) alGetProcAddress((ALubyte *)"EAXSet");
		if (pfPropSet != NULL)
		{
		    lEAXVal = -10000;
			pfPropSet(&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ROOM, 0, &lEAXVal, sizeof(long));
			ulEAXEnv += 1;
			if (ulEAXEnv >= EAX_ENVIRONMENT_COUNT) { ulEAXEnv = EAX_ENVIRONMENT_GENERIC; }
			pfPropSet(&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ENVIRONMENT, 0, &ulEAXEnv, sizeof(unsigned long));
		}
	}
	return (int) ulEAXEnv;
#else // not using EAX
	return 0;
#endif // USE_EAX
}



