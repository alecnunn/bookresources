#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "Vector.h"
#include "DopplerEffect.h"
#include "WavRead.h"

CVector listener_position( 0, 30, 0 );
CVector source_position( -250, 0, 0 );
CVector source_speed( 100, 0,0 );
CDopplerEffect doppler_effect;
CWavReader wav_reader;
CWavWriter wav_writer;

static char filename[]="input.wav";
static char output_filename[]="output.wav";

//
void Init()
{
    if( !wav_reader.Open(filename) )
    {
        printf("Can't open file % : exiting\n", filename);
        exit(-1);
    }

    if( !wav_writer.Open( "output.wav" ) )
    {
        printf("Can't open output file %s : exiting\n", output_filename);
        exit(-1);
    }

    doppler_effect.SetInputBuffer( &wav_reader );
    doppler_effect.SetOutputBuffer( &wav_writer );
    doppler_effect.SetSourcePosition( source_position );
    doppler_effect.SetListenerPosition( listener_position );
}

// This function simulate the update of the entity
void Update( float time_step )
{
    source_position += source_speed * time_step;
    doppler_effect.UpdateSourcePosition( source_position );
    doppler_effect.UpdateListenerPosition( listener_position );

    doppler_effect.UpdateSpeed( time_step );
    doppler_effect.Update( time_step );
}

void main()
{
    Init();

    srand( (unsigned)time( NULL ) );

    float times = 0.0f;

    while( 1 )
    {
        float time_step = 0.040f ;// 25FPS

        //Make the timestep vary
        //time_step += 0.010f * ( 2.0f * (rand()/(float)RAND_MAX ) - 1.0f );

        times += time_step;

        if( times > 2.95f )
        {
            int test = 0;
        }

        Update( time_step );

        if( doppler_effect.HasFinished() )
        {
            wav_writer.Close();
            break;
        }
    }
}