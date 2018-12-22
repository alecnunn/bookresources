#ifndef  _CSYNUTIL_H
#define  _CSYNUTIL_H
/*
** CSyn Utilities for Windows, Macintosh (and Linux)
**
** This is a convenience layer built on top of CSyn,
** a Modular Cross-Platform Software Synthesizer.
** 
** NOTE: The CSynUtil library may only be used for evaluating
** the functionality of the CSyn Utilities. It may not be incorporated
** in a product, or redistributed by any means without a license
** from SoftSynth.com. For information on obtaining the latest version
** of the software, and for information on licensing, please visit:
** 
**    http://www.softsynth.com   
**
** This Audio library allows sounds to be loaded as AIFF files,
** and played using noteOn() and noteOff() functions similar to MIDI.
** The library supports multiple synthesis architectures.
** Each architecture can have multiple sounds, and multiple voices.
** This demo library currently supports two architectures:
**    
**  CIRCTYPE_BASIC_SAMPLER   = a plain variable rate sample player.
**  CIRCTYPE_AMPENV_SAMPLER  = a sampler plus a multi-stage amplitude envelope
**
** Author: Phil Burk
** Copyright 2001 SoftSynth.com, All Rights Reserved.
*/

#ifdef WIN32
#include <windows.h>
#else
#include <ctype.h>
#endif

#include "csyn.h"
#include "audio_sample.h"

/* Opaque type definitions for CSyn "objects" */
typedef void   *CSynMix;   /* Contains all of the architectures, sounds and voices. */
typedef void   *CSynSound; /* Contains data, for example a sample and envelope. */
typedef void   *CSynVoice; /* Can play one note of a sound. */

/*
** Fractional control parameters are specified in 17.15 fixed-point fractions
** to allow easy porting to machines that do not support floating point math.
*/
typedef int32  CSynFixedPoint;
#define ConvertIntegerToFixed(i)          ((i)<<15)
#define ConvertDoubleToFixed(d)           ( (int32) ((d)*(1<<15)) )
#define ConvertFixedToDouble(fxp)         ( (double) ((fxp)*(1.0/(1<<15))) )
#define ConvertPitchToFixed(pitch,cents)  ( ((pitch)<<15) + (((cents)<<15)/100) )

/* Use these to select a synthesis architecture when loading a sound. */
typedef enum CSynCircuitType
{
	CIRCTYPE_BASIC_SAMPLER = 0,
	CIRCTYPE_AMPENV_SAMPLER
} CSynCircuitType;

#define CSYNUTIL_MAX_ENV_FRAMES    (16)
typedef struct CSynEnvelopeInfo
{
	EnvelopeFrame *frames;  /* duration, value pairs */
	int            numFrames;
	int       onLoopStart;   /* Frame index of beginning of loop for noteOn, or -1 if no loop. */
	int       onLoopSize;    /* Number of frames in loop for noteOn. Zero for a sustain point. */
	int       offLoopStart;  /* Frame index of beginning of loop for noteOff, or -1 if no loop. */
	int       offLoopSize;   /* Number of frames in loop for noteOff. Zero for a sustain point. */
	int       flags;         /* Flags for controlling envelope. */
} CSynEnvelopeInfo;


#ifdef __cplusplus
extern "C" {
#endif

/* Create a mixing context that will be used by a group of sounds.
 */
CSynErr CSynMix_Create( CSynContext context, CSynMix *mixPtr, unsigned long flags);
CSynErr CSynMix_Delete( CSynMix mix);

/* Load an instrument, or sound effect description from a file.
** If the file is an AIFF or WAV sample file, then a simple sample
** playing voice will be created. AIFF samples contains a base frequency
** which allow the sample to be played in a tune.
*/
CSynErr CSynSound_Load( CSynMix mix, CSynCircuitType circType,
					   const char *fileName,  CSynSound *soundPtr );
CSynErr CSynSound_Delete( CSynSound sound);

/* Change the envelope definition for a CIRCTYPE_AMPENV_SAMPLER sound. */
CSynErr CSynSound_SetAmpEnvelope( CSynSound sound, CSynEnvelopeInfo *envInfo );

/* Get AudioSample structure associated with a sound, or NULL. */
AudioSample *CSynSound_GetAudioSample( CSynSound sound );

/* Instantiate a voice from a circuit type and an optional file.
** For CIRCTYPE_BASIC_SAMPLER and CIRCTYPE_AMPENV_SAMPLER,
** the fileName should be NULL.
** The fileName will be used for future circuit types that require a
** circuit definition file.
*/
CSynErr CSynVoice_Load( CSynMix mix, CSynCircuitType circType,
					   const char *fileName, CSynVoice *voicePtr );
/* Instantiate a voice from a sound description. */
CSynErr CSynVoice_Create( CSynSound sound, CSynVoice *voicePtr );
CSynErr CSynVoice_Delete( CSynVoice voice );

/* Set the sound to be used on a voice.
** CircuitType of new sound must match original sound's circuit type.
*/
CSynErr CSynVoice_SetSound( CSynVoice voice, CSynSound sound );

/* Pan is a 17.15 fixed-point fraction ranging from -1.0 for full left
** to +1.0 for full right.
*/
CSynErr CSynVoice_SetPan( CSynVoice voice, int time, CSynFixedPoint pan );

/*
** Convert a MIDI standard pitch to frequency in Hertz
** assuming a 12 tone equal tempered scale.
** Pitch is a 17.15 fixed-point fraction to allow micro-tuning.
*/
CSynFixedPoint CSynConvert_PitchToFrequency( CSynFixedPoint pitch );

/*
** Convert a MIDI standard velocity to a fixed point amplitude 
** ranging from 0.0 to 1.0.
*/
CSynFixedPoint CSynConvert_VelocityToAmplitude( CSynFixedPoint velocity );

/*
** Start synthesis of voice. Starts consuming CPU cycles.
** This is called by CSynVoice_NoteOn() so you do not have to.
*/
CSynErr CSynVoice_Start( CSynVoice voice, int time );

/*
** Stops synthesis of voice. Stops consuming CPU cycles.
** Note that generally a voice will stop automatically 
** when its sample or envelope finishes so you do not
** normally need to call this.
*/
CSynErr CSynVoice_Stop( CSynVoice voice, int time );

/* Control a voice using MIDI like commands.
** Frequency in Hertz is a 17.15 fixed-point fraction.
** If recorded at the same rate as the engine is running
** then a sample can be pitch-shifted down to zero,
** but can only be pitch-shifted up an octave.
** Calls CSynVoice_Start() internally.
*/
CSynErr CSynVoice_NoteOn( CSynVoice voice, int time,
						 CSynFixedPoint frequency,
						 CSynFixedPoint amplitude );

/* Change the frequency of a note after it is already playing. */
CSynErr CSynVoice_SetFrequency( CSynVoice voice, int time, CSynFixedPoint frequency );

/* Change the amplitude of a note after it is already playing. */
CSynErr CSynVoice_SetAmplitude( CSynVoice voice, int time, CSynFixedPoint amplitude );

/*
** Release any sustain loops and lets sound decay to silence.
** Passes back a time in CSyn ticks that it will take for the note to
** fully decay. This can be used by a voice allocator to determine
** when the voice can be reused.
*/
CSynErr CSynVoice_NoteOff( CSynVoice voice, int time, int *releaseTimePtr );


#ifdef __cplusplus
}
#endif

#endif /*  _CSYNUTIL_H  */
