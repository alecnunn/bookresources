#include "sgx/core/core.hpp"

CConfiguration	*CConfiguration::m_pSingleton;

CConfiguration *CConfiguration::Get()
{
	if (!m_pSingleton) {
		m_pSingleton = Create();
	}
	return m_pSingleton;
}

CConfiguration *CConfiguration::Create()
{
	if (!m_pSingleton) {
		m_pSingleton = new CConfiguration;
	}
	return m_pSingleton;
}

void 
CConfiguration::SetOption(const sgxString &opt, tINT32 value)
{
    if (m_bComplete) {
        sgxTrace("Option '%s' ignored!", opt.c_str());
        return;
    }

    m_Options[opt] = value;
}

tINT32	CConfiguration::GetOption(const sgxString &opt, tINT32 opt_default/*=0*/)
{
	sgxOptionMap::iterator it = m_Options.find(opt);

	if (it == m_Options.end()) {
		return opt_default;
	}

	return (*it).second;
}

void 
CConfiguration::Complete(void)
{
	m_bComplete = TRUE;
}
