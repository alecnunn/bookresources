
#include "stdafx.h"

#include "DSPdemo.h"
#include "cDspFDN4.h"
#include "cSoundBuffer.h"

#include <assert.h>
#include <math.h>


cDspFDN4::cDspFDN4()
{
	m_status = UNINIT;
}

cDspFDN4::~cDspFDN4()
{
	Uninit();
}


void cDspFDN4::Init( unsigned *newlengths )
{
	Uninit();

	for( unsigned i = 0; i < 4; i++ )
	{
		// choose an appropriate power of 2 for the memory buffer 
		// with 50% leeway for sweeping
		m_length[i] = newlengths[i];
		m_size[i] = 1;
		while( m_size[i] < 3 * m_length[i] / 2 ) 
			m_size[i] <<= 1;

		m_mask[i] = m_size[i] - 1;
		m_line[i] = new float[ m_size[i] ];
		for( unsigned j = 0; j < m_size[i]; j++ )
			m_line[i][j] = 0;

		m_write[i] = 0;
		m_read[i] = m_size[i] - m_length[i];
		m_p[i] = 0;
		m_dp[i] = 0;
		m_g[i] = 0;
		m_beta[i] = 0;
		m_alpha[i] = 0;
		m_filter[i] = 0;
	}

	m_dry = 0;
	m_wet = 0;

	m_sweepcounter = 0;
	m_sweepdivisor = 0;
	m_sweepdepth = 0;
	m_framecounter = 0;

	m_status = INIT;
}


void cDspFDN4::Uninit()
{
	if( m_status == INIT )
	{
		for( unsigned i = 0; i < 4; i++ )
			delete[] m_line[i];
	}

	m_status = UNINIT;
}


void cDspFDN4::SetHalflives( float halflife_LF, float halflife_HF )
{
	for( unsigned i = 0; i < 4; i++ )
	{
		m_g[i] = powf( 2, - float( m_length[i] ) / halflife_LF );
		m_beta[i] = powf( 2, - float( m_length[i] ) / halflife_HF ) / m_g[i];
		m_alpha[i] = 2 * m_beta[i] / ( 1 + m_beta[i] );
		m_alpha_orig[i] = m_alpha[i];
	}
}


