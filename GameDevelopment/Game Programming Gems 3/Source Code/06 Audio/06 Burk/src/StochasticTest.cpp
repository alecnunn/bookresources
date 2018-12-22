/*
 * StochasticTest.c
 * Play various sounds using stochastic synthesis.
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
#include "Unit_MixingBus.h"
#include "Circuit_SonarPing.h"
#include "Circuit_Wind.h"
#include "Circuit_Rocket.h"
#include "Circuit_Helicopter.h"
#include "Circuit_Rain.h"

#define SAMPLE_RATE              (44100.0)

// set loudness of various sounds
#define SONAR_GAIN               (0.5f)
#define ROCKET_GAIN              (0.5f)
#define RAIN_GAIN                (0.7f)
#define WIND_GAIN                (0.5f)
#define HELICOPTER_GAIN          (0.5f)

// how much rain increases when 'R' hit
#define RAIN_INCREMENT           (0.0001f)

/*******************************************************************
 * Define a circuit classes that mixes various synthetic sounds.
 */
class StochasticTestCircuit
{
public:
	StochasticTestCircuit();
	~StochasticTestCircuit();

    void gen( void );

    // add various sound generating circuits
    bool                 enablePing;
    Circuit_SonarPing    ping;

    bool                 enableWind;
    Circuit_Wind         wind1;
    Circuit_Wind         wind2;

    bool                 enableRocket;
    Circuit_Rocket       rocket;

    bool                 enableHelicopter;
    Circuit_Helicopter  *helicopter;

    GGSynthFloat         rainRate;
    Circuit_Rain         rain;

    // mix sounds for each channel
    Unit_MixingBus       leftMixer;
    Unit_MixingBus       rightMixer;
private:
protected:
    
};

/*******************************************************************
 * Constructor, set default values.
 */
StochasticTestCircuit::StochasticTestCircuit()
{
    enablePing = false;
    ping.rate = 0.0f;
    enableWind = false;
    enableRocket = false;

    // define using diameter of UH-I Huey rotors
    helicopter = new Circuit_Helicopter( 48.0f ); 
    enableHelicopter = false;

    rainRate = 0.0f;
}

StochasticTestCircuit::~StochasticTestCircuit()
{
    delete helicopter;
}

/*******************************************************************
 * Generate samples and mix them together for output.
 */
void StochasticTestCircuit::gen( void )
{
    GGSynthFloat amplitude;
/* Clear mixer busses before adding together signals. */
    leftMixer.clear();
    rightMixer.clear();

/* Generate Sonar Ping. */
    ping.gen( SONAR_GAIN, 700.0f);
/* Add sonar ping to mix. */
    leftMixer.gen( ping.outputs, 0.7f );
    rightMixer.gen( ping.outputs, 0.3f );

/* Generate rocket */
    rocket.gen( enableRocket ? ROCKET_GAIN : 0.0f );
/* Add rocket to mix. */
    leftMixer.gen( rocket.outputs, 0.6f );
    rightMixer.gen( rocket.outputs, 0.4f );

/* Generate wind */
    amplitude = enableWind ? WIND_GAIN : 0.0f;
    wind1.gen( amplitude );
    wind2.gen( amplitude );
/* Add wind to center mix. */
    leftMixer.gen( wind1.outputs, 0.5f );
    rightMixer.gen( wind2.outputs, 0.5f );
    
/* Generate rain */
    rain.gen( RAIN_GAIN, rainRate );
/* Add rain to mix. */
    leftMixer.gen( rain.leftOutputs, 0.5f );
    rightMixer.gen( rain.rightOutputs, 0.5f );

/* Generate helicopter */
    amplitude = enableHelicopter ? WIND_GAIN : 0.0f;
    helicopter->gen( amplitude );

/* Add rocket to mix. */
    leftMixer.gen( helicopter->outputs, 0.4f );
    rightMixer.gen( helicopter->outputs, 0.6f );
}


class MyPortAudio : public PortAudioDevice
{
public:
    StochasticTestCircuit      *circuit;
    void fillOutput( float *outputBuffer, int framesPerBuffer );
};

/*******************************************************************/
/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
void MyPortAudio::fillOutput( float *outputBuffer, int framesPerBuffer )
{
// generate samples
    circuit->gen();

// Fill PortAudio buffer with output from circuit.
	for( int i=0; i<framesPerBuffer; i++ )
	{
		*outputBuffer++ = circuit->leftMixer.outputs[i];  // Left channel.
		*outputBuffer++ = circuit->rightMixer.outputs[i]; // Right channel.
	}
}

/*************************************************************
 * Print information on how to use this program.
 */
static void usage( void )
{
    printf("  p = toggle sonar (P)ing on and off\n");
    printf("  h = toggle (H)elicopter rotor on and off\n");
    printf("  e = toggle rocket (E)ngine on and off\n");
    printf("  w = toggle (W)ind on and off\n");
    printf("  r = decrease (R)ain rate\n");
    printf("  R = increase (R)ain rate\n");

    printf("  u = print CPU (U)sage\n");
    printf("  q = (Q)uit\n");
    printf("  ? = help\n");
}

/*******************************************************************/
int main( void );
int main( void )
{
	PaError          err;
    MyPortAudio      myPA;
    int c;

    printf("Stochastic Synthesis Demo for Game Gems III\n");
    printf("(C) 2002 Phil Burk, http://www.softsynth.com\n");

    usage();

    myPA.circuit =  new StochasticTestCircuit();

// setup audio interface
	err = myPA.start( SAMPLE_RATE );
	if( err != paNoError ) goto error;

// user interface loop
    do
	{
	// get key command
		printf("Hit p,h,r,R,w, u,q or ?: ");
		c = _getche();
		printf("\n");

	// process command
		switch( c )
		{
		case 'q': break;

		case 'p':
            myPA.circuit->enablePing = !myPA.circuit->enablePing;
            myPA.circuit->ping.rate = myPA.circuit->enablePing ? 0.5f : 0.0f;
            printf("SonarPing %s\n", myPA.circuit->enablePing ? "enabled" : "disabled" );
            break;

		case 'w':
            myPA.circuit->enableWind = !myPA.circuit->enableWind;
            break;

		case 'e':
            myPA.circuit->enableRocket = !myPA.circuit->enableRocket;
            break;

		case 'h':
            myPA.circuit->enableHelicopter = !myPA.circuit->enableHelicopter;
            break;

		case 'r':
            myPA.circuit->rainRate -= RAIN_INCREMENT;
            if( myPA.circuit->rainRate < 0.0f ) myPA.circuit->rainRate = 0.0f;
            printf("Rain rate = %f\n", myPA.circuit->rainRate );
            break;

		case 'R':
            myPA.circuit->rainRate += RAIN_INCREMENT;
            printf("Rain rate = %f\n", myPA.circuit->rainRate );
            break;

		case 'u':
            printf("CPU Usage = %6.3f%c\n", myPA.getCPULoad() * 100.0, '%' );
            break;

		default: printf("Unrecognized command.");
		case '?': usage(); break;
        }
	} while( c != 'q' );

	err = myPA.stop();
	if( err != paNoError ) goto error;

	printf("Demo complete.\n" );
	fflush(stdout);

    delete myPA.circuit;

	return 0;

error:
	return 1;	
}
