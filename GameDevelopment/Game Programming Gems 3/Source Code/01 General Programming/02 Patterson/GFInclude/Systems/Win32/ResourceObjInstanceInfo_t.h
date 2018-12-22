// ResourceObjInstanceInfo_t.h: interface for the ResourceObjInstanceInfo_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESOURCEOBJINSTANCEINFO_T_H__52038258_CD29_454C_91E3_6376D6B3D671__INCLUDED_)
#define AFX_RESOURCEOBJINSTANCEINFO_T_H__52038258_CD29_454C_91E3_6376D6B3D671__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ResourceObjInstanceInfo_t  
{
public:
	ResourceObjInstanceInfo_t();
	virtual ~ResourceObjInstanceInfo_t();

	void clear() {}

	int m_nInstanceID;
	int m_nConfigID;
};

typedef std::vector<ResourceObjInstanceInfo_t> ResourceObjInstanceInfoVector_t;

#endif // !defined(AFX_RESOURCEOBJINSTANCEINFO_T_H__52038258_CD29_454C_91E3_6376D6B3D671__INCLUDED_)