static const float matrices[][4][4] = 
{
  // householder 0
  {
	{  1, -1, -1, -1 }, 
	{ -1,  1, -1, -1 }, 
	{ -1, -1,  1, -1 }, 
	{ -1, -1, -1,  1 }, 
  },

  // householder 1
  {
    { -1,  1, -1, -1, },
    { -1, -1,  1, -1, },
    { -1, -1, -1,  1, },
    {  1, -1, -1, -1, },
  },

  // householder 2
  {
    { -1, -1,  1, -1, },
    { -1, -1, -1,  1, },
    {  1, -1, -1, -1, },
    { -1,  1, -1, -1, },
  },

  // householder 3
  {
    { -1, -1, -1,  1, },
    {  1, -1, -1, -1, },
    { -1,  1, -1, -1, },
    { -1, -1,  1, -1, },
  },

  // householder 4
  {
    { -1,  1, -1, -1, },
    {  1, -1, -1, -1, },
    { -1, -1, -1,  1, },
    { -1, -1,  1, -1, },
  },

  // householder 5
  {
    {  1, -1, -1, -1, },
    { -1, -1,  1, -1, },
    { -1,  1, -1, -1, },
    { -1, -1, -1,  1, },
  },

  // householder 6
  {
    { -1, -1,  1, -1, },
    {  1, -1, -1, -1, },
    { -1, -1, -1,  1, },
    { -1,  1, -1, -1, },
  },

  // householder 7
  {
    {  1,  1, -1,  1, },
    { -1,  1,  1,  1, },
    {  1,  1,  1, -1, },
    {  1, -1,  1,  1, },
  },
  
  // stautner 0
  {
    {  0,  1,  1,  0, },
    { -1,  0,  0, -1, },
    {  1,  0,  0, -1, },
    {  0,  1, -1,  0, },
  },

  // stautner 1
  {
    {  0,  1,  1,  0, },
    { -1,  0,  0, -1, },
    {  0,  1, -1,  0, },
    {  1,  0,  0, -1, },
  },

  // stautner 2
  {
    {  1,  0,  1,  0, },
    {  0, -1,  0, -1, },
    {  1,  0, -1,  0, },
    {  0,  1,  0, -1, },
  },
  
  // stautner 3
  {
    {  1,  0,  1,  0, },
    {  0, -1,  0, -1, },
    {  0,  1,  0, -1, },
    {  1,  0, -1,  0, },
  },
  
  // hadamard 0
  {
    {  1,  1,  1,  1, },
    { -1,  1, -1,  1, },
    { -1, -1,  1,  1, },
    {  1, -1, -1,  1, },
  },

  // hadamard 1
  {
    {  1,  1,  1,  1, },
    {  1, -1,  1, -1, },
    {  1,  1, -1, -1, },
    {  1, -1, -1,  1, },
  },

  // hadamard 2
  {
    {  1, -1, -1,  1, },
    {  1,  1, -1, -1, },
    {  1, -1,  1, -1, },
    {  1,  1,  1,  1, },
  },

  // hadamard 3
  {
    {  1, -1, -1,  1, },
    { -1, -1,  1,  1, },
    { -1,  1, -1,  1, },
    {  1,  1,  1,  1, },
  },

  // positive identity
  {
    {  1,  0,  0,  0, },
    {  0,  1,  0,  0, },
    {  0,  0,  1,  0, },
    {  0,  0,  0,  1, },
  },

  // tiny rotation
  {
    0.980416164f, -0.121205295f, -0.121205295f, -0.096964236f,
    0.096964236f,  0.983109615f, -0.121205295f, -0.096964236f,
    0.121205295f,  0.096964236f,  0.980416164f, -0.121205295f,
    0.121205295f,  0.096964236f,  0.096964236f,  0.983109615f,
  },
 
  // little rotation
  {
    0.8271971909f, -0.3741443293f, -0.3741443293f, -0.1891474501f,  
    0.1891474501f,  0.8879540606f, -0.3741443293f, -0.1891474501f,  
    0.3741443293f,  0.1891474501f,  0.8271971909f, -0.3741443293f,  
    0.3741443293f,  0.1891474501f,  0.1891474501f,  0.8879540606f,  
  },

  // medium rotation
  {
    0.4027342996f, -0.7482592774f, -0.5147750269f, -0.1136640546f,  
    0.1136640546f,  0.6111487307f, -0.7482592774f, -0.2317012589f,  
    0.5147750269f,  0.1136640546f,  0.4027342996f, -0.7482592774f,  
    0.7482592774f,  0.2317012589f,  0.1136640546f,  0.6111487307f,  
  } ,
  
  // negative identity
  {
    { -1,  0,  0,  0, },
    {  0, -1,  0,  0, },
    {  0,  0, -1,  0, },
    {  0,  0,  0, -1, },
  },
  
  // negative tiny rotation
  {
    -0.980416164f,  0.121205295f,  0.121205295f,  0.096964236f,
    -0.096964236f, -0.983109615f,  0.121205295f,  0.096964236f,
    -0.121205295f, -0.096964236f, -0.980416164f,  0.121205295f,
    -0.121205295f, -0.096964236f, -0.096964236f, -0.983109615f,
  },

  // negative little rotation
  {
    -0.8271971909f,  0.3741443293f,  0.3741443293f,  0.1891474501f,  
    -0.1891474501f, -0.8879540606f,  0.3741443293f,  0.1891474501f,  
    -0.3741443293f, -0.1891474501f, -0.8271971909f,  0.3741443293f,  
    -0.3741443293f, -0.1891474501f, -0.1891474501f, -0.8879540606f,  
  },

  // negative medium rotation
  {
    -0.4027342996f,  0.7482592774f,  0.5147750269f,  0.1136640546f,  
    -0.1136640546f, -0.6111487307f,  0.7482592774f,  0.2317012589f,  
    -0.5147750269f, -0.1136640546f, -0.4027342996f,  0.7482592774f,  
    -0.7482592774f, -0.2317012589f, -0.1136640546f, -0.6111487307f,  
  } ,
};

static const float unitize[] = 
{
  // 1/2 for Householder reflections
  .5, .5, .5, .5, 
  .5, .5, .5, .5,
  
  // 1/sqrt(2) for Stautner matrices
  0.70710678f,
  0.70710678f,
  0.70710678f,
  0.70710678f,
  
  // 1/2 for Hadamard matrices
  0.5,
  0.5,
  0.5,
  0.5,
  
  // 1 for identities and rotations
  1, 1, 1, 1, 1, 1, 1, 1
};


void cDspFDN4::SetFeedbackMatrix( unsigned which )
{
	for( unsigned i = 0; i < 4; i++ )
		for( unsigned j = 0; j < 4; j++ )
			m_A[i][j] = unitize[ which ] * matrices[ which ][i][j];
}

