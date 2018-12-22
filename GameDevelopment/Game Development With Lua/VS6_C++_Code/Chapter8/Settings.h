// Settings.h: interface for the Settings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETTINGS_H__6BBC2A1F_3C25_4AE7_A9D1_F8797CC6FD0E__INCLUDED_)
#define AFX_SETTINGS_H__6BBC2A1F_3C25_4AE7_A9D1_F8797CC6FD0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Settings  
{
public:
					Settings();
	virtual			~Settings();

	void			Init(const char *baseKey);

	int				GetInteger(const char *key, int defaultValue = 0);
	std::string		GetString(const char *key, const char *defaultString = NULL);

	void			SetInteger(const char *key, int value);
	void			SetString(const char *key, const char *value);

protected:
	std::string		m_baseKey;
	void			RegWriteValue(LPCSTR key_name, void *buffer, DWORD size, DWORD type);
	bool			RegReadValue(LPCSTR key_name, void *buffer, DWORD size, DWORD type);

};

#endif // !defined(AFX_SETTINGS_H__6BBC2A1F_3C25_4AE7_A9D1_F8797CC6FD0E__INCLUDED_)
