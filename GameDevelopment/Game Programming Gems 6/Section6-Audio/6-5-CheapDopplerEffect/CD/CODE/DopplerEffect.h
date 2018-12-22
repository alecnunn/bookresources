#pragma once

#include "Vector.h"
#include "WavWriter.h"
#include "WavRead.h"

class CDopplerEffect
{
public:
    CDopplerEffect(void);
    ~CDopplerEffect(void);

    void UpdateListenerPosition( const CVector& position )
    {
        m_ListenerOldPosition = m_ListenerPosition;
        m_ListenerPosition = position;
    }

    void SetListenerPosition(const CVector& position )
    {
        m_ListenerOldPosition = position;
        m_ListenerPosition = position;
    }

    void UpdateSourcePosition( const CVector& position )
    {
        m_SourceOldPosition = m_SourcePosition;
        m_SourcePosition = position;
    }

    void SetSourcePosition(const CVector& position )
    {
        m_SourceOldPosition = position;
        m_SourcePosition = position;
    }

    void SetInputBuffer( CWavReader* input  )
    {
        m_Input = input;
        m_InputBufferPosition = 0.0f;
        m_OutputTimeLeft = 0.0f;
        m_Finished = false;
    }

    void SetOutputBuffer( CWavWriter* output ) 
    {
        m_Output = output;
    }

    short Interpolate( short* source_array, float wished_index );

    void Update( float time_step );
    void UpdateSpeed( float time_step );

    bool HasFinished()
    {
        return m_Finished;
    }

    void ApplyDoppler( short *dst_signal, float ratio_start, float ratio_end, int frame_sample_count );

private:

    CVector m_ListenerPosition, m_ListenerOldPosition;
    CVector m_SourcePosition, m_SourceOldPosition;
    float m_SourceSpeed, m_SourceOldSpeed;
    float m_ListenerSpeed, m_ListenerOldSpeed;

    CWavReader* m_Input;
    CWavWriter* m_Output;
    double m_InputBufferPosition;
    float m_OutputTimeLeft; //store the time left from previous update
    bool m_Finished;
};