void cDspFDN4::SetInOutRotations( float in, float out )
{
	const float deg2rad = 0.01745329f;

	m_c[0][0] = sinf( deg2rad * ( in + 0 ) );
	m_c[0][1] = sinf( deg2rad * ( in + 90 ) );
	m_c[0][2] = sinf( deg2rad * ( in + 180 ) );
	m_c[0][3] = sinf( deg2rad * ( in + 270 ) );
	m_c[1][0] = cosf( deg2rad * ( in + 0 ) );
	m_c[1][1] = cosf( deg2rad * ( in + 90 ) );
	m_c[1][2] = cosf( deg2rad * ( in + 180 ) );
	m_c[1][3] = cosf( deg2rad * ( in + 270 ) );

	m_d[0][0] = .5f * sinf( deg2rad * ( out + 0 ) );
	m_d[0][1] = .5f * cosf( deg2rad * ( out + 0 ) );
	m_d[1][0] = .5f * sinf( deg2rad * ( out + 90 ) );
	m_d[1][1] = .5f * cosf( deg2rad * ( out + 90 ) );
	m_d[2][0] = .5f * sinf( deg2rad * ( out + 180 ) );
	m_d[2][1] = .5f * cosf( deg2rad * ( out + 180 ) );
	m_d[3][0] = .5f * sinf( deg2rad * ( out + 270 ) );
	m_d[3][1] = .5f * cosf( deg2rad * ( out + 270 ) );
}


void cDspFDN4::SetMixes( float dry, float wet )
{
	m_dry = dry;
	m_wet = wet;
}


void cDspFDN4::SetModulation( float period, float depth )
{
	m_sweepdivisor = int( 1000.f * period / 6282.f );

	if( m_sweepdivisor == 0 )
	{
		m_sweepcounter = 0;
		m_sweepdepth = 0;
		m_framecounter = 0;
	}

	else
	{
		m_sweepcounter = 0;
		m_sweepdepth = 0.01f * depth;

		if( m_sweepdepth > 0.49f )
			m_sweepdepth = 0.49f;

		m_framecounter = 0;

	}

	// reset partial cursors
	for( unsigned i = 0; i < 4; i++ )
	{
		m_p[i] = 0;
		m_dp[i] = 0;
	}
}



static const float phasetable[] = { 0, 1.57f, 3.14f, 4.71f };

void cDspFDN4::SweepingModulation()
{
	m_framecounter++;

	for( unsigned i = 0; i < 4; i++ )
	{
		// calcluate actual and new delay length
		float actlen = float( ( m_size[i] + m_write[i] - m_read[i] ) & m_mask[i] );        
		float newlen = float( m_length[i] ) * ( 1 + m_sweepdepth * ( sinf( m_framecounter + phasetable[i] ) ) );

		// calculate sweeping velocity: 
		// reach the new length in m_sweepcounter number of samples
		m_dp[i] = - ( newlen - actlen ) / float( m_sweepcounter );

		// heurisic: boost the high frequency gain dependent of the sweeping velocity
		// saturate at a factor of 1.75, but stay proportional to dp[i]^2 for very small dp[i]
		float newbeta = m_beta[i] * ( 1 + 0.7f * ( m_dp[i] * m_dp[i] ) / ( 1e-9f + ( m_dp[i] * m_dp[i] ) ) );
		m_alpha[i] = 2 * newbeta / ( 1 + newbeta );
	}
}



