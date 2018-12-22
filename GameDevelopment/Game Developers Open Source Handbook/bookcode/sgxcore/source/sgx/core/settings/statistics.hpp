#ifndef SGX_CORE_SETTINGS_STATISTICS_HPP
#define SGX_CORE_SETTINGS_STATISTICS_HPP	1

class CStatisticsOptions : public CConfiguration
{
public:
	virtual void	Reset()
	{
		m_Options.clear();
	}
	
	virtual tINT32	IncOption(const sgxString &opt)
	{
		sgxOptionMap::iterator it = m_Options.find(opt);

		if (it == m_Options.end()) {
			SetOption(opt, 1);
			return 1;
		}

		return ++(*it).second;
	}

	virtual void	ReportAll()
	{
		sgxOptionMap::iterator it = m_Options.begin();
		while(it != m_Options.end())
		{
			sgxTrace("%s => %d", (*it).first.c_str(), (*it).second);
			++it;
		}

	}
};

#endif /* SGX_CORE_SETTINGS_STATISTICS_HPP */
