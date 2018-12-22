#ifndef SGX_CORE_SGXLIB_UTIL_HPP
#define SGX_CORE_SGXLIB_UTIL_HPP

template <typename T>
const T sgxMin(const T &v1, const T &v2)
{
	if (v1 < v2) {
		return v1;
	} else {
		return v2;
	}
}

template <typename T>
const T sgxMax(const T &v1, const T &v2)
{
	if (v1 > v2) {
		return v1;
	} else {
		return v2;
	}
}

template <typename T>
const T sgxMin(const T &v1, const T &v2, const T &v3)
{
	if (v1 < v2 && v1 < v3) {
		return v1;
	} else if (v2 < v3) {
		return v2;
	} else {
		return v3;
	}
}

template <typename T>
const T sgxMax(const T &v1, const T &v2, const T &v3)
{
	if (v1 > v2 && v1 > v3) {
		return v1;
	} else if (v2 > v3) {
		return v2;
	} else {
		return v3;
	}
}

template <typename T>
const T sgxRange(const T &val, const T &minv, const T &maxv)
{
	if (val < minv) {
		return minv;
	} else if (val > maxv) {
		return maxv;
	} else {
		return val;
	}
}

template <typename T>
void sgxSwap(T &val1, T &val2)
{
	T temp = val1;
	val1 = val2;
	val2 = temp;
}

SGX_INLINE tUINT32 
sgxRoundUpToPower2(const tUINT32 v)
{
	for(tUINT32 i=sizeof(tUINT32)*8;--i;) {
		// Find first used bit
		if (v & (1ul<<i)) {
			if (v == 1ul<<i) { // no other bits set, therefore it's already rounded
				return v;
			}
			// Other bits set, therefore next highest power is the 'next' one
			return 1<<(i+1);
		}
	}
	// Catch the last two cases, where v==0 or v==1
	return 1;
}

#endif /* SGX_CORE_SGXLIB_UTIL_HPP */