void cDspFDN4::Process( cSoundBuffer<float> &buffer )
{
	// This is the mono version 

	if( m_status == INIT )
	{
		unsigned numsamples = buffer.GetSize();
		float *samples = buffer.GetData();

		for( unsigned i = 0; i < numsamples; i++ )
		{
			// update the sweeping modulation every n samples 
			if( m_sweepdepth && m_sweepcounter == 0 )
			{    
				m_sweepcounter = m_sweepdivisor;
				SweepingModulation();
			}
			m_sweepcounter--;
			
			// get input samples
			float input = samples[i];
			
			// step 1a: add velocity and bump integer cursor on occasion
			m_p[0] += m_dp[0];
			m_p[1] += m_dp[1];
			m_p[2] += m_dp[2];
			m_p[3] += m_dp[3];
			m_read[0] = ( m_read[0] + int( floorf( m_p[0] ) ) ) & m_mask[0];
			m_read[1] = ( m_read[1] + int( floorf( m_p[1] ) ) ) & m_mask[1];
			m_read[2] = ( m_read[2] + int( floorf( m_p[2] ) ) ) & m_mask[2];
			m_read[3] = ( m_read[3] + int( floorf( m_p[3] ) ) ) & m_mask[3];
			m_p[0] -= floorf( m_p[0] );
			m_p[1] -= floorf( m_p[1] );
			m_p[2] -= floorf( m_p[2] );
			m_p[3] -= floorf( m_p[3] );

			// step 1b: read from delay lines with linear interpolation
			// and advance read cursor
			float r[4];
			r[0] = m_line[0][ m_read[0] ];
			r[1] = m_line[1][ m_read[1] ];
			r[2] = m_line[2][ m_read[2] ];
			r[3] = m_line[3][ m_read[3] ];
			m_read[0] = ( m_read[0] + 1 ) & m_mask[0];
			m_read[1] = ( m_read[1] + 1 ) & m_mask[1];
			m_read[2] = ( m_read[2] + 1 ) & m_mask[2];
			m_read[3] = ( m_read[3] + 1 ) & m_mask[3];
			r[0] += m_p[0] * ( m_line[0][ m_read[0] ] - r[0] );
			r[1] += m_p[1] * ( m_line[1][ m_read[1] ] - r[1] );
			r[2] += m_p[2] * ( m_line[2][ m_read[2] ] - r[2] );
			r[3] += m_p[3] * ( m_line[3][ m_read[3] ] - r[3] );

			// step 2: apply output matrix
			float output = 
				r[0] * m_d[0][0] + r[1] * m_d[1][0] + r[2] * m_d[2][0] + r[3] * m_d[3][0] +
				r[0] * m_d[0][1] + r[1] * m_d[1][1] + r[2] * m_d[2][1] + r[3] * m_d[3][1];

			// step 3: apply feedback matrix
			float w[4];
			w[0] = r[0] * m_A[0][0] + r[1] * m_A[1][0] + r[2] * m_A[2][0] + r[3] * m_A[3][0];
			w[1] = r[0] * m_A[0][1] + r[1] * m_A[1][1] + r[2] * m_A[2][1] + r[3] * m_A[3][1];
			w[2] = r[0] * m_A[0][2] + r[1] * m_A[1][2] + r[2] * m_A[2][2] + r[3] * m_A[3][2];
			w[3] = r[0] * m_A[0][3] + r[1] * m_A[1][3] + r[2] * m_A[2][3] + r[3] * m_A[3][3];

			// step 4: apply input matrix and add
			w[0] += input * ( m_c[0][0] + m_c[1][0] );
			w[1] += input * ( m_c[0][1] + m_c[1][1] );
			w[2] += input * ( m_c[0][2] + m_c[1][2] );
			w[3] += input * ( m_c[0][3] + m_c[1][3] );

			// step 5: run through gain and filter
			w[0] = m_g[0] * ( m_filter[0] += m_alpha[0] * ( w[0] - m_filter[0]  ) );
			w[1] = m_g[1] * ( m_filter[1] += m_alpha[1] * ( w[1] - m_filter[1] ) );
			w[2] = m_g[2] * ( m_filter[2] += m_alpha[2] * ( w[2] - m_filter[2] ) );
			w[3] = m_g[3] * ( m_filter[3] += m_alpha[3] * ( w[3] - m_filter[3] ) );

			// step 6: write to delay lines
			m_line[0][ m_write[0] ] = w[0];
			m_line[1][ m_write[1] ] = w[1];
			m_line[2][ m_write[2] ] = w[2];
			m_line[3][ m_write[3] ] = w[3];

			// step 7: advance write cursors
			m_write[0] = ( m_write[0] + 1 ) & m_mask[0];	
			m_write[1] = ( m_write[1] + 1 ) & m_mask[1];
			m_write[2] = ( m_write[2] + 1 ) & m_mask[2];
			m_write[3] = ( m_write[3] + 1 ) & m_mask[3];

			// write output samples
			samples[i] = m_wet * output + m_dry * input;
		}	
	}
}




