/*
 * BiquadTest.c
 * Sweep a biquad filter in various modes.
 *
 * Author: Phil Burk, http://www.softsynth.com
 *
 * This program uses PortAudio, the Portable Audio Library.
 * For more information see: http://www.portaudio.com/
 *
 * Copyright (c) 2002 Phil Burk
 *
 */

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************/
/*********************** Stochastic Synth Includes *****************/
/*******************************************************************/
#include "ggsynth.h"
#include "PortAudioDevice.h"
#include "Unit_WhiteNoise.h"
#include "Unit_ImpulseOscillator.h"
#include "Unit_SineOscillator.h"
#include "Unit_LowpassFilter.h"
#include "Unit_HighpassFilter.h"
#include "Unit_BandpassFilter.h"
#include "Unit_Multiply.h"

#define SAMPLE_RATE              (44100.0)

/*******************************************************************/

class BiquadTestCircuit
{
public:
	BiquadTestCircuit();
	~BiquadTestCircuit();

    void gen( void );
    void selectFilter( int letter );

    int           excitationSource;
    int           monitorSource;
    GGSynthFloat  oscFrequency;
    GGSynthFloat  filterQ;
    GGSynthFloat  filterAmplitude;
    GGSynthFloat  lfoFrequency;

    GGSynthFloat *outputs;

    const static int EXCITATION_NOISE;
    const static int EXCITATION_IMPULSE;
    const static int MONITOR_FILTER;
    const static int MONITOR_EXCITATION;

private:
protected:
    
    Unit_WhiteNoise        noise;
    Unit_ImpulseOscillator impulse;
    Unit_SineOscillator    lfo;
    Unit_LowpassFilter     lowpass;
    Unit_HighpassFilter    highpass;
    Unit_BandpassFilter    bandpass;
    Unit_Multiply          multiplier;

    Unit_BiquadFilter     *filter;
    GGSynthFloat          *excitationOutputs;
};


const int BiquadTestCircuit::EXCITATION_NOISE = 0;
const int BiquadTestCircuit::EXCITATION_IMPULSE = 1;
const int BiquadTestCircuit::MONITOR_FILTER = 0;
const int BiquadTestCircuit::MONITOR_EXCITATION = 1;

/*******************************************************************
 * Constructor, set default values.
 */
BiquadTestCircuit::BiquadTestCircuit()
{
    lfoFrequency = 0.1f; /* Hertz */
    oscFrequency = 50.0f; /* Hertz */
    filterQ = 4.0f;
    filterAmplitude = 1.0f;
    filter = &lowpass;
    excitationSource = EXCITATION_NOISE;
    monitorSource = MONITOR_FILTER;
}

BiquadTestCircuit::~BiquadTestCircuit()
{
}

/*******************************************************************
 * Choose a filter based on letter from user.
 */
void BiquadTestCircuit::selectFilter( int letter )
{
    switch( letter )
    {
    case 'l': filter = &lowpass; break;
	case 'h': filter = &highpass; break;
	case 'b': filter = &bandpass; break;
    }
}

/*******************************************************************
 * Generate samples and write them to output.
 */
void BiquadTestCircuit::gen( void )
{
/* Calculate LFO. */
    lfo.gen( 1000.0f, lfoFrequency );

/* Use control rate LFO signal to modulate filter frequency. */
    filter->calculateCoefficients( lfo.outputs[0] + 1100.0f, filterQ );

/* Select input (excitation) to filter. */
    if( excitationSource == EXCITATION_NOISE )
    {
        noise.gen( 0.5f );
        excitationOutputs = noise.outputs;
    }
    else if( excitationSource == EXCITATION_IMPULSE )
    {
        impulse.gen( 3.0f, oscFrequency );
        excitationOutputs = impulse.outputs;
    }

/* Calculate filter. */
	filter->gen( excitationOutputs );

/* Scale output of filter. */
	multiplier.gen( filter->outputs, filterAmplitude  );

/* Select signal to listen to. */
    if( monitorSource == MONITOR_EXCITATION )
    {
        outputs = excitationOutputs;
    }
    else
    {
        outputs = multiplier.outputs;
    }
}


