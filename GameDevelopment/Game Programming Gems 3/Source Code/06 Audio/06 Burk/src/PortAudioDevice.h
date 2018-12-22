#ifndef _GAMEGEMS3_UNIT_PORTAUDIODEVICE_H
#define _GAMEGEMS3_UNIT_PORTAUDIODEVICE_H

/*
 * PortAudioDevice.h
 *
 * Author: Phil Burk, http://www.softsynth.com
 *
 * This program uses PortAudio, the Portable Audio Library.
 * For more information see: http://www.portaudio.com/
 *
 * Copyright (c) 2002 Phil Burk
 *
 */

#include "ggsynth.h"
#include "portaudio.h"

#define SAMPLES_PER_FRAME        (2) // stereo
#define FRAMES_PER_BUFFER        (32 * GGSYNTH_FRAMES_PER_BLOCK)

class PortAudioDevice
{
public:
	PortAudioDevice();
	~PortAudioDevice();

    PaError start( double sampleRate );
    PaError stop();

// users should redefine this method 
    virtual void fillOutput( float *outputBuffer, int numFrames ) = 0;
    double getCPULoad();

private:
    PortAudioStream  *stream;
};

#endif /* _GAMEGEMS3_UNIT_PORTAUDIODEVICE_H */
