/*
s_p_oneil@hotmail.com
Copyright (c) 2000, Sean O'Neil
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of this project nor the names of its contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __PlanetInfo_h__
#define __PlanetInfo_h__

#include "Matrix.h"

class CInfo
{
protected:
	char m_szFile[_MAX_PATH];
	char m_szKey[_MAX_PATH];

public:
	CInfo()
	{
	}
	CInfo(const char *pszFile, const char *pszKey)
	{
		Init(pszFile, pszKey);
	}
	CInfo(const CInfo &info, const char *pszKey)
	{
		Init(info, pszKey);
	}
	~CInfo()
	{
		Cleanup();
	}

	void Init(const char *pszFile, const char *pszKey)
	{
		strcpy(m_szFile, pszFile);
		strcpy(m_szKey, pszKey);
	}
	void Init(const CInfo &info, const char *pszKey)
	{
		strcpy(m_szFile, info.m_szFile);
		strcpy(m_szKey, pszKey);
	}
	void Cleanup()
	{
		// Stub for future cleanup code
	}

	const char *GetFile()			{ return m_szFile; }
	const char *GetKey()			{ return m_szKey; }

	const char *GetTextValue(const char *pszValue, const char *pszDefault="")
	{
		const char *psz = pszDefault;
		static char szTemp[_MAX_PATH];
		GetPrivateProfileString(m_szKey, pszValue, "", szTemp, _MAX_PATH, m_szFile);
		if(*szTemp)
			psz = szTemp;
		return psz;
	}
	int GetIntValue(const char *pszValue, int nDefault=0)
	{
		int nValue = nDefault;
		char szTemp[_MAX_PATH];
		GetPrivateProfileString(m_szKey, pszValue, "", szTemp, _MAX_PATH, m_szFile);
		if(*szTemp)
			sscanf(szTemp, "%d", &nValue);
		return nValue;
	}
	float GetFloatValue(const char *pszValue, float fDefault=0.0f)
	{
		float fValue = fDefault;
		char szTemp[_MAX_PATH];
		GetPrivateProfileString(m_szKey, pszValue, "", szTemp, _MAX_PATH, m_szFile);
		if(*szTemp)
			sscanf(szTemp, "%f", &fValue);
		return fValue;
	}
	CVector GetVectorValue(const char *pszValue, CVector vDefault=CVector(0.0f))
	{
		CVector vValue = vDefault;
		char szTemp[_MAX_PATH];
		GetPrivateProfileString(m_szKey, pszValue, "", szTemp, _MAX_PATH, m_szFile);
		if(*szTemp)
			sscanf(szTemp, "%f, %f, %f", &vValue.x, &vValue.y, &vValue.z);
		return vValue;
	}
	CVector4 GetVector4Value(const char *pszValue, CVector4 vDefault=CVector4(0.0f))
	{
		CVector4 vValue = vDefault;
		char szTemp[_MAX_PATH];
		GetPrivateProfileString(m_szKey, pszValue, "", szTemp, _MAX_PATH, m_szFile);
		if(*szTemp)
			sscanf(szTemp, "%f, %f, %f, %f", &vValue.x, &vValue.y, &vValue.z, &vValue.w);
		return vValue;
	}
	CColor GetColorValue(const char *pszValue, CColor cDefault=CColor(0, 0, 0, 0))
	{
		CColor cValue = cDefault;
		char szTemp[_MAX_PATH];
		GetPrivateProfileString(m_szKey, pszValue, "", szTemp, _MAX_PATH, m_szFile);
		if(*szTemp)
		{
			int r, g, b, a;
			sscanf(szTemp, "%d, %d, %d, %d", &r, &g, &b, &a);
			cValue = CColor((unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a);
		}
		return cValue;
	}
};

#endif // __PlanetInfo_h__
