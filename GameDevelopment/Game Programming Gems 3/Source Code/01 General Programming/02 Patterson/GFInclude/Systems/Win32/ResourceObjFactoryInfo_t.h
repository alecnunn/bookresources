// ResourceObjFactoryInfo_t.h: interface for the ResourceObjFactoryInfo_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESOURCEOBJFACTORYINFO_T_H__301173D6_CB50_4991_B592_2E266B617EFF__INCLUDED_)
#define AFX_RESOURCEOBJFACTORYINFO_T_H__301173D6_CB50_4991_B592_2E266B617EFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ResourceObjInstanceInfo_t.h"

class ResourceObjFactoryInfo_t  
{
public:
	ResourceObjFactoryInfo_t();
	virtual ~ResourceObjFactoryInfo_t();

	void clear() { m_InstanceInfoVector.clear(); }

	int m_nFactoryID;
	ResourceObjInstanceInfoVector_t m_InstanceInfoVector;
};

typedef std::vector<ResourceObjFactoryInfo_t> ResourceObjFactoryInfoVector_t;

#endif // !defined(AFX_RESOURCEOBJFACTORYINFO_T_H__301173D6_CB50_4991_B592_2E266B617EFF__INCLUDED_)
