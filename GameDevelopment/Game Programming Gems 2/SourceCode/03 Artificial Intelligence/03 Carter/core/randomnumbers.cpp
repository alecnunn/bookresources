#include "priv_coreprecompiled.h"

#include "randomnumbers.h"

RandomNumbers::RandomNumbers(RandType seed /*= 12345678*/)
	: m_currentSeed(seed)
{
}

void RandomNumbers::SetSeed(RandType newseed)
{
	m_currentSeed = newseed;
	srand(newseed);
}

RandomNumbers::RandType RandomNumbers::GetSeed() const
{
	return m_currentSeed;
}

// Various random number parameters, all assuming M = 2^32
// Taken from http://www.npac.syr.edu/users/paulc/lectures/montecarlo/node105.html

// VAX
//const RandomNumbers::RandType k_A = 69069;
//const RandomNumbers::RandType k_C = 0;	// Or 1

// Unix rand routine
const RandomNumbers::RandType k_A = 1103515245;
const RandomNumbers::RandType k_C = 12345;

// From the VisualC++ C run time source code
//const RandomNumbers::RandType k_A = 214013;
//const RandomNumbers::RandType k_C = 2531011;

RandomNumbers::RandType RandomNumbers::GetRandom() const
{
	// Linear congruential generator. All math is done
	// modulo M. In this case M is conveniently chosen to be a power of two.
	// k_C can be zero, but in that case you must be careful
	// not to seed the generator with zero.
	//RandType randres = rand();	// Handy for comparing to the built in rand().
	m_currentSeed = k_A * m_currentSeed + k_C;
	// The results of linear congruential generators tend to be
	// weakest in the low bits - the last bit will have a cycle
	// of length no greater than two, the last two bits will have a cycle
	// of length no greater than four, etc. By shifting the
	// results down we avoid that problem.
	RandType resultShifted = (m_currentSeed >> 16) & 0x7FFF;
	return resultShifted;
}
