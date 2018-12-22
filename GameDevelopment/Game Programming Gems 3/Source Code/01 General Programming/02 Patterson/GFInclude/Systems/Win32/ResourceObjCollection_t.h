// ResourceObjCollection_t.h: interface for the ResourceObjCollection_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESOURCEOBJCOLLECTION_T_H__C917AEA7_31EB_486E_B9F3_5D73C8FDA9D1__INCLUDED_)
#define AFX_RESOURCEOBJCOLLECTION_T_H__C917AEA7_31EB_486E_B9F3_5D73C8FDA9D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ResourceObjTypeInfo_t.h"

class ResourceObjCollection_t  
{
public:
	ResourceObjCollection_t();
	virtual ~ResourceObjCollection_t();

	void clear() { m_TypeInfoVector.clear(); }

	ResourceObjTypeInfoVector_t m_TypeInfoVector;
};

typedef std::vector<ResourceObjCollection_t> ResourceObjCollectionVector_t;

#endif // !defined(AFX_RESOURCEOBJCOLLECTION_T_H__C917AEA7_31EB_486E_B9F3_5D73C8FDA9D1__INCLUDED_)
