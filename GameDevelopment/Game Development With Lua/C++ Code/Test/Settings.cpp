// Settings.cpp: implementation of the Settings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Settings.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Settings::Settings()
{

}

Settings::~Settings()
{

}

void Settings::Init(const char *baseKey)
{
	m_baseKey = baseKey;
}

int Settings::GetInteger(const char *key)
{
	return 0;
}

std::string Settings::GetString(const char *key)
{
	return "unimp";
}


void Settings::SetInteger(const char *key, int value)
{
}

void Settings::SetString(const char *key, const char *value)
{
}

