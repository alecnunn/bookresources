#ifndef SGX_CORE_REGEX_SGXWRAPPER_HPP
#define SGX_CORE_REGEX_SGXWRAPPER_HPP	1

#include "sgx/core/regex/pcre/re.h"

class sgxRegex {
public:
	sgxRegex();
	sgxRegex(const char *pRegexStr);
	sgxRegex(const sgxString &regex);

	tUINT32	Match(const char *pString);
	tUINT32 Match(const char *pString, sgxVector<sgxString> &results);

	tUINT32	Match(const sgxString &string);

private:
	Regexp	re;
	tBOOL	m_bReady;
};

#endif /* SGX_CORE_REGEX_SGXWRAPPER_HPP */
