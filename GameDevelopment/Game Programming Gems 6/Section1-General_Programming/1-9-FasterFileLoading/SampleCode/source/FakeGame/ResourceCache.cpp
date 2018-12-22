#include "stdafx.h"
#include "ResourceCache.h"

ResourceCache::~ResourceCache()
{
	DataMap::iterator iter = m_Cache.begin();
	while(iter != m_Cache.end())
	{
		if(iter->second)
		{
			delete [] iter->second;
		}
		++iter;
	}
}

bool ResourceCache::Add(const char* sResourceName, uint8* pData)
{
	if(m_Cache.find(sResourceName) != m_Cache.end())
	{
		return false;
	}

	m_Cache[sResourceName] = pData;
	return true;
}

bool ResourceCache::Find(const char* sResourceName, uint8* &pData)
{
	DataMap::iterator iter = m_Cache.find(sResourceName);

	if(iter == m_Cache.end())
	{
		pData = NULL;
		return false;
	}

	pData = iter->second;
	return true;
}

