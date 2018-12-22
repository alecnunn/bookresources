// ResourceObjTypeInfo_t.h: interface for the ResourceObjTypeInfo_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESOURCEOBJTYPEINFO_T_H__8E0B655A_3FDD_4F26_8531_4855D76AB469__INCLUDED_)
#define AFX_RESOURCEOBJTYPEINFO_T_H__8E0B655A_3FDD_4F26_8531_4855D76AB469__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ResourceObjFactoryInfo_t.h"

class ResourceObjTypeInfo_t  
{
public:
	ResourceObjTypeInfo_t();
	virtual ~ResourceObjTypeInfo_t();

	void clear() { m_FactoryInfoVector.clear(); }

	int m_nTypeID;
	ResourceObjFactoryInfoVector_t m_FactoryInfoVector;
};

typedef std::vector<ResourceObjTypeInfo_t> ResourceObjTypeInfoVector_t;



#endif // !defined(AFX_RESOURCEOBJTYPEINFO_T_H__8E0B655A_3FDD_4F26_8531_4855D76AB469__INCLUDED_)
