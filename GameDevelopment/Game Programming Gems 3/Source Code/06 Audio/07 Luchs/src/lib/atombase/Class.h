// VMClass.h: Schnittstelle für die Klasse CClass.
//
//////////////////////////////////////////////////////////////////////
// a class can have a list of prototype objects which you can use as templates
// to create new objects


#ifndef _atomosCClass_
#define _atomosCClass_


#include <map>
#include "Object.h"

namespace atomos
{
/*
interface IClass : public IObject
{
	virtual UINT32				GetClassID()=0;
	virtual BOOL				GetBaseClassID()=0;
	virtual CHAR8*				GetName()=0;
	virtual UINT32				EqualsName(const CHAR8* pszName, UINT32 nFlags=0)=0;

	virtual void*				CreateObject()=0;
	virtual BOOL				IsA(UINT32 idClass)=0;
	virtual IClass*				GetBaseClass()=0;

};
*/

class ATOMBASE_DLLCLASS CClass : public CObject 
{

public:

	// IObject
	enum { CID = CID_Class };
	static const CClass classCClass;
	virtual const IClass*		GetClass();

	UINT32					m_nClassID;
	UINT32					m_nBaseClassID;
	CHAR8*					m_pszClassName;
	ATOM_CALL_CREATEOB		m_pCreateCall;


	virtual UINT32				GetClassID() { return(m_nClassID); }
	virtual BOOL				GetBaseClassID() { return(m_nBaseClassID); }
	virtual CHAR8*				GetName() { return(m_pszClassName); }
	virtual UINT32				EqualsName(const CHAR8* pszName, UINT32 nFlags=0);

	virtual void*				CreateObject();
	virtual BOOL				IsA(UINT32 idClass);
	virtual IClass*				GetBaseClass();

	CClass(UINT32 id=0, UINT32 idBase=0, CHAR8* pszClassName=NULL, ATOM_CALL_CREATEOB m_pCreateCall=NULL);
	~CClass();


//	CObjects	m_Prototypes; // object templates
//	CObjects	m_Objects; // created objects

};



} // namespace

#endif // _atomosCClass_
