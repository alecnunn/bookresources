#include "sgx/core/types/types.hpp"

/*
This file contains a pruned version of George Marsaglia's post 
from http://www.ciphersbyritter.com/NEWS4/RANDC.HTM 

Minor alterations have been made to fit in with the sgx naming scheme.
*/


/* Random seeds must be used to reset z,w,jsr,jco
*/
namespace {
	tUINT32 z=362436069, w=521288629, jsr=123456789, jcong=380116160;
};

#define znew  ((z=36969*(z&65535)+(z>>16))<<16)
#define wnew  ((w=18000*(w&65535)+(w>>16))&65535)
#define MWC   (znew+wnew)
#define SHR3  (jsr=(jsr=(jsr=jsr^(jsr<<17))^(jsr>>13))^(jsr<<5))
#define CONG  (jcong=69069*jcong+1234567)
#define KISS  ((MWC^CONG)+SHR3)
#define UNI   (KISS*2.328306e-10f)

void 
sgxSetRandSeed(tUINT32 nz, tUINT32 nw, tUINT32 njsr, tUINT32 njcong)
{
	z = nz;
	w = nw;
	jsr = njsr;
	jcong = njcong;
}

tREAL32 
sgxRand()
{
	return UNI;
}

tUINT32 
sgxRand(int n)
{
	return tUINT32(UNI*n);
}

tUINT32 
sgxRand(const int from, const int to)
{
	return tUINT32(UNI*(to-from)) + from;
}

