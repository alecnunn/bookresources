#include "sgx/core/core.hpp"
#include "sgx/core/regex/regex.hpp"

// The regex wrapper requires a one-line change to the
// (usually generic) sgxRegex class definition. That change
// being a reference to the underlying handling code.
// This does require extra work when/if the regex code is
// replaced, but due to the complexity of regex libraries,
// PCRE probably won't be replaced often.
sgxRegex::sgxRegex()
{
	m_bReady = FALSE;
}

sgxRegex::sgxRegex(const char *pRegexStr)
{
	re.compile(pRegexStr);
	m_bReady = TRUE;
}

sgxRegex::sgxRegex(const sgxString &regex)
{
	re.compile(regex);
	re.options(PCRE_NO_AUTO_CAPTURE);
	m_bReady = TRUE;
}

tUINT32
sgxRegex::Match(const char *pString)
{
	if (!m_bReady) {
		return 0;
	}

	vector<Regexp::markers> mrk = re.match(pString);
	return mrk.size();
}

tUINT32
sgxRegex::Match(const char *pString, sgxVector<sgxString> &results)
{
	if (!m_bReady) {
		return 0;
	}

	vector<Regexp::markers> mrk = re.match(pString);
	vector<Regexp::markers>::iterator it = mrk.begin();

	for(;it!=mrk.end();++it) {
		results.push_back(sgxString(&pString[(*it).first], (*it).second));
	}

	return mrk.size();
}

tUINT32
sgxRegex::Match(const sgxString &string)
{
	return Match(string.c_str());
}

