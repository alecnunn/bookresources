// linearpredictor.cpp
//
// LinearPredictor class

#include "linearpredictor.h"
#include <stdlib.h>

// LinearPredictor
//
// order = order of predictor

LinearPredictor::LinearPredictor(int order) : order(order)
{
#if ENCODE
	AC = new float[order+1];	// not used outside GetFilter
	G = new float[order];		// ditto
	H = new float[order];		// ditto
	IFS = new float[order];		// state needed between frames
#endif
	FFS = new float[order+1];	// state needed between frames

	Reset();
}

// ~LinearPredictor

LinearPredictor::~LinearPredictor()
{
#if ENCODE
	delete AC;
	delete G;
	delete H;
	delete IFS;
#endif
	delete FFS;
}

// Reset
//
// reset to begin new input sequence

void LinearPredictor::Reset()
{
#if ENCODE
	InitInverseFilter();
#endif
	InitForwardFilter();
}

#if ENCODE
// autocorrelation
//
// compute the autocorrelation of a signal, where
// x[i] = 0 for i < 0 or i >= n
//
// IN	x[0...n-1]		samples
// OUT	ac[0...lag-1]	autocorrelation

static void autocorrelation(const float* x, int n, float* ac, int lag)
{
    while (lag--)
	{
		float	d = 0;

        for (int ii = lag; ii < n; ii++)
		{
			d += x[ii] * x[ii - lag];
		}

        ac[lag] = d;
    }
}

// GetFilter
//
// find the reflection coefficients that provide the least-square
// residual
//
// IN	x[0...n-1]		input frame
// OUT	rc[0...p-1]		reflection coefficients
//
// returns minimum mean-square error

float LinearPredictor::GetFilter(const float* x, int n, float* rc)
{
	// get autocorrelation vector
	autocorrelation(x, n, AC, order + 1);

	float	error = AC[0];

    if (error == 0)
	{
		// no signal
        for (int ii = 0; ii < order; ii++)
		{
			rc[ii] = 0;
		}
    }
	else
	{
		// Schur's algorithm
		int	ii;
		
		// initialize G & H
		for (ii = 0; ii < order; ii++)
		{
			G[ii] = H[ii] = AC[ii + 1];
		}

		// one iteration per coefficient
		for (ii = 0; ii < order; ii++)
		{
			// calculate rc and update error
			float r = -H[0] / error;

			rc[ii] = r;
			error += H[0] * r;

			// update G & H
			for (int m = 0; m < order - ii; m++)
			{
				H[m] = H[m + 1] + r * G[m];
				G[m] = H[m + 1] * r + G[m];
			}
		}
    }

	return(error);
}

// InitInverseFilter
//
// clear inverse filter state

void LinearPredictor::InitInverseFilter()
{
	for (int ii = 0; ii < order; ii++)
	{
		IFS[ii] = 0;
	}
}

// InverseFilter
//
// inverse-filter the input samples according to the reflection
// coefficients
// 
// IN	x[0...n-1]		signal
// IN	rc[0...p-1]		reflection coefficients
// OUT	res[0...n-1]	residual

void LinearPredictor::InverseFilter(const float* x, int n, const float* rc, float* res)
{
	while (n--)
	{
		float	s = *x++;
		float	ifsp = s;

		for (int ii = 0; ii < order; ii++)
		{
			// IFS'[ii + 1] = IFS[ii] + s * rc[ii]
			// s += IFS[ii] * rc[ii]
			float	tmp = IFS[ii] + s * rc[ii];

			s += IFS[ii] * rc[ii];

			IFS[ii] = ifsp;
			ifsp = tmp;
		}
		*res++ = s;
	}
}

// EncodeFrame
//
// encode a frame of audio
//
// IN	x[0...n-1]		signal
// OUT	rc[0...p-1]		reflection coefficients
// OUT	res[0...n-1]	residual

float LinearPredictor::EncodeFrame(const float* x, int n, float* rc, float* res)
{
	float error = GetFilter(x, n, rc);

	InverseFilter(x, n, rc, res);

	return error;
}
#endif	// ENCODE

// InitForwardFilter
//
// clear forward filter state

void LinearPredictor::InitForwardFilter()
{
	for (int ii = 0; ii <= order; ii++)
	{
		FFS[ii] = 0;
	}
}

// ForwardFilter
//
// forward-filter the input samples according to the reflection
// coefficients
//
// OUT	x[0...n-1]		signal
// IN	rc[0...p-1]		reflection coefficients
// IN	res[0...n-1]	residual

void LinearPredictor::ForwardFilter(float* x, int n, const float* rc, const float* res)
{
	while (n--)
	{
		float	s = *res++;

		for (int ii = order; ii--;)
		{
			// s -= FFS[ii] * rc[ii]
			// FFS'[ii + 1] = FFS[ii] + s * rc[ii]
			s -= FFS[ii] * rc[ii];
			FFS[ii + 1] = FFS[ii] + s * rc[ii];
		}
		*x++ = FFS[0] = s;
	}
}