class MyPortAudio : public PortAudioDevice
{
public:
    BiquadTestCircuit      *circuit;
    void fillOutput( float *outputBuffer, int framesPerBuffer );
};

/*******************************************************************/
/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
void MyPortAudio::fillOutput( float *outputBuffer, int framesPerBuffer )
{
    int i;

    circuit->gen();

/* Fill PortAudio buffer with output from circuit. */
	for( i=0; i<framesPerBuffer; i++ )
	{
		*outputBuffer++ = circuit->outputs[i];  /* Left channel. */
		*outputBuffer++ = circuit->outputs[i];  /* Right channel. */
	}
}

/*************************************************************
 * Prinite information on how to use this program.
 */
static void usage( void )
{
    printf("  n = use (N)oise as excitation for filter\n");
    printf("  i = use (I)mpulse as excitation for filter\n");

    printf("  l = use (L)owpass filter\n");
    printf("  h = use (H)ighpass filter\n");
    printf("  b = use (B)andpass filter\n");

    printf("  r = to reduce (R)esonance, /2\n");
    printf("  R = to increase (R)esonance, *2\n");

    printf("  a = to reduce filter (a)mplitude, /2\n");
    printf("  A = to increase filter (A)mplitude, *2\n");

    printf("  e = listen to (E)xcitation\n");
    printf("  f = listen to (F)ilter\n");

    printf("  u = print CPU (U)sage\n");
    printf("  q = (Q)uit\n");
    printf("  ? = help\n");
}

/*******************************************************************/
int main( void );
int main( void )
{
	PaError          err;
    MyPortAudio     myPA;
    int c;

    printf("Biquad Filter Demo for Game Gems III\n");
    printf("(C) 2002 Phil Burk, http://www.softsynth.com\n");

    usage();

/* Create Unit Generators */
    myPA.circuit =  new BiquadTestCircuit();

	err = myPA.start( SAMPLE_RATE );
	if( err != paNoError ) goto error;

	do
	{
	// get key command
		printf("Hit n,i, l,h,b, r,R, a,A, e,f, u,q or ?: ");
		c = _getche();
		printf("\n");
	// process command
		switch( c )
		{
		case 'q': break;

		case 'l': 
		case 'h':
		case 'b':
            myPA.circuit->selectFilter( c );
            break;

		case 'i': myPA.circuit->excitationSource = BiquadTestCircuit.EXCITATION_IMPULSE; break;
		case 'n': myPA.circuit->excitationSource = BiquadTestCircuit.EXCITATION_NOISE; break;

		case 'r':
            myPA.circuit->filterQ /= 2.0;
            printf("Resonance = %f\n", myPA.circuit->filterQ );
            break;

		case 'R':
            myPA.circuit->filterQ *= 2.0;
            printf("Resonance = %f\n", myPA.circuit->filterQ );
            break;

		case 'a':
            myPA.circuit->filterAmplitude /= 2.0;
            printf("FilterAmplitude = %f\n", myPA.circuit->filterAmplitude );
            break;

		case 'A':
            myPA.circuit->filterAmplitude *= 2.0;
            printf("FilterAmplitude = %f\n", myPA.circuit->filterAmplitude );
            break;

		case 'e': myPA.circuit->monitorSource = BiquadTestCircuit.MONITOR_EXCITATION; break;
		case 'f': myPA.circuit->monitorSource = BiquadTestCircuit.MONITOR_FILTER; break;

		case 'u':
            printf("CPU Usage = %6.3f%c\n", myPA.getCPULoad() * 100.0, '%' );
            break;

		default: printf("Unrecognized command.");
		case '?': usage(); break;
        }
        

	} while( c != 'q' );

	err = myPA.stop();
	if( err != paNoError ) goto error;

	printf("Test complete.\n" );
	fflush(stdout);

    delete myPA.circuit;

	return 0;

error:
	return 1;	
}
