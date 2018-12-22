#include "CHash.h"

#define INVALID_HASH 0xffffffff
#define HASH_INIT	0x811c9dc5
#define HASH_PRIME	0x01000193
#define CONVERT_BACKSLASH
#define CASE_INSENSITIVE

CHash::CHash() : mHashValue(INVALID_HASH)
{
}

CHash::CHash(uint32 hashValue) : mHashValue(hashValue)
{
}

CHash::CHash(const CHash &rhs) : mHashValue(rhs.mHashValue)
{

}

CHash::CHash(const char *pString)
{
	mHashValue = MakeHash(pString);
}

CHash &CHash::operator=(const CHash &rhs)
{
	if (&rhs != this)
	{
		mHashValue = rhs.mHashValue;
	}
	return *this;
}

bool CHash::IsValid() const
{
	return (mHashValue != INVALID_HASH);
}

uint32 CHash::MakeHash(const char *hashString)
{
	if (!hashString || !hashString[0])
		return INVALID_HASH;

	const unsigned char* string = (const unsigned char*)hashString;
	uint32 hash = HASH_INIT;

	while (*string)
	{
		hash *= HASH_PRIME;

		char c = *string++;

#ifdef CONVERT_BACKSLASH
		if (c == '\\')
		{
			c = '/';
		}
#endif

#ifdef CASE_INSENSITIVE
		if ((c >= 'a') && (c <= 'z'))
		{
			c -= 'a' - 'A';
		}
#endif
		hash ^= (uint32)c;
	}
	return hash;
}

void CHash::Set(const uint32 hash)
{
	mHashValue = hash;
}

const bool CHash::operator<(const CHash &rhs) const
{
	return mHashValue < rhs.mHashValue;
}

const bool CHash::operator>(const CHash &rhs) const
{
	return mHashValue > rhs.mHashValue;
}

const bool CHash::operator<=(const CHash &rhs) const
{
	return mHashValue <= rhs.mHashValue;
}

const bool CHash::operator>=(const CHash &rhs) const
{
	return mHashValue >= rhs.mHashValue;
}

const bool CHash::operator==(const CHash &rhs) const
{
	return mHashValue == rhs.mHashValue;
}

const bool CHash::operator!=(const CHash &rhs) const
{
	return (!(*this == rhs));
}