void cDspFDN4::ProcessStereo( cSoundBuffer<float> &bufferL, cSoundBuffer<float> &bufferR )
{
	if( m_status == INIT )
	{
		unsigned numsamples = bufferL.GetSize();
		float *samplesL = bufferL.GetData();
		float *samplesR = bufferR.GetData();

		for( unsigned i = 0; i < numsamples; i++ )
		{
			// update the sweeping modulation every n samples 
			if( m_sweepdepth && m_sweepcounter == 0 )
			{    
				m_sweepcounter = m_sweepdivisor;
				SweepingModulation();
			}
			m_sweepcounter--;
			
			// get input samples
			float inputL = samplesL[i];
			float inputR = samplesR[i];
			
			// step 1a: add velocity and bump integer cursor on occasion
			m_p[0] += m_dp[0];
			m_p[1] += m_dp[1];
			m_p[2] += m_dp[2];
			m_p[3] += m_dp[3];
			m_read[0] = ( m_read[0] + int( floorf( m_p[0] ) ) ) & m_mask[0];
			m_read[1] = ( m_read[1] + int( floorf( m_p[1] ) ) ) & m_mask[1];
			m_read[2] = ( m_read[2] + int( floorf( m_p[2] ) ) ) & m_mask[2];
			m_read[3] = ( m_read[3] + int( floorf( m_p[3] ) ) ) & m_mask[3];
			m_p[0] -= floorf( m_p[0] );
			m_p[1] -= floorf( m_p[1] );
			m_p[2] -= floorf( m_p[2] );
			m_p[3] -= floorf( m_p[3] );

			// step 1b: read from delay lines with linear interpolation
			// and advance read cursor
			float r[4];
			r[0] = m_line[0][ m_read[0] ];
			r[1] = m_line[1][ m_read[1] ];
			r[2] = m_line[2][ m_read[2] ];
			r[3] = m_line[3][ m_read[3] ];
			m_read[0] = ( m_read[0] + 1 ) & m_mask[0];
			m_read[1] = ( m_read[1] + 1 ) & m_mask[1];
			m_read[2] = ( m_read[2] + 1 ) & m_mask[2];
			m_read[3] = ( m_read[3] + 1 ) & m_mask[3];
			r[0] += m_p[0] * ( m_line[0][ m_read[0] ] - r[0] );
			r[1] += m_p[1] * ( m_line[1][ m_read[1] ] - r[1] );
			r[2] += m_p[2] * ( m_line[2][ m_read[2] ] - r[2] );
			r[3] += m_p[3] * ( m_line[3][ m_read[3] ] - r[3] );

			// step 2: apply output matrix
			float outputL = r[0] * m_d[0][0] + r[1] * m_d[1][0] + r[2] * m_d[2][0] + r[3] * m_d[3][0];
			float outputR = r[0] * m_d[0][1] + r[1] * m_d[1][1] + r[2] * m_d[2][1] + r[3] * m_d[3][1];

			// step 3: apply feedback matrix
			float w[4];
			w[0] = r[0] * m_A[0][0] + r[1] * m_A[1][0] + r[2] * m_A[2][0] + r[3] * m_A[3][0];
			w[1] = r[0] * m_A[0][1] + r[1] * m_A[1][1] + r[2] * m_A[2][1] + r[3] * m_A[3][1];
			w[2] = r[0] * m_A[0][2] + r[1] * m_A[1][2] + r[2] * m_A[2][2] + r[3] * m_A[3][2];
			w[3] = r[0] * m_A[0][3] + r[1] * m_A[1][3] + r[2] * m_A[2][3] + r[3] * m_A[3][3];

			// step 4: apply input matrix and add
			w[0] += inputL * m_c[0][0] + inputR * m_c[1][0];
			w[1] += inputL * m_c[0][1] + inputR * m_c[1][1];
			w[2] += inputL * m_c[0][2] + inputR * m_c[1][2];
			w[3] += inputL * m_c[0][3] + inputR * m_c[1][3];

			// step 5: run through gain and filter
			w[0] = m_g[0] * ( m_filter[0] += m_alpha[0] * ( w[0] - m_filter[0]  ) );
			w[1] = m_g[1] * ( m_filter[1] += m_alpha[1] * ( w[1] - m_filter[1] ) );
			w[2] = m_g[2] * ( m_filter[2] += m_alpha[2] * ( w[2] - m_filter[2] ) );
			w[3] = m_g[3] * ( m_filter[3] += m_alpha[3] * ( w[3] - m_filter[3] ) );

			// step 6: write to delay lines
			m_line[0][ m_write[0] ] = w[0];
			m_line[1][ m_write[1] ] = w[1];
			m_line[2][ m_write[2] ] = w[2];
			m_line[3][ m_write[3] ] = w[3];

			// step 7: advance write cursors
			m_write[0] = ( m_write[0] + 1 ) & m_mask[0];	
			m_write[1] = ( m_write[1] + 1 ) & m_mask[1];
			m_write[2] = ( m_write[2] + 1 ) & m_mask[2];
			m_write[3] = ( m_write[3] + 1 ) & m_mask[3];

			// write output samples
			samplesL[i] = m_wet * outputL + m_dry * inputL;
			samplesR[i] = m_wet * outputR + m_dry * inputR;
		}	
	}
}


float cDspFDN4::GetActualLength( unsigned i ) const
{
	return float( ( m_size[i] + m_write[i] - m_read[i] ) & m_mask[i] ) - m_p[i];

}












