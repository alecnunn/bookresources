#ifndef __AUDIO_SAMPLE_H
#define __AUDIO_SAMPLE_H

/****************************************************************************
**
**  Audio Sample Information Structure
**
** Author: Phil Burk
** Copyright 2001 SoftSynth.com, All Rights Reserved.
****************************************************************************/

#include "csyn.h"

/* -------------------- AudioSample */

typedef unsigned int PackedID;

/* This structure is returned by AudioSample_LoadAIFF() */
typedef struct AudioSample
{
        /* Data */
    void   *sampleData;              /* Points to first sample if data was loaded
                                    ** (AUDIOSAMPLE_FLAG_SKIP_DATA not set).
                                    ** This memory is allocated with malloc. */
    uint32  numFrames;         /* Size of sample data in frames */
    uint32  sampleDataSize;          /* Size of sample data in bytes */
    uint32  sampleDataOffset;        /* Byte position in AIFF file of first sample frame */
    uint8   flags;   
    uint8   reserved1[3];

        /* Format */
    uint8   channelsPerFrame;          /* channels per frame, 1 = mono, 2=stereo */
    uint8   bitsPerSample;              /* ORIGINAL bits per sample BEFORE any compression. */
    uint8   compressionRatio;  /* 2 = 2:1, 4 = 4:1 */
    uint8   reserved2[1];
    PackedID  compressionType;

        /* Loops (Frame number of loop point or -1) */
    int32   sustainBegin;      /* Set to -1 if no sustain loop */
    int32   sustainEnd;
    int32   releaseBegin;      /* Set to -1 if no release loop */
    int32   releaseEnd;

        /* Tuning */
    double  sampleRate;        /* Sample Rate recorded at */
    uint8   baseNote;          /* MIDI Note when played at original sample rate. */
    int8    detune;            /* Amount (in cents) to detune asmp_BaseNote to reach
                                    ** pitch when played at original sample rate. */
    uint8   reserved3[2];

        /* Multisample */
    uint8   lowNote;           /* Lowest MIDI note number for this sample */
    uint8   highNote;          /* Highest MIDI note number for this sample */
    uint8   lowVelocity;       /* Lowest MIDI velocity for this sample */
    uint8   highVelocity;      /* Highest MIDI velocity for this sample */

} AudioSample;

    /* asmp_Flags */
#define AUDIOSAMPLE_FLAG_SAMPLE_ALLOCATED   (1<<0)   /* When set, data pointed to by sampleData is
                                                ** automatically freed by DeleteAudioSample() */
#define AUDIOSAMPLE_FLAG_SKIP_DATA          (1<<1)

/* -------------------- Functions */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* low AIFF loader */
CSynErr  AudioSample_LoadAIFF( const char *fileName, uint32 flags, AudioSample **newAudioSample );
void AudioSample_Delete( AudioSample * );
void AudioSample_Dump( const AudioSample * );
void AudioSample_DeleteData (AudioSample *asmp);

#ifdef __cplusplus
}
#endif /* __cplusplus */


/*****************************************************************************/


#endif /* _AUDIO_SAMPLE_INFO_H */
