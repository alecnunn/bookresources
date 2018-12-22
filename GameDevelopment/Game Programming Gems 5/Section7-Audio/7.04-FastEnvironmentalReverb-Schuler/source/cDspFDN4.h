
#ifndef DSP_FDN4_INCLUDED
#define DSP_FDN4_INCLUDED

template< typename >
class cSoundBuffer;

class cDspFDN4
{
protected:
	enum { INIT, UNINIT } m_status;

	float *m_line[4];			// memory buffer for each delay line
	unsigned m_size[4];			// size of each memory bufferr
	unsigned m_mask[4];			// bitmask for modulo operation
	unsigned m_length[4];		// length of each delay line
	unsigned m_write[4];		// write cursors
	unsigned m_read[4];			// read cursors	
	float m_A[4][4];			// feedback matrix
	float m_c[2][4];			// input matrix
	float m_d[4][2];			// output matrix
	float m_p[4];				// partial read cursor
	float m_dp[4];				// velocity
	float m_g[4];				// scalar feedback gains
	float m_beta[4];			// extra feedback gains for high freq
	float m_alpha[4];			// filter alphas
	float m_filter[4];			// filter state

	float m_dry;				// dry mix
	float m_wet;				// wet mix

	int m_sweepcounter;			// counts samples until next modulation update
	int m_sweepdivisor;			// number of samples between modulation updates
	float m_sweepdepth;			// use this fraction of the delay lengths for modulation
	float m_framecounter;		// counts modulation frames

	// feedback matrix library
	static float s_matrices[][4][4];	
	static float s_unitize[];			

	float m_alpha_orig[4];		// filter alphas before correction

	void SweepingModulation();

	friend class CPlugInFDN4;

public:

	enum 
	{
		house0 = 0, house1, house2, house3, house4, house5, house6, house7,
		staut0, staut1, staut2, staut3, hada0, hada1, hada2, hada3,
		plus, tinyrot, littlerot, mediumrot, minus, negtiny, neglittle, negmedium
	};

	float GetActualLength( unsigned ) const;

	void Init( unsigned *newlengths );
	void Uninit();
	void SetHalflives( float halflife_LF, float halflife_HF );
	void SetFeedbackMatrix( unsigned which );
	void SetInOutRotations( float in, float out );
	void SetMixes( float dry, float wet );
	void SetModulation( float speed, float depth );
	void Process( cSoundBuffer<float> &buffer );
	void ProcessStereo( cSoundBuffer<float> &bufferL, cSoundBuffer<float> &bufferR );
	cDspFDN4();
	~cDspFDN4();
};

#endif 



