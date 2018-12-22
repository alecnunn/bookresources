// linearpredictor.h
//
// LinearPredictor class

#ifndef LINEARPREDICTOR_H
#define LINEARPREDICTOR_H

class LinearPredictor
{
public:
	LinearPredictor(int order = 8);
	~LinearPredictor();

	const int	order;	// referred to as p in comments

#if ENCODE
	// find optimal filter rc[0...p-1]
	float	GetFilter(const float* x, int n, float* rc);

	// inverse-filter signal x[0...n-1] to residual res[0...n-1] with filter rc[0...p-1]
	// res and x can point to the same array
	void	InitInverseFilter();
	void	InverseFilter(const float* x, int n, const float* rc, float* res);

	// init encoder
	void	InitEncoder()	{ InitInverseFilter(); }

	// encode frame x[0...n-1] to residual res[0...n-1] plus rc[0...p-1]
	float	EncodeFrame(const float* x, int n, float* rc, float* res);
#endif

	// forward-filter signal res[0...n-1] with filter rc[0...p-1] to give x[0...n-1]
	// res and x can point to the same array
	void	InitForwardFilter();
	void	ForwardFilter(float* x, int n, const float* rc, const float* res);

	// reset filters
	void	Reset();

private:

#if ENCODE
	float*	AC;		// auto-correlation buffer
	float*	G;		// buffer for schur algorithm
	float*	H;		// buffer for schur algorithm
	float*	IFS;	// inverse filter state
#endif
	float*	FFS;	// forward filter state
};

#endif	// LINEARPREDICTOR_H
