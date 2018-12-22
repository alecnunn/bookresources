
#include "cDsp0Pole.h"
#include "cSoundBuffer.h"

cDsp0Pole::cDsp0Pole()
{
	m_ParameterA = 0;
}


void cDsp0Pole::SetParameterA( float param )
{
	m_ParameterA = param;
}

void cDsp0Pole::Process( cSoundBuffer<float> &buffer )
{
	float *samples = buffer.GetData();
	int numsamples = buffer.GetSize();
	
	for( int i = 0; i < numsamples; ++i )
		samples[i] = samples[i] * m_ParameterA;		
}
