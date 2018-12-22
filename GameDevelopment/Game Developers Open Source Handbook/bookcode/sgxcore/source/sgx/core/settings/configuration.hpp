#ifndef SGX_CORE_SETTINGS_CONFIGURATION_HPP
#define SGX_CORE_SETTINGS_CONFIGURATION_HPP	1

class CConfiguration {
public:
	static CConfiguration *Get();
	static CConfiguration *Create();

	tINT32	GetOption(const sgxString &opt, tINT32 opt_default=0);
	void	SetOption(const sgxString &opt, tINT32 value);
	// Completion denies all future SetOption's
	void    Complete(void);

protected:
	static CConfiguration		*m_pSingleton;

	typedef sgxMap<sgxString, tINT32> sgxOptionMap;
	sgxOptionMap				m_Options;

private:
	tBOOL						m_bComplete;
};

#endif /* SGX_CORE_SETTINGS_CONFIGURATION_HPP */
