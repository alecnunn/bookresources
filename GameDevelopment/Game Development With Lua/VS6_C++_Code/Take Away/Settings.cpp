// Settings.cpp: implementation of the Settings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Settings.h"

#define REG_STRING_SIZE 256
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Settings::Settings()
{

}

Settings::~Settings()
{

}


void Settings::RegWriteValue(LPCSTR key_name, void *buffer, DWORD size, DWORD type)
{
	HKEY hkey;
	DWORD success, disposition;

	if ( (success = RegCreateKeyEx(HKEY_CURRENT_USER, m_baseKey.c_str(), 0, "CRM", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, &disposition)) == ERROR_SUCCESS)
	{
		success = RegSetValueEx(hkey, key_name, 0, type, (const BYTE *) buffer, size);
		RegCloseKey(hkey);
	}

}

bool Settings::RegReadValue(LPCSTR key_name, void *buffer, DWORD size, DWORD type)
{
	HKEY hkey;
	DWORD sucess, disposition;

	if ( (sucess = RegCreateKeyEx(HKEY_CURRENT_USER, m_baseKey.c_str(), 0, "CRM", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, &disposition)) == ERROR_SUCCESS)
	{
		sucess = RegQueryValueEx(hkey, key_name, 0, &type, (BYTE *) buffer, &size);

		RegCloseKey(hkey);
	}

	return (sucess==ERROR_SUCCESS);
}

void Settings::Init(const char *baseKey)
{
	m_baseKey = "Software\\CRMLua\\";
	m_baseKey += baseKey;
}

int Settings::GetInteger(const char *key, int defaultValue)
{
	int val = defaultValue;

	if(!RegReadValue(key, &val, sizeof(DWORD), REG_DWORD))
	{
		val = defaultValue;
	}

	return val;
}

static char stringBuf[REG_STRING_SIZE];

std::string Settings::GetString(const char *key, const char *defaultString)
{

	if(RegReadValue(key, stringBuf, sizeof(stringBuf), REG_SZ))
	{
		return stringBuf;
	}

	
	return defaultString;
}


void Settings::SetInteger(const char *key, int value)
{
	RegWriteValue(key, &value, sizeof(DWORD), REG_DWORD);
}

void Settings::SetString(const char *key, const char *value)
{
	RegWriteValue(key, (void *) value, REG_STRING_SIZE, REG_SZ);
}

