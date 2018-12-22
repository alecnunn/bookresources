#ifndef _RESOURCE_CACHE_H_
#define _RESOURCE_CACHE_H_

#include "defs.h"

class ResourceCache
{
public:
	ResourceCache(){}
	virtual ~ResourceCache();
	virtual bool Add(const char* sResourceName, uint8* pData);
	virtual bool Find(const char* sResourceName, uint8* &pData);
	virtual int  Size(){return (int)m_Cache.size();}

protected:
	DataMap		m_Cache;
};


#endif
