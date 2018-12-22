
#ifndef DSP_0POLE_INCLUDED
#define DSP_0POLE_INCLUDED


template< typename >
class cSoundBuffer;

class cDsp0Pole
{
	float m_ParameterA;
public:
	void SetParameterA( float param );
	void Process( cSoundBuffer<float> &buffer );
	cDsp0Pole();
};

#endif 
