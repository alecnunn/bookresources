#ifndef SGX_CORE_MATH_NUMERICS_INLINE
#define SGX_CORE_MATH_NUMERICS_INLINE	1

SGX_INLINE tREAL32 sgxAbs(tREAL32 f)
{
    if (f < 0) return -f;
    return f;
}

SGX_INLINE tBOOL sgxEq(tREAL32 fValue1, tREAL32 fValue2, tREAL32 epsilon=sgxEPSILON)
{
    return sgxAbs(fValue1 - fValue2) < epsilon;
}

SGX_INLINE tBOOL sgxNeq(tREAL32 fValue1, tREAL32 fValue2, tREAL32 epsilon=sgxEPSILON)
{
    return sgxAbs(fValue1 - fValue2) >= epsilon;
}

SGX_INLINE tREAL32 sgxSqr(tREAL32 f)
{
	return sqrtf(f);
}

SGX_INLINE tREAL32 sgxSign(tREAL32 f)
{
	if (f < 0) {
		return -1.0f;
	} else if (f > 0) {
		return 1.0f;
	} else {
		return 0;
	}
}

SGX_INLINE tINT32 sgxFloor(tREAL32 f)
{
	return SGX_STATIC_CAST(tINT32,f);
}

SGX_INLINE tREAL32 sgxApproach(const tREAL32 from, const tREAL32 approaching, const tREAL32 max_delta=0.1f)
{
	tREAL32 next = from;

	if (next > approaching) {
		next -= max_delta;
		if (next < approaching) {
			return approaching;
		}
	} else if (next < approaching) {
		next += max_delta;
		if (next > approaching) {
			return approaching;
		}
	}

	return next;			
}

SGX_INLINE tREAL32 sgxConvert(const tINT32 from, const tREAL32 scale)
{
	tREAL32 val = SGX_STATIC_CAST(tREAL32, from);
	val /= scale;
	return val;
}

SGX_INLINE tINT32 sgxConvert(const tREAL32 from, const tINT32 scale)
{
	tINT32 val = SGX_STATIC_CAST(tINT32, from);
	val *= scale;
	return val;
}

#endif /* SGX_CORE_MATH_NUMERICS_INLINE */
