#ifndef SGX_CORE_MATH_TRIGONOMETRY_INLINE
#define SGX_CORE_MATH_TRIGONOMETRY_INLINE	1


SGX_INLINE tREAL32 sgxSin(tREAL32 theta)
{
	extern tREAL32			sgxSineTable[];
    const tREAL32 mul = 162.9746617261f;
    return sgxSineTable[((tUINT32)(theta * mul)) & 1023];
}

SGX_INLINE tREAL32 sgxCos(tREAL32 theta)
{
	extern tREAL32			sgxSineTable[];
    const tREAL32 mul = 162.9746617261f;
    return sgxSineTable[(((tUINT32)(theta * mul))+256) & 1023];
}

// This function exists so that platforms with more optimal
// handling of sin/cos can save time, without affecting the rest.
SGX_INLINE void sgxSinCos(tREAL32 &sine, tREAL32 &cosine, const tREAL32 theta)
{
	sine = sgxSin(theta);
	cosine = sgxCos(theta);
}

SGX_INLINE tREAL32 sgxASin(tREAL32 theta)
{
	return asinf(theta);
}

SGX_INLINE tREAL32 sgxACos(tREAL32 theta)
{
	return acosf(theta);
}

SGX_INLINE tREAL32 sgxAtan2(tREAL32 dx, tREAL32 dy)
{
	return atan2f(dx, dy);
}

SGX_INLINE tREAL32 sgxCosf(tREAL32 theta)
{
	return cosf(theta);
}

SGX_INLINE tREAL32 sgxSinf(tREAL32 theta)
{
	return sinf(theta);
}

SGX_INLINE tREAL32 sgxToDegrees(tREAL32 radians)
{
	return radians * 57.295779513082320876798154817014f;
}

SGX_INLINE tREAL32 sgxToRadians(tREAL32 degrees)
{
	return degrees * 0.017453292519943295769236907684886f;
}


#endif /* SGX_CORE_MATH_TRIGONOMETRY_INLINE */
