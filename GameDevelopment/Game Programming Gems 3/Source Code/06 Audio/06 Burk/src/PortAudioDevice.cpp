/*
 * PortAudioDevice.c
 *
 * Author: Phil Burk, http://www.softsynth.com
 *
 * This program uses PortAudio, the Portable Audio Library.
 * For more information see: http://www.portaudio.com/
 *
 * Copyright (c) 2002 Phil Burk, http://www.softsynth.com
 *
 */

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "portaudio.h"
#include "PortAudioDevice.h"

/*******************************************************************/
/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int paCallback(   void *inputBuffer, void *outputBuffer,
                             unsigned long framesPerBuffer,
                             PaTimestamp outTime, void *userData )
{
    PortAudioDevice *paDevice = (PortAudioDevice *) userData;
    float *out = (float*)outputBuffer;
    unsigned long iBuffer;
    (void) outTime; /* Prevent unused variable warnings. */
    (void) inputBuffer;

/* Fill the audio buffer. */
    for( iBuffer=0; iBuffer<framesPerBuffer; iBuffer += GGSYNTH_FRAMES_PER_BLOCK )
    {
        paDevice->fillOutput( out, GGSYNTH_FRAMES_PER_BLOCK );
        out += GGSYNTH_FRAMES_PER_BLOCK * SAMPLES_PER_FRAME; 
    }
    return 0;
}

/*******************************************************************/
PortAudioDevice::PortAudioDevice()
{
    Pa_Initialize();
}
/*******************************************************************/
PortAudioDevice::~PortAudioDevice()
{
    Pa_Terminate();
}

/*******************************************************************/
PaError PortAudioDevice::start( double sampleRate )
{
    PaError err = Pa_OpenStream( &stream,

              paNoDevice,/* default input device */
              0,              /* no input */
              paFloat32,  /* 32 bit floating point input */
              NULL,

              Pa_GetDefaultOutputDeviceID(), /* default output device */
              2,          /* stereo output */
              paFloat32,  /* 32 bit floating point output */
              NULL,

              sampleRate,
              FRAMES_PER_BUFFER,
              0,              /* number of buffers, if zero then use default minimum */
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              paCallback,
              this );
    if( err != paNoError ) goto error;

    err = Pa_StartStream( stream );
    if( err != paNoError ) goto error;

    return paNoError;

error:
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
}

/*******************************************************************/
PaError PortAudioDevice::stop()
{
    PaError err = Pa_StopStream( stream );
    if( err != paNoError ) goto error;

    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto error;
    return paNoError;

error:
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
}



/*******************************************************************/
double PortAudioDevice::getCPULoad()
{
    return Pa_GetCPULoad( stream );
}
