#ifndef __CHASH_H
#define __CHASH_H

#include "types.h"

class CHash
{
public:
	CHash();
	CHash(uint32 hashValue);
	CHash(const char *pString);
	CHash(const CHash &rhs);
	CHash &operator=(const CHash &rhs);

	void Set(const uint32 hash);

	bool IsValid() const;
	operator unsigned int()			{ return mHashValue; }

	const bool operator<(const CHash &rhs) const;
	const bool operator>(const CHash &rhs) const;
	const bool operator<=(const CHash &rhs) const;
	const bool operator>=(const CHash &rhs) const;
	const bool operator==(const CHash &rhs) const;
	const bool operator!=(const CHash &rhs) const;
private:
	uint32 MakeHash(const char *pString);

	uint32 mHashValue;
};
#endif //__CHASH_H