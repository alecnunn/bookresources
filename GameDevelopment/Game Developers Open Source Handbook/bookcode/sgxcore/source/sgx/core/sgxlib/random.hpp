#ifndef SGX_CORE_SGXLIB_RANDOM_HPP
#define SGX_CORE_SGXLIB_RANDOM_HPP	1

void sgxSetRandSeed(tUINT32 nz, tUINT32 nw, tUINT32 njsr, tUINT32 njcong);
tREAL32 sgxRand();
tUINT32 sgxRand(int n);
tUINT32 sgxRand(const int from, const int to);

#endif /* SGX_CORE_SGXLIB_RANDOM_HPP */
