#include "DopplerEffect.h"
#include <math.h>
#include <stdlib.h>

#define SpeedOfSound 340.0f

CDopplerEffect::CDopplerEffect(void)
{
}

CDopplerEffect::~CDopplerEffect(void)
{
}

short CDopplerEffect::Interpolate( short* source_array, float wished_index )
{
    int index = (int) floor( wished_index );
    float delta = wished_index - index;
    
    return (short) ( source_array[index] * ( 1.0f - delta )
		+ source_array[index+1] * ( delta ) );
}

void CDopplerEffect::ApplyDoppler( short *dst_signal, float ratio_start, float ratio_end, int frame_sample_count )
{
    //I add volume change to simulate the distance of the source. This volume control can be done in the audio card
    float scale = 30.0f;
    //Compute the speed on the axis
    CVector listener_source_axis;

    listener_source_axis = m_ListenerPosition;
    listener_source_axis -= m_SourcePosition;
    float volume = scale/listener_source_axis.GetLength();


    double delta_ratio = ( ratio_end - ratio_start )/ (double)frame_sample_count;
	double ratio = ratio_start;

	for( int i=0; i< frame_sample_count; i++, m_InputBufferPosition += ratio)
    {
	    dst_signal[i] = volume * Interpolate( m_Input->GetBuffer(), m_InputBufferPosition );
        ratio += delta_ratio;
    }
}

void CDopplerEffect::UpdateSpeed( float time_step )
{
    m_SourceOldSpeed = m_SourceSpeed;
    m_ListenerOldSpeed = m_ListenerSpeed;
    
    //Compute the speed on the axis
    CVector listener_source_axis;

    listener_source_axis = m_ListenerPosition;
    listener_source_axis -= m_SourcePosition;
    listener_source_axis.Normalize();

    //compute the projection of speed on the source-listener axis using sign convention of the article
    m_SourceSpeed = listener_source_axis.GetDotProduct( (m_SourcePosition-m_SourceOldPosition)/time_step );
    m_ListenerSpeed = - listener_source_axis.GetDotProduct( (m_ListenerPosition-m_ListenerOldPosition)/time_step );
}

void CDopplerEffect::Update( float time_step )
{
    //Compute the number of sample to computed this time
    short output_buffer[3000];
    m_OutputTimeLeft += time_step;
    int computed_sample = int(m_OutputTimeLeft * 44100);
    m_OutputTimeLeft -= computed_sample /(float) 44100;

    float old_ratio = (SpeedOfSound + m_ListenerOldSpeed)/(SpeedOfSound - m_SourceOldSpeed);
    float ratio = (SpeedOfSound + m_ListenerSpeed) /(SpeedOfSound - m_SourceSpeed);

    //take the biggest ratio, and compute if there are enough sample 
    float higher_ratio = ( old_ratio > ratio )?old_ratio: ratio;
 
    if( m_Input->GetSize() < m_InputBufferPosition + higher_ratio * computed_sample )
    {
        m_Finished = true;
        return;
    }

    ApplyDoppler( output_buffer, old_ratio , ratio , computed_sample);
    m_Output->AddBuffer( output_buffer, computed_sample );
}